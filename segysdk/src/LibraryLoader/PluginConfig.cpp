/*
Copyright 2022-2024 Schlumberger

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

#include "../LibraryLoader/PluginConfig.h"
#include "../Logger/Logger.h"
#include "../segysdk_details/SegyException.h"
#include "../Utilities/JsonUtilities.h"

#include <string>

namespace segysdk::library_loader
{

PluginConfig::PluginConfig(const std::filesystem::path& configPath)
{
    loadConfig(configPath);
}

bool
PluginConfig::isLogWriterPluginConfigured() const
{
    return !logWriterPluginPath_.empty() and std::filesystem::exists(logWriterPluginPath_);
}

bool
PluginConfig::isComputePolygonPluginConfigured() const
{
    return !computePolygonPluginPath_.empty() and std::filesystem::exists(computePolygonPluginPath_);
}

bool
PluginConfig::isCalculateStatisticsPluginConfigured() const
{
    return !calculateBinGridPluginPath_.empty() and std::filesystem::exists(calculateBinGridPluginPath_);
}

bool 
PluginConfig::isExtract2DNavigationPluginConfigured() const
{
    return !extract2DnavigationPluginPath_.empty() and std::filesystem::exists(extract2DnavigationPluginPath_);
}

bool PluginConfig::isCalculateBinGridPluginConfigured() const 
{
    return !calculateBinGridPluginPath_.empty() and std::filesystem::exists(calculateBinGridPluginPath_);
}

std::filesystem::path
PluginConfig::getLogWriterPluginPath() const
{
    return getValidPluginPath(logWriterPluginPath_);
}

std::filesystem::path
PluginConfig::getValidPluginPath(const std::filesystem::path& pluginPath) const
{
    if (!pluginPath.empty() and std::filesystem::exists(pluginPath)) {
        return pluginPath;
    }

    if (!pluginPath.empty() and !std::filesystem::exists(pluginPath)) {
        SEGYLOG_REPORT_ERROR("Requested plugin " + pluginPath.string() + " does not exist");
    }

    return std::filesystem::path();
}

std::filesystem::path
PluginConfig::getComputePolygonPluginPath() const
{
    return getValidPluginPath(computePolygonPluginPath_);
}

std::filesystem::path
PluginConfig::getCalculateStatisticsPluginPath() const
{
    return getValidPluginPath(calculateStatisticsPluginPath_);
}

std::filesystem::path
PluginConfig::getExtract2DNavigationPluginPath() const
{
    return getValidPluginPath(extract2DnavigationPluginPath_);
}

std::filesystem::path
PluginConfig::getCalculateBinGridPluginPath() const
{
    return getValidPluginPath(calculateBinGridPluginPath_);
}

void
PluginConfig::loadConfig(const std::filesystem::path& configPath)
{
    Json::Value config = segysdk::utilities::JsonUtilities::GetJsonObjectFromFile(configPath.string());

    auto getLogWriterPluginPath = [&config]() {
        return config[LogWriterPluginPathName_];
    };

    logWriterPluginPath_ = getLogWriterPluginPath().asString();

    auto getComputePolygonPluginPath = [&config]() {
        return config[ComputePolygonPluginPathName_];
    };
    computePolygonPluginPath_ = getComputePolygonPluginPath().asString();

    auto getCalculateStatisticsPluginPath = [&config]() {
        return config[CalculateStatisticsPluginPathName_];
    };
    calculateStatisticsPluginPath_ = getCalculateStatisticsPluginPath().asString();

    auto getExtract2DNavigationPluginPath = [&config]() {
        return config[Extract2DNavigationPluginPathName_];
    };
    extract2DnavigationPluginPath_ = getExtract2DNavigationPluginPath().asString();

    auto calculateBinGridPluginPath = [&config]() {
        return config[CalculateBinGridPluginName_];
    };
    calculateBinGridPluginPath_ = calculateBinGridPluginPath().asString();
}

} // namespace segysdk::library_loader
