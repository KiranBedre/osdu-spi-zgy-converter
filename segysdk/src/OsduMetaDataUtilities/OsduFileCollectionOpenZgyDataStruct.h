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

#include "../OsduMetaDataUtilities/OsduAbstractCommonResourcesStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractCompressionInfoStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractDatasetStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractFileCollectionStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractSeismicPrecisionStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"

namespace segysdk::metadata::osdu
{
struct SEGYSDK_DLL_PUBLIC OsduFileCollectionOpenZgyDataStruct final : public OsduMetadataStructBase
{
    OsduFileCollectionOpenZgyDataStruct(std::string osduPartition);
    OsduFileCollectionOpenZgyDataStruct(const OsduFileCollectionOpenZgyDataStruct& other) = default;
    OsduFileCollectionOpenZgyDataStruct(OsduFileCollectionOpenZgyDataStruct&& other) noexcept = default;
    OsduFileCollectionOpenZgyDataStruct& operator=(const OsduFileCollectionOpenZgyDataStruct& rhs) = default;
    OsduFileCollectionOpenZgyDataStruct& operator=(OsduFileCollectionOpenZgyDataStruct&& rhs) noexcept = default;
    ~OsduFileCollectionOpenZgyDataStruct() override = default;

    std::optional<OsduAbstractCommonResourcesStruct> commonResourcesStruct;
    std::optional<OsduAbstractDatasetStruct> datasetStruct;
    std::optional<OsduAbstractFileCollectionStruct> fileCollectionStruct;
    std::optional<OsduAbstractSeismicPrecisionStruct> precisionStruct;
    std::optional<OsduAbstractCompressionInfoStruct> compressionInfoStruct;

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
    inline static constexpr const char* const className_{"dataset--FileCollectionData.OpenZgy"};
    inline static constexpr const char* const classVersion_{"1.1.0"};

    
};
} // namespace segysdk::metadata::osdu
