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

#include "../OsduMetaDataUtilities/OsduAbstractCommonResourcesStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractSeismicPrecisionStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractSpatialLocationStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractWorkProductComponentStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractWPCGroupTypeStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"
#include "../OsduMetaDataUtilities/OsduProcessingParametersStruct.h"
#include "../OsduMetaDataUtilities/OsduStackRangesStruct.h"
#include "../OsduMetaDataUtilities/OsduTimeLapseStruct.h"

namespace segysdk::metadata::osdu
{
struct SEGYSDK_DLL_PUBLIC OsduSeismicTraceDataSpecificPropertiesStruct final : public OsduMetadataStructBase
{
    OsduSeismicTraceDataSpecificPropertiesStruct(std::string osduPartition);
    OsduSeismicTraceDataSpecificPropertiesStruct(const OsduSeismicTraceDataSpecificPropertiesStruct& other) = default;
    OsduSeismicTraceDataSpecificPropertiesStruct(OsduSeismicTraceDataSpecificPropertiesStruct&& other) noexcept = default;
    OsduSeismicTraceDataSpecificPropertiesStruct& operator=(const OsduSeismicTraceDataSpecificPropertiesStruct& rhs) = default;
    OsduSeismicTraceDataSpecificPropertiesStruct& operator=(OsduSeismicTraceDataSpecificPropertiesStruct&& rhs) noexcept = default;
    ~OsduSeismicTraceDataSpecificPropertiesStruct() override = default;

    std::optional<OsduAbstractCommonResourcesStruct> commonResources;
    std::optional<OsduAbstractWPCGroupTypeStruct> wpcGroupTypeStruct;
    std::optional<OsduAbstractWorkProductComponentStruct> workProductComponentStruct;

    std::optional<OsduAbstractSpatialLocationStruct> liveTraceOutline;
    std::optional<std::string> binGridID;
    std::optional<std::string> horizontalCRSID;
    std::optional<std::string> seismicLineGeometryID;
    std::optional<std::string> traceRelationFileID;
    std::optional<std::string> principalAcquisitionProjectID;

    std::optional<std::string> processingProjectID;
    std::optional<std::string> preferred3DInterpretationSetID;
    std::optional<std::string> preferred2DInterpretationSetID;
    std::optional<std::string> seismicTraceDataDimensionalityTypeID;
    std::optional<std::string> seismicDomainTypeID;

    std::optional<std::string> seismic2DName;
    std::optional<std::string> seismicMigrationTypeID;
    std::optional<std::string> seismicStackingTypeID;
    std::optional<std::string> seismicAttributeTypeID;
    std::optional<std::string> seismicFilteringTypeID;

    std::optional<std::string> seismicPhaseID;
    std::optional<std::string> seismicPolarityID;
    std::optional<std::string> seismicProcessingStageTypeID;
    std::optional<double> sampleInterval;
    std::optional<int> sampleCount;

    std::optional<bool> difference;
    std::optional<double> startTime;
    std::optional<double> endTime;
    std::optional<double> startDepth;
    std::optional<double> endDepth;
    std::optional<int> traceCount;

    std::optional<double> traceLength;

    std::optional<std::string> traceDomainUOM;
    std::optional<double> firstShotPoint;
    std::optional<double> lastShotPoint;
    std::optional<double> firstCMP;
    std::optional<double> lastCMP;
    std::optional<double> inlineMin;
    std::optional<double> inlineMax;
    std::optional<double> inlineIncrement;

    std::optional<double> crosslineMin;
    std::optional<double> crosslineMax;
    std::optional<double> crosslineIncrement;
    std::optional<double> verticalDatumOffset;
    std::optional<std::string> verticalMeasurementTypeID;

    std::optional<double> replacementVelocity;
    std::optional<double> shiftApplied;

    std::optional<OsduAbstractSeismicPrecisionStruct> precisionStruct;
    std::optional<std::vector<OsduProcessingParametersStruct>> processingParametersVector;
    std::optional<double> coveragePercent;
    std::optional<std::vector<std::string>> textualFileHeader;

    std::optional<double> rangeAmplitudeMax;
    std::optional<double> rangeAmplitudeMin;

    std::optional<double> stackAngleRangeMax;
    std::optional<double> stackAngleRangeMin;

    std::optional<double> stackAzimuthRangeMax;
    std::optional<double> stackAzimuthRangeMin;

    std::optional<double> stackOffsetRangeMax;
    std::optional<double> stackOffsetRangeMin;

    std::optional<std::string> seismicWaveTypeID;

    std::optional<std::vector<OsduStackRangesStruct>> stackAngleRangesStructVector;
    std::optional<std::vector<OsduStackRangesStruct>> stackAzimuthRangesStructVector;
    std::optional<std::vector<OsduStackRangesStruct>> stackOffsetRangesStructVector;

    std::optional<std::string> sortOrderID;
    std::optional<std::string> gatherTypeID;
    std::optional<OsduTimeLapseStruct> timeLapseStruct;

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
    inline static constexpr const char* const className_{"SeismicTraceDataSpecificProperties"};
    inline static constexpr const char* const classVersion_{"1.0.0"};

    
};

} // namespace segysdk::metadata::osdu
