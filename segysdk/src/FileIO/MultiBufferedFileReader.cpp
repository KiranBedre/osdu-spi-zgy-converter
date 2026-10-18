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

#include "../FileIO/MultiBufferedFileReader.h"

#include "../FileIO/BufferedFileReader.h"
#include "../FileIO/FileIOException.h"
#include "../FileIO/FileReader.h"
#include "../Logger/Logger.h"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <numeric>
#include <thread>
#include <utility>

namespace segysdk::fileio
{

MultiBufferedFileReader::MultiBufferedFileReader(std::vector<std::shared_ptr<fileio::FileReader>>&& files)
  : readers_(files.size(), nullptr)
{
    if (readers_.size() < MultiBufferedFileReader::minReadersCount) {
        throw FileIOException("Error: MultiBufferedFileReader has got too few readers");
    }

    std::transform(files.cbegin(), files.cend(), readers_.begin(),
                   [](auto const& file) {
                       return std::make_shared<BufferedFileReader>(file);
                   });
}

size_t
MultiBufferedFileReader::getBufferSize() const
{
    return totalBufferSize_;
}

size_t
MultiBufferedFileReader::getReadSize() const
{
    return totalReadSize_;
}

void
MultiBufferedFileReader::setBufferSize(size_t size)
{
    // Expected size is total read cache for all the readers!
    // Align up to the closest read chunk-size boundary
    //
    size_t const newReaderBufferSize = alignMultiToBoundary(readers_.size(), size);
    if (newReaderBufferSize != readerBufferSize_) {
        readerBufferSize_ = newReaderBufferSize;
        readerReadSize_ = newReaderBufferSize;

        std::for_each(readers_.begin(), readers_.end(),
                      [this](auto& reader) {
                          reader->setBufferSize(readerBufferSize_);
                      });

        totalBufferSize_ = readerBufferSize_ * readers_.size();
        totalReadSize_ = readerReadSize_ * readers_.size();
    }
}

void
MultiBufferedFileReader::setReadSize(size_t size)
{
    size_t const alignedReaderSize = alignMultiToBoundary(readers_.size(), size);
    if (alignedReaderSize > readerBufferSize_) {
        setBufferSize(size);
    }
    else {
        readerReadSize_ = alignedReaderSize;
        std::for_each(readers_.begin(), readers_.end(),
                      [this](auto& reader) {
                          reader->setReadSize(readerReadSize_);
                      });
        totalReadSize_ = readerReadSize_ * readers_.size();
    }
}

uint64_t
MultiBufferedFileReader::getFileSize() const
{
    return readers_[0]->getFileSize();
}

size_t
MultiBufferedFileReader::getReadersCount() const
{
    return readers_.size();
}

std::string const&
MultiBufferedFileReader::getFileName() const
{
    return readers_[0]->getFileName();
}

uint8_t const*
MultiBufferedFileReader::doReadAligned(const FileReadParams& params)
{
    auto found = std::find_if(readers_.cbegin(), readers_.cend(),
                              [&params](auto const& reader) {
                                  return reader->isCacheHit(params.offset_, params.size_);
                              });

    if (readers_.cend() == found) {
        loadCache(params);

        // perform the double search over the just updated cache
        found = std::find_if(readers_.cbegin(), readers_.cend(),
                             [&params](auto const& reader) {
                                 return reader->isCacheHit(params.offset_, params.size_);
                             });
    }

    if (readers_.cend() == found) {
        throw FileIOException("Error: cache hit is false while readAligned");
    }

    return (*found)->readAligned(params);
}

bool
MultiBufferedFileReader::isCacheHit(size_t offset, size_t size) const
{
    return std::any_of(readers_.cbegin(), readers_.cend(),
                       [&offset, &size](auto const& reader) {
                           return reader->isCacheHit(offset, size);
                       });
}

void
MultiBufferedFileReader::loadCache(const CacheRequestParams& params)
{
    if (params.size_ > totalReadSize_) {
        setReadSize(params.size_);
    }

    std::unique_lock<std::mutex> lock(readOffsetMapMutex_);
    const auto offsetItem = readOffsetMap_.find(params.offset_);

    if (readOffsetMap_.end() == offsetItem) {
        const std::string msg = "multiBufferedFileReader; Error: read offset map is missing the offset [" + std::to_string(params.offset_) + "].  readOffsetMap has [" + std::to_string(readOffsetMap_.size()) + "] entries.\n";
        SEGYLOG_REPORT_ERROR(msg);
        size_t ix = 0;
        for (const auto& item : readOffsetMap_) {
            const std::string m1{"\tMap element " + std::to_string(ix++) + " is: [" + std::to_string(item.first) + "]"};
            SEGYLOG_DEBUG(m1);
            for (std::size_t rix = 0; rix < item.second.size(); ++rix) {
                const std::string m2{"\t\treader" + std::to_string(rix) + " is: [" + std::to_string(item.second[rix]) + "]"};
                SEGYLOG_DEBUG(m2);
            }
        }

        throw FileIOException(msg);
    }

    if (offsetItem->second.size() < readers_.size()) {
        const std::string msg = "multiBufferedFileReader; FileIOException: Reader read offsets are missing"; 
        SEGYLOG_REPORT_ERROR(msg);
        throw FileIOException(msg);
    }

    std::vector<std::thread> runners(readers_.size());

    for (size_t readerNo = 0; readerNo < readers_.size(); ++readerNo) {

        size_t const readerOffset = offsetItem->second[readerNo];

        runners[readerNo] = std::thread(
            [this](size_t readerNo, size_t readerOffset) {
                readers_[readerNo]->loadCache(CacheRequestParams{}.withOffset(readerOffset).withSize(readerReadSize_));
            },
            readerNo, readerOffset);
    }

    std::for_each(runners.begin(), runners.end(), [](auto& runner) {
        if (runner.joinable()) {
            runner.join();
        }
    });
}

void
MultiBufferedFileReader::dropCache()
{
    std::for_each(readers_.begin(), readers_.end(),
                  [this](auto& reader) {
                      reader->dropCache();
                  });
}

uint8_t const*
MultiBufferedFileReader::getPointerToCached(uint64_t offset, size_t size) const
{
    const auto found = std::find_if(readers_.cbegin(), readers_.cend(),
                                    [&offset, &size](auto const& reader) {
                                        return reader->isCacheHit(offset, size);
                                    });

    if (readers_.cend() == found) {
        throw FileIOException("Error: cache hit is false while getPointerToCached");
    }

    return (*found)->getPointerToCached(offset, size);
}

size_t
MultiBufferedFileReader::getFirstCachedOffset() const
{
    return readers_[0]->getFirstCachedOffset();
}

size_t
MultiBufferedFileReader::getCachedSize() const
{
    return std::accumulate(readers_.begin(), readers_.end(),
                           0U,
                           [](size_t cachedSize, auto const& reader) {
                               return cachedSize + reader->getCachedSize();
                           });
}

void
MultiBufferedFileReader::clearReadOffsetMap()
{
    std::unique_lock<std::mutex> lock(readOffsetMapMutex_);
    readOffsetMap_.clear();
}

void
MultiBufferedFileReader::setReadOffsetMap(ReadOffsetMap&& readOffsetMap)
{
    if (readOffsetMap.empty()) {
        throw FileIOException("Error: given readOffsetMap is empty");
    }

    std::unique_lock<std::mutex> lock(readOffsetMapMutex_);
    readOffsetMap_.swap(readOffsetMap);
}

void
MultiBufferedFileReader::addReadOffsetMapItem(ReadOffsetMapItem&& item)
{
    std::unique_lock<std::mutex> lock(readOffsetMapMutex_);
    readOffsetMap_.emplace(std::move(item));
}

size_t
MultiBufferedFileReader::alignMultiToBoundary(size_t readersCount, size_t size) const
{
    // Here we are expecting to receive size equal to the total of individual read caches, not a single read-cache.

    auto const readerBufferSize = size / readersCount + size % readersCount;
    return alignToBoundary(readerBufferSize);
}

} // namespace segysdk::fileio
