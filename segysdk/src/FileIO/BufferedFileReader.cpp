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

#include "../FileIO/BufferedFileReader.h"

#include "../FileIO/FileIOException.h"
#include "../FileIO/FileReader.h"
#include "../Logger/Logger.h"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <iostream>
#include <utility>

namespace segysdk::fileio
{

BufferedFileReader::BufferedFileReader(std::shared_ptr<FileReader> file)
  : file_(std::move(file))
{
}

size_t
BufferedFileReader::getBufferSize() const
{
    return buffer_.size();
}

size_t
BufferedFileReader::getReadSize() const
{
    return readSize_;
}

void
BufferedFileReader::setBufferSize(size_t size)
{
    // Align up to closest read-size boundary
    const size_t newBufferSize = alignToBoundary(size);
    if (newBufferSize != buffer_.size()) {
        ByteBuffer(newBufferSize).swap(buffer_);
        readSize_ = newBufferSize;
        currentOffset_ = 0;
        currentSize_ = 0;
    }
}

void
BufferedFileReader::setReadSize(size_t size)
{
    // AlignedSize will be a multiple of desired read-size, e.g. 1MB
    size_t const alignedSize = alignToBoundary(size);
    if (alignedSize > buffer_.size()) {
        setBufferSize(size);
    }
    else {
        readSize_ = alignedSize;
    }
}

uint64_t
BufferedFileReader::getFileSize() const
{
    return file_->getFileSize();
}

size_t
BufferedFileReader::getReadersCount() const
{
    // This is a single buffered reader.
    return 1U;
}

std::string const&
BufferedFileReader::getFileName() const
{
    return file_->getFileName();
}

uint8_t*
BufferedFileReader::getAlignedBuffer(size_t size)
{
    if (size > alignedBuffer_.size()) {
        alignedBuffer_.resize(size);
    }

    return &alignedBuffer_[0];
}

uint8_t const*
BufferedFileReader::doReadAligned(const FileReadParams& params)
{
    uint8_t const* bufferedData = read(params);
    if (bufferedData != nullptr && params.alignment_ > 1 && (((params.offset_ - currentOffset_) & (params.alignment_ - 1)) != 0)) {
        uint8_t* alignedBuffer = getAlignedBuffer(params.size_);
        std::copy(&bufferedData[0], &bufferedData[params.size_], &alignedBuffer[0]);
        return alignedBuffer;
    }
    return bufferedData;
}

bool
BufferedFileReader::isCacheHit(size_t offset, size_t size) const
{
    return offset >= currentOffset_ && offset + size <= currentOffset_ + currentSize_;
}

void
BufferedFileReader::loadCache(const CacheRequestParams& params)
{
    if (params.size_ > readSize_) {
        setReadSize(params.size_);
    }
    currentOffset_ = params.offset_;

    {
        size_t const wholeChunksCount = readSize_ / loadChunkSize_;
        size_t const partialChunkSize = readSize_ % loadChunkSize_;

        currentSize_ = 0U;

        size_t fileOffset = params.offset_;
        buffer_.clear();

        for (size_t wholeChunkIdx = 0U; wholeChunkIdx < wholeChunksCount; ++wholeChunkIdx) {

            const auto readAmount = appendContainer(
                buffer_, file_->readUnbuffered(
                             FileReadParams{request::Params{params}}
                                 .withOffset(fileOffset)
                                 .withSize(loadChunkSize_)));

            if (not readAmount) {
                break;
            }

            fileOffset += readAmount;
            currentSize_ += readAmount;
        }

        if (partialChunkSize) {
            currentSize_
                += appendContainer(
                    buffer_, file_->readUnbuffered(
                                 FileReadParams{request::Params{params}}
                                     .withOffset(fileOffset)
                                     .withSize(partialChunkSize)));
        }
    }

    if (currentSize_ == 0) {
        throw FileIOException("Error reading from " + file_->getFileName());
    }
}

void
BufferedFileReader::dropCache()
{
    currentSize_ = 0;
}

uint8_t const*
BufferedFileReader::getPointerToCached(uint64_t offset, size_t size) const
{
    (void)size;
    return &buffer_[offset - currentOffset_];
}

size_t
BufferedFileReader::getFirstCachedOffset() const
{
    return currentOffset_;
}

size_t
BufferedFileReader::getCachedSize() const
{
    return currentSize_;
}

} // namespace segysdk::fileio
