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

#include "../OsduMetaDataUtilities/OsduSeismicTraceDataSpecificPropertiesStruct.h"
#include "../OsduMetaDataUtilities/OsduStackRanges.h"

namespace segysdk::metadata::osdu
{

OsduSeismicTraceDataSpecificPropertiesStruct::OsduSeismicTraceDataSpecificPropertiesStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

bool
OsduSeismicTraceDataSpecificPropertiesStruct::isValid() const
{
    return structValid(commonResources) and structValid(wpcGroupTypeStruct) and structValid(workProductComponentStruct)

           and structValid(liveTraceOutline)

           and isValidIdString(binGridID, IdStringCheckType::Reference)
           and isValidIdString(horizontalCRSID, IdStringCheckType::Reference) and isValidIdString(seismicLineGeometryID, IdStringCheckType::Reference)
           and isValidIdString(traceRelationFileID, IdStringCheckType::Reference) and isValidIdString(principalAcquisitionProjectID, IdStringCheckType::Reference)
           and isValidIdString(processingProjectID, IdStringCheckType::Reference) and isValidIdString(preferred3DInterpretationSetID, IdStringCheckType::Reference)
           and isValidIdString(preferred2DInterpretationSetID, IdStringCheckType::Reference) and isValidIdString(seismicTraceDataDimensionalityTypeID, IdStringCheckType::Reference)
           and isValidIdString(seismicDomainTypeID, IdStringCheckType::Reference) and isValidIdString(seismicMigrationTypeID, IdStringCheckType::Reference)
           and isValidIdString(seismicStackingTypeID, IdStringCheckType::Reference) and isValidIdString(seismicAttributeTypeID, IdStringCheckType::Reference)
           and isValidIdString(seismicFilteringTypeID, IdStringCheckType::Reference) and isValidIdString(seismicPhaseID, IdStringCheckType::Reference)
           and isValidIdString(seismicPolarityID, IdStringCheckType::Reference) and isValidIdString(seismicProcessingStageTypeID, IdStringCheckType::Reference)

           and (!sampleInterval.has_value() or sampleInterval.value() > 0)
           and (!sampleCount.has_value() or sampleCount.value() > 0)

           and isValidIdString(traceDomainUOM, IdStringCheckType::Reference) and isValidIdString(verticalMeasurementTypeID, IdStringCheckType::Reference)

           and structValid(precisionStruct)

           and arrayStructValid(processingParametersVector)

           and isValidIdString(seismicWaveTypeID, IdStringCheckType::Reference)

           and arrayStructValid(stackAngleRangesStructVector) and arrayStructValid(stackAzimuthRangesStructVector)
           and arrayStructValid(stackOffsetRangesStructVector)

           and isValidIdString(sortOrderID, IdStringCheckType::Reference) and isValidIdString(gatherTypeID, IdStringCheckType::Reference)

           and structValid(timeLapseStruct);
}

std::vector<std::string>
OsduSeismicTraceDataSpecificPropertiesStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }

    std::vector<std::string> results;
    results.emplace_back("OsduSeismicTraceDataSpecificPropertiesStruct::isValid() failed.");

    performStructValidityScan(results, commonResources);
    performStructValidityScan(results, wpcGroupTypeStruct);
    performStructValidityScan(results, workProductComponentStruct);

    performStructValidityScan(results, liveTraceOutline);

    performIdStringValidityScan(results, binGridID, "OsduSeismicTraceDataSpecificPropertiesStruct::binGridId", IdStringCheckType::Reference);
    performIdStringValidityScan(results, horizontalCRSID, "OsduSeismicTraceDataSpecificPropertiesStruct::horizontalCRSID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, seismicLineGeometryID, "OsduSeismicTraceDataSpecificPropertiesStruct::seismicLineGeometryID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, traceRelationFileID, "OsduSeismicTraceDataSpecificPropertiesStruct::traceRelationFileID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, principalAcquisitionProjectID, "OsduSeismicTraceDataSpecificPropertiesStruct::principalAcquisitionProjectID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, processingProjectID, "OsduSeismicTraceDataSpecificPropertiesStruct::processingProjectID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, preferred3DInterpretationSetID, "OsduSeismicTraceDataSpecificPropertiesStruct::preferred3DInterpretationSetID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, preferred2DInterpretationSetID, "OsduSeismicTraceDataSpecificPropertiesStruct::preferred2DInterpretationSetID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, seismicTraceDataDimensionalityTypeID, "OsduSeismicTraceDataSpecificPropertiesStruct::seismicTraceDataDimensionalityTypeID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, seismicDomainTypeID, "OsduSeismicTraceDataSpecificPropertiesStruct::seismicDomainTypeID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, seismicMigrationTypeID, "OsduSeismicTraceDataSpecificPropertiesStruct::seismicMigrationTypeID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, seismicStackingTypeID, "OsduSeismicTraceDataSpecificPropertiesStruct::seismicStackingTypeID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, seismicAttributeTypeID, "OsduSeismicTraceDataSpecificPropertiesStruct::seismicAttributeTypeID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, seismicFilteringTypeID, "OsduSeismicTraceDataSpecificPropertiesStruct::seismicFilteringTypeID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, seismicPhaseID, "OsduSeismicTraceDataSpecificPropertiesStruct::seismicPhaseID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, seismicPolarityID, "OsduSeismicTraceDataSpecificPropertiesStruct::seismicPolarityID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, seismicProcessingStageTypeID, "OsduSeismicTraceDataSpecificPropertiesStruct::seismicProcessingStageTypeID", IdStringCheckType::Reference);

    if (sampleInterval.has_value() and sampleInterval.value() <= 0) {
        results.emplace_back("OsduSeismicTraceDataSpecificPropertiesStruct:sampleInterval must be > 0");
    }
    if (sampleCount.has_value() and sampleCount.value() <= 0) {
        results.emplace_back("OsduSeismicTraceDataSpecificPropertiesStruct:sampleInterval must be > 0");
    }
    performIdStringValidityScan(results, traceDomainUOM, "OsduSeismicTraceDataSpecificPropertiesStruct::traceDomainUOM", IdStringCheckType::Reference);
    performIdStringValidityScan(results, verticalMeasurementTypeID, "OsduSeismicTraceDataSpecificPropertiesStruct::verticalMeasurementTypeID", IdStringCheckType::Reference);

    performStructValidityScan(results, precisionStruct);
    performArrayStructValidityScan(results, processingParametersVector);

    performIdStringValidityScan(results, seismicWaveTypeID, "OsduSeismicTraceDataSpecificPropertiesStruct::seismicWaveTypeID", IdStringCheckType::Reference);

    performArrayStructValidityScan(results, stackAngleRangesStructVector);
    performArrayStructValidityScan(results, stackAzimuthRangesStructVector);
    performArrayStructValidityScan(results, stackOffsetRangesStructVector);

    performIdStringValidityScan(results, sortOrderID, "OsduSeismicTraceDataSpecificPropertiesStruct::sortOrderID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, gatherTypeID, "OsduSeismicTraceDataSpecificPropertiesStruct::gatherTypeID", IdStringCheckType::Reference);

    performStructValidityScan(results, timeLapseStruct);

    return results;
}

void
OsduSeismicTraceDataSpecificPropertiesStruct::propagatePartition(const std::string& partition)
{
    if (commonResources.has_value()) {
        commonResources->setPartition(partition);
    }
    if (wpcGroupTypeStruct.has_value()) {
        wpcGroupTypeStruct->setPartition(partition);
    }
    if (workProductComponentStruct.has_value()) {
        workProductComponentStruct->setPartition(partition);
    }
    if (liveTraceOutline.has_value()) {
        liveTraceOutline->setPartition(partition);
    }
    if (precisionStruct.has_value()) {
        precisionStruct->setPartition(partition);
    }
    if (processingParametersVector.has_value()) {
        for (auto& processingParams : processingParametersVector.value()) {
            processingParams.setPartition(partition);
        }
    }
    if (stackAngleRangesStructVector.has_value()) {
        for (auto& range : stackAngleRangesStructVector.value()) {
            range.setPartition(partition);
        }
    }
    if (stackAzimuthRangesStructVector.has_value()) {
        for (auto& range : stackAzimuthRangesStructVector.value()) {
            range.setPartition(partition);
        }
    }
    if (stackOffsetRangesStructVector.has_value()) {
        for (auto& range : stackOffsetRangesStructVector.value()) {
            range.setPartition(partition);
        }
    }
    if (timeLapseStruct.has_value()) {
        timeLapseStruct->setPartition(partition);
    }
}
} // namespace segysdk::metadata::osdu
