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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../BulkAccess/BulkReaderBase.h"

#include "../FileIO/BufferedFileReader.h"
#include "../FileIO/BufferedFileReaderBase.h"
#include "../FileIO/MultiBufferedFileReader.h"

#include <memory>
#include <optional>
#include <vector>

namespace segysdk::bulkaccess
{
struct SEGYSDK_DLL_PUBLIC MultiBulkReader final : public BulkReaderBase
{
    using OptionalReadOffsetMapItem = std::optional<fileio::MultiBufferedFileReader::ReadOffsetMap::value_type>;

    MultiBulkReader(std::vector<std::shared_ptr<fileio::FileReader>>&& readers,
                    size_t numberOfTraces,
                    const std::shared_ptr<BulkIndex>& bulkIndex);

    size_t getBulksCount() const;

    void setReadOffsetMap(fileio::MultiBufferedFileReader::ReadOffsetMap&& readOffsetMap);

    OptionalReadOffsetMapItem getReadOffsetMapItem(size_t brickInlineNo, size_t brickCrossLineNo) const;

private:
    MultiBulkReader(const MultiBulkReader& other) = delete;
    MultiBulkReader& operator=(const MultiBulkReader& other) = delete;

    virtual BulkIndex const& getBulkIndex() const override;
    virtual BulkIndex::TraceIndexer& getTraceIndexer() const override;
    virtual fileio::BufferedFileReaderBase& getTraceIndexReader() const override;
    virtual fileio::BufferedFileReaderBase& getFileReader() const override;
    virtual void dropCachedData() override;
    virtual void LoadDataToCache(size_t fileOffset, size_t dataSize) override;
    virtual std::optional<size_t> setReadOffsetHint(size_t inlineIndex, size_t xlineIndex, size_t sampleIndex) override;
    virtual void clearReadOffsetHints() override;
    virtual bool read(size_t i0, size_t j0, size_t k0, size_t ni, size_t nj, size_t nk, float* buffer) override;
    virtual bool read_trace_header_data(size_t i0, size_t j0, size_t ni, size_t nj, const std::vector<segyformat::external::TraceHeaderField>& headers_list, std::vector<std::vector<TraceHeaderResult>>& result) override;
    virtual void set_inline_read_increment(size_t value) override;

private:
    std::shared_ptr<fileio::MultiBufferedFileReader> fileReader_;
    size_t const bulksCount_;
    size_t const numberOfTraces_;
    size_t const fileSize_;
    std::shared_ptr<BulkIndex> bulkIndex_{nullptr};
    std::shared_ptr<BulkIndex::TraceIndexer> traceIndexer_{nullptr};
};

} // namespace segysdk::bulkaccess
