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

#include "../Concurrent/ThreadPool.h"
#include "../FileIO/FileReader.h"
#include "../FileIO/PipelineFileReaderBase.h"
#include "../FileIO/Utilities/ReaderUnit.h"

#include <cstdint>
#include <deque>
#include <memory>
#include <mutex>
#include <vector>

namespace segysdk::fileio
{

class SEGYSDK_DLL_PUBLIC SectionFileReader final : public PipelineFileReaderBase
{
public:
    using ReadOffsetMapItem = size_t;
    using ReadOffsetMap = std::deque<ReadOffsetMapItem>;
    using ReaderUnitVector = PipelineFileReaderBase::ReaderUnitVector;

public:
    SectionFileReader(std::vector<std::shared_ptr<fileio::FileReader>>&& files);
    virtual ~SectionFileReader() override;

    virtual void setBufferSize(size_t bufferSize) override;
    virtual void setReadSize(size_t readSize) override;
    virtual void loadCache(const CacheRequestParams& params) override;

    void clearReadOffsetMap();
    void setReadOffsetMap(ReadOffsetMap&& readOffsetMap);
    void addReadOffsetMapItem(ReadOffsetMapItem&& item);

private:
    SectionFileReader(const SectionFileReader&) = delete;
    SectionFileReader(SectionFileReader&&) noexcept = delete;
    SectionFileReader& operator=(const SectionFileReader&) = delete;
    SectionFileReader& operator=(SectionFileReader&&) = delete;

    size_t getOffsetPerReader() const;
    bool requestNextChunk(size_t chunkOffset, details::RequestedCacheItem& chunkRequest);
    ReaderUnitVector::iterator selectReader(size_t offset, size_t size);
    uint8_t const* doReadAligned(const FileReadParams& params) override;
    size_t alignMultiToBoundary(size_t readersCount, size_t size) const;

private:
    std::mutex readOffsetMapMutex_;
    ReadOffsetMap readOffsetMap_{};
    size_t readOffsetMapIndex_{0U};
};

} // namespace segysdk::fileio
