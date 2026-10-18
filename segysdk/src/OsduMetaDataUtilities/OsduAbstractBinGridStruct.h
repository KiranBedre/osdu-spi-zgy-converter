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

#include "../OsduMetaDataUtilities/OsduAbstractSpatialLocationStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"

namespace segysdk::metadata::osdu
{
struct SEGYSDK_DLL_PUBLIC OsduAbstractBinGridStruct final : public OsduMetadataStructBase
{
    OsduAbstractBinGridStruct(std::string osduPartition);
    OsduAbstractBinGridStruct(const OsduAbstractBinGridStruct& other) = default;
    OsduAbstractBinGridStruct(OsduAbstractBinGridStruct&& other) noexcept = default;
    OsduAbstractBinGridStruct& operator=(const OsduAbstractBinGridStruct& rhs) = default;
    OsduAbstractBinGridStruct& operator=(OsduAbstractBinGridStruct&& rhs) noexcept = default;
    ~OsduAbstractBinGridStruct() override = default;

    std::optional<std::string> name;
    std::optional<std::string> typeId;
    std::optional<int64_t> sourceId;
    std::optional<std::string> sourceAppId;
    std::optional<double> coveragePercent;
    std::optional<std::string> definitionMethodTypeId;
    std::optional<OsduAbstractSpatialLocationStruct> abcdSpatialLocationStruct;

    std::optional<int64_t> p6TransformationMethod;
    std::optional<double> p6BinGridOriginI;
    std::optional<double> p6BinGridOriginJ;
    std::optional<double> p6BinGridOriginEasting;
    std::optional<double> p6BinGridOriginNorthing;
    std::optional<double> p6ScaleFactorOfBinGrid;
    std::optional<double> p6BinWidthOnIaxis;
    std::optional<double> p6BinWidthOnJaxis;
    std::optional<double> p6MapGridBearingOfBinGridJaxis;
    std::optional<int64_t> p6BinNodeIncrementOnIaxis;
    std::optional<int64_t> p6BinNodeIncrementOnJaxis;

    bool isValid() const override;
    bool isCompleteP6Definition() const;

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
    inline static constexpr const char* const className_{"AbstractBinGrid"};
    inline static constexpr const char* const classVersion_{"1.1.0"};

    
};
} // namespace segysdk::metadata::osdu
