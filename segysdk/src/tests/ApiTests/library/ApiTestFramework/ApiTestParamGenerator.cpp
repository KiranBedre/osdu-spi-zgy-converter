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

#include "ApiTestParamGenerator.h"

#include <Utilities/Environment.h>
#include <Utilities/JsonUtilities.h>

#include <filesystem>

namespace segysdk::api::tests
{
std::list<ApiTestConfig>
ApiTestParamGenerator::get(const std::string& testName, const std::string& jsonConfigPath)
{
    Json::Value config = segysdk::utilities::JsonUtilities::GetJsonObjectFromFile(jsonConfigPath);
    std::list<ApiTestConfig> results;
    const auto& tests = config["tests"];
    for (const auto& value : tests) {
        std::string const name = value["name"].asString();
        if (name == testName) {
            for (const auto& data : value["data"]) {
                results.emplace_back(data);
            }
        }
    }
    return results;
}

std::string
ApiTestParamGenerator::getConfigFilePath(std::string path)
{
    std::filesystem::path testPath = segysdk::utilities::Environment::instance().SegyLocalTestDataPath();
    auto resultPath = testPath / path;
    return resultPath.string();
}
} // namespace segysdk::api::tests