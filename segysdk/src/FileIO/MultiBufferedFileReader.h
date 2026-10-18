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

#include "../FileIO/BufferedFileReaderBase.h"
#include "../FileIO/FileReader.h"

#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <vector>

namespace segysdk::fileio
{

class SEGYSDK_DLL_PUBLIC MultiBufferedFileReader final : public BufferedFileReaderBase
{
public:
    using ReadOffsetMapItem = std::pair<size_t, std::vector<size_t>>;
    using ReadOffsetMap = std::map<ReadOffsetMapItem::first_type, ReadOffsetMapItem::second_type>;

public:
    static constexpr size_t const minReadersCount = 1U;

public:
    MultiBufferedFileReader(std::vector<std::shared_ptr<fileio::FileReader>>&& files);

    virtual size_t getBufferSize() const override;
    virtual size_t getReadSize() const override;
    virtual uint64_t getFileSize() const override;
    virtual size_t getReadersCount() const override;
    virtual std::string const& getFileName() const override;
    virtual void setBufferSize(size_t bufferSize) override;
    virtual void setReadSize(size_t readSize) override;
    virtual bool isCacheHit(size_t offset, size_t size) const override;
    virtual void loadCache(const CacheRequestParams& params) override;
    virtual void dropCache() override;
    virtual uint8_t const* getPointerToCached(uint64_t offset, size_t size) const override;
    virtual size_t getFirstCachedOffset() const override;
    virtual size_t getCachedSize() const override;

    void clearReadOffsetMap();
    void setReadOffsetMap(ReadOffsetMap&& readOffsetMap);
    void addReadOffsetMapItem(ReadOffsetMapItem&& item);

private:
    MultiBufferedFileReader(const MultiBufferedFileReader&) = delete;
    MultiBufferedFileReader(MultiBufferedFileReader&&) noexcept = delete;
    MultiBufferedFileReader& operator=(const MultiBufferedFileReader&) = delete;
    MultiBufferedFileReader& operator=(MultiBufferedFileReader&&) = delete;

    size_t alignMultiToBoundary(size_t readersCount, size_t size) const;
    uint8_t const* doReadAligned(const FileReadParams& params) override;

private:
    std::vector<std::shared_ptr<BufferedFileReaderBase>> readers_;
    std::mutex readOffsetMapMutex_;
    ReadOffsetMap readOffsetMap_{};

    size_t totalBufferSize_{};
    size_t totalReadSize_{};
    size_t readerBufferSize_{};
    size_t readerReadSize_{};
};

} // namespace segysdk::fileio
