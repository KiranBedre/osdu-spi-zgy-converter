/*
Copyright 2023 Schlumberger

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

#include "../OsduMetaDataUtilities/OsduMetafileApi.h"
#include "../Utilities/Environment.h"
#include "../Utilities/JsonException.h"
#include "../Utilities/JsonUtilities.h"
#include "../Utilities/StringUtils.h"

#include <gtest/gtest.h>
#include <json/json.h>
#include <string>

namespace segysdk::osduMetadata::OsduSeismicTraceDataFullyPopulatedTest
{
using namespace segysdk::metadata::osdu;

Json::Value getTestFileJson(const std::string& jsonFileName)
{
    const std::string testFolderPath = segysdk::utilities::Environment::instance().SegyLocalTestDataPath();
    if (!std::filesystem::exists(testFolderPath)) {
        const std::string msg = "Input Test Folder path does not exist! " + testFolderPath;
        SEGYLOG_REPORT_ERROR(msg);
        throw(segysdk::utilities::JsonParseException(msg));
    }

    auto testJsonPath = std::filesystem::path(testFolderPath) / jsonFileName;
    if (!std::filesystem::exists(testJsonPath)) {
        const std::string msg = "Input Json file does not exist! " + testJsonPath.string();
        SEGYLOG_REPORT_ERROR(msg);
        throw(segysdk::utilities::JsonParseException(msg));
    }

    return segysdk::utilities::JsonUtilities::GetJsonObjectFromFile(testJsonPath.string());
}

std::string
getPartitionFromId(const std::string& id)
{
    const auto parts = segysdk::utilities::StringUtils::split(id, ":");
    if (parts.size() <= 2) {
        return "dummy";
    }
    return parts.at(0);
}

std::shared_ptr<segysdk::metadata::osdu::OsduSeismicTraceDataStruct>
getMetadataStructFromJson(const std::string& jsonFileName)
{
    auto myJson = getTestFileJson(jsonFileName);
    auto myPartition = myJson.isMember("id")
                           ? getPartitionFromId(myJson["id"].asString())
                           : "opendes";
   
    [[maybe_unused]] const std::vector<std::string> members = myJson.getMemberNames();

    auto OsduSeismicTraceData = segysdk::metadata::osdu::OsduSeismicTraceData(myJson, myPartition);
    return std::dynamic_pointer_cast<segysdk::metadata::osdu::OsduSeismicTraceDataStruct>(OsduSeismicTraceData.getMetaDataStruct());
}

TEST(OsduSeismicTraceDataFullyPopulatedTest, CheckIfFieldsSurviveBeingLoaded)
{
    const auto seismicTraceDataStruct_ptr = getMetadataStructFromJson("OSDU/Metadata/SeismicTraceData_Consistent.json");
    ASSERT_FALSE(seismicTraceDataStruct_ptr == nullptr);
    
    auto updatedSTD = segysdk::metadata::osdu::OsduSeismicTraceData(*seismicTraceDataStruct_ptr);
    const auto exportedJson = updatedSTD.getAsFormattedString();

    auto posCheck1 = exportedJson.find("BinGridID");
    ASSERT_FALSE(posCheck1 == std::string::npos);

    auto posCheck2 = exportedJson.find("CrosslineIncrement");
    ASSERT_FALSE(posCheck2 == std::string::npos);

    auto posCheck3 = exportedJson.find("CrosslineMax");
    ASSERT_FALSE(posCheck3 == std::string::npos);

    auto posCheck4 = exportedJson.find("Description");
    ASSERT_FALSE(posCheck4 == std::string::npos);

    auto posCheck5 = exportedJson.find("Datasets");
    ASSERT_FALSE(posCheck5 == std::string::npos);
}

TEST(OsduSeismicTraceDataFullyPopulatedTest, VerifyInconsistentMetafileFails)
{
    const auto seismicTraceDataStruct_ptr = getMetadataStructFromJson("OSDU/Metadata/SeismicTraceData_Inconsistent.json");
    ASSERT_FALSE(seismicTraceDataStruct_ptr == nullptr);

    ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties.has_value());
    ASSERT_FALSE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties.value().binGridID == std::nullopt);
    ASSERT_FALSE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties.value().crosslineMax == std::nullopt);
    ASSERT_FALSE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties.value().crosslineMin == std::nullopt);
    ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties.value().commonResources.has_value());
    ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties.value().wpcGroupTypeStruct.has_value());
    ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties.value().wpcGroupTypeStruct.value().datasets.has_value());
    ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties.value().principalAcquisitionProjectID == std::nullopt);
    ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties.value().processingProjectID == std::nullopt);


    auto updatedSTD = segysdk::metadata::osdu::OsduSeismicTraceData(*seismicTraceDataStruct_ptr);
    const auto exportedJson = updatedSTD.getAsFormattedString();
    ASSERT_FALSE(exportedJson.empty());
}

} // namespace segysdk::osduMetadata::OsduSeismicTraceDataFullyPopulatedTest
