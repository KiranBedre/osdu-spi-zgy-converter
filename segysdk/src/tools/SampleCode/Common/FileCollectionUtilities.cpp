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

#include "FileCollectionUtilities.h"

#include "EnvironmentVariables.h"
#include "JsonParseException.h"
#include "JsonToolKit.h"

#include "../FileIO/GenericFileReader.h"
#include "../Utilities/Environment.h"
#include "../Utilities/StringUtils.h"
#include "../Utilities/Settings.h"
#include "../Utilities/TokenSourceFactory.h"

#include <iostream>


namespace SampleCode::Common
{

void
listMembers(const Json::Value& json)
{
    const auto member_list = json.getMemberNames();

    std::cout << "Json Members: " << std::endl;
    for (const std::string& member : member_list) {
        std::cout << "   " << member << std::endl;
    }
}

/**
 * \brief Utility method to get and validate the OSDU FileCollection.SEGY Json structure.
 * \param json_string Throws if json string is invalid or if it does not contain mandatory members of the FileCollection.SEGY structure
 * \return Valid Json structure
 */
Json::Value
FileCollectionUtilities::GetFileCollectionSEGYFromJsonString(const std::string& json_string)
{
    auto json = JsonToolkit::ParseJsonString(json_string);

    if (json.isMember("kind") && json.isMember("data")) {
        if (const auto kind = json["kind"].asString(); !JsonToolkit::StringMatches(kind, "dataset--FileCollection.SEGY")) {
            throw SampleCode::Common::JsonParseException("GetFileCollectionSEGYFromJsonString: JSon string is not of kind [dataset--FileCollection.SEGY] !");
        }
        if (json["data"].isMember("DatasetProperties") && json["data"]["DatasetProperties"].isMember("FileSourceInfos")) {
            return json;
        }
        throw SampleCode::Common::JsonParseException("GetFileCollectionSEGYFromJsonString: This is not a properly populated FileCollection.SEGY Json string!");
    }
    throw SampleCode::Common::JsonParseException("GetFileCollectionSEGYFromJsonString: This is not a valid FileCollection.SEGY Json string!");
}

Json::Value
FileCollectionUtilities::GetFileCollectionSEGYJsonFromInputFile(const std::string& json_file_name)
{
    const auto generic_reader = JsonToolkit::CreateGenericFileReader(json_file_name);

    return GetFileCollectionSEGYJsonFromInputFile(generic_reader);
}

Json::Value
FileCollectionUtilities::GetFileCollectionSEGYJsonFromInputFile(const std::shared_ptr<segysdk::fileio::GenericFileReader>& reader)
{

    const std::string json_string = JsonToolkit::GetJsonFromFile(reader);

    if (json_string.empty() || json_string == emptyJson) {
        throw SampleCode::Common::JsonParseException("FileCollection.SEGY: JSon string cannot be empty!");
    }
    Json::Value json = GetFileCollectionSEGYFromJsonString(json_string);

    if (json.isMember("kind") && json.isMember("data")) {
        const auto kind = json["kind"].asString();
        if (!segysdk::utilities::StringUtils::keyMatches(kind, "dataset--FileCollection.SEGY")) {
            throw SampleCode::Common::JsonParseException("FileCollection.SEGY: JSon string is not of kind [dataset--FileCollection.SEGY] !");
        }
    }
    else {
        throw SampleCode::Common::JsonParseException("FileCollection.SEGY: This is not a valid FileCollection.SEGY Json string!");
    }
    return json;
}

void
FileCollectionUtilities::PopulateFileListFromFileCollection(const std::string& osduFileCollectionSEGYName, std::vector<std::string>& inputFileList)
{
    auto reader = JsonToolkit::CreateGenericFileReader(osduFileCollectionSEGYName);

    if (!reader->exists()) {
        std::cout << "Input file " << osduFileCollectionSEGYName << " does not exist." << std::endl;
    }
    auto fcJson = GetFileCollectionSEGYJsonFromInputFile(reader);
    if (fcJson.empty()) {
        throw SampleCode::Common::JsonParseException("FileCollection.SEGY cannot be empty!");
    }

    // Pick up required environment variables in case we need to access file in the cloud
    const std::string sdms_url = segysdk::utilities::Environment::instance().sdUrl();
    const std::string api_key = segysdk::utilities::Environment::instance().sdApiKey();

    if (!fcJson.isMember("data")) {
        throw SampleCode::Common::JsonParseException("PopulateFileListFromFileCollection: FileCollection.SEGY does not contain required node [DatasetProperties]");
    }
    Json::Value fcData = fcJson["data"];
    if (!fcData.isMember("DatasetProperties")) {
        throw SampleCode::Common::JsonParseException("PopulateFileListFromFileCollection: FileCollection.SEGY does not contain required node [data]");
    }
    Json::Value fcDsp = fcData["DatasetProperties"];
    if (!fcDsp.isMember("FileSourceInfos")) {
        throw SampleCode::Common::JsonParseException("PopulateFileListFromFileCollection: FileCollection.SEGY does not contain required node [FileSourceInfos]");
    }
    Json::Value fsiVector = fcDsp["FileSourceInfos"];

    for (Json::Value fsi : fsiVector) {
        if (!fsi.isMember("FileSource")) {
            throw SampleCode::Common::JsonParseException("PopulateFileListFromFileCollection: FileCollection.SEGY does not contain required node [FileSource]");
        }
        std::string segyFile = fsi["FileSource"].asString();
        if (!segyFile.empty()) {
            if (segysdk::fileio::GenericFileReader::isSDDataset(segyFile)) {
                if (segysdk::fileio::GenericFileReader::exists(segyFile,
                                                               sdms_url,
                                                               api_key,
                                                               segysdk::utilities::TokenSourceFactory::getTokenSource(),
                                                               segysdk::utilities::Settings::instance().getHttpHeaders())) 
                {
                    inputFileList.emplace_back(segyFile);
                }
            }
            else {
                if (segysdk::fileio::GenericFileReader::exists(segyFile)) {
                    inputFileList.emplace_back(segyFile);
                }
            }
        }
    }
}

} // namespace SampleCode::Common
