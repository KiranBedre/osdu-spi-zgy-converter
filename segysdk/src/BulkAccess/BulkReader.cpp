/*
Copyright 2021 Schlumberger

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

#include "../BulkAccess/BulkReader.h"

#include "../BulkAccess/BulkIndex.h"
#include "../SegyIO/Primitives.h"
#include "../SegyIO/SegyIOException.h"
#include "../SegyIO/ValueConversion/SampleValueConverter.h"
#include "../Logger/Logger.h"
#include <algorithm>
#include <cstring>
#include <stdexcept>
#include <string>

namespace segysdk::bulkaccess
{
using segyio::Primitives;

BulkReader::BulkReader(const std::shared_ptr<fileio::FileReader>& file,
                       const std::shared_ptr<fileio::FileReader>& indexFile,
                       const std::shared_ptr<BulkIndex>& bulkIndex)
  : BulkReaderBase(Primitives::getSampleSize(bulkIndex->getSampleFormat())),
    fileReader_(std::make_shared<fileio::BufferedFileReader>(file)),
    traceIndexReader_(std::make_shared<fileio::BufferedFileReader>(indexFile)),
    bulkIndex_(bulkIndex),
    traceIndexer_(bulkIndex_->createTraceIndexer(traceIndexReader_))
{
    fileReader_->setBufferSize(defaultBufferSize_);
}

BulkIndex const&
BulkReader::getBulkIndex() const
{
    return *bulkIndex_.get();
}

BulkIndex::TraceIndexer&
BulkReader::getTraceIndexer() const
{
    return *traceIndexer_.get();
}

fileio::BufferedFileReaderBase&
BulkReader::getTraceIndexReader() const
{
    return *traceIndexReader_.get();
}

fileio::BufferedFileReaderBase&
BulkReader::getFileReader() const
{
    return *fileReader_.get();
}

void
BulkReader::dropCachedData()
{
    fileReader_->dropCache();
}

void
BulkReader::LoadDataToCache(size_t fileOffset, size_t dataSize)
{
    fileReader_->loadCache(fileio::CacheRequestParams{}.withOffset(fileOffset).withSize(dataSize));
}

std::optional<size_t>
BulkReader::setReadOffsetHint(size_t inlineIndex, size_t xlineIndex, size_t sampleIndex)
{
    (void)inlineIndex;
    (void)xlineIndex;
    (void)sampleIndex;

    /* DO NOTHING */

    return {std::nullopt};
}

void
BulkReader::clearReadOffsetHints()
{
    /* DO NOTHING */
}

bool
BulkReader::read(size_t i0, size_t j0, size_t k0, size_t ni, size_t nj, size_t nk, float* buffer)
{
    try {

        return read_impl(i0, j0, k0, ni, nj, nk, buffer);
    }
    catch (std::exception& e) {
        const std::string msg = "BulkReader::read: Exception caught: " + std::string(e.what());
        SEGYLOG_REPORT_ERROR(msg);
        throw;
    }
}

bool
BulkReader::read_trace_header_data(size_t i0, size_t j0, size_t ni, size_t nj, const std::vector<segyformat::external::TraceHeaderField>& headers_list, std::vector<std::vector<TraceHeaderResult>>& result)
{
    try {
        return read_trace_header_data_impl(i0, j0, ni, nj, headers_list, result);
    }
    catch (std::exception& e) {
        const std::string msg = "BulkReader::read_trace_header_data: Exception caught: " + std::string(e.what());
        SEGYLOG_REPORT_ERROR(msg);
        throw;
    }
}

void
segysdk::bulkaccess::BulkReader::set_inline_read_increment(size_t value)
{
    set_inline_read_increment_impl(value);
}

} // namespace segysdk::bulkaccess
