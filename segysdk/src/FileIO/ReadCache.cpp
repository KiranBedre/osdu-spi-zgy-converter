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

#include "../FileIO/ReadCache.h"

#include "../FileIO/FileIOException.h"

#include <algorithm>
#include <future>

namespace segysdk::fileio
{

ReadCache::ReadCache(const CacheSizeOption& cacheSize)
  : tick_counter_(0), cacheSize_(cacheSize), cacheMutex_(), pages_()
{
    if (cacheSize.pageSize() <= 0 || cacheSize.pages() <= 0)
        throw FileIOException("Missing or invalid SD read cache settings.");
}

size_t
ReadCache::pageSize() const
{
    return cacheSize_.pageSize();
}

static ByteBuffer
readBlock(FileReader* fileReader, const FileReadParams& params)
{
    return fileReader->read(params);
}

ByteBuffer
ReadCache::asyncReadBlocks(const std::size_t pageOffset, FileReaders& fileReaders, const request::Params& params) const
{
    auto readTasks = std::vector<std::future<ByteBuffer>>{};
    for (std::size_t i = 0; i < cacheSize_.blocksPerPage(); ++i) {
        const auto blockOffset = i * cacheSize_.blockSize();
        auto task = std::async(
            std::launch::async,
            readBlock,
            fileReaders[i].get(),
            FileReadParams{params}.withOffset(pageOffset + blockOffset).withSize(cacheSize_.blockSize()));
        readTasks.push_back(std::move(task));
    }
    auto result = ByteBuffer{};
    for (auto& task : readTasks) {
        task.wait();
        appendContainer(result, task.get());
    }
    return result;
}

ByteBuffer
ReadCache::read(const ReadCacheParams& params, FileReaders& readers)
{
    const auto firstPageIndex = params.offset_ / cacheSize_.pageSize();
    const auto lastPageIndex = (params.offset_ + params.size_ - 1) / cacheSize_.pageSize();
    auto result = ByteBuffer{};
    bool keepReading = true;
    for (auto pageIndex = firstPageIndex; (pageIndex <= lastPageIndex) && keepReading; ++pageIndex) {
        cacheMutex_.lock();
        dropExcessPages();

        const auto pageOffset = pageIndex * cacheSize_.pageSize();
        auto key = CacheKey{params.segyFileNumber_, pageIndex};
        bool isReadingRequired = false;

        auto it = pages_.find(key);
        if (it == pages_.end()) {
            pages_.try_emplace(key);
            isReadingRequired = true;
        }
        auto& page = pages_[key];
        page.increaseUsage();
        cacheMutex_.unlock();

        if (isReadingRequired) {
            auto bufferRead = (cacheSize_.blocksPerPage() > 1)
                                  ? asyncReadBlocks(pageOffset, readers, params)
                                  : readers[0]->read(FileReadParams{request::Params{params}}.withOffset(pageOffset).withSize(cacheSize_.pageSize()));
            page.setBuffer(std::move(bufferRead));
        }

        cacheMutex_.lock();
        page.wait_ready();
        const auto pageBufferOffset = pageOffset > params.offset_ ? 0 : params.offset_ - pageOffset;
        const auto bytesCopied = appendContainer(result, page.getBytes(pageBufferOffset, params.size_ - result.size()));
        if (bytesCopied > 0) {
            page.markHit(tick_counter_++);
        }
        else {
            keepReading = false;
        }
        page.decreaseUsage();
        cacheMutex_.unlock();
    }
    return result;
}

CachePage::CachePage()
  : isReady_(false), pageMutex_(), isReadyConditionalVariable_(), buffer_(0), lastHitMark_(0), usageCounter_(0)
{
}

void
CachePage::setBuffer(ByteBuffer&& buffer)
{
    std::unique_lock<std::mutex> lock(pageMutex_);
    buffer_ = std::move(buffer);
    isReady_ = true;
    isReadyConditionalVariable_.notify_all();
}

void
CachePage::dropBuffer()
{
    std::unique_lock<std::mutex> lock(pageMutex_);
    buffer_.clear();
    isReady_ = false;
}

void
CachePage::wait_ready()
{
    std::unique_lock<std::mutex> lock(pageMutex_);
    isReadyConditionalVariable_.wait(lock, [this] { return isReady_; });
}

ByteBuffer
CachePage::getBytes(std::size_t start, std::size_t count) const
{
    std::unique_lock<std::mutex> lock(pageMutex_);
    start = std::min(start, buffer_.size());
    count = std::min((buffer_.size() - start), count);
    return {std::next(std::begin(buffer_), start), std::next(std::begin(buffer_), start + count)};
}

void
CachePage::markHit(const std::uint64_t mark)
{
    std::unique_lock<std::mutex> lock(pageMutex_);
    lastHitMark_ = mark;
}

void
ReadCache::dropExcessPages()
{
    auto remainingPages = pages_.size();
    while (pages_.size() > cacheSize_.pages() && remainingPages-- > 0) {
        const auto pageToDrop = std::min_element(
            pages_.begin(), pages_.end(),
            [](const auto& page1, const auto& page2) { return page1.second.isOlderThan(page2.second); });
        if (!pageToDrop->second.isInUse()) {
            pageToDrop->second.dropBuffer();
            pages_.erase(pageToDrop);
        }
    }
}

bool
CachePage::isOlderThan(const CachePage& other) const
{
    return lastHitMark_ < other.lastHitMark_ && usageCounter_ == 0;
}

ReadCacheParams&
ReadCacheParams::withSegyFileNumber(const std::uint16_t value)
{
    segyFileNumber_ = value;
    return *this;
}

} // namespace segysdk::fileio
