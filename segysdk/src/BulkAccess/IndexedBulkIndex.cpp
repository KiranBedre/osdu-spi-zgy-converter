/*
Copyright 2021-2022 Schlumberger

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#include "IndexedBulkIndex.h"

#include "../SegyFormat/Internal/FileHeaderDefinitionStringConstsTraceHeader.h"
#include "../SegyIO/HeaderDefinitionProvider.h"
#include "../SegyIO/Primitives.h"
#include "../SegyIO/SegyIOException.h"
#include "BufferedTraceHeaderReader.h"

#include <cmath>
#include <limits.h>
#include <utility>

#include "BufferedTraceHeaderReader.cpp" // implementation of 'template<typename READER> class BufferedTraceHeaderReader'

namespace segysdk::bulkaccess
{
using segyio::FileHeaderDescription;
using segyio::HeaderDefinitionProvider;
using segyio::Primitives;

// ********************************************************************************
class IndexedBulkIndex::Cube
{
private:
    segysdk::segyindex::HorizontalExtent const inlineExtent_;
    segysdk::segyindex::HorizontalExtent const crosslineExtent_;
    segysdk::segyindex::VerticalExtent sampleExtent_;

public:
    explicit Cube(segyindex::SeismicCubeExtents const& extents)
      : inlineExtent_(extents.getJVectorExtent()),
        crosslineExtent_(extents.getIVectorExtent()),
        sampleExtent_(extents.getSampleExtent())
    {
        // Revert sample extent to file units (tofs), based on current tribal knowledge
        if (extents.getSampleExtent().domain() == segyio::VerticalDomain::Time) {
            sampleExtent_ = {
                sampleExtent_.first() * 1000.0,
                sampleExtent_.last() * 1000.0,
                sampleExtent_.step() * 1000.0};
        }
    }

    int getInlineValue(size_t index) const
    {
        return inlineExtent_.first() + static_cast<int>(index) * inlineExtent_.step();
    }

    int getCrosslineValue(size_t index) const
    {
        return crosslineExtent_.first() + static_cast<int>(index) * crosslineExtent_.step();
    }

    int getSampleIndex(double value) const
    {
        return static_cast<int>(std::round((value - sampleExtent_.first()) / sampleExtent_.step()));
    }
};
// ********************************************************************************

// ********************************************************************************
template<typename READER>
class IndexedBulkIndex::Indexer : public BulkIndex::TraceIndexer
{
private:
    std::shared_ptr<segysdk::segyindex::SegyIndex> index_;
    std::shared_ptr<BufferedTraceHeaderReader<READER>> reader_;
    int ixSampleStart_ = -1;
    int ixSampleCount_ = -1;
    int ixTraceWeight_ = -1;
    bool isConstTraceHeaderValues_ = false;

public:
    Indexer(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index, const std::shared_ptr<BufferedTraceHeaderReader<READER>>& reader, size_t sampleCount, float traceWeight)
      : BulkIndex::TraceIndexer(sampleCount),
        index_(index),
        reader_(reader)
    {
        setTraceWeight(traceWeight);

        if (reader != nullptr) {
            ixSampleStart_ = reader->getValueIndex(segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ShotToRecordingLagTime);
            ixSampleCount_ = reader->getValueIndex(segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::TraceLength);
            ixTraceWeight_ = reader->getValueIndex(segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::TraceWeightFactor);
        }

        isConstTraceHeaderValues_ = index_->hasConstantTraceWeightingFactor()
                                    and index_->hasConstantTraceLength();

        if (index_->hasConstantTraceLength()) {
            setSampleCount(index_->getConstantTraceLength());
        }

        if (index_->hasConstantTraceWeightingFactor()) {
            setTraceWeight(index_->getConstantTraceWeightingFactor());
        }
    }

    ~Indexer() override
    {
        /* EMPTY */
    }

    virtual int getSampleIndex(double value) const = 0;

    const std::shared_ptr<segysdk::segyindex::SegyIndex>& getIndex() const
    {
        return index_;
    }

    bool hasConstTraceHeaderValues() const
    {
        return isConstTraceHeaderValues_;
    }

    bool readNonConstHeaderValues(size_t offset)
    {
        if (reader_ == nullptr) {
            return true;
        }

        double const* const values = reader_->getValues(offset);
        if (values == nullptr) {
            return false;
        }

        if (ixSampleStart_ >= 0) {
            int const sampleIndex = getSampleIndex(values[ixSampleStart_]);
            setSampleIndex(sampleIndex);
        }

        if (not index_->hasConstantTraceLength()
            and ixSampleCount_ >= 0) {
            auto const sampleCount = static_cast<size_t>(values[ixSampleCount_]);
            setSampleCount(sampleCount);
        }

        if (not index_->hasConstantTraceWeightingFactor()
            and ixTraceWeight_ >= 0) {
            const auto traceWeight = static_cast<float>(std::pow(2.0, -values[ixTraceWeight_]));
            setTraceWeight(traceWeight);
        }

        return true;
    }
};

