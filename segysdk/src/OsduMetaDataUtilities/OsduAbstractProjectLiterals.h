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
class SEGYSDK_DLL_PUBLIC OsduAbstractProjectLiterals
{
public:
    inline static constexpr const char* const ProjectID{"ProjectID"};
    inline static constexpr const char* const ProjectName{"ProjectName"};
    inline static constexpr const char* const Purpose{"Purpose"};
    inline static constexpr const char* const ProjectBeginDate{"ProjectBeginDate"};
    inline static constexpr const char* const ProjectEndDate{"ProjectEndDate"};
    inline static constexpr const char* const FundsAuthorizations{"FundsAuthorizations"};
    inline static constexpr const char* const ContractIDs{"ContractIDs"};
    inline static constexpr const char* const Operator{"Operator"};
    inline static constexpr const char* const Contractors{"Contractors"};
    inline static constexpr const char* const Personnel{"Personnel"};
    inline static constexpr const char* const ProjectSpecifications{"ProjectSpecifications"};
    inline static constexpr const char* const ProjectStates{"ProjectStates"};
};
} // namespace segysdk::metadata::osdu
