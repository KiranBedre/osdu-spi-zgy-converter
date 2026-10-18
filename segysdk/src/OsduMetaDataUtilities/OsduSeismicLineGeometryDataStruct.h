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
#include "../OsduMetaDataUtilities/OsduAbstractWorkProductComponentStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractWPCGroupTypeStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"

namespace segysdk::metadata::osdu
{
struct SEGYSDK_DLL_PUBLIC OsduSeismicLineGeometryDataStruct final : public OsduMetadataStructBase
{
    OsduSeismicLineGeometryDataStruct(std::string osduPartition);
    OsduSeismicLineGeometryDataStruct(const OsduSeismicLineGeometryDataStruct& other) = default;
    OsduSeismicLineGeometryDataStruct(OsduSeismicLineGeometryDataStruct&& other) noexcept = default;
    OsduSeismicLineGeometryDataStruct& operator=(const OsduSeismicLineGeometryDataStruct& rhs) = default;
    OsduSeismicLineGeometryDataStruct& operator=(OsduSeismicLineGeometryDataStruct&& rhs) noexcept = default;
    ~OsduSeismicLineGeometryDataStruct() override = default;

    std::optional<OsduAbstractCommonResourcesStruct> commonResourcesStruct;
    std::optional<OsduAbstractWPCGroupTypeStruct> wpcGroupTypeStruct;
    std::optional<OsduAbstractWorkProductComponentStruct> workProductComponentStruct;
    std::optional<std::string> notionalSeismicLineID;
    std::optional<std::string> preferred2DInterpretationSetID;
    std::optional<std::string> horizontalCRSID;
    std::optional<double> firstCMP;
    std::optional<double> lastCMP;
    std::optional<OsduAbstractSpatialLocationStruct> firstLocation;
    std::optional<OsduAbstractSpatialLocationStruct> lastLocation;
    std::optional<std::string> firstStationLabel;
    std::optional<std::string> lastStationLabel;
    std::optional<bool> hasMonotonicLabelling;
    std::optional<bool> hasCMPIncreaseByOne;

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
    inline static constexpr const char* const className_{"SeismicLineGeometryData"};
    inline static constexpr const char* const classVersion_{"1.0.0"};

    
};
} // namespace segysdk::metadata::osdu
