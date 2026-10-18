/*
Copyright 2025 Schlumberger

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

#include "segysdk_details/DLL_EXPORT.h"

#include <cstdint>
#include <string>
#include <string_view>

namespace segysdk::metadata::osdu
{

struct OsduFileImagePngParams
{
    OsduFileImagePngParams& withFileSource(std::string_view value);
    OsduFileImagePngParams& withFileSize(const std::uint64_t value);
    OsduFileImagePngParams& withDataFlowId(std::string_view value);
    OsduFileImagePngParams& withSliceType(std::string_view value);

    std::string fileSource_;
    std::uint64_t fileSize_{};
    std::string dataFlowId_;
    std::string sliceType_;
};

class SEGYSDK_DLL_PUBLIC OsduFileImagePng
{
public:
    explicit OsduFileImagePng(std::string&& fileCollectionJson);
    std::string asString(const OsduFileImagePngParams& params) const;
    static std::string getResourceKind();

private:
    std::string fileCollectionJson_;
};

} // namespace segysdk::metadata::osdu
