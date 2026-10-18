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

#include <cstdint>
#include <cstdlib>
#include <string>

namespace segysdk::fileio
{

class SEGYSDK_DLL_PUBLIC FileWriter
{
private:
    std::string fileName_;

public:
    explicit FileWriter(std::string fileName);
    virtual ~FileWriter();
    FileWriter(const FileWriter&) = delete;
    FileWriter(FileWriter&&) = delete;
    FileWriter& operator=(const FileWriter&) = delete;
    FileWriter& operator=(FileWriter&&) = delete;

    const std::string& getFileName() const { return fileName_; }

    virtual uint64_t getFileSize() = 0;
    virtual bool write(const uint8_t* buffer, size_t bufferSize) = 0;
    virtual void close() = 0;
};

} // namespace segysdk::fileio
