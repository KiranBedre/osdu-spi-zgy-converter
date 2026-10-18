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

#include "../FileIO/FileWriter.h"
#include "../FileIO/WriteMode.h"

#include <cstdint>
#include <fstream>
#include <mutex>

namespace segysdk::fileio
{

class SEGYSDK_DLL_PUBLIC DiskFileWriter : public FileWriter
{
private:
    std::fstream file_;
    std::string fileName_;
    std::recursive_mutex mutex_;

    void open(const std::string& fileName, WriteMode mode);

public:
    DiskFileWriter(const std::string& fileName, WriteMode mode);
    virtual ~DiskFileWriter();

    virtual uint64_t getFileSize() override;
    virtual bool write(const uint8_t* buffer, size_t bufferSize) override;
    virtual void close() override;
};

} // namespace segysdk::fileio
