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

#include "../segysdk_details/DLL_EXPORT.h"

namespace segysdk::metadata::osdu
{

class SEGYSDK_DLL_PUBLIC OsduGeoJsonPointPropertiesElementLiterals
{
public:
    // OsduGeoJsonPointPropertiesLiterals
    inline static constexpr const char* const Inline{"Inline"};
    inline static constexpr const char* const Crossline{"Crossline"};
    inline static constexpr const char* const Label{"Label"};
    inline static constexpr const char* const ShotPointNumbers{"ShotPointNumbers"};
    inline static constexpr const char* const CommonMidPointNumbers{"CommonMidPointNumbers"};
};

} // namespace segysdk::metadata::osdu
