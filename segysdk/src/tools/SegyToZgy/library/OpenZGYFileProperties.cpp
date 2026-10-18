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

#include "../tools/SegyToZgy/library/OpenZGYFileProperties.h"

#include "../Utilities/Adapters.h"

#include <SDAPI/SDManager.h>
#include <SDAPI/SDUtils.h>

#include <fstream>
#include <utility>

static size_t
getSize(const std::string& fileName, const ZgyOptionsOsdu& options)
{
    if (segysdk::utilities::isSDDataset(fileName)) {
        const auto sd = options.createSDManager();
        return static_cast<size_t>(seismicdrive::SDUtils(sd.get()).datasetSize(fileName));
    }
    else {
        std::ifstream file(fileName);
        file.seekg(0, std::ios::end);
        return static_cast<size_t>(file.tellg());
    }
}

OpenZGYFileProperties::OpenZGYFileProperties(const std::string& fileName, const ZgyOptionsOsdu& options)
  : OpenZGYFileProperties(fileName, getSize(fileName, options))
{
}

uint64_t
OpenZGYFileProperties::fileSize() const
{
    return size_;
}

std::string
OpenZGYFileProperties::fileName() const
{
    return fileName_;
}

OpenZGYFileProperties
OpenZGYFileProperties::testBackdoor(const std::string& fileName, uint64_t size)
{
    return OpenZGYFileProperties(fileName, size);
}

OpenZGYFileProperties::OpenZGYFileProperties(std::string fileName, uint64_t size)
  : fileName_(std::move(fileName)),
    size_(size)
{
}
