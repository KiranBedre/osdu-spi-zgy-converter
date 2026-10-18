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

#include "../OsduMetaDataUtilities/OsduContractorStruct.h"
#include "../OsduMetaDataUtilities/OsduFundsAuthorizationStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"
#include "../OsduMetaDataUtilities/OsduMetadataStructBase.h"
#include "../OsduMetaDataUtilities/OsduPersonalStruct.h"
#include "../OsduMetaDataUtilities/OsduProjectSpecificationStruct.h"
#include "../OsduMetaDataUtilities/OsduProjectStateStruct.h"

namespace segysdk::metadata::osdu
{
struct SEGYSDK_DLL_PUBLIC OsduAbstractProjectStruct final : public OsduMetadataStructBase
{
    explicit OsduAbstractProjectStruct(std::string osduPartition);
    OsduAbstractProjectStruct(const OsduAbstractProjectStruct& other) = default;
    OsduAbstractProjectStruct(OsduAbstractProjectStruct&& other) noexcept = default;
    OsduAbstractProjectStruct& operator=(const OsduAbstractProjectStruct& rhs) = default;
    OsduAbstractProjectStruct& operator=(OsduAbstractProjectStruct&& rhs) noexcept = default;
    ~OsduAbstractProjectStruct() override = default;

    std::optional<std::string> id;
    std::optional<std::string> name;
    std::optional<std::string> purpose;
    std::optional<std::string> beginDate;
    std::optional<std::string> endDate;
    std::optional<std::vector<OsduFundsAuthorizationStruct>> fundsAuthorizations;
    std::optional<std::vector<std::string>> contractIds;
    std::optional<std::string> operator_;
    std::optional<std::vector<OsduContractorStruct>> contractorsStruct;
    std::optional<std::vector<OsduPersonalStruct>> personnelStruct;
    std::optional<std::vector<OsduProjectSpecificationStruct>> specificationsStruct;
    std::optional<std::vector<OsduProjectStateStruct>> statesStruct;

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
    inline static constexpr const char* const className_{"AbstractProject"};
    inline static constexpr const char* const classVersion_{"1.0.0"};

    
};

} // namespace segysdk::metadata::osdu
