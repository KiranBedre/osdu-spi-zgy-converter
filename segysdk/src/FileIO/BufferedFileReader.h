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

#include "../segysdk_details/DLL_EXPORT.h"

#include "../FileIO/BufferedFileReaderBase.h"
#include "../FileIO/FileReader.h"

#include <cstdint>
#include <memory>

namespace segysdk::fileio
{

class SEGYSDK_DLL_PUBLIC BufferedFileReader final : public BufferedFileReaderBase
{
public:
    explicit BufferedFileReader(std::shared_ptr<FileReader> file);

    virtual size_t getBufferSize() const override;
    virtual size_t getReadSize() const override;
    virtual uint64_t getFileSize() const override;
    virtual size_t getReadersCount() const override;
    virtual std::string const& getFileName() const override;
    virtual void setBufferSize(size_t bufferSize) override;
    virtual void setReadSize(size_t readSize) override;
    virtual bool isCacheHit(size_t offset, size_t size) const override;
    virtual void loadCache(const CacheRequestParams& params) override;
    virtual void dropCache() override;
    virtual uint8_t const* getPointerToCached(uint64_t offset, size_t size) const override;
    virtual size_t getFirstCachedOffset() const override;
    virtual size_t getCachedSize() const override;

private:
    BufferedFileReader(const BufferedFileReader&) = delete;
    BufferedFileReader(BufferedFileReader&&) noexcept = delete;
    BufferedFileReader& operator=(const BufferedFileReader&) = delete;
    BufferedFileReader& operator=(BufferedFileReader&&) = delete;

    uint8_t* getAlignedBuffer(size_t size);
    uint8_t const* doReadAligned(const FileReadParams& params) override;

private:
    std::shared_ptr<FileReader> file_;
    ByteBuffer buffer_;
    ByteBuffer alignedBuffer_;
    size_t currentOffset_{};
    size_t currentSize_{};
    size_t readSize_{};
};

} // namespace segysdk::fileio
