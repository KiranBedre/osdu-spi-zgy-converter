/*
Copyright 2024 Schlumberger

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

#include "../Request/Params.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <cstdint>
#include <string>

namespace segysdk::fileio
{

struct SEGYSDK_DLL_PUBLIC FileAccessParams : public request::Params
{
    FileAccessParams& withFileName(const std::string& value);
    std::string fileName_{};
};

struct SEGYSDK_DLL_PUBLIC FileReadParams : public request::Params
{
    FileReadParams& withOffset(const std::uint64_t value);
    FileReadParams& withSize(const std::size_t value);
    FileReadParams& withAlignment(const std::size_t value);
    std::uint64_t offset_{};
    std::size_t size_{};
    std::size_t alignment_{};
};

} // namespace segysdk::fileio
