/*
Copyright 2021 Schlumberger

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

#include <json/json.h>

#include <string>

#include <filesystem>

namespace segysdk::segyindex::tests
{

enum class DataGridType
{
    Regular = 0,
    NonRegular
};

class UnitTestDataProvider
{
private:
    std::filesystem::path dataFilePath_;
    std::filesystem::path regular_;
    std::filesystem::path nonRegular_;

private:
    static std::string getJsonString(const std::filesystem::path& filePath);
    std::filesystem::path evaluateTestDataRootFolder() const;
    std::filesystem::path evaluateRegularFilesFolder() const;
    std::filesystem::path evaluateNonRegularFilesFolder() const;

public:
    UnitTestDataProvider();

public:
    Json::Value getDataAsJson(const std::string& jsonFileName, DataGridType gridType) const;

    std::filesystem::path getRegularFilesFolder() const;
    std::filesystem::path getNonRegularFilesFolder() const;
};

} // namespace segysdk::segyindex::tests
