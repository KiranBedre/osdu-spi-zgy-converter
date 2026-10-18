/*
Copyright 2024 Schlumberger

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

#include "Segy2DReaderImpl.h"
#include "../Logger/Logger.h"
#include "../SegyIO/Primitives.h"

namespace segysdk::bulkaccess
{

Segy2DReaderImpl::Segy2DReaderImpl(const std::shared_ptr<fileio::FileReader>& file, const std::shared_ptr<fileio::FileReader>& indexFile, const std::shared_ptr<BulkIndex>& bulkIndex)
  : sampleSize_(segyio::Primitives::getSampleSize(bulkIndex->getSampleFormat())),
    fileReader_(std::make_shared<fileio::BufferedFileReader>(file)),
    traceIndexReader_(std::make_shared<fileio::BufferedFileReader>(indexFile)),
    bulkIndex_(bulkIndex),
    converter_(segyio::valueconversion::SampleValueConverter::create(bulkIndex_->getSampleFormat(), bulkIndex_->isByteSwapped())),
    traceIndexer_(bulkIndex_->createTraceIndexer(traceIndexReader_))
{
    fileReader_->setBufferSize(defaultBufferSize_);
}

template<typename RangeIndex>
RangeIndex
calcRangeSize(const RangeIndex indexFirst, const RangeIndex indexLast)
{
    return indexLast - indexFirst + 1;
}

Samples
Segy2DReaderImpl::doRead(const Segy2DReaderParams& params)
{
    auto result = Samples(calcRangeSize(params.traceStart_, params.traceEnd_) * calcRangeSize(params.sampleStart_, params.sampleEnd_));
    readByTraceNo(params.traceStart_, params.traceEnd_, params.sampleStart_, params.sampleEnd_, result.data());
    return result;
}

size_t
Segy2DReaderImpl::readByCmp(int cmpStart, int cmpEnd, size_t sampleStart, size_t sampleEnd, float* buffer)
{
    size_t traceStart = getTraceNoByCmp(cmpStart);
    size_t traceEnd = getTraceNoByCmp(cmpEnd);
    return readByTraceNo(traceStart, traceEnd, sampleStart, sampleEnd, buffer);
}

size_t
Segy2DReaderImpl::readByTraceNo(size_t traceNoStart, size_t traceNoEnd, size_t sampleStart, size_t sampleEnd, float* buffer)
{
    if (traceNoEnd < traceNoStart) {
        throw SegyInvalidArgumentException("Invalid requested Trace range.");
    }
    if (traceNoEnd - traceNoStart > bulkIndex_->getTracesCount()) {
        // Don't report, just warn
        SEGYLOG_WARNING("Requested read trace size is bigger than actual number of traces");
    }
    if (sampleEnd < sampleStart) {
        throw SegyInvalidArgumentException("Invalid requested Sample range.");
    }

    if (buffer == nullptr) {
        throw SegyInvalidArgumentException("Null output buffer.");
    }

    const size_t sampleAlignment = converter_->getRequiredAlignment();
    const size_t sampleOffset = getByteCount(sampleStart);
    const size_t samplesCount = sampleEnd - sampleStart + 1;

    size_t tracesRead = 0;
    for (size_t traceNo = traceNoStart; traceNo <= traceNoEnd; ++traceNo) {
        float* output = buffer + (traceNo - traceNoStart) * samplesCount;
        memset(output, 0, samplesCount * sizeof(float));

        if (!traceIndexer_->findTraceByTraceNo(traceNo)) {
            // Should not happen
            SEGYLOG_WARNING(std::string("Requested TraceNo:" + std::to_string(traceNo) + " doesn't exist."));
            continue;
        }
        size_t offset = traceIndexer_->getOffset() + sampleOffset;
        float* target = output;
        size_t samples = samplesCount;
        size_t byteCount = getByteCount(samples);

        if (traceIndexer_->getSampleCount() < bulkIndex_->getSamplesCount()) {
            // align if the found trace has fewer samples
            const size_t actualStart = std::max(sampleStart, static_cast<size_t>(traceIndexer_->getSampleIndex()));
            const size_t actualEnd = std::min(sampleEnd, traceIndexer_->getSampleIndex() + traceIndexer_->getSampleCount());

            if (actualStart >= actualEnd) { // outside the requested region
                continue;
            }

            if (actualStart > sampleStart) {
                // move pointer to the actual region where data is present
                target += actualStart - sampleStart;
            }

            offset = traceIndexer_->getOffset() + getByteCount(actualStart - traceIndexer_->getSampleIndex());
            samples = actualEnd - actualStart;
            byteCount = getByteCount(samples);
        }
        uint8_t const* bytes = readAligned(offset, byteCount, sampleAlignment);
        if (bytes == nullptr) {
            continue;
        }
        convert(bytes, target, samples);
        ++tracesRead;
    }
    return tracesRead;
}

size_t
Segy2DReaderImpl::readTracesByCmp(int cmpStart, size_t traces, size_t sampleStart, size_t sampleEnd, float* buffer)
{
    size_t traceStart = getTraceNoByCmp(cmpStart);
    return readByTraceNo(traceStart, traceStart + traces, sampleStart, sampleEnd, buffer);
}

void
Segy2DReaderImpl::setBufferLoadChunk(size_t newChunk) const
{
    fileReader_->setBufferSizeBoundary(newChunk);
    fileReader_->setBufferLoadChunk(newChunk);
}

uint8_t const*
Segy2DReaderImpl::readAligned(uint64_t offset, size_t size, size_t alignment) const
{
    return fileReader_->readAligned(offset, size, alignment);
}

void
Segy2DReaderImpl::convert(uint8_t const* bytes, float* target, size_t sampleCount) const
{
    if (bulkIndex_->hasTraceWeights()) {
        converter_->convertScaled(bytes, target, sampleCount, traceIndexer_->getTraceWeight());
    }
    else {
        converter_->convert(bytes, target, sampleCount);
    }
}

size_t
Segy2DReaderImpl::getTraceNoByCmp(int cmp) const
{
    size_t traceNo = traceIndexer_->getTraceNoForCMP(cmp);
    if (traceNo == 0) {
        traceNo = traceIndexer_->getTraceNoForCMP(cmp, true);
        SEGYLOG_WARNING("TraceNo for CMP: " + std::to_string(cmp) + " not found. Using closest: " + std::to_string(traceNo));
    }
    return traceNo;
}

size_t
Segy2DReaderImpl::getByteCount(size_t sampleCount) const
{
    return sampleSize_ * sampleCount;
}

size_t
Segy2DReaderImpl::getTraceSize() const
{
    return segyio::Primitives::getTraceHeaderSize() + getByteCount(bulkIndex_->getSamplesCount());
}

} // namespace segysdk::bulkaccess
