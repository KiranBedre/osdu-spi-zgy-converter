/*
Copyright 2021-2022 Schlumberger

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

namespace segysdk::utilities::osdu
{

class SEGYSDK_DLL_PUBLIC OsduFieldNameLiterals
{
public:
    inline static constexpr char const* const Data{"data"};
    inline static constexpr char const* const Id{"id"};
    inline static constexpr char const* const Version{"version"};
    inline static constexpr char const* const Kind{"kind"};

    inline static constexpr char const* const Name{"Name"};
    inline static constexpr char const* const Description{"Description"};
    inline static constexpr char const* const TotalSize{"TotalSize"};
    inline static constexpr char const* const Precision{"Precision"};

    inline static constexpr char const* const FileSourceInfos{"FileSourceInfos"};
    inline static constexpr char const* const FileCollectionPath{"FileCollectionPath"};
    inline static constexpr char const* const FileSource{"FileSource"};
    inline static constexpr char const* const FileSize{"FileSize"};
    inline static constexpr char const* const DatasetProperties{"DatasetProperties"};
    inline static constexpr char const* const Datasets{"Datasets"};
    inline static constexpr char const* const Components{"Components"};
    inline static constexpr char const* const BinGridID{"BinGridID"};
    inline static constexpr char const* const Artefacts{"Artefacts"};
    inline static constexpr char const* const RoleID{"RoleID"};
    inline static constexpr char const* const ResourceKind{"ResourceKind"};
    inline static constexpr char const* const ResourceID{"ResourceID"};
    inline static constexpr char const* const Legal{"legal"};
    inline static constexpr char const* const LegalTags{"legaltags"};
    inline static constexpr char const* const Ancestry{"ancestry"};
    inline static constexpr char const* const Parents{"parents"};
    inline static constexpr char const* const Meta{"meta"};
    inline static constexpr char const* const LiveTraceOutline{"LiveTraceOutline"};
    inline static constexpr char const* const AsIngestedCoordinates{"AsIngestedCoordinates"};
};

} // namespace segysdk::utilities::osdu
