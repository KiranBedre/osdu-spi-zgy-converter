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

#include "../FileIO/GenericFileReader.h"

#include "../FileIO/FileReaderOptions.h"
#include "../FileIO/FileReference.h"
#include "../FileIO/GenericFileManager.h"
#include "../FileIO/SDCredentials.h"
#include "../segysdk_details/SegyException.h"

namespace segysdk::fileio
{
GenericFileReader::~GenericFileReader() = default;

bool
GenericFileReader::exists(const std::string& fileName)
{
    if (isSDDataset(fileName))
        throw SegyInvalidArgumentException("GenericFileReader::exists: Provided file is cloud hosted, cloud connection parameters required.");

    const segysdk::fileio::FileReaderOptions fro(SDCredentials::none(), CacheSizeOption::none());
    const segysdk::fileio::FileReference fr(fileName, fro);

    return segysdk::fileio::GenericFileManager::exists(fr);
}

bool
GenericFileReader::exists(const std::string& fileName,
                          const std::string& sdms_url,
                          const std::string& api_key,
                          std::unique_ptr<segysdk::utilities::auth::TokenSource> tokenSource,
                          std::map<std::string, std::string> headers)
{
    const segysdk::fileio::FileReaderOptions fro(
        SDCredentials(
            sdms_url,
            api_key,
            std::move(tokenSource),
            headers),
        CacheSizeOption::none());
    const segysdk::fileio::FileReference fr(fileName, fro);

    return segysdk::fileio::GenericFileManager::exists(fr);
}

bool
GenericFileReader::isSDDataset(const std::string& fileName)
{
    return segysdk::fileio::GenericFileManager::isSDDataset(fileName);
}

} // namespace segysdk::fileio
