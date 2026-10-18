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

#include "CleanupFiles.h"

#include <FileIO/GenericFileManager.h>

namespace segysdk::api::tests
{

void
CleanupFiles::addToCleanup(std::filesystem::path filePath)
{
    filesToCleanup_.insert(std::move(filePath));
}

void
CleanupFiles::cleanup()
{
    for (auto& f : filesToCleanup_) {
        segysdk::fileio::FileReference fr(f.string(), segysdk::fileio::FileReaderOptions::localOnly());
        // Internally it'll check for file presence
        segysdk::fileio::GenericFileManager::remove(fr);
    }
}

} // namespace segysdk::api::tests