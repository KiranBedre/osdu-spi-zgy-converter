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
class SEGYSDK_DLL_PUBLIC OsduAbstractBinGridLiterals
{
public:
    inline static constexpr const char* const BinGridName{"BinGridName"};
    inline static constexpr const char* const BinGridTypeID{"BinGridTypeID"};
    inline static constexpr const char* const SourceBinGridID{"SourceBinGridID"};
    inline static constexpr const char* const SourceBinGridAppID{"SourceBinGridAppID"};
    inline static constexpr const char* const CoveragePercent{"CoveragePercent"};
    inline static constexpr const char* const BinGridDefinitionMethodTypeID{"BinGridDefinitionMethodTypeID"};
    inline static constexpr const char* const ABCDBinGridSpatialLocation{"ABCDBinGridSpatialLocation"};
    inline static constexpr const char* const P6TransformationMethod{"P6TransformationMethod"};
    inline static constexpr const char* const P6BinGridOriginI{"P6BinGridOriginI"};
    inline static constexpr const char* const P6BinGridOriginJ{"P6BinGridOriginJ"};
    inline static constexpr const char* const P6BinGridOriginEasting{"P6BinGridOriginEasting"};
    inline static constexpr const char* const P6BinGridOriginNorthing{"P6BinGridOriginNorthing"};
    inline static constexpr const char* const P6ScaleFactorOfBinGrid{"P6ScaleFactorOfBinGrid"};
    inline static constexpr const char* const P6BinWidthOnIaxis{"P6BinWidthOnIaxis"};
    inline static constexpr const char* const P6BinWidthOnJaxis{"P6BinWidthOnJaxis"};
    inline static constexpr const char* const P6MapGridBearingOfBinGridJaxis{"P6MapGridBearingOfBinGridJaxis"};
    inline static constexpr const char* const P6BinNodeIncrementOnIaxis{"P6BinNodeIncrementOnIaxis"};
    inline static constexpr const char* const P6BinNodeIncrementOnJaxis{"P6BinNodeIncrementOnJaxis"};
};
} // namespace segysdk::metadata::osdu
