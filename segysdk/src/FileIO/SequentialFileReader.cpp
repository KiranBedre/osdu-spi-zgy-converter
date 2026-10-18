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

#include "../FileIO/SequentialFileReader.h"

#include "../FileIO/BufferedFileReader.h"
#include "../FileIO/FileIOException.h"

#include <algorithm>
#include <utility>

namespace segysdk::fileio
{

SequentialFileReader::SequentialFileReader(const PipelineFileReaderParams& params)
  : PipelineFileReaderBase(params)
{
    /* EMPTY */
}

SequentialFileReader::~SequentialFileReader()
{
    /* EMPTY */
}

void
SequentialFileReader::setBufferSize(size_t size)
{
    // Align up to closest boundary
    size_t const newReaderBufferSize = alignMultiToBoundary(readerUnits_.size(), size);
    if (newReaderBufferSize != readerBufferSize_) {
        readerBufferSize_ = newReaderBufferSize;
        readerReadSize_ = newReaderBufferSize;

        std::for_each(readerUnits_.begin(), readerUnits_.end(),
                      [this](auto& reader) {
                          if (reader.isBusy()) {
                              throw FileIOException("Error: unexpected reader busy status on setting the buffer size.");
                          }
                          reader.setBufferSize(readerBufferSize_);
                      });

        totalBufferSize_ = readerBufferSize_ * readerUnits_.size();
        totalReadSize_ = readerReadSize_ * readerUnits_.size();
    }
}

void
SequentialFileReader::setReadSize(size_t size)
{
    size_t const alignedReaderSize = alignMultiToBoundary(readerUnits_.size(), size);
    if (alignedReaderSize > readerBufferSize_) {
        setBufferSize(size);
    }
    else {
        readerReadSize_ = alignedReaderSize;
        std::for_each(readerUnits_.begin(), readerUnits_.end(),
                      [this](auto& reader) {
                          if (reader.isBusy()) {
                              throw FileIOException("Error: unexpected reader busy status on setting the read size.");
                          }
                          reader.setReadSize(readerReadSize_);
                      });
        totalReadSize_ = readerReadSize_ * readerUnits_.size();
    }
}

uint8_t const*
SequentialFileReader::doReadAligned(const FileReadParams& params)
{
    if (params.size_ > SequentialFileReader::readSizeOverlap) {
        throw FileIOException("Error: requested to read amount is greater than overlap area.");
    }

    auto selectedReader = selectReader(params.offset_, params.size_);

    if (readerUnits_.end() == selectedReader) {
        loadCache(params);

        // perform the double search over the just updated cache
        selectedReader = selectReader(params.offset_, params.size_);
    }

    if (readerUnits_.end() == selectedReader) {
        throw FileIOException("Error: cache hit is false while readAligned");
    }

    if (selectedReader != recentReader_) {
        recentReader_ = selectedReader;

        size_t const selectedReaderCacheOffset = selectedReader->getRequestedCacheOffset();

        for (auto reader = readerUnits_.begin(); reader != readerUnits_.end(); ++reader) {
            if (reader == selectedReader) {
                continue; // keep the selected reader unmodified
            }

            if (reader->isBusy()) {
                continue; // avoid to update the reader in busy state
            }

            if (reader->getRequestedCacheOffset() < selectedReaderCacheOffset) {
                size_t const chunkOffset = lastLoadedCacheOffset_ + getOffsetPerReader();
                details::RequestedCacheItem chunkRequest{};
                if (requestNextChunk(chunkOffset, chunkRequest)) {
                    notifyReader(*reader, chunkRequest);
                }
            }
        }
    }

    // wait for the selected reader to complete the cache loading
    waitForReaderIsReady(*selectedReader);

    return selectedReader->readAligned(params.offset_, params.size_, params.alignment_);
}

void
SequentialFileReader::loadCache(const CacheRequestParams& params)
try {
    waitForEveryReaderIsReady();

    if (params.size_ > totalReadSize_) {
        setReadSize(params.size_);
    }

    if (this->getFileSize() <= params.offset_) {
        throw FileIOException("Error: requested the reading beyond the end of file.");
    }
    for (size_t readerNo = 0; readerNo < readerUnits_.size(); ++readerNo) {
        size_t const chunkOffset = params.offset_ + readerNo * getOffsetPerReader();
        details::RequestedCacheItem chunkRequest{};
        if (requestNextChunk(chunkOffset, chunkRequest)) {
            auto& reader = readerUnits_[readerNo];
            if (reader.isBusy()) {
                throw FileIOException("Error: unexpected reader busy status on initiating of the cache loading.");
            }
            notifyReader(reader, chunkRequest);
        }
    }
}
catch (...) {
    // release every thread to avoid possible blockage
    finishEveryReaderThread();

    // let the call stack to unwind
    throw;
}

size_t
SequentialFileReader::getOffsetPerReader() const
{
    return readerReadSize_ - readSizeOverlap;
}

bool
SequentialFileReader::requestNextChunk(size_t chunkOffset, details::RequestedCacheItem& chunkRequest)
{
    bool result = false;
    if (this->getFileSize() > chunkOffset) {
        lastLoadedCacheOffset_ = chunkOffset;

        chunkRequest = {chunkOffset, readerReadSize_};

        result = true;
    }
    return result;
}

SequentialFileReader::ReaderUnitVector::iterator
SequentialFileReader::selectReader(size_t offset, size_t size)
{
    if (recentReader_->isCacheHit(offset, size)) {
        return recentReader_;
    }
    else {
        // search for any reader cache containing the required data
        auto anyReader = std::find_if(readerUnits_.begin(), readerUnits_.end(),
                                      [&offset, &size](auto const& reader) {
                                          return reader.isCacheHit(offset, size);
                                      });
        if (readerUnits_.end() != anyReader) {
            // search for the complementary reader cache containing the same required data due to overlapping
            const auto anotherReader = std::find_if(readerUnits_.rbegin(), readerUnits_.rend(),
                                                    [&offset, &size](auto const& reader) {
                                                        return reader.isCacheHit(offset, size);
                                                    });
            if (readerUnits_.rend() != anotherReader) {
                if (anotherReader->getRequestedCacheOffset() > anyReader->getRequestedCacheOffset()) {
                    // return the reader cache with the highest starting address
                    return anotherReader.base() - 1; // here is the revers- to forward- iterator conversion
                }
            }
        }
        return anyReader;
    }
}

size_t
SequentialFileReader::alignMultiToBoundary(size_t readersCount, size_t size) const
{
    auto const readerBufferSize = size / readersCount + size % readersCount;
    return alignToBoundary(readerBufferSize + readSizeOverlap);
}

} // namespace segysdk::fileio
