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

namespace segysdk::metadata::osdu
{

class SEGYSDK_DLL_PUBLIC OsduAbstractTechnicalAssuranceLiterals
{
public:
    inline static constexpr const char* const TechnicalAssuranceTypeID{"TechnicalAssuranceTypeID"};
    inline static constexpr const char* const Reviewers{"Reviewers"};
    inline static constexpr const char* const AcceptableUsage{"AcceptableUsage"};
    inline static constexpr const char* const UnacceptableUsage{"UnacceptableUsage"};
    inline static constexpr const char* const EffectiveDate{"EffectiveDate"};
    inline static constexpr const char* const Comment{"Comment"};
};

} // namespace segysdk::osdu::metadata
