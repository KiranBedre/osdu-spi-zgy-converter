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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../FileIO/FileReference.h"
#include "../FileIO/SDCredentials.h"

namespace segysdk::fileio::utilities
{

class SEGYSDK_DLL_PUBLIC FileReferenceUtilities
{

public:
    static FileReference getGenericFileReference(const std::string& fileName,
                                                 const SDCredentials& credentials,
                                                 bool isLocal,
                                                 size_t cacheSize = 2UL * 1024 * 1024,
                                                 size_t pageCount = 32,
                                                 size_t blocksPerPage = 1,
                                                 size_t chunkSize = 1024U * 1024U);
};
} // namespace segysdk::fileio::utilities
