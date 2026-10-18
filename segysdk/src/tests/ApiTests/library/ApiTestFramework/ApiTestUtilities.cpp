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

#include "ApiTestUtilities.h"
#include "ApiTestException.h"

#include <FileIO/GenericFileManager.h>
#include <Utilities/Environment.h>
#include <Utilities/JsonUtilities.h>

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <fstream>

namespace segysdk::api::tests
{
using namespace segysdk::fileio;
using namespace segysdk::utilities;

std::vector<std::string>
parseJsonStringArray(const Json::Value& json)
{
    if (not json.isArray()) {
        throw ApiTestException("Expecting JSON array");
    }

    std::vector<std::string> array;
    std::for_each(json.begin(), json.end(), [&array](const Json::Value& value) { array.emplace_back(value.asString()); });
    return array;
}

std::filesystem::path
getTestdataPath()
{
    static const std::filesystem::path testdataPath{segysdk::utilities::Environment::instance().SegyLocalTestDataPath()};
    return testdataPath;
}

std::filesystem::path
getTestdataPath(const std::string& relPath)
{
#ifdef _WIN32
    // Use forward slashes on Windows to avoid compatibility issues with Linux
    auto path = (getTestdataPath() / relPath).string();
    std::replace(path.begin(), path.end(), '\\', '/');
    return path;
#else
    return (getTestdataPath() / relPath);
#endif
}

bool
compareFiles(const std::filesystem::path& firstPath, const std::filesystem::path& secondPath)
{
    std::ifstream first(firstPath, std::ios::in | std::ios::ate | std::ios::binary);
    std::ifstream second(secondPath, std::ios::in | std::ios::ate | std::ios::binary);

    if (first.fail() || second.fail()) {
        return false;
    }

    if (first.tellg() != second.tellg()) {
        return false;
    }

    first.seekg(0);
    second.seekg(0);

    std::istreambuf_iterator<char> iFirst(first);
    std::istreambuf_iterator<char> iSecond(second);

    return std::equal(iFirst, std::istreambuf_iterator<char>(), iSecond);
}

void
dumpTextFile(const std::filesystem::path& path, const std::string& text)
{
    if (not GenericFileManager::writeTextFile(FileReference(path.string(), FileReaderOptions::localOnly()), text, true)) {
        throw ApiTestException("Failed to write text file: " + path.string());
    }
}

std::string
readTextFile(const std::filesystem::path& path)
{
    auto [success, text] = GenericFileManager::readTextFile(fileio::FileManagerParams{{path.string(), FileReaderOptions::localOnly()}});
    if (!success) {
        throw ApiTestException("Failed to read text file: " + path.string());
    }
    return text;
}

void
dumpFile(const std::filesystem::path& path, const char* buffer, const size_t size)
{
    std::ofstream out(path.string(), std::ios::binary | std::ios::out | std::ios::trunc);
    out.write(buffer, size);
    if (not out.good())
        throw ApiTestException("Error writing to - " + path.string() + " with error: " + std::strerror(errno));
}

void
dumpJsonFile(const std::filesystem::path& path, const Json::Value& json)
{
    Json::StyledWriter jsonWriter;
    std::ofstream out(path);
    out << jsonWriter.write(json);
}

std::string
updateDatasetPathInString(const std::string& value, const std::filesystem::path& parentDir)
{
    std::string result = value;
    std::string replacement = (parentDir / "").string();
#ifdef _WIN32
    // Use forward slashes on Windows to avoid compatibility issues with Linux
    std::replace(replacement.begin(), replacement.end(), '\\', '/');
#endif
    size_t pos = 0;
    while ((pos = result.find(ApiJsonPlaceholders::PARENT_DIR)) != std::string::npos) {
        result.replace(pos, ApiJsonPlaceholders::PARENT_DIR.size(), replacement);
    }
    return result;
}

void
updateDatasetPathInJsonBaseline(Json::Value& json, const std::filesystem::path& parentDir)
{
    if (json.isString()) {
        std::string strValue = json.asString();
        json = updateDatasetPathInString(strValue, parentDir);
    }
    else if (json.isArray() || json.isObject()) {
        for (auto& element : json) {
            updateDatasetPathInJsonBaseline(element, parentDir);
        }
    }
}

std::filesystem::path
addDumpSuffix(std::filesystem::path path)
{
    return path.replace_filename(path.filename().stem().string() + ".dmp" + path.extension().string());
}

bool
compareJson(const Json::Value& lhs, const Json::Value& rhs)
{
    constexpr const double precision = 1e-9;

    if (lhs.isNumeric() && rhs.isNumeric()) {
        if (lhs.isInt64() && rhs.isInt64()) {
            return lhs.asInt64() == rhs.asInt64();
        }
        if (lhs.isUInt64() && rhs.isUInt64()) {
            return lhs.asUInt64() == rhs.asUInt64();
        }
        return std::abs(lhs.asDouble() - rhs.asDouble()) < precision;
    }

    if (lhs.isArray() && rhs.isArray()) {
        if (lhs.size() != rhs.size()) {
            return false;
        }
        bool result = true;
        auto lhsEl = lhs.begin();
        auto rhsEl = rhs.begin();
        for (; result && lhsEl != lhs.end(); lhsEl++, rhsEl++) {
            result &= compareJson(*lhsEl, *rhsEl);
        }
        return result;
    }

    if (lhs.isObject() && rhs.isObject()) {
        if (lhs.size() != rhs.size()) {
            return false;
        }
        bool result = true;
        auto lhsEl = lhs.begin();
        auto rhsEl = rhs.begin();
        for (; result && lhsEl != lhs.end(); lhsEl++, rhsEl++) {
            result &= (lhsEl.key() == rhsEl.key()) && compareJson(*lhsEl, *rhsEl);
        }
        return result;
    }

    if (lhs == ApiJsonPlaceholders::DATE_TIME or rhs == ApiJsonPlaceholders::DATE_TIME) {
        // Date or time values should be skipped
        return true;
    }

    return lhs == rhs;
}

} // namespace segysdk::api::tests