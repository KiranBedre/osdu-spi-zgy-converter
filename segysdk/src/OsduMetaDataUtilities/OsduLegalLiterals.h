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

class SEGYSDK_DLL_PUBLIC OsduLegalLiterals
{
public:
    // osdu:Legal
    inline static constexpr const char* const Legal{"legal"};
    inline static constexpr const char* const Legaltags{"legaltags"};
    inline static constexpr const char* const OtherRelevantDataCountries{"otherRelevantDataCountries"};
    inline static constexpr const char* const Status{"status"};

    inline static constexpr const char* const StatusCompliant{"compliant"};
    inline static constexpr const char* const StatusUncompliant{"uncompliant"};
};

} // namespace segysdk::metadata::osdu
