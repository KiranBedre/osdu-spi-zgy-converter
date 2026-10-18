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

#include "OptionsOsdu.h"

#include "../FileIO/SDCredentials.h"

#include <string>

namespace segysdk::utilities
{
class SEGYSDK_DLL_PUBLIC OpenFileProperties
{
public:
    OpenFileProperties(const std::string& fileName, const OptionsOsdu& options);

    uint64_t fileSize() const;
    std::string fileName() const;

    static OpenFileProperties testBackdoor(const std::string& fileName, uint64_t size);

private:
    OpenFileProperties(std::string fileName, uint64_t size);

    const std::string fileName_;
    const uint64_t size_;
};

} // namespace segysdk::utilities
