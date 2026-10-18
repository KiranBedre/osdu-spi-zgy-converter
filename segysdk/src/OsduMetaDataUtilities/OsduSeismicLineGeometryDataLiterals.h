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
class SEGYSDK_DLL_PUBLIC OsduSeismicLineGeometryDataLiterals
{
public:
    inline static constexpr const char* const NotionalSeismicLineID{"NotionalSeismicLineID"};
    inline static constexpr const char* const Preferred2DInterpretationSetID{"Preferred2DInterpretationSetID"};
    inline static constexpr const char* const HorizontalCRSID{"HorizontalCRSID"};
    inline static constexpr const char* const FirstCMP{"FirstCMP"};
    inline static constexpr const char* const LastCMP{"LastCMP"};
    inline static constexpr const char* const FirstLocation{"FirstLocation"};
    inline static constexpr const char* const LastLocation{"LastLocation"};
    inline static constexpr const char* const FirstStationLabel{"FirstStationLabel"};
    inline static constexpr const char* const LastStationLabel{"LastStationLabel"};
    inline static constexpr const char* const HasMonotonicLabelling{"HasMonotonicLabelling"};
    inline static constexpr const char* const HasCMPIncreaseByOne{"HasCMPIncreaseByOne"};
};
} // namespace segysdk::metadata::osdu
