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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include <string>
#include <filesystem>

namespace segysdk::library_loader
{

class SEGYSDK_DLL_PUBLIC PluginConfig
{
public:
    inline static const char* LogWriterPluginPathName_ = "LOG_WRITER_PLUGIN_PATH";
    inline static const char* ComputePolygonPluginPathName_ = "COMPUTE_POLYGON_PLUGIN_PATH";
    inline static const char* CalculateStatisticsPluginPathName_ = "CALCULATE_STATISTICS_PLUGIN_PATH";
    inline static const char* Extract2DNavigationPluginPathName_ = "EXTRACT_2D_NAVIGATION_PLUGIN_PATH";
    inline static const char* CalculateBinGridPluginName_ = "CALCULATE_BIN_GRID_PLUGIN_PATH";

    explicit PluginConfig(const std::filesystem::path& configPath);

    PluginConfig(const PluginConfig&) = delete;
    PluginConfig& operator=(const PluginConfig&) = delete;

    bool isLogWriterPluginConfigured() const;
    bool isComputePolygonPluginConfigured() const;
    bool isCalculateStatisticsPluginConfigured() const;
    bool isExtract2DNavigationPluginConfigured() const;
    bool isCalculateBinGridPluginConfigured() const;

    std::filesystem::path getLogWriterPluginPath() const;
    std::filesystem::path getComputePolygonPluginPath() const;
    std::filesystem::path getCalculateStatisticsPluginPath() const;
    std::filesystem::path getExtract2DNavigationPluginPath() const;
    std::filesystem::path getCalculateBinGridPluginPath() const;

private:
    void loadConfig(const std::filesystem::path& configPath);
    std::filesystem::path getValidPluginPath(const std::filesystem::path& pluginPath) const;

    std::filesystem::path logWriterPluginPath_;
    std::filesystem::path computePolygonPluginPath_;
    std::filesystem::path calculateStatisticsPluginPath_;
    std::filesystem::path extract2DnavigationPluginPath_;
    std::filesystem::path calculateBinGridPluginPath_;
};

} // namespace segysdk::library_loader
