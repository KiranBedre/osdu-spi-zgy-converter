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

#include "../FileIO/FileManager.h"
#include "../FileIO/FileReaderOptions.h"
#include "../FileIO/FileReference.h"

#include <map>
#include <mutex>

namespace segysdk::fileio
{

struct SEGYSDK_DLL_PUBLIC FileManagerParams : public FileAccessParams
{
    FileManagerParams() = default;
    explicit FileManagerParams(const FileAccessParams& params);
    explicit FileManagerParams(const FileReference& reference);
    FileManagerParams(const request::Params& params, const FileReference& reference);
    FileManagerParams& withReaderOptions(const FileReaderOptions& value);
    FileReaderOptions readerOptions_{};
};

class SEGYSDK_DLL_PUBLIC GenericFileManager
{
private:
    static std::map<std::string, std::shared_ptr<FileManager>> Managers;
    static std::shared_ptr<FileManager> getManager(const FileReference& fileReference);
    static std::shared_ptr<FileManager> getManager(const FileManagerParams& params);
    static std::recursive_mutex Mutex;

private:
    GenericFileManager();

public:
    static bool exists(const FileReference& fileReference);
    static size_t getSize(const FileReference& fileReference);

    static TextFileReadResult readTextFile(const FileManagerParams& params);
    static bool writeTextFile(const FileReference& fileReference, const std::string& text, bool allowOverwrite);

    static std::shared_ptr<FileReader> createReader(const FileManagerParams& params);
    static std::shared_ptr<FileWriter> createWriter(const FileReference& fileReference, WriteMode mode);

    static std::shared_ptr<FileReader> createRawReader(const FileManagerParams& params);

    static bool remove(const FileReference& fileReference);
    static void eraseCache(const FileReference& fileReference);
    static void reset();

    // Seismic drive configuration
    static bool isSDDataset(const std::string& fileName);
    static std::string getSDLegalTag(const FileManagerParams& params);
};

} // namespace segysdk::fileio
