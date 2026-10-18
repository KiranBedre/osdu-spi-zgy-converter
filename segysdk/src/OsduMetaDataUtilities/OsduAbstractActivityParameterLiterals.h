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
class SEGYSDK_DLL_PUBLIC OsduAbstractActivityParameterLiterals
{
public:
    inline static constexpr const char* const Title{"Title"};
    inline static constexpr const char* const Index{"Index"};
    inline static constexpr const char* const Selection{"Selection"};
    inline static constexpr const char* const Keys{"Keys"};
    inline static constexpr const char* const DataObjectParameter{"DataObjectParameter"};
    inline static constexpr const char* const DataQuantityParameter{"DataQuantityParameter"};
    inline static constexpr const char* const IntegerQuantityParameter{"IntegerQuantityParameter"};
    inline static constexpr const char* const StringParameter{"StringParameter"};
    inline static constexpr const char* const TimeIndexParameter{"TimeIndexParameter"};
    inline static constexpr const char* const BooleanParameter{"BooleanParameter"};
    inline static constexpr const char* const ParameterKindID{"ParameterKindID"};
    inline static constexpr const char* const ParameterRoleID{"ParameterRoleID"};
    inline static constexpr const char* const DataQuantityParameterUOMID{"DataQuantityParameterUOMID"};
};
} // namespace segysdk::metadata::osdu
