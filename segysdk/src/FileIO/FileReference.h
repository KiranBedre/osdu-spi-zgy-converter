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

#include "../FileIO/FileReaderOptions.h"

#include <cstddef>
#include <memory>
#include <string>

namespace segysdk::fileio
{

/**
 * @brief A file path and all parameters necessary to open the file in a FileManager.
 *
 * A FileReference should be preferred over passing file paths as strings when FileManager operations are expected to be performed on the file.
 */
struct SEGYSDK_DLL_PUBLIC FileReference
{
    std::string fileName() const;
    FileReaderOptions readerOptions() const;

    FileReference(std::string fileName, FileReaderOptions readerOptions);

    private:
    struct Impl;
    std::shared_ptr<Impl> impl_;
};

} // namespace segysdk::fileio
