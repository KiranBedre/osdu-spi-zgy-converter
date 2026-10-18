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

#pragma once

#include "ApiTestException.h"
#include "../common/TypeChecks.h"

#include <filesystem>
#include <json/json.h>

namespace segysdk::api::tests
{
class ApiJsonPlaceholders
{
public:
    inline static const std::string PARENT_DIR = "${PARENTDIR}";
    inline static const std::string DATE_TIME = "${DATE_TIME}";
};

std::filesystem::path
getTestdataPath();

std::filesystem::path
getTestdataPath(const std::string& relPath);

bool
compareFiles(const std::filesystem::path& firstPath, const std::filesystem::path& secondPath);

void
dumpTextFile(const std::filesystem::path& path, const std::string& text);

std::string
readTextFile(const std::filesystem::path& path);

void
dumpFile(const std::filesystem::path& path, const char* buffer, const size_t size);

void
dumpJsonFile(const std::filesystem::path& path, const Json::Value& json);

std::string
updateDatasetPathInString(const std::string& value, const std::filesystem::path& parentDir);

void
updateDatasetPathInJsonBaseline(Json::Value& json, const std::filesystem::path& parentDir);

std::filesystem::path
addDumpSuffix(std::filesystem::path path);

bool
compareJson(const Json::Value& lhs, const Json::Value& rhs);
} // namespace segysdk::api::tests