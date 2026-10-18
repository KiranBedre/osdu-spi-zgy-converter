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

#include "../segysdk_details/DLL_EXPORT.h"

#include "../FileIO/FileReference.h"

#include <json/json.h>

namespace segysdk::fileio::utilities
{

class SEGYSDK_DLL_PUBLIC JsonFileUtilities
{

private:
public:
    static Json::Value getGenericFileReference(const std::string& fileName, const SDCredentials& credentials);
    static Json::Value getJsonFromFile(const FileReference& fileReference);
};

} // namespace segysdk::fileio::utilities
