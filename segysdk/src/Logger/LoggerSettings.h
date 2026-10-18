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

#include "../LibraryLoader/PluginConfig.h"

#include <string>
#include <filesystem>

namespace segysdk::logger
{

class SEGYSDK_DLL_PUBLIC LoggerSettings
{
public:
    LoggerSettings();

    LoggerSettings(const LoggerSettings&) = delete;
    LoggerSettings& operator=(const LoggerSettings&) = delete;

    void setCorrelationId(std::string id);
    void setFileCollectionId(std::string id);
    void setLogWriterPluginPath(const segysdk::library_loader::PluginConfig& config);

    const std::string& getCorrelationId() const;
    const std::string& getFileCollectionId() const;
    const std::filesystem::path& getLogWriterPluginPath() const;

private:
    std::string correlationId_;
    std::string fileCollectionId_;
    std::filesystem::path logWriterPluginPath_;
};

} // namespace segysdk::logger