template<typename READER>
class IndexedBulkIndex::Indexer3D : public IndexedBulkIndex::Indexer<READER>
{
private:
    Cube cube_;

private:
    bool findTraceByIndexImpl(size_t i, size_t j, segyindex::IndexField& entry)
    {
        int const iline = cube_.getInlineValue(i);
        int const xline = cube_.getCrosslineValue(j);
        entry = Indexer<READER>::getIndex()->getIndexAt(iline, xline);
        if (entry.getOffset() == 0) {
            return false;
        }

        Indexer<READER>::setOffset(entry.getOffset() + Primitives::getTraceHeaderSize());

        return true;
    }

public:
    Indexer3D(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index, const std::shared_ptr<BufferedTraceHeaderReader<READER>>& reader, size_t sampleCount, float traceWeight)
      : IndexedBulkIndex::Indexer<READER>(index, reader, sampleCount, traceWeight),
        cube_(index->getFileLayoutExtents())
    {
    }

    int getSampleIndex(double value) const override
    {
        return cube_.getSampleIndex(value);
    }

    ~Indexer3D() override
    {
        /* EMPTY */
    }

    bool findTrace(size_t i, size_t j) override
    {
        segyindex::IndexField entry(/*segyFileNo*/ USHRT_MAX, /*inlineNo*/ 0, /*crosslineNo*/ 0, /*offset*/ 0);

        bool const isTraceFound = findTraceByIndexImpl(i, j, entry);
        if (not isTraceFound) {
            return false;
        }

        if (not Indexer<READER>::hasConstTraceHeaderValues()) {
            return Indexer<READER>::readNonConstHeaderValues(entry.getOffset());
        }

        return true;
    }

    bool findTraceByIndex(size_t i, size_t j) override
    {
        segyindex::IndexField entry(/*segyFileNo*/ USHRT_MAX, /*inlineNo*/ 0, /*crosslineNo*/ 0, /*offset*/ 0);

        return findTraceByIndexImpl(i, j, entry);
    }
};

template<typename READER>
class IndexedBulkIndex::Indexer2D : public IndexedBulkIndex::Indexer<READER>
{
public:
    Indexer2D(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index, const std::shared_ptr<BufferedTraceHeaderReader<READER>>& reader, size_t sampleCount, float traceWeight)
      : IndexedBulkIndex::Indexer<READER>(index, reader, sampleCount, traceWeight)
    {
    }

    int getSampleIndex(double value) const override
    {
        auto extents = Indexer<READER>::getIndex()->getLineExtents();
        return static_cast<int>(std::round((value - extents.getSampleExtent().first()) / extents.getSampleExtent().step()));
    }

    ~Indexer2D() override
    {
        /* EMPTY */
    }

    [[noreturn]] bool findTrace([[maybe_unused]] size_t i, [[maybe_unused]] size_t j) override
    {
        throw segyio::SegyRuntimeException("Should not be called for 2D Segy Index");
    }

    [[noreturn]] bool findTraceByIndex([[maybe_unused]] size_t i, [[maybe_unused]] size_t j) override
    {
        throw segyio::SegyRuntimeException("Should not be called for 2D Segy Index");
    }

    bool findTraceByCmp(int cmp) override
    {
        size_t offset = Indexer<READER>::getIndex()->getOffsetForCMP(cmp);
        if (offset == 0) {
            return false;
        }

        Indexer<READER>::setOffset(offset + Primitives::getTraceHeaderSize());

        if (not Indexer<READER>::hasConstTraceHeaderValues()) {
            return Indexer<READER>::readNonConstHeaderValues(offset);
        }

        return true;
    }

