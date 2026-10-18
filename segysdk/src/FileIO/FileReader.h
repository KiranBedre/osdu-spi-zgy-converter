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

#include "FileAccessParams.h"

#include "../segysdk_details/DLL_EXPORT.h"

#include <string>

namespace segysdk::fileio
{

class SEGYSDK_DLL_PUBLIC FileReader
{
private:
    std::string fileName_;
    size_t bufferSize_{};
    virtual ByteBuffer doRead(const FileReadParams& params) = 0;
    virtual ByteBuffer doReadUnbuffered(const FileReadParams& params);

protected:
    explicit FileReader(std::string fileName);

public:
    virtual ~FileReader();
    FileReader(const FileReader&) = delete;
    FileReader(FileReader&&) = delete;
    FileReader& operator=(const FileReader&) = delete;
    FileReader& operator=(FileReader&&) = delete;

    const std::string& getFileName() const { return fileName_; }
    ByteBuffer read(const FileReadParams& params);
    ByteBuffer readUnbuffered(const FileReadParams& params);

    virtual size_t getBufferSize() const;
    virtual void setBufferSize(size_t bufferSize);

    virtual uint64_t getFileSize() = 0;

    virtual void close() = 0;

    static size_t getDefaultBufferSize();
    static void setDefaultBufferSize(size_t bufferSize);
};

} // namespace segysdk::fileio
