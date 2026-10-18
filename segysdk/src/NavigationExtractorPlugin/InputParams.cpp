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

#include "NavigationExtractorPlugin/InputParams.h"

#include "OsduMetaDataUtilities/OsduMetadataHelper.h"
#include "Utilities/Environment.h"
#include "Utilities/JsonUtilities.h"

namespace segysdk::navigation_extractor
{

namespace ConfigurationOptions
{
constexpr auto FileCollection = "FileCollection";
constexpr auto ForceOverwrite = "ForceOverwrite";
constexpr auto NavigationSource = "NavigationSource";
constexpr auto SegyLineList = "SegyLineList";
constexpr auto SegyType = "SegyType";
constexpr auto Seismic2DInterpretationSet = "Seismic2DInterpretationSet";
constexpr auto SeismicLineGeometry = "SeismicLineGeometry";
constexpr auto SeismicTraceData = "SeismicTraceData";
} // namespace ConfigurationOptions

InputParams::InputParams()
  : osduPartition_(segysdk::utilities::Environment::instance().osduDataPartitionId()),
    navigationSource_(NavigationSource::TraceHeaders)
{
}

InputParams::InputParams(const std::filesystem::path& configPath)
  : InputParams{segysdk::utilities::JsonUtilities::GetJsonObjectFromFile(configPath.string()), FromJsonValueTag{}}
{
}

static auto
parseSegyLines(const Json::Value& config)
{
    auto result = SegyLines{};
    for (const auto& entry : config[ConfigurationOptions::SegyLineList]) {
        result.push_back(
            SegyLine{}
                .withSeismicTraceDataId(entry[ConfigurationOptions::SeismicTraceData].asString())
                .withFileCollectionId(entry[ConfigurationOptions::FileCollection].asString())
                .withSeismicLineGeometryId(entry[ConfigurationOptions::SeismicLineGeometry].asString()));
    }
    return result;
}

SegyLine&
SegyLine::withSeismicTraceDataId(std::string_view value)
{
    seismicTraceDataId_ = value;
    return *this;
}

SegyLine&
SegyLine::withFileCollectionId(std::string_view value)
{
    fileCollectionId_ = value;
    return *this;
}

SegyLine&
SegyLine::withSeismicLineGeometryId(std::string_view value)
{
    seismicLineGeometryId_ = value;
    return *this;
}

InputParams::InputParams(const Json::Value& config, FromJsonValueTag)
  : osduPartition_{segysdk::utilities::Environment::instance().osduDataPartitionId()},
    segyLines_{parseSegyLines(config)}
{
    loadConfig(config);
}

SegyLines
InputParams::getLines() const
{
    return segyLines_;
}

InputParams::InputParams(std::string fileCollectionSegyId, std::string seismicTraceDataId)
  : osduPartition_(segysdk::utilities::Environment::instance().osduDataPartitionId()),
    fileCollectionAndSeismicTraceData_{{std::move(fileCollectionSegyId), std::move(seismicTraceDataId)}},
    navigationSource_(NavigationSource::TraceHeaders)
{
}

InputParams::InputParams(std::string fileCollectionSegyId, std::string seismicTraceDataId, const bool forceOverwrite, const bool verbosity)
  : osduPartition_(segysdk::utilities::Environment::instance().osduDataPartitionId()),
    fileCollectionAndSeismicTraceData_{{std::move(fileCollectionSegyId), std::move(seismicTraceDataId)}},
    navigationSource_(NavigationSource::TraceHeaders),
    forceOverwrite_(forceOverwrite),
    verbosity_(verbosity)
{
}

InputParams::InputParams(std::string fileCollectionSegyId, std::string seismicTraceDataId, const bool forceOverwrite, const bool verbosity, const NavigationSource navSource)
  : osduPartition_(segysdk::utilities::Environment::instance().osduDataPartitionId()),
    fileCollectionAndSeismicTraceData_{{std::move(fileCollectionSegyId), std::move(seismicTraceDataId)}},
    navigationSource_(navSource),
    forceOverwrite_(forceOverwrite),
    verbosity_(verbosity)
{
}

InputParams::InputParams(std::string fileCollectionSegyId, std::string seismicTraceDataId, std::string interpretationSetId)
  : osduPartition_(segysdk::utilities::Environment::instance().osduDataPartitionId()),
    fileCollectionAndSeismicTraceData_{{std::move(fileCollectionSegyId), std::move(seismicTraceDataId)}},
    osduSeismic2DInterpretationSetId_(std::move(interpretationSetId)),
    navigationSource_(NavigationSource::TraceHeaders)
{
}

InputParams::InputParams(std::string fileCollectionSegyId, std::string seismicTraceDataId, std::string interpretationSetId, const bool forceOverwrite, const bool verbosity)
  : osduPartition_(segysdk::utilities::Environment::instance().osduDataPartitionId()),
    fileCollectionAndSeismicTraceData_{{std::move(fileCollectionSegyId), std::move(seismicTraceDataId)}},
    osduSeismic2DInterpretationSetId_(std::move(interpretationSetId)),
    navigationSource_(NavigationSource::TraceHeaders),
    forceOverwrite_(forceOverwrite),
    verbosity_(verbosity)
{
}

InputParams::InputParams(std::string fileCollectionSegyId, std::string seismicTraceDataId, std::string interpretationSetId, const bool forceOverwrite, const bool verbosity, const NavigationSource navSource)
  : osduPartition_(segysdk::utilities::Environment::instance().osduDataPartitionId()),
    fileCollectionAndSeismicTraceData_{{std::move(fileCollectionSegyId), std::move(seismicTraceDataId)}},
    osduSeismic2DInterpretationSetId_(std::move(interpretationSetId)),
    navigationSource_(navSource),
    forceOverwrite_(forceOverwrite),
    verbosity_(verbosity)
{
}

bool
InputParams::IsValid() const
{
    const bool emptyData = std::any_of(fileCollectionAndSeismicTraceData_.begin(), fileCollectionAndSeismicTraceData_.end(),
                                       [](const auto& elem) { return elem.first.empty() or elem.second.empty(); });
    return !osduPartition_.empty() and !fileCollectionAndSeismicTraceData_.empty() and !emptyData;
}

void
InputParams::ValidateParameters()
{
    errorList_.clear();

    if (osduPartition_.empty()) {
        errorList_.emplace_back("OSDU Partition parameter must be defined!");
    }
    for (const auto& [fileCollectionId, seismicTraceDataId] : fileCollectionAndSeismicTraceData_) {
        if (fileCollectionId.empty()) {
            errorList_.emplace_back("Input FileCollectionSegyId parameter must be a valid file name or id!");
        }

        if (seismicTraceDataId.empty()) {
            errorList_.emplace_back("Input SeismicTraceDataId or WorkProductId parameter must be a valid file name or id!");
        }
    }
}

void
InputParams::loadConfig(const Json::Value& config)
{
    auto getForceOverwrite = [&config](const bool defaultVal = false) {
        if (config.isMember(ConfigurationOptions::ForceOverwrite)) {
            return config[ConfigurationOptions::ForceOverwrite].asBool();
        }
        return defaultVal;
    };
    forceOverwrite_ = getForceOverwrite();

    auto isSegy2D = [&config](const bool defaultVal = false) {
        constexpr std::string_view segy2D = "2D";
        if (config.isMember(ConfigurationOptions::SegyType)) {
            return config[ConfigurationOptions::SegyType].asString() == segy2D;
        }
        return defaultVal;
    };
    isSegy2D_ = isSegy2D();

    auto getNavigationSource = [&config]() {
        constexpr auto defaultVal = segysdk::navigation_extractor::NavigationSource::TraceHeaders;
        const std::map<std::string, segysdk::navigation_extractor::NavigationSource> navigationMap = {
            {"TraceHeaders", segysdk::navigation_extractor::NavigationSource::TraceHeaders},
            {"ExternalSource", segysdk::navigation_extractor::NavigationSource::ExternalSource}};
        if (not config.isMember(ConfigurationOptions::NavigationSource)) {
            return defaultVal;
        }
        const std::string source = config[ConfigurationOptions::NavigationSource].asString();
        const auto it = navigationMap.find(source);
        return it != navigationMap.end() ? it->second : defaultVal;
    };
    navigationSource_ = getNavigationSource();

    auto getSeismicInterpretationSet = [&config]() {
        if (config.isMember(ConfigurationOptions::Seismic2DInterpretationSet)) {
            return config[ConfigurationOptions::Seismic2DInterpretationSet].asString();
        }
        return std::string();
    };
    osduSeismic2DInterpretationSetId_ = getSeismicInterpretationSet();

    auto getSegyLines = [&config]() {
        std::map<std::string, std::string> lines;

        if (not config.isMember(ConfigurationOptions::SegyLineList)) {
            return lines;
        }
        for (const auto& pair : config[ConfigurationOptions::SegyLineList]) {
            if (not pair.isMember(ConfigurationOptions::FileCollection) or not pair.isMember(ConfigurationOptions::SeismicTraceData)) {
                continue;
            }
            std::string fileCollection = pair[ConfigurationOptions::FileCollection].asString();
            std::string seismicTrace = pair[ConfigurationOptions::SeismicTraceData].asString();
            lines.try_emplace(fileCollection, seismicTrace);
        }
        return lines;
    };

    fileCollectionAndSeismicTraceData_ = getSegyLines();
}

} // namespace segysdk::navigation_extractor
