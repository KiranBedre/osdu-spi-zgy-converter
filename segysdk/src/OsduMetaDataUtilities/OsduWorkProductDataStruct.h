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
#include "../OsduMetaDataUtilities/OsduAbstractSpatialLocation.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"
#include "../OsduMetaDataUtilities/OsduLineageAssertionStruct.h"

namespace segysdk::metadata::osdu
{

struct SEGYSDK_DLL_PUBLIC OsduWorkProductDataStruct final : public OsduMetadataStructBase
{
    OsduWorkProductDataStruct(std::string osduPartition);
    OsduWorkProductDataStruct(const OsduWorkProductDataStruct& other) = default;
    OsduWorkProductDataStruct(OsduWorkProductDataStruct&& other) noexcept = default;
    OsduWorkProductDataStruct& operator=(const OsduWorkProductDataStruct& rhs) = default;
    OsduWorkProductDataStruct& operator=(OsduWorkProductDataStruct&& rhs) noexcept = default;
    ~OsduWorkProductDataStruct() override = default;

    std::optional<OsduAbstractCommonResourcesStruct> commonResources;
    std::optional<std::vector<std::string>> components;
    std::optional<bool> isExtendedLoad;
    std::optional<bool> isDiscoverable;
    std::optional<std::string> name;
    std::optional<std::string> description;
    std::optional<std::string> creationDateTime;
    std::optional<std::vector<std::string>> tags;
    std::optional<OsduAbstractSpatialLocationStruct> spatialPointStruct;
    std::optional<OsduAbstractSpatialLocationStruct> spatialAreaStruct;
    std::optional<std::string> submitterName;
    std::optional<std::vector<std::string>> businessActivities;
    std::optional<std::vector<std::string>> authorIDs;
    std::optional<std::vector<OsduLineageAssertionStruct>> lineageAssertionsStruct;
    std::optional<std::vector<std::string>> annotations;

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
    inline static constexpr const char* const className_{"WorkProductData"};
    inline static constexpr const char* const classVersion_{"1.0.0"};

    
};

} // namespace segysdk::metadata::osdu
