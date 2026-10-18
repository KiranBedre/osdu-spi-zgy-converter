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

#include "../BulkAccess/SimpleBulkIndex.h"
#include "../SegyIO/Primitives.h"
#include "../SegyIO/SegyIOException.h"

namespace segysdk::bulkaccess
{
using segyio::Primitives;

// ********************************************************************************
// class SimpleBulkIndex::Indexer

class SimpleBulkIndex::Indexer : public BulkIndex::TraceIndexer
{
private:
    size_t const firstOffset_ = Primitives::getCombinedHeaderSize(0) + Primitives::getTraceHeaderSize();
    size_t const strideI_;
    size_t const strideJ_;

public:
    Indexer(size_t sizeJ, size_t sizeK, SampleFormat sampleFormat)
      : BulkIndex::TraceIndexer(sizeK),
        strideI_(Primitives::getTraceSize(sampleFormat, sizeK) * sizeJ),
        strideJ_(Primitives::getTraceSize(sampleFormat, sizeK))
    {
        /* EMPTY */
    }

    virtual ~Indexer()
    {
        /* EMPTY */
    }

    std::shared_ptr<segysdk::segyindex::SegyIndex> getIndex() const
    {
        return {nullptr};
    }

    virtual bool findTrace(size_t i, size_t j) override
    {
        setOffset(firstOffset_ + i * strideI_ + j * strideJ_);
        return true;
    }

    virtual bool findTraceByIndex(size_t i, size_t j) override
    {
        return findTrace(i, j);
    }
};

// ********************************************************************************
// class SimpleBulkIndex

SimpleBulkIndex::SimpleBulkIndex(size_t sizeI, size_t sizeJ, size_t sizeK, SampleFormat sampleFormat)
  : BulkIndex(sizeI, sizeJ, sizeK, sampleFormat)
{
    /* EMPTY */
}

std::shared_ptr<BulkIndex::TraceIndexer>
SimpleBulkIndex::createTraceIndexerImpl()
{
    return std::make_shared<Indexer>(getSizeJ(), getSizeK(), getSampleFormat());
}

std::shared_ptr<BulkIndex::TraceIndexer>
SimpleBulkIndex::createTraceIndexer([[maybe_unused]] std::shared_ptr<fileio::BufferedFileReaderBase> reader)
{
    return createTraceIndexerImpl();
}

} // namespace segysdk::bulkaccess
