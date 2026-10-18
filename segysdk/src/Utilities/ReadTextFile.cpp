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

#include "ReadTextFile.h"

#include "StringUtils.h"

#include "../FileIO/FileReaderOptions.h"
#include "../FileIO/GenericFileManager.h"
#include "../FileIO/SDCredentials.h"
#include "../segysdk_details/SegyException.h"

namespace segysdk::utilities
{

bool
ReadTextFile::isCloudFile(const std::string& fileName)
{
    return fileio::GenericFileManager::isSDDataset(fileName);
}

fileio::FileReference
ReadTextFile::getFileReference(const std::string& fileName,
                               const std::string& sdms_app_key,
                               const std::string& sdms_url,
                               std::unique_ptr<segysdk::utilities::auth::TokenSource> tokenSource,
                               std::map<std::string, std::string> headers)
{
    if (isCloudFile(fileName)) {

        const SDCredentials sdCredentials{sdms_url, sdms_app_key, std::move(tokenSource), headers};

        const fileio::FileReaderOptions readerOptions{sdCredentials, CacheSizeOption{2 * 1024 * 1024, 32, 1, 1024U * 1024U}};

        return fileio::FileReference{fileName, readerOptions};
    }

    return fileio::FileReference{fileName, fileio::FileReaderOptions::localOnly()};
}

std::string
ReadTextFile::ReadAll(const std::string& fileName,
                      const std::string& sdms_app_key,
                      const std::string& sdms_url,
                      std::unique_ptr<segysdk::utilities::auth::TokenSource> tokenSource,
                      std::map<std::string, std::string> headers)
{
    if (isCloudFile(fileName)) {
        if (StringUtils::Trim_copy(sdms_app_key).empty()) {
            throw SegyInvalidArgumentException("ReadTextFile: sdms_app_key cannot be empty for  cloud based file [ " + fileName + " ]");
        }

        if (StringUtils::Trim_copy(sdms_url).empty()) {
            throw SegyInvalidArgumentException("ReadTextFile: sdms_url cannot be empty for  cloud based file [ " + fileName + " ]");
        }

        if (StringUtils::Trim_copy(tokenSource->token()).empty()) {
            throw SegyInvalidArgumentException("ReadTextFile: token cannot be empty for  cloud based file [ " + fileName + " ]");
        }

        std::string contents;

        const fileio::FileReference fileReference = getFileReference(fileName, sdms_app_key, sdms_url, std::move(tokenSource), headers);

        const size_t filesize = fileio::GenericFileManager::getSize(fileReference);
        if (filesize == 0)
            return contents;

        return fileio::GenericFileManager::readTextFile(fileio::FileManagerParams{fileReference}).text_;
    }

    const auto fr = getFileReference(fileName, sdms_app_key, sdms_url, std::move(tokenSource));

    if (!fileio::GenericFileManager::exists(fr)) {
        throw SegyInvalidArgumentException("ReadTextFile: Text file [ " + fileName + " ] not found.");
    }
    return fileio::GenericFileManager::readTextFile(fileio::FileManagerParams{fr}).text_;
}

} // namespace segysdk::utilities