    bool findTraceByTraceNo(size_t traceNo) override
    {
        size_t offset = Indexer<READER>::getIndex()->getOffsetForTraceNo(traceNo);
        if (offset == 0) {
            return false;
        }

        Indexer<READER>::setOffset(offset + Primitives::getTraceHeaderSize());

        if (not Indexer<READER>::hasConstTraceHeaderValues()) {
            return Indexer<READER>::readNonConstHeaderValues(offset);
        }

        return true;
    }

    size_t getTraceNoForCMP(int cmp, bool findNextClosest = false) override
    {
        return Indexer<READER>::getIndex()->getTraceNoForCMP(cmp, findNextClosest);
    }
};

// ********************************************************************************
// class IndexedBulkIndex

IndexedBulkIndex::IndexedBulkIndex(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index,
                                   std::shared_ptr<const FileHeaderDescription> headerDescription,
                                   const segysdk::segyio::ConversionDetails& details)
  : BulkIndex(),
    index_(index),
    headerDescription_(std::move(headerDescription))
{
    if (index_->isSegy3D()) {
        const auto extents = index->getFileLayoutExtents();
        setSize(static_cast<size_t>(extents.getJVectorExtent().count()),
                static_cast<size_t>(extents.getIVectorExtent().count()),
                static_cast<size_t>(extents.getSampleExtent().count()));
    }
    else {
        const auto extents = index->getLineExtents();
        setSamplesCount(extents.getSampleExtent().count());
        setTracesCount(index->getIndexedTraceCount());
    }
    setSampleFormat(details.getSampleValueType());
    setByteSwapped(details.needsByteSwap());

    // Not sure whether this is accurate, might add some clause on regularity
    setCrosslineOrdered(!index->isInlineOrdered());

    if (!Primitives::isFloatFormat(getSampleFormat())
        && (!index->hasConstantTraceWeightingFactor()
            || index->getConstantTraceWeightingFactor() > 0)) {
        setTraceWeighting(true);
    }
}

IndexedBulkIndex::~IndexedBulkIndex()
{
    /* EMPTY */
}

template<typename READER>
std::shared_ptr<BulkIndex::TraceIndexer>
IndexedBulkIndex::createTraceIndexerImpl(std::shared_ptr<READER> reader)
{
    float traceWeight = 1.0f;
    std::vector<std::string> headers;

    if (!index_->hasConstantTraceLength()) {
        // TODO: Indexing logic for variable first time is incomplete
        headers.push_back(segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ShotToRecordingLagTime);
        headers.push_back(segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::TraceLength);
    }

    if (hasTraceWeights()) {
        if (index_->hasConstantTraceWeightingFactor()) // For now, also support negative weights
            traceWeight = static_cast<float>(std::pow(2.0, -index_->getConstantTraceWeightingFactor()));
        else
            headers.push_back(segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::TraceWeightFactor);
    }

    std::shared_ptr<BufferedTraceHeaderReader<READER>> headerReader;
    if (!headers.empty()) {
        ConversionDetails details(isByteSwapped(), getSampleFormat());
        HeaderDefinitionProvider provider = HeaderDefinitionProvider(*headerDescription_->getTraceHeaders());
        headerReader = std::make_shared<BufferedTraceHeaderReader<READER>>(reader, provider, headers, details);
    }

    if (not index_->isSegy3D()) {
        size_t sampleCount = index_->getLineExtents().getSampleExtent().count();
        return std::make_shared<IndexedBulkIndex::Indexer2D<READER>>(index_, headerReader, sampleCount, traceWeight);
    }
    return std::make_shared<IndexedBulkIndex::Indexer3D<READER>>(index_, headerReader, getSizeK(), traceWeight);
}

std::shared_ptr<BulkIndex::TraceIndexer>
IndexedBulkIndex::createTraceIndexer(std::shared_ptr<fileio::BufferedFileReaderBase> reader)
{
    return createTraceIndexerImpl<fileio::BufferedFileReaderBase>(reader);
}

} // namespace segysdk::bulkaccess
