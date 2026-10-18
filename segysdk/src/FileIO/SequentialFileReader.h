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

#include "../FileIO/FileReader.h"
#include "../FileIO/PipelineFileReaderBase.h"

#include <cstdint>
#include <memory>
#include <vector>

namespace segysdk::fileio
{

class SEGYSDK_DLL_PUBLIC SequentialFileReader final : public PipelineFileReaderBase
{
public:
    using ReaderUnitVector = PipelineFileReaderBase::ReaderUnitVector;

public:
    explicit SequentialFileReader(const PipelineFileReaderParams& params);
    virtual ~SequentialFileReader() override;

    virtual void setBufferSize(size_t bufferSize) override;
    virtual void setReadSize(size_t readSize) override;
    virtual void loadCache(const CacheRequestParams& params) override;

private:
    SequentialFileReader(const SequentialFileReader&) = delete;
    SequentialFileReader(SequentialFileReader&&) noexcept = delete;
    SequentialFileReader& operator=(const SequentialFileReader&) = delete;
    SequentialFileReader& operator=(SequentialFileReader&&) = delete;

    size_t getOffsetPerReader() const;
    bool requestNextChunk(size_t chunkOffset, details::RequestedCacheItem& chunkRequest);
    ReaderUnitVector::iterator selectReader(size_t offset, size_t size);
    uint8_t const* doReadAligned(const FileReadParams& params) override;
    size_t alignMultiToBoundary(size_t readersCount, size_t size) const;

private:
    size_t lastLoadedCacheOffset_{};

    // We need to set overlp size large enough to hold a full trace...
    // Set it to hold up to 10,000 * 4 byte samples for now.
    static constexpr size_t const readSizeOverlap = 10U * 4U * 1024U;
};

} // namespace segysdk::fileio
