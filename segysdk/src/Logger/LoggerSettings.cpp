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

#include "../Logger/LoggerSettings.h"

namespace segysdk::logger
{

LoggerSettings::LoggerSettings()
{
}

void
LoggerSettings::setCorrelationId(std::string id)
{
    std::swap(correlationId_, id);
}

void
LoggerSettings::setFileCollectionId(std::string id)
{
    std::swap(fileCollectionId_, id);
}

void
LoggerSettings::setLogWriterPluginPath(const segysdk::library_loader::PluginConfig& config)
{
    logWriterPluginPath_ = config.getLogWriterPluginPath();
}

const std::string&
LoggerSettings::getCorrelationId() const
{
    return correlationId_;
}

const std::string&
LoggerSettings::getFileCollectionId() const
{
    return fileCollectionId_;
}

const std::filesystem::path&
LoggerSettings::getLogWriterPluginPath() const
{
    return logWriterPluginPath_;
}

} // namespace segysdk::logger
