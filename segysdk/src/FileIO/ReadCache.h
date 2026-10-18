/*
Copyright 2023 Schlumberger

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

#include "../FileIO/CacheSizeOption.h"
#include "../FileIO/FileReader.h"

#include <map>
#include <mutex>
#include <condition_variable>
#include <vector>

namespace segysdk::fileio
{

using FileReaders = std::vector<std::shared_ptr<fileio::FileReader>>;
using CacheKey = std::pair<std::uint16_t, std::uint64_t>;

class CachePage
{
public:
    explicit CachePage();
    ByteBuffer getBytes(std::size_t start, std::size_t count) const;
    void markHit(std::uint64_t mark);
    bool isOlderThan(const CachePage& other) const;
    void setBuffer(ByteBuffer&& buffer);
    void dropBuffer();
    void wait_ready();
    bool isReady()
    {
        std::unique_lock<std::mutex> lock(pageMutex_);
        return isReady_;
    }

    bool isInUse() {
        std::unique_lock<std::mutex> lock(pageMutex_);
        return usageCounter_ > 0;
    }

    void increaseUsage()
    {
        std::unique_lock<std::mutex> lock(pageMutex_);
        usageCounter_++;
    }

    void decreaseUsage()
    {
        std::unique_lock<std::mutex> lock(pageMutex_);
        usageCounter_--;
    }

private:
    bool isReady_;
    mutable std::mutex pageMutex_;
    std::condition_variable isReadyConditionalVariable_;
    ByteBuffer buffer_;
    std::uint64_t lastHitMark_;
    size_t usageCounter_;
};

struct SEGYSDK_DLL_PUBLIC ReadCacheParams : public FileReadParams
{
    ReadCacheParams& withSegyFileNumber(const std::uint16_t value);
    std::uint16_t segyFileNumber_{};
};

class SEGYSDK_DLL_PUBLIC ReadCache final
{
public:
    explicit ReadCache(const CacheSizeOption& cacheSize);
    ReadCache(const ReadCache&) = delete;
    ReadCache(ReadCache&&) = delete;
    ReadCache& operator=(const ReadCache&) = delete;
    ReadCache& operator=(ReadCache&&) = delete;

    size_t pageSize() const;
    ByteBuffer read(const ReadCacheParams& params, FileReaders& readers);

    bool isMultiThread() const { return cacheSize_.blocksPerPage() > 1; }
    size_t threadCount() const { return cacheSize_.blocksPerPage(); }

private:
    uint64_t tick_counter_;
    const CacheSizeOption cacheSize_;

    void dropExcessPages();
    ByteBuffer asyncReadBlocks(const std::size_t pageOffset, FileReaders& fileReaders, const request::Params& params) const;
    CachePage& getPage(const CacheKey& key, std::uint64_t pageOffset, FileReaders& fileReaders);

    std::recursive_mutex cacheMutex_;
    std::map<CacheKey, CachePage> pages_;
};

} // namespace segysdk::fileio
