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

#include <string>

namespace segysdk::details
{

class SEGYSDK_DLL_PUBLIC JSONTraceHeaderDefStrings
{

private:
public:
    inline static const std::string Id = "Id";
    inline static const std::string ExternalId = "ExternalId";
    inline static const std::string ScalarHeaderId = "ScalarHeaderId";
    inline static const std::string DisplayName = "DisplayName";
    inline static const std::string Offset = "Offset";
    inline static const std::string ByteCount = "ByteCount";
    inline static const std::string DataType = "DataType";
    inline static const std::string ScalarIndicator = "ScalarIndicator";
    inline static const std::string ScalarOverride = "ScalarOverride";
    inline static const std::string IsWellKnown = "IsWellKnown";
    inline static const std::string UoM = "UoM";
    inline static const std::string ExtraUnitScalar = "ExtraUnitScalar";
};
} // namespace segysdk::details
