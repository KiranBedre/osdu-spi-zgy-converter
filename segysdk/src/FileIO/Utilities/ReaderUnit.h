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

#include "../Concurrent/BufferQueue.h"
#include "../FileIO/BufferedFileReaderBase.h"

#include <atomic>
#include <cstdint>
#include <memory>
#include <mutex>
#include <string>
#include <utility>

namespace segysdk::fileio::details
{
using SemaphoreCounter = segysdk::concurrent::details::SemaphoreCounter;

struct SEGYSDK_DLL_PUBLIC ReaderUnit
{
    using ReaderPtr = std::shared_ptr<BufferedFileReaderBase>;

    ReaderUnit(ReaderPtr&& reader, size_t sleepTimeout);

    ReaderUnit(ReaderUnit&& other);
    ReaderUnit& operator=(ReaderUnit&& other);

    bool isBusy() const;
    void setIsBusy(bool busyness);
    size_t getFirstCachedOffset() const;
    uint64_t getFileSize() const;
    std::string const& getFileName() const;
    uint8_t const* readAligned(size_t offset, size_t size, size_t alignment);
    uint8_t const* getPointerToCached(uint64_t offset, size_t size) const;
    void dropCache();
    void notifyReader(size_t offset, size_t size);
    void loadReaderCache(const request::Params& params);
    size_t getCachedSize() const;
    size_t getRequestedCacheOffset() const;
    size_t getRequestedCacheSize() const;
    bool isCacheHit(size_t offset, size_t size) const;
    void setBufferSize(size_t bufferSize);
    void setReadSize(size_t readSize);
    void wakeUp();
    bool sleep();

private:
    ReaderUnit(ReaderUnit const& other) = delete;
    ReaderUnit& operator=(ReaderUnit const& other) = delete;

    void initStateVariables();
    bool isRequestedCacheHit(size_t offset, size_t size) const;

private:
    std::atomic_bool isBusy_{false};
    ReaderPtr reader_{nullptr};
    std::atomic_size_t requestedCacheOffset_{0U};
    std::atomic_size_t requestedCacheSize_{0U};

    // any operation with the reader might potentially cause some race conditions
    // so let us protect them using the lock section.
    // here the 'mutable' keyword is used because the reader is accessed
    // via const- and non-const- iterators based on the context
    mutable std::mutex busyMutex_;

    // some barriers are necessary to get control over or trigger the reader cache loading
    // so let us protect them using the semaphore
    size_t const sleepTimeout_;
    SemaphoreCounter emptyCounter_;
};

} // namespace segysdk::fileio::details
