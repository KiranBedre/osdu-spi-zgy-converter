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
#include "../FileIO/FileWriter.h"
#include "../FileIO/WriteMode.h"
#include "../Request/Params.h"

#include <memory>

namespace segysdk::fileio
{

struct SEGYSDK_DLL_PUBLIC TextFileReadResult
{
    bool success_{false};
    std::string text_{};
};

class SEGYSDK_DLL_PUBLIC FileManager
{
protected:
    FileManager();

public:
    virtual ~FileManager();

    static bool isSDDataset(const std::string& fileName);

    virtual bool exists(const std::string& fileName) = 0;
    virtual size_t getSize(const std::string& fileName) = 0;

    virtual TextFileReadResult readTextFile(const FileAccessParams& params) = 0;
    virtual bool writeTextFile(const std::string& fileName, const std::string& text, bool allowOverwrite) = 0;

    virtual std::shared_ptr<FileReader> createReader(const FileAccessParams& params) = 0;
    virtual std::shared_ptr<FileWriter> createWriter(const std::string& fileName, WriteMode mode) = 0;

    // Reader without caching or buffering, use with care
    virtual std::shared_ptr<FileReader> createRawReader(const FileAccessParams& params) = 0;

    virtual bool remove(const std::string& fileName) = 0;
};

} // namespace segysdk::fileio
