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

#include "../FileIO/FileManager.h"

namespace segysdk::fileio
{

class SEGYSDK_DLL_PUBLIC DiskFileManager : public FileManager
{
private: // non-copyable
    DiskFileManager(const DiskFileManager&) = delete;
    DiskFileManager& operator=(const DiskFileManager&) = delete;

public:
    DiskFileManager();

public:
    bool exists(const std::string& fileName) override;
    size_t getSize(const std::string& fileName) override;

    TextFileReadResult readTextFile(const FileAccessParams& params) override;
    bool writeTextFile(const std::string& fileName, const std::string& text, bool allowOverwrite) override;

    std::shared_ptr<FileReader> createReader(const FileAccessParams& params) override;
    std::shared_ptr<FileWriter> createWriter(const std::string& fileName, WriteMode mode) override;

    std::shared_ptr<FileReader> createRawReader(const FileAccessParams& params) override;

    bool remove(const std::string& fileName) override;
};

} // namespace segysdk::fileio
