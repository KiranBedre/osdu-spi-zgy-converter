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

#include "../FileIO/FileManager.h"

namespace segysdk::fileio
{

FileManager::FileManager()
{
    /* EMPTY */
}

FileManager::~FileManager()
{
    /* EMPTY */
}

bool
FileManager::isSDDataset(const std::string& fileName)
{
    return (fileName.substr(0, 5) == "sd://" || fileName.substr(0, 7) == "\\\\sd://");
}

} // namespace segysdk::fileio
