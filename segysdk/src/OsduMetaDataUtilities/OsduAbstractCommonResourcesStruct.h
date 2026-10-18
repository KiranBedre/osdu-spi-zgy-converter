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

#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"
#include "../OsduMetaDataUtilities/OsduMetadataStructBase.h"

#include <optional>

namespace segysdk::metadata::osdu
{
struct SEGYSDK_DLL_PUBLIC OsduAbstractCommonResourcesStruct final : public OsduMetadataStructBase
{
    explicit OsduAbstractCommonResourcesStruct(std::string osduPartition);
    OsduAbstractCommonResourcesStruct(const OsduAbstractCommonResourcesStruct& other);
    OsduAbstractCommonResourcesStruct(OsduAbstractCommonResourcesStruct&& other) noexcept;
    OsduAbstractCommonResourcesStruct& operator=(const OsduAbstractCommonResourcesStruct& rhs);
    OsduAbstractCommonResourcesStruct& operator=(OsduAbstractCommonResourcesStruct&& rhs) noexcept;
    ~OsduAbstractCommonResourcesStruct() override = default;

    std::optional<std::string> resourceHomeRegionID;
    std::optional<std::vector<std::string>> resourceHostRegionIDs;
    std::optional<std::string> resourceCurationStatus;
    std::optional<std::string> resourceLifecycleStatus;
    std::optional<std::string> resourceSecurityClassification;
    std::optional<std::string> source;
    std::optional<std::string> existenceKind;

    bool isValid() const override;
    std::vector<std::string> performAttributeValidityScan() const override;

public:
    static std::string getClassPartition() { return partition_; }
    static std::string getClassAreaPath() { return classAreaPath_; }
    static std::string getClassName() { return className_; }
    static std::string getClassVersion() { return classVersion_; }
    static std::string getOsduKind(const std::string& version = getClassVersion()) { return OsduMetadataHelper::osduWksReferenceLookupString(getClassName(), version); }

private:
    inline static constexpr const char* const partition_{"osdu"};
    inline static constexpr const char* const classAreaPath_{"wks"};
    inline static constexpr const char* const className_{"AbstractCommonResources"};
    inline static constexpr const char* const classVersion_{"1.0.0"};

    
};

} // namespace segysdk::metadata::osdu
