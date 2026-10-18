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

#include "../OsduMetaDataUtilities/OsduAbstractContentStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractSpatialLocationStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractWPCGroupTypeStruct.h"
#include "../OsduMetaDataUtilities/OsduLineSegmentStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"
#include "../OsduMetaDataUtilities/OsduMetadataStructBase.h"

namespace segysdk::metadata::osdu
{

struct SEGYSDK_DLL_PUBLIC OsduSeismicTraceNavigationStruct final : public OsduMetadataStructBase
{
    OsduSeismicTraceNavigationStruct(std::string osduPartition);
    OsduSeismicTraceNavigationStruct(const OsduSeismicTraceNavigationStruct& other) = default;
    OsduSeismicTraceNavigationStruct(OsduSeismicTraceNavigationStruct&& other) noexcept = default;
    OsduSeismicTraceNavigationStruct& operator=(const OsduSeismicTraceNavigationStruct& rhs) = default;
    OsduSeismicTraceNavigationStruct& operator=(OsduSeismicTraceNavigationStruct&& rhs) noexcept = default;
    ~OsduSeismicTraceNavigationStruct() override = default;

    OsduAbstractContentStruct contentStruct;
    std::optional<std::string> originalSeismicLineGeometryId;
    std::optional<std::vector<OsduLineSegmentStruct>> lineSegmentsStruct;
    std::optional<int> totalNumberOfLocations;
    std::optional<OsduAbstractSpatialLocationStruct> spatialLocationStruct;

    bool isValid() const override;

    std::vector<std::string> performAttributeValidityScan() const override;
    void propagatePartition(const std::string& partition) override;

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
    inline static constexpr const char* const className_{"content--SeismicTraceNavigation"};
    inline static constexpr const char* const classVersion_{"1.0.0"};

    OsduMetadataHelper helper_;
};

} // namespace segysdk::metadata::osdu
