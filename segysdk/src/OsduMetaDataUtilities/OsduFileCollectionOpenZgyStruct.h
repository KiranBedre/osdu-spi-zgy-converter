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

#include "../OsduMetaDataUtilities/OsduCommonMetadataStruct.h"
#include "../OsduMetaDataUtilities/OsduFileCollectionOpenZgyDataStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"
#include "../tools/SegyToZgy/library/OpenZGYFileProperties.h"

namespace segysdk::metadata::osdu
{

struct SEGYSDK_DLL_PUBLIC OsduFileCollectionOpenZgyStruct final : public OsduMetadataStructBase
{
    OsduFileCollectionOpenZgyStruct(const std::string& osduPartition);

    OsduFileCollectionOpenZgyStruct(const OsduFileCollectionOpenZgyStruct& other) = default;
    OsduFileCollectionOpenZgyStruct(OsduFileCollectionOpenZgyStruct&& other) noexcept = default;
    OsduFileCollectionOpenZgyStruct& operator=(const OsduFileCollectionOpenZgyStruct& rhs) = default;
    OsduFileCollectionOpenZgyStruct& operator=(OsduFileCollectionOpenZgyStruct&& rhs) noexcept = default;
    ~OsduFileCollectionOpenZgyStruct() override = default;

    OsduCommonMetadataStruct commonMetadataStruct;
    std::optional<OsduFileCollectionOpenZgyDataStruct> dataStruct;

    bool isValid() const override;

    std::vector<std::string> performAttributeValidityScan() const override;
    void propagatePartition(const std::string& partition) override;

    void addFileProperties(const std::string& zgyFileName, uint64_t fileSize);
    void addPrecision(segysdk::metadata::osdu::OsduAbstractSeismicPrecisionStruct precisionStruct);
    void addCompressionInfo(OsduAbstractCompressionInfoStruct compressionInfoStruct);

    OsduMetadataHelper getMetadataHelper() const { return helper_; }

public:
    static std::string getClassPartition() { return partition_; }
    static std::string getClassAreaPath() { return classAreaPath_; }
    static std::string getClassName() { return className_; }
    static std::string getClassVersion() { return classVersion_; }
    static std::string getOsduKind(const std::string& version = getClassVersion()) { return OsduMetadataHelper::osduWksReferenceLookupString(getClassName(), version); }

private:
    inline static constexpr const char* const partition_{"osdu"};
    inline static constexpr const char* const classAreaPath_{"wks"};
    inline static constexpr const char* const className_{"dataset--FileCollection.Slb.OpenZGY"};

     // We are actually supporting up to version "1.1.0"
    inline static constexpr const char* const classVersion_{"1.0.0"};

    OsduMetadataHelper helper_;
};

} // namespace segysdk::metadata::osdu
