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

#include "../FileIO/CacheSizeOption.h"
#include "../FileIO/FileManager.h"
#include "../FileIO/SDCredentials.h"
#include "../FileIO/SDFileWriter.h"

#include <SDAPI/SDManager.h>

#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <map>

namespace segysdk::fileio
{

class SEGYSDK_DLL_PUBLIC SDFileManager : public FileManager
{

private:
    std::shared_ptr<seismicdrive::SDManager> createSDManager();

    std::recursive_mutex mutex_;
    std::shared_ptr<SDCredentials> sdCredentials_;
    CacheSizeOption cacheSize_;
    std::optional<std::string> sdLegalTag_ = std::nullopt;
    bool logging_;
    std::shared_ptr<seismicdrive::SDManager> manager_;

    static constexpr size_t defaultBlockSize_ = 16UL * 1024 * 1024; // TODO: Evaluate performance and other effects

    const std::string sauth2_provider = "SAuth2";
    std::shared_ptr<SDFileWriter> createSDWriter(const std::string& fileName, WriteMode mode, size_t blockSize);
    std::map<std::string, std::shared_ptr<FileReader>> readers_;
    std::map<std::string, std::shared_ptr<FileReader>> rawReaders_;

public:
    SDFileManager(const SDCredentials& sdCredentials, const CacheSizeOption& cacheSize,
                  const std::optional<std::string> sdLegalTag = std::nullopt);

    ~SDFileManager();

    bool exists(const std::string& fileName) override;
    size_t getSize(const std::string& fileName) override;

    TextFileReadResult readTextFile(const FileAccessParams& params) override;
    bool writeTextFile(const std::string& fileName, const std::string& text, bool allowOverwrite) override;

    std::shared_ptr<FileReader> createReader(const FileAccessParams& params) override;
    std::shared_ptr<FileWriter> createWriter(const std::string& fileName, WriteMode mode) override;
    std::shared_ptr<FileReader> createRawReader(const FileAccessParams& params) override;

    bool remove(const std::string& fileName) override;
    std::shared_ptr<seismicdrive::SDManager> getSDManager();
};

} // namespace segysdk::fileio
