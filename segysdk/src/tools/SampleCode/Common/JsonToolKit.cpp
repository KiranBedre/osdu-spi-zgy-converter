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

#include "JsonToolKit.h"

#include "JsonParseException.h"

#include "../FileIO/FileIOException.h"
#include "../FileIO/FileReference.h"
#include "../FileIO/GenericFileReaderFactory.h"
#include "../Utilities/Environment.h"
#include "../Utilities/auth/TokenSource.h"
#include "../Utilities/TokenSourceFactory.h"
#include "../segysdk_details/SegyException.h"

#include <iostream>

namespace SampleCode::Common
{
/**
 * \brief Parses an input json string and returns a proper Json object
 * \param json_string string with the input json data
 * \return Json Object.
 */
Json::Value
JsonToolkit::ParseJsonString(const std::string& json_string)
{
    if (isValidJson(json_string)) {
        auto json = getJsonFromString(json_string);
        return json;
    }
    std::cout << "ParseJsonString: Not a valid Json string... " << std::endl;
    throw SampleCode::Common::JsonParseException("ParseJsonString: Not a valid Json string... ");
}

/**
 * \brief Utility method to check if input string str contains substring match
 * \param str input string to check
 * \param match string providing the matching pattern
 * \return true if match can be found in str, otherwise false
 */
bool
JsonToolkit::StringMatches(const std::string& str, const std::string& match)
{
    return str.find(match) != std::string::npos;
}

/**
 * \brief Checks if a string is empty or only contains CR or CRLF characters
 * \param str  input string
 * \return true if str is empty or if it only contains CR or LF characters, otherwise false
 */
bool
JsonToolkit::isEmptyString(const std::string& str)
{
    for (auto& c : str) {

        const bool isEmpty = std::string::npos == str.find_first_not_of(" \r\n");
        if (!isEmpty) {
            std::cout << " string not empty - encountered [" << std::to_string(c) << "]" << std::endl;
            return false;
        }
    }
    return true;
}

/**
 * \brief Perform cursory checks to validate if the input string is a valid Json structure.
 * \param json_string string holding the json structure
 * \return true if the string is not empty and it contains at least "{" and at some point further into the string it contains a "}"
 */
bool
JsonToolkit::isValidJson(const std::string& json_string)
{
    const auto fpos = json_string.find("{");
    if (fpos == std::string::npos)
        return false;

    if (fpos > 0) {
        if (!isEmptyString(json_string.substr(0, fpos)))
            return false;
    }

    const auto lpos = json_string.rfind("}");
    if (lpos == std::string::npos)
        return false;

    if (lpos + 1 >= json_string.size())
        return true;

    return isEmptyString(json_string.substr(lpos + 1));
}

/**
 * \brief Utility method to provide an appropriate reader for an input text file (either local or located in SDMS cloud storage
 * \param fileName  std::string with a valid input file name. A file whose name begins with sd:// is assumed to be in SDMS cloud storage
 * \return std::shared_ptr<segysdk::fileio::GenericFileReader> Generic file reader to be used for text files.
 */
std::shared_ptr<segysdk::fileio::GenericFileReader>
JsonToolkit::CreateGenericFileReader(const std::string& fileName)
{

    // Pick up required environment variables in case we need to access file in the cloud
    const std::string sdms_url = segysdk::utilities::Environment::instance().sdUrl();
    const std::string api_key = segysdk::utilities::Environment::instance().sdApiKey();
    auto tokenSource = segysdk::utilities::TokenSourceFactory::getTokenSource();

    if (segysdk::fileio::GenericFileReader::isSDDataset(fileName)) {
        if (sdms_url.empty()) {
            const std::string msg = "Environment variable not defined for accessing cloud based file: [SD_SVC_URL]";
            std::cout << msg << std::endl;
            throw segysdk::fileio::FileIOException(msg);
        }
        if (api_key.empty()) {
            const std::string msg = "Environment variable not defined for accessing cloud based file: [SD_SVC_API_KEY]";
            std::cout << msg << std::endl;
            throw segysdk::fileio::FileIOException(msg);
        }
        if (not tokenSource
            or tokenSource->token().empty()) {
            const std::string msg = "Environment variable not defined for accessing cloud based file: [SD_SVC_TOKEN]";
            std::cout << msg << std::endl;
            throw segysdk::fileio::FileIOException(msg);
        }
    }
    const auto factory = segysdk::fileio::GenericFileReader::isSDDataset(fileName)
                             ? segysdk::api::GenericFileReaderFactory(fileName, sdms_url, api_key, std::move(tokenSource))
                             : segysdk::api::GenericFileReaderFactory(fileName);
    auto reader = factory.CreateGenericFileReader();
    return reader;
}

/**
 * \brief Utility method to extract a string from input file name
 * \param osduFileCollectionJson std::string with a valid input file name. A file whose name begins with sd:// is assumed to be in SDMS cloud storage
 * \return std::string with the contents of the file
 */
std::string
JsonToolkit::GetJsonFromFile(const std::string& osduFileCollectionJson)
{

    // Use FileCollectionUtilities to read input json file
    const auto generic_reader = CreateGenericFileReader(osduFileCollectionJson);

    if (!generic_reader->exists()) {
        std::cout << "GetJsonFromFile: Input file " << osduFileCollectionJson << " does not exist!" << std::endl;
        throw segysdk::fileio::FileIOException(std::string("GetJsonFromFile: input file " + osduFileCollectionJson + " does not exist"));
    }
    return GetJsonFromFile(generic_reader);
}

/**
 * \brief Utility method to read full contents of the file pointed to by the generic file reader
 * \param reader generic file reader
 * \return std::string with full contents of the file.
 */
std::string
JsonToolkit::GetJsonFromFile(const std::shared_ptr<segysdk::fileio::GenericFileReader>& reader)
{
    std::string result = "{}";

    if (!reader->exists(reader->getFileReference().fileName())) {
        throw segysdk::fileio::FileIOException("GetJsonFromFile: Input file [ " + reader->getFileReference().fileName() + "] does not exist!");
    }

    const auto fileSize = reader->getSize();
    if (fileSize > 0) {
        result = reader->readText();
    }
    if (!isValidJson(result)) {
        throw SampleCode::Common::JsonParseException("GetJsonFromFile: This is not a Json string!");
    }

    return result;
}

/**
 * \brief Parses an input string and converts it to a valid Json structure
 * \param json_string json structure in a single string. A value of "{}" indicates an empty Json element
 * \return Json structure
 */
Json::Value
JsonToolkit::getJsonFromString(const std::string& json_string)
{
    const Json::CharReaderBuilder builder;
    const std::unique_ptr<Json::CharReader> reader(builder.newCharReader());

    Json::Value json;
    std::string errors;

    const bool parsingSuccessful = reader->parse(
        json_string.c_str(),
        json_string.c_str() + json_string.size(),
        &json,
        &errors);

    if (!parsingSuccessful) {
        std::string msg = "Failed to parse the JSON string. Errors:\n";
        msg += errors;
        throw std::invalid_argument(msg);
    }

    return json;
}

} // namespace SampleCode::Common
