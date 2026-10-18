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

#include "../OsduMetaDataUtilities/OsduAclStruct.h"
#include "../OsduMetaDataUtilities/OsduAncestryStruct.h"
#include "../OsduMetaDataUtilities/OsduCommonMetadataDataStruct.h"
#include "../OsduMetaDataUtilities/OsduLegalStruct.h"
#include "../OsduMetaDataUtilities/OsduMetaArrayStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"
#include "../OsduMetaDataUtilities/OsduTagArrayStruct.h"

namespace segysdk::metadata::osdu
{
struct SEGYSDK_DLL_PUBLIC OsduCommonMetadataStruct final : public OsduMetadataStructBase
{
    explicit OsduCommonMetadataStruct(std::string osduPartition);
    OsduCommonMetadataStruct(const OsduCommonMetadataStruct& other) = default;
    OsduCommonMetadataStruct(OsduCommonMetadataStruct&& other) noexcept = default;
    OsduCommonMetadataStruct& operator=(const OsduCommonMetadataStruct& rhs) = default;
    OsduCommonMetadataStruct& operator=(OsduCommonMetadataStruct&& rhs) noexcept = default;
    ~OsduCommonMetadataStruct() override = default;

    std::optional<std::string> id;
    std::string kind;
    std::optional<int64_t> version;
    OsduAclStruct aclStruct;
    OsduLegalStruct legalStruct;
    std::optional<OsduTagArrayStruct> tagsStruct;
    std::optional<std::string> createTime;
    std::optional<std::string> createUser;
    std::optional<std::string> modifyTime;
    std::optional<std::string> modifyUser;
    std::optional<OsduAncestryStruct> ancestryStruct;
    OsduMetaArrayStruct metaStruct;
    std::optional<OsduCommonMetadataDataStruct> dataStruct;

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
    inline static constexpr const char* const partition_{""};
    inline static constexpr const char* const classAreaPath_{""};
    inline static constexpr const char* const className_{""};
    inline static constexpr const char* const classVersion_{""};

    
};

} // namespace segysdk::metadata::osdu
