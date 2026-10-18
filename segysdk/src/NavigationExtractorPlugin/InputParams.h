/*
Copyright 2024 Schlumberger

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

#include "segysdk_details/DLL_EXPORT.h"

#include <json/json.h>

#include <filesystem>
#include <map>
#include <string>
#include <string_view>
#include <vector>

namespace segysdk::navigation_extractor
{

enum class SEGYSDK_DLL_PUBLIC NavigationSource {
    TraceHeaders,
    ExternalSource
};

struct SEGYSDK_DLL_PUBLIC SegyLine
{
    std::string seismicTraceDataId_;
    std::string fileCollectionId_;
    std::string seismicLineGeometryId_;
    SegyLine& withSeismicTraceDataId(std::string_view value);
    SegyLine& withFileCollectionId(std::string_view value);
    SegyLine& withSeismicLineGeometryId(std::string_view value);
};

using SegyLines = std::vector<SegyLine>;

class SEGYSDK_DLL_PUBLIC InputParams
{
public:
    using FileCollectionAndSeismicMap = std::map<std::string, std::string>;

    InputParams();
    explicit InputParams(const std::filesystem::path& configPath);
    InputParams(std::string fileCollectionSegyId, std::string seismicTraceDataId);
    InputParams(std::string fileCollectionSegyId, std::string seismicTraceDataId, bool forceOverwrite, bool verbosity);
    InputParams(std::string fileCollectionSegyId, std::string seismicTraceDataId, bool forceOverwrite, bool verbosity, NavigationSource navSource);
    InputParams(std::string fileCollectionSegyId, std::string seismicTraceDataId, std::string interpretationSetId);
    InputParams(std::string fileCollectionSegyId, std::string seismicTraceDataId, std::string interpretationSetId, bool forceOverwrite, bool verbosity);
    InputParams(std::string fileCollectionSegyId, std::string seismicTraceDataId, std::string interpretationSetId, bool forceOverwrite, bool verbosity, NavigationSource navSource);

    std::string getOsduPartition() const { return osduPartition_; }
    void setOsduPartition(const std::string& value) { osduPartition_ = value; }
    FileCollectionAndSeismicMap getSegyLines() const { return fileCollectionAndSeismicTraceData_; }
    std::string getSeismic2DInterpretationSetId() const { return osduSeismic2DInterpretationSetId_; }
    bool isSegy2D() const { return isSegy2D_; }
    bool getVerbosity() const { return verbosity_; }
    bool getForceOverwrite() const { return forceOverwrite_; }
    bool shouldAddCorrelationTag() const { return !correlationId_.empty(); }
    std::string getCorrelationId() const { return correlationId_; }
    void setCorrelationId(const std::string& value) { correlationId_ = value; }
    bool IsValid() const;
    void ValidateParameters();
    std::vector<std::string> getErrorList() const { return errorList_; }
    SegyLines getLines() const;

private:
    struct FromJsonValueTag
    {};
    InputParams(const Json::Value& config, FromJsonValueTag);
    void loadConfig(const Json::Value& config);

    std::string osduPartition_{};
    FileCollectionAndSeismicMap fileCollectionAndSeismicTraceData_{};
    std::string osduSeismic2DInterpretationSetId_{};
    NavigationSource navigationSource_{};
    std::string correlationId_{};
    bool forceOverwrite_ = true;
    bool verbosity_ = true;
    bool isSegy2D_ = true;
    std::vector<std::string> errorList_{};
    SegyLines segyLines_;
};

} // namespace segysdk::navigation_extractor
