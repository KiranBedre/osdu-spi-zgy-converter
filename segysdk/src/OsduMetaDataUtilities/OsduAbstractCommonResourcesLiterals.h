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

class SEGYSDK_DLL_PUBLIC OsduAbstractCommonResourcesLiterals
{
public:
    // osdu:AbstractCommonResources
    inline static constexpr const char* const ResourceHomeRegionID{"ResourceHomeRegionID"};
    inline static constexpr const char* const ResourceHostRegionIDs{"ResourceHostRegionIDs"};
    inline static constexpr const char* const ResourceCurationStatus{"ResourceCurationStatus"};
    inline static constexpr const char* const ResourceLifecycleStatus{"ResourceLifecycleStatus"};
    inline static constexpr const char* const ResourceSecurityClassification{"ResourceSecurityClassification"};
    inline static constexpr const char* const Source{"Source"};
    inline static constexpr const char* const ExistenceKind{"ExistenceKind"};

    inline static constexpr const char* const referenceDataExistenceKind{"reference-data--ExistenceKind"};
    inline static constexpr const char* const referenceDataLifecycleStatus{"reference-data--ResourceLifecycleStatus"};
    inline static constexpr const char* const referenceDataResourceSecurityClassification{"reference-data--ResourceSecurityClassification"};
    inline static constexpr const char* const actualType{"Actual"};
    inline static constexpr const char* const loadingType{"Loading"};
    inline static constexpr const char* const restrictedType{"RESTRICTED"};
};

} // namespace segysdk::metadata::osdu
