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

#include "Settings.h"

#include "../FileIO/FileIOException.h"
#include "../FileIO/FileReference.h"
#include "../FileIO/GenericFileManager.h"
#include "../FileIO/GenericFileReaderFactory.h"
#include "../Utilities/Authorization.h"
#include "../Utilities/JsonException.h"
#include "../Utilities/JsonUtilities.h"
#include "../Utilities/SessionUtilities.h"
#include "../Utilities/StorageSvcClient.h"
#include "../Utilities/StringUtils.h"
#include "../Utilities/TokenSourceFactory.h"

#include <filesystem>
#include <iostream>

namespace segysdk::utilities
{

bool
JsonUtilities::isOsduId(const std::string& osduId)
{
    // valid OSDU id:
    // "partition:reference-data--BinGridDefinitionMethodType:P6:"
    const auto id_parts = StringUtils::split(osduId, ":");

    // A valid OSDU has at least 2 colons, which will split into at least 3 parts
    return id_parts.size() > 2;
}

bool
JsonUtilities::isValidOSDUReferenceIdString(const std::string& idValue)
{
    return isOsduId(idValue) or idValue.find("sd://") == 0;
}

bool
JsonUtilities::isLocalFile(const std::string& filePath)
{
    if (isValidOSDUReferenceIdString(filePath)) {
        return false;
    }

    // expect filePath to be either sd:// or  it is a local file
    return !segysdk::fileio::GenericFileManager::isSDDataset(filePath);
}

segysdk::fileio::FileReaderOptions
JsonUtilities::getFileReaderOptions(const std::string& fileName, const std::optional<std::string>& legalTag)
{
    if (isLocalFile(fileName)) {
        return segysdk::fileio::FileReaderOptions::localOnly();
    }

    const segysdk::SDCredentials credentials(
        segysdk::utilities::Environment::instance().sdUrl(),
        segysdk::utilities::Environment::instance().sdApiKey(),
        segysdk::utilities::TokenSourceFactory::getTokenSource(),
        segysdk::utilities::Settings::instance().getHttpHeaders());

    return {credentials, SessionUtilities::getSDCacheSize(), legalTag};
}

segysdk::fileio::FileReference
JsonUtilities::getFileReference(const std::string& fileName, const std::optional<std::string>& legalTag)
{
    auto options = getFileReaderOptions(fileName, legalTag);
    return segysdk::fileio::FileReference(fileName, options);
}

std::shared_ptr<fileio::GenericFileReader>
JsonUtilities::CreateGenericFileReader(const std::string& fileName)
{

    const std::string sdms_url = Environment::instance().sdUrl();
    const std::string api_key = Environment::instance().sdApiKey();

    const auto factory = fileio::GenericFileReader::isSDDataset(fileName)
                             ? api::GenericFileReaderFactory(fileName, sdms_url, api_key, TokenSourceFactory::getTokenSource())
                             : api::GenericFileReaderFactory(fileName);
    auto reader = factory.CreateGenericFileReader();

    return reader;
}

bool
isEmptyString(const std::string& str)
{
    const bool isEmpty = std::string::npos == str.find_first_not_of(" \r\n");
    if (!isEmpty) {
        return false;
    }

    return true;
}

/**
 * \brief Perform cursory checks to validate if the input string is a valid Json structure.
 * \param json_string string holding the json structure
 * \return true if the string is not empty and it contains at least "{" and at some point further into the string it contains a "}"
 */
bool
isValidJson(const std::string& json_string)
{
    const auto fpos = json_string.find('{');
    if (fpos == std::string::npos) {
        return false;
    }

    if (fpos > 0) {
        if (!isEmptyString(json_string.substr(0, fpos))) {
            return false;
        }
    }

    const auto lpos = json_string.rfind('}');
    if (lpos == std::string::npos) {
        return false;
    }

    if (lpos + 1 >= json_string.size()) {
        return true;
    }

    return isEmptyString(json_string.substr(lpos + 1));
}

static auto
makePathOrOsduErrorMessage(const std::string& functionName)
{
    return functionName + ": pathOrOsduId cannot be empty";
}

/**
 * \brief Utility method to extract a Json object from an OSDU record file or id
 * \param pathOrOsduId std::string with a valid input OSDU record id or valid file path to the relevant metadata file
 * \return Json::Value with from contents of the file pointed to by the pathOrOsduId
 */
Json::Value
JsonUtilities::getJsonObjectFromPathOrOsduId(const std::string& pathOrOsduId)
{
    if (pathOrOsduId.empty()) {
        throw SegyInvalidArgumentException(makePathOrOsduErrorMessage(__func__));
    }

    const auto result = StringUtils::split(pathOrOsduId, ":");

    if (result.size() < 3) {
        return GetJsonObjectFromFile(pathOrOsduId);
    }

    return getJsonFromString(GetJsonFromId(pathOrOsduId));
}

/**
 * \brief Utility method to extract a Json string from an OSDU record file or id
 * \param pathOrOsduId std::string with a valid input OSDU record id or valid file path to the relevant metadata file
 * \return string with from contents of the file pointed to by the pathOrOsduId
 */
std::string
JsonUtilities::getJsonStringFromPathOrOsduId(const std::string& pathOrOsduId)
{
    if (pathOrOsduId.empty()) {
        throw SegyInvalidArgumentException(makePathOrOsduErrorMessage(__func__));
    }

    const auto result = StringUtils::split(pathOrOsduId, ":");

    if (result.size() < 3) {
        return GetJsonStringFromFile(pathOrOsduId);
    }

    return GetJsonFromId(pathOrOsduId);
}

/**
 * \brief Utility method to extract a Json object from an OSDU record id
 * \param recordId std::string with a valid input OSDU record id
 * \return Json::Value with from contents of the file pointed to by the record id
 */
std::string
JsonUtilities::GetJsonFromId(const std::string& recordId)
{
    if (recordId.empty()) {
        throw SegyInvalidArgumentException(std::string("GetJsonFromId: recordId cannot be empty"));
    }

    const StorageSvcClient storageSvcClient_(Authorization::storageSvcTokenProvider());
    auto result = storageSvcClient_.getRecord(recordId);

    return result;
}

/**
 * \brief Utility method to extract a string from input file name
 * \param fileName std::string with a valid input file name. A file whose name begins with sd:// is assumed to be in SDMS cloud storage
 * \return std::string with the contents of the file
 */
std::string
JsonUtilities::GetJsonStringFromFile(const std::string& fileName)
{
    if (fileName.empty()) {
        return "{}";
    }

    // Use FileCollectionUtilities to read input json file
    const auto generic_reader = CreateGenericFileReader(fileName);

    if (generic_reader == nullptr || !generic_reader->exists()) {
        throw fileio::FileIOException(std::string("GetJsonStringFromFile: input file " + fileName + " does not exist"));
    }

    return GetJsonStringFromFile(generic_reader);
}

/**
 * \brief Utility method to read full contents of the file pointed to by the generic file reader
 * \param reader generic file reader
 * \return std::string with full contents of the file.
 */
std::string
JsonUtilities::GetJsonStringFromFile(const std::shared_ptr<fileio::GenericFileReader>& reader)
{
    std::string result = "{}";

    if (!reader->exists()) {
        throw fileio::FileIOException("GetJsonFromFile: Input file [ " + reader->getFileReference().fileName() + "] does not exist!");
    }

    const auto fileSize = reader->getSize();
    if (fileSize > 0) {
        result = reader->readText();
    }
    if (!isValidJson(result)) {
        throw JsonParseException("GetJsonFromFile: This is not a Json string!");
    }

    return result;
}

Json::Value
JsonUtilities::GetJsonObjectFromFile(const std::string& fileName)
{
    return getJsonFromString(GetJsonStringFromFile(fileName));
}

Json::Value
JsonUtilities::getJsonFromString(const std::string& jsonString)
{
    std::istringstream stream(jsonString, std::ios::binary);
    return getJsonFromStream(stream);
}

Json::Value
JsonUtilities::getJsonFromStream(std::istream& stream)
{
    std::string errors;
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    Json::Value root;

    if (!parseFromStream(builder, stream, &root, &errors)) {
        throw JsonParseException("Error parsing JSON: " + errors);
    }

    return root;
}

bool
JsonUtilities::isValidJson(const std::string& json_string)
{
    const auto fpos = json_string.find('{');
    if (fpos == std::string::npos) {
        return false;
    }

    if (fpos > 0) {
        if (!isEmptyString(json_string.substr(0, fpos))) {
            return false;
        }
    }

    const auto lpos = json_string.rfind('}');
    if (lpos == std::string::npos) {
        return false;
    }

    if (lpos + 1 >= json_string.size()) {
        return true;
    }

    return isEmptyString(json_string.substr(lpos + 1));
}

} // namespace segysdk::utilities
