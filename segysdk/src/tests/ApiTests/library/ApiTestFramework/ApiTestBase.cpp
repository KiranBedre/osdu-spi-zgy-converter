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

#include "ApiTestBase.h"
#include <json/json.h>
#include <iostream>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>

namespace segysdk::api::tests
{

std::string
getTimeStamp()
{
    time_t rawTime;
    (void)time(&rawTime);
    const tm* timeInfo = localtime(&rawTime);

    std::ostringstream timeStamp;
    timeStamp << std::put_time(timeInfo, "%d%m%Y_%H%M%S");

    return timeStamp.str();
}

std::filesystem::path
getOutputFileName(std::filesystem::path failedTestsPath, const std::string& resultType)
{
    const auto timeStamp = getTimeStamp();
    const std::string actualFileName = resultType + "_" + timeStamp + ".txt";

    const auto outputFile = failedTestsPath / actualFileName;
    if (std::filesystem::exists(outputFile))
    {
        std::filesystem::remove(outputFile);
    }
    return outputFile;
}

void
ApiTestBase::SetUp()
{
    const auto& params = GetParam();
    if (params.getType() == ResultType::R_FILE || params.getType() == ResultType::R_JSON_FILE) {
        cleanup_.addToCleanup(params.getExpectedFile());
    }
}

void
ApiTestBase::TearDown()
{
    if (not ApiEnvironment::instance().keepOutputFiles()) {
        cleanup_.cleanup();
    }
}

void
ApiTestBase::addToCleanup(std::filesystem::path filePath)
{
    cleanup_.addToCleanup(filePath);
}

void
ApiTestBase::copyExpectedFileAsBaselineDump(const std::filesystem::path& baselinePath)
{
    std::filesystem::copy_file(GetParam().getExpectedFile(), addDumpSuffix(baselinePath), std::filesystem::copy_options::overwrite_existing);
}

void
ApiTestBase::checkJson(const Json::Value& result)
{
    auto baselinePath = GetParam().getBaselineJsonFilePath();
    if (ApiEnvironment::instance().dumpBaselines()) {
        dumpJsonFile(addDumpSuffix(baselinePath), result);
    }

    Json::Value expectedJson = JsonUtilities::GetJsonObjectFromFile(baselinePath.string());
    updateDatasetPathInJsonBaseline(expectedJson, getTestdataPathParameter("dataset").parent_path());

#if defined(_WIN32)
    try {

        constexpr bool debugAPITest = true;

        if (debugAPITest and not compareJson(result, expectedJson)) {
            auto testName = baselinePath.has_filename()
                                        ? baselinePath.filename().string()
                                        : "Unnamed_test";

            if (baselinePath.has_extension()) {
                const auto len = testName.find(baselinePath.extension().string());
                testName = testName.substr(0, len);
            }

            const auto apiTestRoot = "D:/Temp/ApiTest/";

            auto error_root = std::filesystem::path(apiTestRoot);
            if (!std::filesystem::exists(error_root)) {
                std::filesystem::create_directories(error_root);
            }
            const auto testFileName = testName + "_" + getTimeStamp() + ".json";
            auto result_path = error_root / testFileName;

            Json::Value resultsJson;
            resultsJson["TestName"] = testName;
            resultsJson["Result"] = result;
            resultsJson["Expected"] = expectedJson;

            std::ofstream errorFile(result_path);
            errorFile << resultsJson.toStyledString() << "\n";
            errorFile.close();      
        }
    }
    catch (const std::exception& e) {
        std::cerr << "DebugApiTest: Exception caught: " << e.what() << std::endl;     
    }

#endif

    EXPECT_TRUE(compareJson(result, expectedJson));
}

void
ApiTestBase::checkJson(const std::filesystem::path& jsonFilePath)
{
    Json::Value json = segysdk::utilities::JsonUtilities::GetJsonObjectFromFile(jsonFilePath.string());
    checkJson(json);
}

std::filesystem::path
ApiTestBase::getTestdataPathParameter(const std::string& name)
{
    return getTestdataPath(getParameter<std::string>(name));
}

std::filesystem::path
ApiTestBase::getCurrentPathParameter(const std::string& name)
{
    return std::filesystem::current_path() / getParameter<std::string>(name);
}
} // namespace segysdk::api::tests