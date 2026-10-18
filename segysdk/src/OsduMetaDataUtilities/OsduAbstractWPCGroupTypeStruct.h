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

#include "../OsduMetaDataUtilities/OsduAbstractArtefactStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractNameAliasStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractTechnicalAssuranceStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"
#include "../OsduMetaDataUtilities/OsduMetadataStructBase.h"

#include <optional>
#include <vector>

namespace segysdk::metadata::osdu
{
struct SEGYSDK_DLL_PUBLIC OsduAbstractWPCGroupTypeStruct final : public OsduMetadataStructBase
{
    OsduAbstractWPCGroupTypeStruct(std::string osduPartition);
    OsduAbstractWPCGroupTypeStruct(const OsduAbstractWPCGroupTypeStruct& other);
    OsduAbstractWPCGroupTypeStruct(OsduAbstractWPCGroupTypeStruct&& other) noexcept;
    OsduAbstractWPCGroupTypeStruct& operator=(const OsduAbstractWPCGroupTypeStruct& rhs);
    OsduAbstractWPCGroupTypeStruct& operator=(OsduAbstractWPCGroupTypeStruct&& rhs) noexcept;
    ~OsduAbstractWPCGroupTypeStruct() override = default;

    std::optional<std::vector<std::string>> datasets;
    std::optional<std::vector<std::string>> ddmsDatasets;
    std::optional<std::vector<OsduAbstractArtefactStruct>> artefacts;
    std::optional<bool> isExtendedLoad;
    std::optional<bool> isDiscoverable;
    std::optional<std::vector<OsduAbstractTechnicalAssuranceStruct>> technicalAssurances;
    std::optional<std::vector<OsduAbstractNameAliasStruct>> nameAliases;

    bool isValid() const override;
    std::vector<std::string> performAttributeValidityScan() const override;
    void propagatePartition(const std::string& partition) override;

public:
    static std::string getClassPartition() { return partition_; }
    static std::string getClassAreaPath() { return classAreaPath_; }
    static std::string getClassName() { return className_; }
    static std::string getClassVersion() { return classVersion_; }
    static std::string getOsduKind(const std::string& version = getClassVersion()) { return OsduMetadataHelper::osduWksReferenceLookupString(getClassName(), version); }

private:
    inline static constexpr const char* const partition_{"osdu"};
    inline static constexpr const char* const classAreaPath_{"wks"};
    inline static constexpr const char* const className_{"WPCGroupType"};
    inline static constexpr const char* const classVersion_{"1.0.0"};

    
};

} // namespace segysdk::metadata::osdu
