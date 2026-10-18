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

#include "OpenFileProperties.h"

#include "Adapters.h"

#include <SDAPI/SDManager.h>
#include <SDAPI/SDUtils.h>

#include <fstream>
#include <utility>

#include "OptionsOsdu.h"

namespace segysdk::utilities
{
static size_t
getSize(const std::string& fileName, const OptionsOsdu& options)
{
    if (isSDDataset(fileName)) {
        const auto sd = options.createSDManager();
        return static_cast<size_t>(seismicdrive::SDUtils(sd.get()).datasetSize(fileName));
    }
    else {
        std::ifstream file(fileName);
        file.seekg(0, std::ios::end);
        return static_cast<size_t>(file.tellg());
    }
}

OpenFileProperties::OpenFileProperties(const std::string& fileName, const OptionsOsdu& options)
  : OpenFileProperties(fileName, getSize(fileName, options))
{
}

uint64_t
OpenFileProperties::fileSize() const
{
    return size_;
}

std::string
OpenFileProperties::fileName() const
{
    return fileName_;
}

OpenFileProperties
OpenFileProperties::testBackdoor(const std::string& fileName, uint64_t size)
{
    return OpenFileProperties(fileName, size);
}

OpenFileProperties::OpenFileProperties(std::string fileName, uint64_t size)
  : fileName_(std::move(fileName)),
    size_(size)
{
}

} // namespace segysdk::utilities
