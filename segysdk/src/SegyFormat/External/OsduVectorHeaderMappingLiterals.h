/*
Copyright 2023 Schlumberger

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

namespace segysdk::segyformat::external::osdu
{

class SEGYSDK_DLL_PUBLIC OsduVectorHeaderMappingLiterals
{
public:
    inline static constexpr const char* const Data{"data"};
    // osdu:VectorHeaderMapping
    inline static constexpr const char* const VectorHeaderMapping{"VectorHeaderMapping"};

    inline static constexpr const char* const KeyName{"KeyName"};
    inline static constexpr const char* const Position{"Position"};
    inline static constexpr const char* const ScalarIndicator{"ScalarIndicator"};

    inline static constexpr const char* const ScalarOverride{"ScalarOverride"};
    inline static constexpr const char* const UoM{"UoM"};
    inline static constexpr const char* const WordFormat{"WordFormat"};
    inline static constexpr const char* const WordWidth{"WordWidth"};

    inline static constexpr const char* const KeyNameReferencePath{"reference-data--HeaderKeyName"};
    inline static constexpr const char* const WordFormatReferencePath{"reference-data--WordFormatType"};
    inline static constexpr const char* const UoMReferencePath{"reference-data--UnitOfMeasure"};
    inline static constexpr const char* const uomUnitlessValue{"Euc"};
    inline static constexpr const char* const uomMetricValue{"m"};
    inline static constexpr const char* const uomImperialFtValue{"ft"};
    inline static constexpr const char* const uomUSFtValue{"ftUS"};

    inline static constexpr const char* const WordFormatIntValue{"INT"};
    inline static constexpr const char* const WordFormatIeeeFloatValue{"IEEEFLOAT"};
    inline static constexpr const char* const WordFormatIbmFloatValue{"IBMFLOAT"};

    inline static constexpr const char* const ScalarIndicatorNoScalarValue{"NOSCALE"};
    inline static constexpr const char* const ScalarIndicatorStandardValue{"STANDARD"};
    inline static constexpr const char* const ScalarIndicatorOverrideValue{"OVERRIDE"};
};

} // namespace segysdk::segyformat::external::osdu
