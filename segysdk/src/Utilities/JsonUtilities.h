/*
Copyright 2021-2023 Schlumberger

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

#include "../FileIO/GenericFileReader.h"

#include <json/json.h>
#include <memory>
#include <string>

namespace segysdk::utilities
{

struct SEGYSDK_DLL_PUBLIC JsonUtilities
{
    inline static const std::string EmptyJson = "{}";

    /**
     * @brief Utility method validate if an id string is a correctly formatted OSDU id
     * @details A correctly formatted id string has the following expected layout:
     *          <partition>:<data type>:<name or unique number>
     *          e.g.: 
     *          "id": "opendes:dataset--FileCollection.SEGY:60e1cb1a-abcd-4113-85fb-143d48eea925",
     * @param osduId  std::string with with the id to be validated
     * @return true if id has expected format, false otherwise
     */
    static bool isOsduId(const std::string& osduId);

    /**
     * @brief Utility method validate if a reference id string is a correctly formatted OSDU reference id
     * @details A correctly formatted reference id string has the following expected layout:
     *          <partition>:<data type>:<name or unique number>:<version or empty for latest version>
     *          e.g.:
     *          "SchemaFormatTypeID": "opendes:reference-data--SchemaFormatType:SEGY:",
     * @param idValue  std::string with with the id to be validated
     * @return true if id has expected format, false otherwise
     */

    static bool isValidOSDUReferenceIdString(const std::string& idValue);

    /**
     * @brief Utility method to check if a file name indicates a local file or a cloud hosted file
     * @details A windows disk file normally starts with a drive letter, followed by a path 
     *          A cloud hosted file will have either a reference id as filePath, or possibly
     *          a valid sd: path.
     *          e.g. a valid windows path could be: "D:\Temp\temp.sgy"
     *          A valid sd-path could be: "sd://opendes/seismic/test.sgy"
     *          A valid id could be:  "opendes:dataset--FileCollection.SEGY:60e1cb1a-abcd-4113-85fb-143d48eea925:",
     *       
     * @param filePath  std::string with with the path to be validated
     * @return true if filePath indicates local file path, false indicates cloud stored file
     */
    static bool isLocalFile(const std::string& filePath);

    /**
     * @brief Utility method to generate fileReaderOptions or fileWriterOptions
     * @details The readerOptions contains caching information and also all required 
     *          parameters to access a file stored in the cloud
     * @param fileName  std::string with with the path to the file. Either a local file path or a sd-path
     * "param legalTag  string parameter holding a legalTag value for cloud-hosted files. 
     * @return  Appopriate FileReaderOptions for the specified fileName
     */
    static segysdk::fileio::FileReaderOptions getFileReaderOptions(const std::string& fileName, const std::optional<std::string>& legalTag);

    /**
     * @brief Utility method to generate fileReference
     * @details The fileReference contains all requiredparameters
     *           to access a file stored in the cloud
     * @param fileName  std::string with with the path to the file. Either a local file path or a sd-path
     * "param legalTag  string parameter holding a legalTag value for cloud-hosted files.
     * @return  Appopriate FileReaderOptions for the specified fileName
     */
    static segysdk::fileio::FileReference getFileReference(const std::string& fileName, const std::optional<std::string>& legalTag);

    /**
     * @brief Utility method to provide an appropriate reader for an input text file (either local or located in SDMS cloud storage
     * @param fileName  std::string with a valid input file name. A file whose name begins with sd:// is assumed to be in SDMS cloud storage
     * @return std::shared_ptr<bulkaccess::fileio::GenericFileReader> Generic file reader to be used for text files.
     */
    static std::shared_ptr<fileio::GenericFileReader> CreateGenericFileReader(const std::string& fileName);

    static Json::Value getJsonObjectFromPathOrOsduId(const std::string& pathOrOsduId);
    static std::string getJsonStringFromPathOrOsduId(const std::string& pathOrOsduId);

    static std::string GetJsonFromId(const std::string& recordId);
    static std::string GetJsonStringFromFile(const std::string& fileName);
    static std::string GetJsonStringFromFile(const std::shared_ptr<fileio::GenericFileReader>& reader);
    static Json::Value GetJsonObjectFromFile(const std::string& fileName);
    static Json::Value getJsonFromString(const std::string& jsonString);
    static Json::Value getJsonFromStream(std::istream& stream);
    static bool isValidJson(const std::string& json_string);
};

} // namespace segysdk::utilities
