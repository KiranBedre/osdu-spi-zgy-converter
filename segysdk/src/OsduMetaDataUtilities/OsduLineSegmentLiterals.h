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
class SEGYSDK_DLL_PUBLIC OsduLineSegmentLiterals
{
public:
    inline static constexpr const char* const NavigationLineName{"NavigationLineName"};
    inline static constexpr const char* const SeismicLineName{"SeismicLineName"};
    inline static constexpr const char* const NumberOfLocations{"NumberOfLocations"};
    inline static constexpr const char* const ShotPointRange{"ShotPointRange"};
    inline static constexpr const char* const CommonMidPointRange{"CommonMidPointRange"};
    inline static constexpr const char* const ShotPointCommonMidPointRatio{"ShotPointCommonMidPointRatio"};
    inline static constexpr const char* const GenericLabels{"GenericLabels"};
    inline static constexpr const char* const IsExtrapolated{"IsExtrapolated"};
    inline static constexpr const char* const IsInterpolated{"IsInterpolated"};
};
} // namespace segysdk::metadata::osdu
