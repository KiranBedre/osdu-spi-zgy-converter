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

#include "../FileIO/SectionFileReader.h"

#include "../FileIO/BufferedFileReader.h"
#include "../FileIO/FileIOException.h"

#include <algorithm>
#include <utility>

namespace segysdk::fileio
{

SectionFileReader::SectionFileReader(std::vector<std::shared_ptr<fileio::FileReader>>&& files)
  : PipelineFileReaderBase(PipelineFileReaderParams{}.withFileReaders(files))
{
    /* EMPTY */
}

SectionFileReader::~SectionFileReader()
{
    /* EMPTY */
}

void
SectionFileReader::setBufferSize(size_t size)
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
SectionFileReader::setReadSize(size_t size)
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
SectionFileReader::doReadAligned(const FileReadParams& params)
{
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

        { // lock section begin
            std::unique_lock<std::mutex> lock(readOffsetMapMutex_);
            for (auto reader = readerUnits_.begin(); reader != readerUnits_.end(); ++reader) {
                if (reader == selectedReader) {
                    continue; // keep the selected reader unmodified
                }

                if (reader->isBusy()) {
                    continue; // avoid to update the reader in busy state
                }

                if (reader->getRequestedCacheOffset() < selectedReaderCacheOffset) {
                    if (readOffsetMapIndex_ < readOffsetMap_.size()) {
                        size_t const chunkOffset = readOffsetMap_[readOffsetMapIndex_++];
                        details::RequestedCacheItem chunkRequest{};
                        if (requestNextChunk(chunkOffset, chunkRequest)) {
                            notifyReader(*reader, chunkRequest);
                        }
                    }
                }
            } // for (auto reader
        } // lock section end
    }

    // wait for the selected reader to complete the cache loading
    waitForReaderIsReady(*selectedReader);

    return selectedReader->readAligned(params.offset_, params.size_, params.alignment_);
}

void
SectionFileReader::loadCache(const CacheRequestParams& params)
try {
    waitForEveryReaderIsReady();

    if (params.size_ > totalReadSize_) {
        setReadSize(params.size_);
    }

    if (this->getFileSize() <= params.offset_) {
        throw FileIOException("Error: requested the reading beyond the end of file.");
    }

    { // lock section begin
        std::unique_lock<std::mutex> lock(readOffsetMapMutex_);

        for (size_t readerNo = 0; readerNo < readerUnits_.size(); ++readerNo) {

            if (readOffsetMapIndex_ >= readOffsetMap_.size()) {
                break; // no items left in the read offset map
            }

            size_t const chunkOffset = readOffsetMap_[readOffsetMapIndex_++];
            details::RequestedCacheItem chunkRequest{};
            if (requestNextChunk(chunkOffset, chunkRequest)) {
                auto& reader = readerUnits_[readerNo];
                if (reader.isBusy()) {
                    throw FileIOException("Error: unexpected reader busy status on initiating of the cache loading.");
                }
                notifyReader(reader, chunkRequest);
            }
        } // for (size_t readerNo

    } // lock section end
}
catch (...) {
    // release every thread to avoid possible blockage
    finishEveryReaderThread();

    // let the call stack to unwind
    throw;
}

void
SectionFileReader::clearReadOffsetMap()
{
    std::unique_lock<std::mutex> lock(readOffsetMapMutex_);
    readOffsetMap_.clear();
    readOffsetMapIndex_ = 0U;
}

void
SectionFileReader::setReadOffsetMap(ReadOffsetMap&& readOffsetMap)
{
    if (readOffsetMap.empty()) {
        throw FileIOException("Error: given readOffsetMap is empty");
    }

    std::unique_lock<std::mutex> lock(readOffsetMapMutex_);
    readOffsetMap_.swap(readOffsetMap);
    readOffsetMapIndex_ = 0U;
}

void
SectionFileReader::addReadOffsetMapItem(ReadOffsetMapItem&& item)
{
    std::unique_lock<std::mutex> lock(readOffsetMapMutex_);
    readOffsetMap_.emplace_back(std::move(item));
}

bool
SectionFileReader::requestNextChunk(size_t chunkOffset, details::RequestedCacheItem& chunkRequest)
{
    bool result = false;
    if (this->getFileSize() > chunkOffset) {

        chunkRequest = {chunkOffset, readerReadSize_};

        result = true;
    }
    return result;
}

SectionFileReader::ReaderUnitVector::iterator
SectionFileReader::selectReader(size_t offset, size_t size)
{
    if (recentReader_->isCacheHit(offset, size)) {
        return recentReader_;
    }
    else {
        // search for any reader cache containing the required data
        return std::find_if(readerUnits_.begin(), readerUnits_.end(),
                            [&offset, &size](auto const& reader) {
                                return reader.isCacheHit(offset, size);
                            });
    }
}

size_t
SectionFileReader::alignMultiToBoundary(size_t readersCount, size_t size) const
{
    auto const readerBufferSize = size / readersCount + size % readersCount;
    return alignToBoundary(readerBufferSize);
}

} // namespace segysdk::fileio
