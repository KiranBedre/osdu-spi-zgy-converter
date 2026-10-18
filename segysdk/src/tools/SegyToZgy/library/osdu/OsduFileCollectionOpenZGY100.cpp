/*
Copyright 2021-2025 Schlumberger

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

#include "OsduFileCollectionOpenZGY100.h"

#include "../tools/SegyToZgy/library/JsonLoader.h"
#include "../tools/SegyToZgy/library/OpenZGYFileProperties.h"

#include "../Logger/Logger.h"

#include "../Utilities/ZgyOutputParams.h"
#include "../Utilities/osdu/OsduFieldNameLiterals.h"
#include "../Utilities/osdu/OsduFileCollectionSegy100.h"
#include "../Utilities/osdu/OsduObject.h"
#include "../Utilities/osdu/WellKnownTypes.h"

#include <algorithm>
#include <filesystem>
#include <stdexcept>
#include <string>

int
getSampleSize(OpenZGY::SampleDataType sampleSize)
{
    switch (sampleSize) {
    case OpenZGY::SampleDataType::int8:
        return 1;
    case OpenZGY::SampleDataType::int16:
        return 2;
    case OpenZGY::SampleDataType::float32:
    case OpenZGY::SampleDataType::unknown:
        return 4;
    }

    return 4;
}

std::string
getPrecisionWord(OpenZGY::SampleDataType sampleSize)
{
    switch (sampleSize) {
    case OpenZGY::SampleDataType::int8:
    case OpenZGY::SampleDataType::int16:
        return "reference-data--WordFormatType:INT:";

    case OpenZGY::SampleDataType::float32:
    case OpenZGY::SampleDataType::unknown:
        return "reference-data--WordFormatType:IEEEFLOAT:";
    }

    return "reference-data--WordFormatType:IEEEFLOAT:";
}

Json::Value
getPrecision(const std::string& partition)
{
    std::string outputFormat = "reference-data--WordFormatType:IEEEFLOAT:";

    Json::Value precision;
    try {
        const segysdk::utilities::ZgyOutputParams zgyOutputParameters = segysdk::utilities::ZgyOutputParams::getZgyOutputParameters();
        const auto zgySampleSize = zgyOutputParameters.getZgySampleSize();
        precision["WordWidth"] = getSampleSize(zgySampleSize);
        precision["WordFormat"] = partition + ":" + getPrecisionWord(zgySampleSize);
    }
    catch (std::exception& e) {
        std::string msg = "Error while parsing ZGY output parameters\n" + std::string(e.what());
        SEGYLOG_REPORT_ERROR(msg);
    }

    return precision;
}

std::string
getPartition(const std::string& id_string)
{
    if (id_string.empty() || id_string.find_first_of(':') == std::string::npos) {
        return "partition_id";
    }

    return id_string.substr(0, id_string.find_first_of(':'));
}

Json::Value
transform(const segysdk::utilities::osdu::OsduFileCollectionSegy100& source, const OpenZGYFileProperties& fileProperties)
{
    auto root = JsonLoader::fromString(source.content());

    segysdk::utilities::osdu::OsduObject obj(root);
    const auto& sourceKind = obj.kind();

    const auto& sourceId = obj.id();
    const std::string partition = getPartition(sourceId);
    if (sourceKind.shortTypeName() != segysdk::utilities::osdu::WellKnownTypes::FileCollectionSEGY) {
        throw std::invalid_argument("Input object is not a FileCollection.SEGY");
    }

    Json::Value sourceData = root[segysdk::utilities::osdu::OsduFieldNameLiterals::Data];
    Json::Value& data = root[segysdk::utilities::osdu::OsduFieldNameLiterals::Data] = Json::Value();

    auto fileName = fileProperties.fileName();
    data[segysdk::utilities::osdu::OsduFieldNameLiterals::Name] = std::filesystem::path{fileName}.filename().string();
    data[segysdk::utilities::osdu::OsduFieldNameLiterals::Description] = sourceData[segysdk::utilities::osdu::OsduFieldNameLiterals::Description];
    data[segysdk::utilities::osdu::OsduFieldNameLiterals::TotalSize] = std::to_string(fileProperties.fileSize());

    // Add new mandatory property - Precision
    data[segysdk::utilities::osdu::OsduFieldNameLiterals::Precision] = getPrecision(partition);

    // TODO data.DatasetProperties.FileCollectionPath | An optional path to the file collection.

    Json::Value newDsp;
    Json::Value newFile;
    newFile[segysdk::utilities::osdu::OsduFieldNameLiterals::FileSource] = fileProperties.fileName();
    newFile[segysdk::utilities::osdu::OsduFieldNameLiterals::FileSize] = std::to_string(fileProperties.fileSize());
    // TODO data.DatasetProperties.FileSourceInfos[].Name | Optional, user-friendly file name.

    newDsp[segysdk::utilities::osdu::OsduFieldNameLiterals::FileSourceInfos].append(newFile);

    std::replace(fileName.begin(), fileName.end(), '\\', '/');
    auto dsp_fcp = std::filesystem::path(fileName).remove_filename().string();

    newDsp[segysdk::utilities::osdu::OsduFieldNameLiterals::FileCollectionPath] = dsp_fcp;

    data[segysdk::utilities::osdu::OsduFieldNameLiterals::DatasetProperties] = newDsp;
    root[segysdk::utilities::osdu::OsduFieldNameLiterals::Kind] = sourceKind.changeType(std::string{segysdk::utilities::osdu::WellKnownTypes::FileCollectionSlbOpenZGY}.append(":1.0.0")).kind();

    // Leave Meta empty for now ...
    Json::Value meta(Json::arrayValue);
    root[segysdk::utilities::osdu::OsduFieldNameLiterals::Meta] = meta;

    const auto id = source.id();
    const auto version = source.version();
    root[segysdk::utilities::osdu::OsduFieldNameLiterals::Ancestry][segysdk::utilities::osdu::OsduFieldNameLiterals::Parents] = Json::arrayValue;
    root[segysdk::utilities::osdu::OsduFieldNameLiterals::Ancestry][segysdk::utilities::osdu::OsduFieldNameLiterals::Parents][0] = id + ":" + version;

    root.removeMember(segysdk::utilities::osdu::OsduFieldNameLiterals::Id);

#ifdef showDebugInfo
    auto dbg = root.toStyledString();
    SEGYLOG_DEBUG("Generated FileCollection.ZGY:");
    SEGYLOG_DEBUG(dbg);
#endif

    return root;
}

OsduFileCollectionOpenZGY100::OsduFileCollectionOpenZGY100(const segysdk::utilities::osdu::OsduFileCollectionSegy100& source, const OpenZGYFileProperties& fileProperties)
  : segysdk::utilities::osdu::OsduObject(transform(source, fileProperties))
{
}

OsduFileCollectionOpenZGY100::OsduFileCollectionOpenZGY100(Json::Value root)
  : segysdk::utilities::osdu::OsduObject(std::move(root))
{
}

std::string
OsduFileCollectionOpenZGY100::zgyFileName() const
{
    return root_[segysdk::utilities::osdu::OsduFieldNameLiterals::Data]
                [segysdk::utilities::osdu::OsduFieldNameLiterals::DatasetProperties]
                [segysdk::utilities::osdu::OsduFieldNameLiterals::FileSourceInfos]
                [0]
                [segysdk::utilities::osdu::OsduFieldNameLiterals::FileSource]
                    .asString();
}

std::string
OsduFileCollectionOpenZGY100::zgyFileSize() const
{
    return root_[segysdk::utilities::osdu::OsduFieldNameLiterals::Data]
                [segysdk::utilities::osdu::OsduFieldNameLiterals::DatasetProperties]
                [segysdk::utilities::osdu::OsduFieldNameLiterals::FileSourceInfos]
                [0]
                [segysdk::utilities::osdu::OsduFieldNameLiterals::FileSize]
                    .asString();
}

std::string
OsduFileCollectionOpenZGY100::totalFileSize() const
{
    return root_[segysdk::utilities::osdu::OsduFieldNameLiterals::Data]
                [segysdk::utilities::osdu::OsduFieldNameLiterals::TotalSize]
                    .asString();
}

std::vector<std::string>
OsduFileCollectionOpenZGY100::legalTags() const
{
    std::vector<std::string> ret{};
    for (const auto& tag : root_[segysdk::utilities::osdu::OsduFieldNameLiterals::Legal][segysdk::utilities::osdu::OsduFieldNameLiterals::LegalTags]) {
        ret.emplace_back(tag.asString());
    }
    return ret;
}

std::vector<std::string>
OsduFileCollectionOpenZGY100::parentIds() const
{
    std::vector<std::string> ret{};
    for (const auto& parents : root_[segysdk::utilities::osdu::OsduFieldNameLiterals::Ancestry][segysdk::utilities::osdu::OsduFieldNameLiterals::Parents]) {
        ret.emplace_back(parents.asString());
    }
    return ret;
}
