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

#include "FileAccessParams.h"

#include "../Request/Params.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <cstddef>
#include <cstdint>
#include <string>

namespace segysdk::fileio
{

using CacheRequestParams = FileReadParams;

class SEGYSDK_DLL_PUBLIC BufferedFileReaderBase
{
public:
    BufferedFileReaderBase();
    virtual ~BufferedFileReaderBase();

    virtual size_t getBufferSize() const = 0;
    virtual size_t getReadSize() const = 0;
    virtual uint64_t getFileSize() const = 0;
    virtual size_t getReadersCount() const = 0;
    virtual std::string const& getFileName() const = 0;
    virtual void setBufferSizeBoundary(size_t newBoundary);
    virtual void setBufferSize(size_t bufferSize) = 0;
    virtual void setBufferLoadChunk(size_t newChunk);
    virtual void setReadSize(size_t readSize) = 0;
    uint8_t const* readAligned(size_t offset, size_t size, size_t alignment);
    uint8_t const* readAligned(const FileReadParams& params);
    virtual bool isCacheHit(size_t offset, size_t size) const = 0;
    virtual void loadCache(const CacheRequestParams& params) = 0;
    virtual void dropCache() = 0;
    virtual uint8_t const* getPointerToCached(uint64_t offset, size_t size) const = 0;
    virtual size_t getFirstCachedOffset() const = 0;
    virtual size_t getCachedSize() const = 0;

protected:
    size_t alignToBoundary(size_t size) const;
    uint8_t const* read(const FileReadParams& params);

protected:
    size_t loadChunkSize_ = 1024U * 1024U;

private:
    BufferedFileReaderBase(const BufferedFileReaderBase&) = delete;
    BufferedFileReaderBase(BufferedFileReaderBase&&) noexcept = delete;
    BufferedFileReaderBase& operator=(const BufferedFileReaderBase&) = delete;
    BufferedFileReaderBase& operator=(BufferedFileReaderBase&&) = delete;
    virtual uint8_t const* doReadAligned(const FileReadParams& params) = 0;

private:
    size_t boundaryValue_ = 1024U * 1024U;
};

} // namespace segysdk::fileio
