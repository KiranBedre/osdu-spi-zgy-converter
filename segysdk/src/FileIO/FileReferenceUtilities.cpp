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

#include "../FileIO/FileReferenceUtilities.h"

#include <iostream>

namespace segysdk::fileio::utilities
{

FileReference
FileReferenceUtilities::getGenericFileReference(const std::string& fileName,
                                                const SDCredentials& credentials,
                                                bool isLocal,
                                                size_t cacheSize,
                                                size_t pageCount,
                                                size_t blocksPerPage,
                                                size_t chunkSize)
{
    if (isLocal) {
        return {fileName, FileReaderOptions::localOnly()};
    }

    return {fileName, {credentials, {cacheSize, pageCount, blocksPerPage, chunkSize}}};
}

} // namespace segysdk::fileio::utilities
