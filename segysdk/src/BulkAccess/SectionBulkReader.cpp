/*
Copyright 2022 Schlumberger

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

#include "../BulkAccess/SectionBulkReader.h"

#include "../SegyIO/Primitives.h"
#include "../SegyIO/SegyIOException.h"

#include <algorithm>

namespace segysdk::bulkaccess
{
using segyio::Primitives;

SectionBulkReader::SectionBulkReader(std::vector<std::shared_ptr<fileio::FileReader>>&& readers,
                                     std::shared_ptr<fileio::FileReader> indexReader,
                                     const std::shared_ptr<BulkIndex>& bulkIndex)
  : BulkReaderBase(Primitives::getSampleSize(bulkIndex->getSampleFormat())),
    fileReader_(nullptr),
    bulksCount_(readers.size()),
    fileSize_(readers[0]->getFileSize()),
    traceIndexReader_(std::make_shared<fileio::BufferedFileReader>(indexReader)),
    bulkIndex_(bulkIndex),
    traceIndexer_(bulkIndex_->createTraceIndexer(traceIndexReader_))
{
    fileReader_ = std::make_shared<fileio::SectionFileReader>(std::move(readers));

    // set the default buffer size as twice as a trace size
    size_t const bufferSize = 2U * getTraceSize() * bulksCount_;
    fileReader_->setBufferSize(bufferSize);
}

size_t
SectionBulkReader::getBulksCount() const
{
    return bulksCount_;
}

void
SectionBulkReader::setReadOffsetMap(fileio::SectionFileReader::ReadOffsetMap&& readOffsetMap)
{
    fileReader_->setReadOffsetMap(std::move(readOffsetMap));
}

BulkIndex const&
SectionBulkReader::getBulkIndex() const
{
    return *bulkIndex_.get();
}

BulkIndex::TraceIndexer&
SectionBulkReader::getTraceIndexer() const
{
    return *traceIndexer_.get();
}

fileio::BufferedFileReaderBase&
SectionBulkReader::getTraceIndexReader() const
{
    return *traceIndexReader_.get();
}

fileio::BufferedFileReaderBase&
SectionBulkReader::getFileReader() const
{
    return *fileReader_.get();
}

void
SectionBulkReader::dropCachedData()
{
    fileReader_->dropCache();
}

void
SectionBulkReader::LoadDataToCache(size_t fileOffset, size_t dataSize)
{
    fileReader_->loadCache(fileio::CacheRequestParams{}.withOffset(fileOffset).withSize(dataSize));
}

std::optional<size_t>
SectionBulkReader::setReadOffsetHint(size_t inlineIndex, size_t xlineIndex, size_t sampleIndex)
{
    OptionalReadOffsetMapItem hint = getReadOffsetMapItem(inlineIndex, xlineIndex, sampleIndex);
    if (hint.has_value()) {
        fileReader_->addReadOffsetMapItem(std::move(hint.value()));
        return {hint.value()};
    }
    return {std::nullopt};
}

void
SectionBulkReader::clearReadOffsetHints()
{
    fileReader_->clearReadOffsetMap();
}

template<ReadDirection READ_DIRECTION>
void
SectionBulkReader::generateReadOffsetMap(size_t inlineIndex, size_t xlineIndex, size_t sampleIndex, size_t xCount)
{
    fileReader_->clearReadOffsetMap();

    if constexpr (READ_DIRECTION == ReadDirection::Inline) {
        // reading along the inline means changing the crossline numbers
        // while the inline one remains the same
        size_t const xlineNoMax = xlineIndex + xCount;
        for (size_t xlineNo = xlineIndex; xlineNo < xlineNoMax; ++xlineNo) {
            setReadOffsetHint(inlineIndex, xlineNo, sampleIndex);
        }
    }
    else if constexpr (READ_DIRECTION == ReadDirection::Crossline) {
        // reading along the crossline means changing the inline numbers
        // while the crossline one remains the same
        size_t const inlineNoMax = inlineIndex + xCount;
        for (size_t inlineNo = inlineIndex; inlineNo < inlineNoMax; ++inlineNo) {
            setReadOffsetHint(inlineNo, xlineIndex, sampleIndex);
        }
    }
    else {
        static_assert(READ_DIRECTION not_eq ReadDirection::Inline
                          and READ_DIRECTION not_eq ReadDirection::Crossline,
                      "Missing implementation: Unsupported readDirection value detected!");
    }
}

bool
SectionBulkReader::read(ReadDirection readDirection, size_t inlineIndex, size_t xlineIndex, size_t sampleIndex, size_t xCount, size_t sampleCount, float* buffer)
{
    switch (readDirection) {
    case ReadDirection::Inline:
        {
            generateReadOffsetMap<ReadDirection::Inline>(inlineIndex, xlineIndex, sampleIndex, xCount);

            // to read the section we specify (1U) as number of requested inlines
            return read_impl(inlineIndex, xlineIndex, sampleIndex, 1U, xCount, sampleCount, buffer);
        }
    case ReadDirection::Crossline:
        {
            generateReadOffsetMap<ReadDirection::Crossline>(inlineIndex, xlineIndex, sampleIndex, xCount);

            // to read the section we specify (1U) as number of requested crosslines
            return read_impl(inlineIndex, xlineIndex, sampleIndex, xCount, 1U, sampleCount, buffer);
        }
    } // switch(readDirection)

    throw segysdk::segyio::SegyFileIOException("Unsupported readDirection value detected!");
}

SectionBulkReader::OptionalReadOffsetMapItem
SectionBulkReader::getReadOffsetMapItem(size_t brickInlineNo, size_t brickCrossLineNo, size_t sampleNo) const
{
    BulkIndex::TraceIndexer& traceIndexer = getTraceIndexer();
    BulkIndex const& bulkIndex = getBulkIndex();

    size_t const inlinesCount = static_cast<size_t>(bulkIndex.getSizeI());
    size_t const crosslinesCount = static_cast<size_t>(bulkIndex.getSizeJ());
    size_t const samplesCount = static_cast<size_t>(bulkIndex.getSizeK());
    size_t const sampleOffset = sampleNo * Primitives::getSampleSize(bulkIndex.getSampleFormat());

    if (samplesCount <= sampleNo) {
        throw segysdk::segyio::SegyFileIOException("Requested sample is beyond the extents");
    }

    const bool swapAxis = bulkIndex.isCrosslineOrdered() && brickInlineNo > 1;
    if (swapAxis) {
        std::swap(brickInlineNo, brickCrossLineNo);
    }

    auto traceExists = [&inlinesCount, &crosslinesCount, &swapAxis, &traceIndexer](size_t inlineNo, size_t crosslineNo) -> bool {
        if (inlineNo >= inlinesCount || crosslineNo >= crosslinesCount) {
            return false; // skip items beyond the file limits
        }
        return swapAxis
                   ? traceIndexer.findTraceByIndex(crosslineNo, inlineNo)
                   : traceIndexer.findTraceByIndex(inlineNo, crosslineNo);
    };

    if (!traceExists(brickInlineNo, brickCrossLineNo)) {
        // return "no value"
        return OptionalReadOffsetMapItem{};
    }

    //
    // Generate the read offset map item starting from the trace's requested sample
    //
    fileio::SectionFileReader::ReadOffsetMap::value_type const offset = traceIndexer.getOffset() + sampleOffset;

    return OptionalReadOffsetMapItem::value_type{offset};
}

} // namespace segysdk::bulkaccess
