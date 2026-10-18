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

#include "../FileIO/Utilities/ReaderUnit.h"

namespace segysdk::fileio::details
{

ReaderUnit::ReaderUnit(ReaderPtr&& reader, size_t sleepTimeout)
  : sleepTimeout_{sleepTimeout},
    emptyCounter_{0U, sleepTimeout_}
{
    reader_.swap(reader);

    initStateVariables();
}

ReaderUnit::ReaderUnit(ReaderUnit&& other)
  : sleepTimeout_{other.sleepTimeout_},
    emptyCounter_{0U, sleepTimeout_}
{
    *this = std::move(other);
}

ReaderUnit&
ReaderUnit::operator=(ReaderUnit&& other)
{
    // swapping of the readers shall be in sync

    std::unique_lock<std::mutex> lockOther(other.busyMutex_);
    std::unique_lock<std::mutex> lockMine(busyMutex_);

    reader_.swap(other.reader_);

    isBusy_ = other.isBusy_.load();
    requestedCacheOffset_ = other.requestedCacheOffset_.load();
    requestedCacheSize_ = other.requestedCacheSize_.load();

    return *this;
}

bool
ReaderUnit::isBusy() const
{
    // the reader business state is an atomic type
    // so reading is safe at any moment
    return isBusy_;
}

void
ReaderUnit::setIsBusy(bool busyness)
{
    // change the reader business state shall be in sync
    std::unique_lock<std::mutex> lock(busyMutex_);
    isBusy_ = busyness;
}

size_t
ReaderUnit::getFirstCachedOffset() const
{
    std::unique_lock<std::mutex> lock(busyMutex_);
    return reader_.get()->getFirstCachedOffset();
}

uint64_t
ReaderUnit::getFileSize() const
{
    // no sync is used: file size is unchanged during the reader life time
    return reader_.get()->getFileSize();
}

std::string const&
ReaderUnit::getFileName() const
{
    // no sync is used: file name is unchanged during the reader life time
    return reader_.get()->getFileName();
}

uint8_t const*
ReaderUnit::readAligned(size_t offset, size_t size, size_t alignment)
{
    std::unique_lock<std::mutex> lock(busyMutex_);
    return reader_.get()->readAligned(offset, size, alignment);
}

uint8_t const*
ReaderUnit::getPointerToCached(uint64_t offset, size_t size) const
{
    std::unique_lock<std::mutex> lock(busyMutex_);
    return reader_.get()->getPointerToCached(offset, size);
}

void
ReaderUnit::dropCache()
{
    std::unique_lock<std::mutex> lock(busyMutex_);
    return reader_.get()->dropCache();
}

void
ReaderUnit::notifyReader(size_t offset, size_t size)
{
    std::unique_lock<std::mutex> lock(busyMutex_);

    // set the busy flag prior the cache loading
    isBusy_ = true;

    // set the cache parameters
    requestedCacheOffset_ = offset;
    requestedCacheSize_ = size;

    // trigger the prefetch thread to get the requested params
    wakeUp();
}

void
ReaderUnit::loadReaderCache(const request::Params& params)
{
    std::unique_lock<std::mutex> lock(busyMutex_);

    // we need to set the internal data size to zero first
    // to prevent the race condition
    reader_.get()->dropCache();

    // load the cache synchronously
    reader_.get()->loadCache(CacheRequestParams{params}.withOffset(requestedCacheOffset_).withSize(requestedCacheSize_));

    // clear the busy flag right after the cache loading
    isBusy_ = false;
}

size_t
ReaderUnit::getCachedSize() const
{
    std::unique_lock<std::mutex> lock(busyMutex_);
    return reader_.get()->getCachedSize();
}

size_t
ReaderUnit::getRequestedCacheOffset() const
{
    return requestedCacheOffset_;
}

size_t
ReaderUnit::getRequestedCacheSize() const
{
    return requestedCacheSize_;
}

bool
ReaderUnit::isCacheHit(size_t offset, size_t size) const
{
    std::unique_lock<std::mutex> lock(busyMutex_);
    return isRequestedCacheHit(offset, size);
}

void
ReaderUnit::setBufferSize(size_t bufferSize)
{
    std::unique_lock<std::mutex> lock(busyMutex_);
    return reader_.get()->setBufferSize(bufferSize);
}

void
ReaderUnit::setReadSize(size_t readSize)
{
    std::unique_lock<std::mutex> lock(busyMutex_);
    return reader_.get()->setReadSize(readSize);
}

void
ReaderUnit::wakeUp()
{
    emptyCounter_.Increase();
}

bool
ReaderUnit::sleep()
{
    return emptyCounter_.DecreaseAndWaitWhenZero();
}

void
ReaderUnit::initStateVariables()
{
    isBusy_ = false;
    requestedCacheOffset_ = 0U;
    requestedCacheSize_ = 0U;
}

bool
ReaderUnit::isRequestedCacheHit(size_t offset, size_t size) const
{
    return (requestedCacheOffset_ <= offset)
           && ((offset + size) <= (requestedCacheOffset_ + requestedCacheSize_));
}

} // namespace segysdk::fileio::details
