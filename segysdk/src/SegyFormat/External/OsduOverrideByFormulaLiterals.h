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

class SEGYSDK_DLL_PUBLIC OsduOverrideByFormulaLiterals
{
public:
    // osdu:VectorHeaderMappingFormulaOverrides
    inline static constexpr const char* const KeyName{"KeyName"};
    inline static constexpr const char* const OverrideByExpressions{"OverrideByExpressions"};
    inline static constexpr const char* const Input{"Input"};
    inline static constexpr const char* const Operator1{"Operator1"};
    inline static constexpr const char* const Operand1Constant{"Operand1Constant"};
    inline static constexpr const char* const Operand1Variable{"Operand1Variable"};
    inline static constexpr const char* const Operator2{"Operator2"};
    inline static constexpr const char* const Operand2Constant{"Operand2Constant"};
    inline static constexpr const char* const Operand2Variable{"Operand2Variable"};
    inline static constexpr const char* const Remark{"Remark"};

    inline static constexpr const char* const PlusOperatorValue{"Plus"};
    inline static constexpr const char* const MinusOperatorValue{"Minus"};
    inline static constexpr const char* const DivideByOperatorValue{"DivideBy"};
    inline static constexpr const char* const DivOperatorValue{"DivideByInteger"};
    inline static constexpr const char* const MultiplyByOperatorValue{"MultiplyBy"};
    inline static constexpr const char* const ModuloOperatorValue{"Modulo"};
    inline static constexpr const char* const EqualsOperatorValue{"Equals"};

    inline static constexpr const char* const KeyReferencePath{"reference-data--HeaderKeyName"};
};

} // namespace segysdk::segyformat::external::osdu
