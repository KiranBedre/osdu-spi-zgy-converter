/*
Copyright 2025 Schlumberger

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

namespace segysdk::metadata::osdu
{

class SEGYSDK_DLL_PUBLIC OsduFileImagePngLiterals
{
public:
    static constexpr auto acl = "acl";
    static constexpr auto ancestry = "ancestry";
    static constexpr auto artefacts = "Artefacts";
    static constexpr auto data = "data";
    static constexpr auto dataFlowId = "dataflowId";
    static constexpr auto datasetProperties = "DatasetProperties";
    static constexpr auto fileSize = "FileSize";
    static constexpr auto fileSource = "FileSource";
    static constexpr auto fileSourceInfo = "FileSourceInfo";
    static constexpr auto isExtendedLoad = "IsExtendedLoad";
    static constexpr auto kind = "kind";
    static constexpr auto legal = "legal";
    static constexpr auto meta = "meta";
    static constexpr auto resourceId = "ResourceID";
    static constexpr auto resourceKind = "ResourceKind";
    static constexpr auto roleId = "RoleID";
    static constexpr auto sliceType = "sliceType";
    static constexpr auto tags = "tags";
};

} // namespace segysdk::metadata::osdu
