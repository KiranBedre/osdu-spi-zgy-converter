/*
Copyright 2024 Schlumberger

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

#include "TmpCleanupFile.h"

#include "../Utilities/Environment.h"
#include "../FileIO/GenericFileManager.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <string>
#include <sstream>
#include <utility>
#include <vector>

static std::random_device rnd_dev;
static std::default_random_engine rnd(rnd_dev());
static std::uniform_int_distribution ud;

TmpCleanupFile::TmpCleanupFile(const std::string& source)
  : random_id_(std::to_string(ud(rnd))),
    path_(copyToTmp(source, random_id_))
{
}

TmpCleanupFile::TmpCleanupFile(const std::string& source, const std::string& randomId)
  : random_id_(randomId),
    path_(copyToTmp(source, random_id_))
{
}

TmpCleanupFile::~TmpCleanupFile()
{
    segysdk::fileio::FileReference fr(path_, segysdk::fileio::FileReaderOptions::localOnly());
    // Internally it'll check for file presence
    segysdk::fileio::GenericFileManager::remove(fr);
}

std::string
TmpCleanupFile::getRandomId() const
{
    return random_id_;
}

std::string
TmpCleanupFile::getPrefix() const
{
    return std::string(tmpPrefix) + random_id_;
}

std::string
TmpCleanupFile::copyToTmp(const std::string& source, const std::string& randomId)
{
    auto sourcePath = std::filesystem::path(source);
    auto tmpPath = (std::filesystem::temp_directory_path().append(tmpPrefix).concat(randomId)).concat(sourcePath.filename().string());

    std::ifstream inFile(sourcePath, std::ios::binary);
    std::ofstream tmpFile(tmpPath, std::ios::binary);
    tmpFile << inFile.rdbuf();
    inFile.close();
    tmpFile.close();

    return tmpPath.string();
}

void
TmpCleanupFile::replaceTmpTestDirMarkerWithPath()
{
    std::ifstream file(path_);

    if (!file.is_open())
        throw std::runtime_error("Could not open file:" + path_);

    std::stringstream buffer;
    buffer << file.rdbuf();

    std::string content = buffer.str();

    file.close();

    std::string parentPath = std::filesystem::path(path_).parent_path().append(getPrefix()).string();
#ifdef _WIN32
    // Use forward slashes on Windows to avoid compatibility issues with Linux
    std::replace(parentPath.begin(), parentPath.end(), '\\', '/');
#endif
    size_t startPos = 0;
    while ((startPos = content.find(TMP_TEST_MARKER, startPos)) != std::string::npos) {
        content.replace(startPos, TMP_TEST_MARKER.length(), parentPath);
        startPos += parentPath.length();
    }

    // Write the modified content back to the file
    std::ofstream outFile(path_);
    outFile << content;

    outFile.close();
}
