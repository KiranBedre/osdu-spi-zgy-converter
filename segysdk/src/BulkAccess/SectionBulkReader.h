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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../BulkAccess/BulkReaderBase.h"

#include "../FileIO/BufferedFileReader.h"
#include "../FileIO/BufferedFileReaderBase.h"
#include "../FileIO/SectionFileReader.h"

#include <memory>
#include <optional>
#include <vector>

namespace segysdk::bulkaccess
{

struct SEGYSDK_DLL_PUBLIC SectionBulkReader final : public BulkReaderBase
{
    using OptionalReadOffsetMapItem = std::optional<fileio::SectionFileReader::ReadOffsetMap::value_type>;

    SectionBulkReader(std::vector<std::shared_ptr<fileio::FileReader>>&& readers,
                      std::shared_ptr<fileio::FileReader> indexReader,
                      const std::shared_ptr<BulkIndex>& bulkIndex);

    size_t getBulksCount() const;

    void setReadOffsetMap(fileio::SectionFileReader::ReadOffsetMap&& readOffsetMap);
    OptionalReadOffsetMapItem getReadOffsetMapItem(size_t brickInlineNo, size_t brickCrossLineNo, size_t sampleNo) const;

private:
    SectionBulkReader(const SectionBulkReader& other) = delete;
    SectionBulkReader& operator=(const SectionBulkReader& other) = delete;

    virtual BulkIndex const& getBulkIndex() const override;
    virtual BulkIndex::TraceIndexer& getTraceIndexer() const override;
    virtual fileio::BufferedFileReaderBase& getTraceIndexReader() const override;
    virtual fileio::BufferedFileReaderBase& getFileReader() const override;
    virtual void dropCachedData() override;
    virtual void LoadDataToCache(size_t fileOffset, size_t dataSize) override;
    virtual std::optional<size_t> setReadOffsetHint(size_t inlineIndex, size_t xlineIndex, size_t sampleIndex) override;
    virtual void clearReadOffsetHints() override;
    virtual bool read(ReadDirection readDirection, size_t inlineIndex, size_t xlineIndex, size_t sampleIndex, size_t xCount, size_t sampleCount, float* buffer) override;

    template<ReadDirection READ_DIRECTION>
    void generateReadOffsetMap(size_t inlineIndex, size_t xlineIndex, size_t sampleIndex, size_t xCount);

private:
    std::shared_ptr<fileio::SectionFileReader> fileReader_;
    size_t const bulksCount_;
    size_t const fileSize_;
    std::shared_ptr<fileio::BufferedFileReader> traceIndexReader_;
    std::shared_ptr<BulkIndex> bulkIndex_;
    std::shared_ptr<BulkIndex::TraceIndexer> traceIndexer_;
};

} // namespace segysdk::bulkaccess
