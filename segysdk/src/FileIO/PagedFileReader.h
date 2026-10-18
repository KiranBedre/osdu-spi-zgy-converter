/*
Copyright 2021 Schlumberger

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

#include <memory>
#include <mutex>

namespace segysdk::fileio
{

class SEGYSDK_DLL_PUBLIC PagedFileReader : public FileReader
{
private:
    struct Page
    {
        size_t index_;
        size_t offset_;
        size_t capacity_;
        ByteBuffer data_;
        Page* other_;
    };

private:
    std::shared_ptr<FileReader> reader_;
    std::recursive_mutex mutex_{};
    Page page_[2];
    Page* lastUsedPage_;

private:
    Page* findCachedPage(size_t pageIndex);
    ByteBuffer doRead(const FileReadParams& params) override;
    ByteBuffer doReadUnbuffered(const FileReadParams& params) override;

public:
    explicit PagedFileReader(const std::shared_ptr<FileReader>& reader);

    uint64_t getFileSize() override;
    void setBufferSize(size_t bufferSize) override;

    void close() override;
};

} // namespace segysdk::fileio
