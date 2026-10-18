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
class SEGYSDK_DLL_PUBLIC OsduAbstractActivityStateLiterals
{
public:
    inline static constexpr const char* const EffectiveDateTime{"EffectiveDateTime"};
    inline static constexpr const char* const TerminationDateTime{"TerminationDateTime"};
    inline static constexpr const char* const ActivityStatusID{"ActivityStatusID"};
    inline static constexpr const char* const Remark{"Remark"};
};
} // namespace segysdk::metadata::osdu
