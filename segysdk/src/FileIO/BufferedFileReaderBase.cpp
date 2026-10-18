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

#include "../FileIO/BufferedFileReaderBase.h"
#include "../FileIO/FileIOException.h"

#include <bitset>
#include <iostream>

namespace segysdk::fileio
{

BufferedFileReaderBase::BufferedFileReaderBase()
{
}

BufferedFileReaderBase::~BufferedFileReaderBase()
{
}

void
BufferedFileReaderBase::setBufferSizeBoundary(size_t newBoundary)
{
    if (newBoundary > 0) {
        boundaryValue_ = newBoundary;
    }
}

uint8_t const*
BufferedFileReaderBase::readAligned(size_t offset, size_t size, size_t alignment)
{
    return readAligned(
        FileReadParams{}
            .withOffset(offset)
            .withSize(size)
            .withAlignment(alignment));
}

uint8_t const*
BufferedFileReaderBase::readAligned(const FileReadParams& params)
{
    return doReadAligned(params);
}

void
BufferedFileReaderBase::setBufferLoadChunk(size_t newChunk)
{
    if (newChunk > 0) {
        loadChunkSize_ = newChunk;
    }
}

uint8_t const*
BufferedFileReaderBase::read(const FileReadParams& params)
{
    if (!isCacheHit(params.offset_, params.size_)) {
        loadCache(params);

        if (!isCacheHit(params.offset_, params.size_)) {
            // partial returns are not supported
            // Old behavior: throw FileIOException("Error reading from " + getFileName());
            // Now returning nullptr instead of throwing FileIOException, so caller
            // can handle it gracefully. In fact, all current callers are waiting
            // nullptr as error and none of them are catching FileIOException.
            return nullptr;
        }
    }

    return getPointerToCached(params.offset_, params.size_);
}

size_t
BufferedFileReaderBase::alignToBoundary(size_t size) const
{
    size_t const boundaryMinusOne = boundaryValue_ - 1U; // Boundary value is 1 Mb

    // Returns if (size > boundaryValue_) n * boundaryValue_  else boundaryValue_;  (boundaryValue_ is smallest read chunk ... )
    // Faster algorithm than using boolean operator...
    return (size + boundaryMinusOne) & ~boundaryMinusOne;
}

} // namespace segysdk::fileio
