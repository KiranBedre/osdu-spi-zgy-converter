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

#include "JsonFileUtilities.h"

#include "../FileIO/FileReferenceUtilities.h"
#include "../FileIO/GenericFileManager.h"
#include "../SegyIO/SegyIOException.h"
#include "../Utilities/JsonUtilities.h"

#include <filesystem>

namespace segysdk::fileio::utilities
{

Json::Value
JsonFileUtilities::getJsonFromFile(const FileReference& fileReference)
{
    return segysdk::utilities::JsonUtilities::getJsonFromString(GenericFileManager::readTextFile(FileManagerParams{fileReference}).text_);
}

Json::Value
JsonFileUtilities::getGenericFileReference(const std::string& fileName, const SDCredentials& credentials)
{
    const bool isLocalFile = fileName.find("sd://") != 0;

    if (isLocalFile && !std::filesystem::exists(fileName)) {
        const std::string msg = "getJsonFromGenericFile: Specified json parameter file does not exist [ " + fileName + " ]\n";
        throw segysdk::segyio::SegyFileIOException(msg);
    }

    const auto fileReference = FileReferenceUtilities::getGenericFileReference(fileName, credentials, isLocalFile);

    return getJsonFromFile(fileReference);
}
} // namespace segysdk::fileio::utilities
