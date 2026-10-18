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

#include "../BulkAccess/MultiBulkReader.h"

#include "../SegyIO/Primitives.h"
#include "../SegyIO/SegyIOException.h"
#include "../SegyIndex/SegyIndex.h"

#include <algorithm>

namespace segysdk::bulkaccess
{
using segyio::Primitives;

MultiBulkReader::MultiBulkReader(std::vector<std::shared_ptr<fileio::FileReader>>&& readers,
                                 size_t numberOfTraces,
                                 const std::shared_ptr<BulkIndex>& bulkIndex)
  : BulkReaderBase(Primitives::getSampleSize(bulkIndex->getSampleFormat())),
    fileReader_(nullptr),
    bulksCount_(readers.size()),
    numberOfTraces_(numberOfTraces),
    fileSize_(readers[0]->getFileSize()),
    bulkIndex_(bulkIndex)
{
    fileReader_ = std::make_shared<fileio::MultiBufferedFileReader>(std::move(readers));
    fileReader_->setBufferSize(defaultBufferSize_);

    traceIndexer_ = bulkIndex_->createTraceIndexer(fileReader_);
}

size_t
MultiBulkReader::getBulksCount() const
{
    return bulksCount_;
}

void
MultiBulkReader::setReadOffsetMap(fileio::MultiBufferedFileReader::ReadOffsetMap&& readOffsetMap)
{
    fileReader_->setReadOffsetMap(std::move(readOffsetMap));
}

BulkIndex const&
MultiBulkReader::getBulkIndex() const
{
    return *bulkIndex_.get();
}

BulkIndex::TraceIndexer&
MultiBulkReader::getTraceIndexer() const
{
    return *traceIndexer_.get();
}

fileio::BufferedFileReaderBase&
MultiBulkReader::getTraceIndexReader() const
{
    return *fileReader_.get();
}

fileio::BufferedFileReaderBase&
MultiBulkReader::getFileReader() const
{
    return *fileReader_.get();
}

void
MultiBulkReader::dropCachedData()
{
    fileReader_->dropCache();
}

void
MultiBulkReader::LoadDataToCache(size_t fileOffset, size_t dataSize)
{
    fileReader_->loadCache(fileio::CacheRequestParams{}.withOffset(fileOffset).withSize(dataSize));
}

std::optional<size_t>
MultiBulkReader::setReadOffsetHint(size_t inlineIndex, size_t xlineIndex, [[maybe_unused]] size_t sampleIndex)
{
    OptionalReadOffsetMapItem mapItem = getReadOffsetMapItem(inlineIndex, xlineIndex);
    if (mapItem.has_value()) {
        fileReader_->addReadOffsetMapItem(std::move(mapItem.value()));
        return {mapItem.value().first};
    }
    return {std::nullopt};
}

void
MultiBulkReader::clearReadOffsetHints()
{
    fileReader_->clearReadOffsetMap();
}

bool
MultiBulkReader::read(size_t i0, size_t j0, size_t k0, size_t ni, size_t nj, size_t nk, float* buffer)
{
    return read_impl(i0, j0, k0, ni, nj, nk, buffer);
}

bool
MultiBulkReader::read_trace_header_data(size_t i0, size_t j0, size_t ni, size_t nj, const std::vector<segyformat::external::TraceHeaderField>& headers_list, std::vector<std::vector<TraceHeaderResult>>& result)
{
    return read_trace_header_data_impl(i0, j0, ni, nj, headers_list, result);
}

void
MultiBulkReader::set_inline_read_increment(size_t value)
{
    (void)value;
    /* Do nothing, Not supported here */
}

MultiBulkReader::OptionalReadOffsetMapItem
MultiBulkReader::getReadOffsetMapItem(size_t brickInlineNo, size_t brickCrossLineNo) const
{
    BulkIndex::TraceIndexer& traceIndexer = getTraceIndexer();
    BulkIndex const& bulkIndex = getBulkIndex();

    size_t const inlinesCount = static_cast<size_t>(bulkIndex.getSizeI());
    size_t const crosslinesCount = static_cast<size_t>(bulkIndex.getSizeJ());

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

    //
    // Scan for the brick's first existing trace
    //
    bool brickContainsTrace{};
    size_t brickFirstTraceOffsetInFile{}; // file offset of the brick column first existing trace
    size_t brickFirstInlineTrace{};       // inline coordinate of the brick column first existing trace

    // The scan is going along each inline to find the brick's first existing trace
    for (size_t line_offset{}; (line_offset < bulksCount_) && !brickContainsTrace; ++line_offset) {
        for (size_t trace_offset{}; (trace_offset < numberOfTraces_) && !brickContainsTrace; ++trace_offset) {
            size_t const inlineToCheck = line_offset + brickInlineNo;
            size_t const crosslineToCheck = trace_offset + brickCrossLineNo;
            if (traceExists(inlineToCheck, crosslineToCheck)) {
                brickContainsTrace = true;
                brickFirstTraceOffsetInFile = traceIndexer.getOffset();
                brickFirstInlineTrace = inlineToCheck;
            }
        } // for (size_t trace_offset
    } // for (size_t line_offset

    if (!brickContainsTrace) {
        // absent value means the particular brick column does not contain any trace
        return OptionalReadOffsetMapItem{};
    }

    if (brickFirstTraceOffsetInFile > Primitives::getTraceHeaderSize()) {
        // adjust the first offset by adding the trace headers' contents
        brickFirstTraceOffsetInFile -= Primitives::getTraceHeaderSize();
    }

    //
    // Generate the read offset map item starting from the brick's first existing trace
    //
    fileio::MultiBufferedFileReader::ReadOffsetMap::value_type::second_type offsetPerBuffer(bulksCount_, brickFirstTraceOffsetInFile);

    // The scan is going along each inline to find the brick's existing trace per reader
    for (size_t readerNo = 1; readerNo < bulksCount_; ++readerNo) {
        for (size_t trace_offset{}; trace_offset < numberOfTraces_; ++trace_offset) {
            size_t const inlineToCheck = readerNo + brickFirstInlineTrace;
            size_t const crosslineToCheck = trace_offset + brickCrossLineNo;
            if (traceExists(inlineToCheck, crosslineToCheck)) {
                size_t readerFileOffset = traceIndexer.getOffset();
                if (fileSize_ > readerFileOffset) {
                    if (readerFileOffset > Primitives::getTraceHeaderSize()) {
                        // adjust the offset by adding the trace headers' contents
                        readerFileOffset -= Primitives::getTraceHeaderSize();
                    }
                    offsetPerBuffer[readerNo] = readerFileOffset;
                }
                break;
            }
        } // for (size_t trace_offset
    } // for (size_t readerNo

    return OptionalReadOffsetMapItem::value_type{brickFirstTraceOffsetInFile, std::move(offsetPerBuffer)};
}

} // namespace segysdk::bulkaccess
