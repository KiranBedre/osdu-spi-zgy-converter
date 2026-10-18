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

#include "../OsduMetaDataUtilities/OsduSeismicTraceDataSpecificProperties.h"

#include "../OsduMetaDataUtilities/OsduAbstractCommonResources.h"
#include "../OsduMetaDataUtilities/OsduAbstractWPCGroupType.h"
#include "../OsduMetaDataUtilities/OsduAbstractWorkProductComponent.h"
#include "../OsduMetaDataUtilities/OsduAbstractSpatialLocation.h"
#include "../OsduMetaDataUtilities/OsduStackRanges.h"
#include "../OsduMetaDataUtilities/OsduTimeLapse.h"
#include "../OsduMetaDataUtilities/OsduAbstractSeismicPrecision.h"
#include "../OsduMetaDataUtilities/OsduProcessingParameters.h"

#include "../OsduMetaDataUtilities/OsduAbstractCommonResourcesLiterals.h"
#include "../OsduMetaDataUtilities/OsduAbstractWPCGroupTypeLiterals.h"
#include "../OsduMetaDataUtilities/OsduAbstractWorkProductComponentLiterals.h"
#include "../OsduMetaDataUtilities/OsduSeismicTraceDataSpecificPropertiesLiterals.h"


namespace segysdk::metadata::osdu
{
OsduSeismicTraceDataSpecificProperties::OsduSeismicTraceDataSpecificProperties(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    seismicTraceDataPropertiesStruct_(OsduSeismicTraceDataSpecificPropertiesStruct(osduPartition))
{
    /* Empty */
}

OsduSeismicTraceDataSpecificProperties::OsduSeismicTraceDataSpecificProperties(OsduSeismicTraceDataSpecificPropertiesStruct dataStruct)
  : OsduMetadataBase(dataStruct.getPartition()), seismicTraceDataPropertiesStruct_(std::move(dataStruct))
{
    populateJsonObject();
}

OsduSeismicTraceDataSpecificProperties::OsduSeismicTraceDataSpecificProperties(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition),
    seismicTraceDataPropertiesStruct_(OsduSeismicTraceDataSpecificPropertiesStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduSeismicTraceDataSpecificProperties::isValidInput() const
{
    return seismicTraceDataPropertiesStruct_.isValid();
}

void
OsduSeismicTraceDataSpecificProperties::initializeJson()
{
    removeMember(OsduAbstractCommonResourcesLiterals::ResourceHomeRegionID);
    removeMember(OsduAbstractCommonResourcesLiterals::ResourceHostRegionIDs);
    removeMember(OsduAbstractCommonResourcesLiterals::ResourceCurationStatus);
    removeMember(OsduAbstractCommonResourcesLiterals::ResourceLifecycleStatus);
    removeMember(OsduAbstractCommonResourcesLiterals::ResourceSecurityClassification);
    removeMember(OsduAbstractCommonResourcesLiterals::Source);
    removeMember(OsduAbstractCommonResourcesLiterals::ExistenceKind);

    removeMember(OsduAbstractWPCGroupTypeLiterals::Datasets);
    removeMember(OsduAbstractWPCGroupTypeLiterals::DDMSDatasets);
    removeMember(OsduAbstractWPCGroupTypeLiterals::Artefacts);
    removeMember(OsduAbstractWPCGroupTypeLiterals::IsExtendedLoad);
    removeMember(OsduAbstractWPCGroupTypeLiterals::IsDiscoverable);
    removeMember(OsduAbstractWPCGroupTypeLiterals::TechnicalAssurances);
    removeMember(OsduAbstractWPCGroupTypeLiterals::NameAliases);
    removeMember(OsduAbstractWPCGroupTypeLiterals::Datasets);

    removeMember(OsduAbstractWorkProductComponentLiterals::Name);
    removeMember(OsduAbstractWorkProductComponentLiterals::Description);
    removeMember(OsduAbstractWorkProductComponentLiterals::CreationDateTime);
    removeMember(OsduAbstractWorkProductComponentLiterals::Tags);
    removeMember(OsduAbstractWorkProductComponentLiterals::SpatialPoint);
    removeMember(OsduAbstractWorkProductComponentLiterals::SpatialArea);
    removeMember(OsduAbstractWorkProductComponentLiterals::GeoContexts);
    removeMember(OsduAbstractWorkProductComponentLiterals::SubmitterName);
    removeMember(OsduAbstractWorkProductComponentLiterals::BusinessActivities);
    removeMember(OsduAbstractWorkProductComponentLiterals::AuthorIDs);
    removeMember(OsduAbstractWorkProductComponentLiterals::LineageAssertions);

    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::LiveTraceOutline);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::BinGridID);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::HorizontalCRSID);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicLineGeometryID);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::TraceRelationFileID);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::PrincipalAcquisitionProjectID);

    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::ProcessingProjectID);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::Preferred3DInterpretationSetID);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::Preferred2DInterpretationSetID);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicTraceDataDimensionalityTypeID);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicDomainTypeID);

    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::Seismic2DName);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicMigrationTypeID);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicStackingTypeID);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicAttributeTypeID);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicFilteringTypeID);

    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicPhaseID);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicPolarityID);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicProcessingStageTypeID);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::SampleInterval);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::SampleCount);

    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::Difference);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::StartTime);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::EndTime);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::StartDepth);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::EndDepth);

    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::TraceCount);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::TraceLength);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::TraceDomainUOM);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::FirstShotPoint);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::LastShotPoint);

    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::FirstCMP);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::LastCMP);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::InlineMin);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::InlineMax);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::InlineIncrement);

    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::CrosslineMin);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::CrosslineMax);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::CrosslineIncrement);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::VerticalDatumOffset);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::VerticalMeasurementTypeID);

    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::ReplacementVelocity);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::ShiftApplied);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::Precision);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::ProcessingParameters);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::CoveragePercent);

    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::TextualFileHeader);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::RangeAmplitudeMax);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::RangeAmplitudeMin);

    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicWaveTypeID);

    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::StackAngleRanges);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::StackAzimuthRanges);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::StackOffsetRanges);

    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::SortOrderID);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::GatherTypeID);
    removeMember(OsduSeismicTraceDataSpecificPropertiesLiterals::TimeLapse);
}

void
OsduSeismicTraceDataSpecificProperties::populateJsonObject()
{
    // Do NOT clean Json object - it is in OsduDataElement space ...
    initializeJson();

    if (seismicTraceDataPropertiesStruct_.commonResources.has_value()) {
        mergeInJsonRecursively(OsduAbstractCommonResources(seismicTraceDataPropertiesStruct_.commonResources.value()).getAsJson());
    }
    if (seismicTraceDataPropertiesStruct_.wpcGroupTypeStruct.has_value()) {
        mergeInJsonRecursively(OsduAbstractWPCGroupType(seismicTraceDataPropertiesStruct_.wpcGroupTypeStruct.value()).getAsJson());
    }
    if (seismicTraceDataPropertiesStruct_.workProductComponentStruct.has_value()) {
        mergeInJsonRecursively(OsduAbstractWorkProductComponent(seismicTraceDataPropertiesStruct_.workProductComponentStruct.value()).getAsJson());
    }

    setOptionalStructParameter<OsduAbstractSpatialLocation>(OsduSeismicTraceDataSpecificPropertiesLiterals::LiveTraceOutline, seismicTraceDataPropertiesStruct_.liveTraceOutline);

    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::BinGridID, seismicTraceDataPropertiesStruct_.binGridID);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::HorizontalCRSID, seismicTraceDataPropertiesStruct_.horizontalCRSID);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicLineGeometryID, seismicTraceDataPropertiesStruct_.seismicLineGeometryID);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::TraceRelationFileID, seismicTraceDataPropertiesStruct_.traceRelationFileID);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::PrincipalAcquisitionProjectID, seismicTraceDataPropertiesStruct_.principalAcquisitionProjectID);

    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::ProcessingProjectID, seismicTraceDataPropertiesStruct_.processingProjectID);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::Preferred3DInterpretationSetID, seismicTraceDataPropertiesStruct_.preferred3DInterpretationSetID);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::Preferred2DInterpretationSetID, seismicTraceDataPropertiesStruct_.preferred2DInterpretationSetID);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicTraceDataDimensionalityTypeID, seismicTraceDataPropertiesStruct_.seismicTraceDataDimensionalityTypeID);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicDomainTypeID, seismicTraceDataPropertiesStruct_.seismicDomainTypeID);

    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::Seismic2DName, seismicTraceDataPropertiesStruct_.seismic2DName);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicMigrationTypeID, seismicTraceDataPropertiesStruct_.seismicMigrationTypeID);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicStackingTypeID, seismicTraceDataPropertiesStruct_.seismicStackingTypeID);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicAttributeTypeID, seismicTraceDataPropertiesStruct_.seismicAttributeTypeID);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicFilteringTypeID, seismicTraceDataPropertiesStruct_.seismicFilteringTypeID);

    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicPhaseID, seismicTraceDataPropertiesStruct_.seismicPhaseID);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicPolarityID, seismicTraceDataPropertiesStruct_.seismicPolarityID);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicProcessingStageTypeID, seismicTraceDataPropertiesStruct_.seismicProcessingStageTypeID);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::SampleInterval, seismicTraceDataPropertiesStruct_.sampleInterval);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::SampleCount, seismicTraceDataPropertiesStruct_.sampleCount);

    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::Difference, seismicTraceDataPropertiesStruct_.difference);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::StartTime, seismicTraceDataPropertiesStruct_.startTime);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::EndTime, seismicTraceDataPropertiesStruct_.endTime);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::StartDepth, seismicTraceDataPropertiesStruct_.startDepth);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::EndDepth, seismicTraceDataPropertiesStruct_.endDepth);

    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::TraceCount, seismicTraceDataPropertiesStruct_.traceCount);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::TraceLength, seismicTraceDataPropertiesStruct_.traceLength);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::TraceDomainUOM, seismicTraceDataPropertiesStruct_.traceDomainUOM);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::FirstShotPoint, seismicTraceDataPropertiesStruct_.firstShotPoint);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::LastShotPoint, seismicTraceDataPropertiesStruct_.lastShotPoint);

    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::FirstCMP, seismicTraceDataPropertiesStruct_.firstCMP);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::LastCMP, seismicTraceDataPropertiesStruct_.lastCMP);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::InlineMin, seismicTraceDataPropertiesStruct_.inlineMin);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::InlineMax, seismicTraceDataPropertiesStruct_.inlineMax);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::InlineIncrement, seismicTraceDataPropertiesStruct_.inlineIncrement);

    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::CrosslineMin, seismicTraceDataPropertiesStruct_.crosslineMin);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::CrosslineMax, seismicTraceDataPropertiesStruct_.crosslineMax);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::CrosslineIncrement, seismicTraceDataPropertiesStruct_.crosslineIncrement);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::VerticalDatumOffset, seismicTraceDataPropertiesStruct_.verticalDatumOffset);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::VerticalMeasurementTypeID, seismicTraceDataPropertiesStruct_.verticalMeasurementTypeID);

    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::ReplacementVelocity, seismicTraceDataPropertiesStruct_.replacementVelocity);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::ShiftApplied, seismicTraceDataPropertiesStruct_.shiftApplied);

    setOptionalStructParameter<OsduAbstractSeismicPrecision>(OsduSeismicTraceDataSpecificPropertiesLiterals::Precision, seismicTraceDataPropertiesStruct_.precisionStruct);

    setOptionalArrayStructParameter<OsduProcessingParameters>(OsduSeismicTraceDataSpecificPropertiesLiterals::ProcessingParameters, seismicTraceDataPropertiesStruct_.processingParametersVector);

    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::CoveragePercent, seismicTraceDataPropertiesStruct_.coveragePercent);

    setOptionalArrayParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::TextualFileHeader, seismicTraceDataPropertiesStruct_.textualFileHeader);

    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::RangeAmplitudeMax, seismicTraceDataPropertiesStruct_.rangeAmplitudeMax);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::RangeAmplitudeMin, seismicTraceDataPropertiesStruct_.rangeAmplitudeMin);

    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::StackAngleRangeMax, seismicTraceDataPropertiesStruct_.stackAngleRangeMax);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::StackAngleRangeMin, seismicTraceDataPropertiesStruct_.stackAngleRangeMin);

    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::StackAzimuthRangeMax, seismicTraceDataPropertiesStruct_.stackAzimuthRangeMax);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::StackAzimuthRangeMin, seismicTraceDataPropertiesStruct_.stackAzimuthRangeMin);

    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::StackOffsetRangeMax, seismicTraceDataPropertiesStruct_.stackOffsetRangeMax);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::StackOffsetRangeMin, seismicTraceDataPropertiesStruct_.stackOffsetRangeMin);

    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicWaveTypeID, seismicTraceDataPropertiesStruct_.seismicWaveTypeID);

    setOptionalArrayStructParameter<OsduStackRanges>(OsduSeismicTraceDataSpecificPropertiesLiterals::StackAngleRanges, seismicTraceDataPropertiesStruct_.stackAngleRangesStructVector);
    setOptionalArrayStructParameter<OsduStackRanges>(OsduSeismicTraceDataSpecificPropertiesLiterals::StackAzimuthRanges, seismicTraceDataPropertiesStruct_.stackAzimuthRangesStructVector);
    setOptionalArrayStructParameter<OsduStackRanges>(OsduSeismicTraceDataSpecificPropertiesLiterals::StackOffsetRanges, seismicTraceDataPropertiesStruct_.stackOffsetRangesStructVector);

    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::SortOrderID, seismicTraceDataPropertiesStruct_.sortOrderID);
    setOptionalParameter(OsduSeismicTraceDataSpecificPropertiesLiterals::GatherTypeID, seismicTraceDataPropertiesStruct_.gatherTypeID);

    setOptionalStructParameter<OsduTimeLapse>(OsduSeismicTraceDataSpecificPropertiesLiterals::TimeLapse, seismicTraceDataPropertiesStruct_.timeLapseStruct);
}

std::shared_ptr<OsduMetadataStructBase>
OsduSeismicTraceDataSpecificProperties::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduSeismicTraceDataSpecificPropertiesStruct>(seismicTraceDataPropertiesStruct_);
}

void
OsduSeismicTraceDataSpecificProperties::initializeMetadataStruct()
{
    seismicTraceDataPropertiesStruct_ = OsduSeismicTraceDataSpecificPropertiesStruct(getOsduPartition());
}

void
OsduSeismicTraceDataSpecificProperties::populateMetadataStruct()
{
    initializeMetadataStruct();

    seismicTraceDataPropertiesStruct_.commonResources = getOptionalStructParameter<OsduAbstractCommonResources, OsduAbstractCommonResourcesStruct>();
    seismicTraceDataPropertiesStruct_.wpcGroupTypeStruct = getOptionalStructParameter<OsduAbstractWPCGroupType, OsduAbstractWPCGroupTypeStruct>();
    seismicTraceDataPropertiesStruct_.workProductComponentStruct = getOptionalStructParameter<OsduAbstractWorkProductComponent, OsduAbstractWorkProductComponentStruct>();

    seismicTraceDataPropertiesStruct_.liveTraceOutline = getOptionalStructParameter<OsduAbstractSpatialLocation, OsduAbstractSpatialLocationStruct>(OsduSeismicTraceDataSpecificPropertiesLiterals::LiveTraceOutline);

    seismicTraceDataPropertiesStruct_.binGridID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::BinGridID);
    seismicTraceDataPropertiesStruct_.horizontalCRSID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::HorizontalCRSID);

    seismicTraceDataPropertiesStruct_.seismicLineGeometryID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicLineGeometryID);
    seismicTraceDataPropertiesStruct_.traceRelationFileID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::TraceRelationFileID);
    seismicTraceDataPropertiesStruct_.principalAcquisitionProjectID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::PrincipalAcquisitionProjectID);

    seismicTraceDataPropertiesStruct_.processingProjectID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::ProcessingProjectID);
    seismicTraceDataPropertiesStruct_.preferred3DInterpretationSetID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::Preferred3DInterpretationSetID);
    seismicTraceDataPropertiesStruct_.preferred2DInterpretationSetID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::Preferred2DInterpretationSetID);
    seismicTraceDataPropertiesStruct_.seismicTraceDataDimensionalityTypeID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicTraceDataDimensionalityTypeID);
    seismicTraceDataPropertiesStruct_.seismicDomainTypeID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicDomainTypeID);

    seismicTraceDataPropertiesStruct_.seismic2DName = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::Seismic2DName);
    seismicTraceDataPropertiesStruct_.seismicMigrationTypeID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicMigrationTypeID);
    seismicTraceDataPropertiesStruct_.seismicStackingTypeID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicStackingTypeID);
    seismicTraceDataPropertiesStruct_.seismicAttributeTypeID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicAttributeTypeID);
    seismicTraceDataPropertiesStruct_.seismicFilteringTypeID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicFilteringTypeID);

    seismicTraceDataPropertiesStruct_.seismicPhaseID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicPhaseID);
    seismicTraceDataPropertiesStruct_.seismicPolarityID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicPolarityID);
    seismicTraceDataPropertiesStruct_.seismicProcessingStageTypeID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicProcessingStageTypeID);
    seismicTraceDataPropertiesStruct_.sampleInterval = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::SampleInterval);
    seismicTraceDataPropertiesStruct_.sampleCount = getOptionalParameter<int>(OsduSeismicTraceDataSpecificPropertiesLiterals::SampleCount);

    seismicTraceDataPropertiesStruct_.difference = getOptionalParameter<bool>(OsduSeismicTraceDataSpecificPropertiesLiterals::Difference);
    seismicTraceDataPropertiesStruct_.startTime = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::StartTime);
    seismicTraceDataPropertiesStruct_.endTime = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::EndTime);
    seismicTraceDataPropertiesStruct_.startDepth = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::StartDepth);
    seismicTraceDataPropertiesStruct_.endDepth = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::EndDepth);

    seismicTraceDataPropertiesStruct_.traceCount = getOptionalParameter<int>(OsduSeismicTraceDataSpecificPropertiesLiterals::TraceCount);
    seismicTraceDataPropertiesStruct_.traceLength = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::TraceLength);
    seismicTraceDataPropertiesStruct_.traceDomainUOM = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::TraceDomainUOM);
    seismicTraceDataPropertiesStruct_.firstShotPoint = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::FirstShotPoint);
    seismicTraceDataPropertiesStruct_.lastShotPoint = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::LastShotPoint);

    seismicTraceDataPropertiesStruct_.firstCMP = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::FirstCMP);
    seismicTraceDataPropertiesStruct_.lastCMP = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::LastCMP);
    seismicTraceDataPropertiesStruct_.inlineMin = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::InlineMin);
    seismicTraceDataPropertiesStruct_.inlineMax = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::InlineMax);
    seismicTraceDataPropertiesStruct_.inlineIncrement = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::InlineIncrement);

    seismicTraceDataPropertiesStruct_.crosslineMin = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::CrosslineMin);
    seismicTraceDataPropertiesStruct_.crosslineMax = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::CrosslineMax);
    seismicTraceDataPropertiesStruct_.crosslineIncrement = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::CrosslineIncrement);

    seismicTraceDataPropertiesStruct_.verticalDatumOffset = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::VerticalDatumOffset);

    seismicTraceDataPropertiesStruct_.verticalMeasurementTypeID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::VerticalMeasurementTypeID);

    seismicTraceDataPropertiesStruct_.replacementVelocity = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::ReplacementVelocity);
    seismicTraceDataPropertiesStruct_.shiftApplied = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::ShiftApplied);
    seismicTraceDataPropertiesStruct_.coveragePercent = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::CoveragePercent);

    seismicTraceDataPropertiesStruct_.precisionStruct = getOptionalStructParameter<OsduAbstractSeismicPrecision, OsduAbstractSeismicPrecisionStruct>(OsduSeismicTraceDataSpecificPropertiesLiterals::Precision);

    seismicTraceDataPropertiesStruct_.processingParametersVector = getOptionalArrayStructParameter<OsduProcessingParameters, OsduProcessingParametersStruct>(OsduSeismicTraceDataSpecificPropertiesLiterals::ProcessingParameters);

    seismicTraceDataPropertiesStruct_.textualFileHeader = getOptionalArrayParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::TextualFileHeader);

    seismicTraceDataPropertiesStruct_.rangeAmplitudeMax = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::RangeAmplitudeMax);
    seismicTraceDataPropertiesStruct_.rangeAmplitudeMin = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::RangeAmplitudeMin);

    seismicTraceDataPropertiesStruct_.stackAngleRangeMax = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::StackAngleRangeMax);
    seismicTraceDataPropertiesStruct_.stackAngleRangeMin = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::StackAngleRangeMin);

    seismicTraceDataPropertiesStruct_.stackAzimuthRangeMax = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::StackAzimuthRangeMax);
    seismicTraceDataPropertiesStruct_.stackAzimuthRangeMin = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::StackAzimuthRangeMin);

    seismicTraceDataPropertiesStruct_.stackOffsetRangeMax = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::StackOffsetRangeMax);
    seismicTraceDataPropertiesStruct_.stackOffsetRangeMin = getOptionalParameter<double>(OsduSeismicTraceDataSpecificPropertiesLiterals::StackOffsetRangeMin);

    seismicTraceDataPropertiesStruct_.seismicWaveTypeID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::SeismicWaveTypeID);

    seismicTraceDataPropertiesStruct_.stackAngleRangesStructVector = getOptionalArrayStructParameter<OsduStackRanges, OsduStackRangesStruct>(OsduSeismicTraceDataSpecificPropertiesLiterals::StackAngleRanges);
    seismicTraceDataPropertiesStruct_.stackAzimuthRangesStructVector = getOptionalArrayStructParameter<OsduStackRanges, OsduStackRangesStruct>(OsduSeismicTraceDataSpecificPropertiesLiterals::StackAzimuthRanges);
    seismicTraceDataPropertiesStruct_.stackOffsetRangesStructVector = getOptionalArrayStructParameter<OsduStackRanges, OsduStackRangesStruct>(OsduSeismicTraceDataSpecificPropertiesLiterals::StackOffsetRanges);

    seismicTraceDataPropertiesStruct_.sortOrderID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::SortOrderID);
    seismicTraceDataPropertiesStruct_.gatherTypeID = getOptionalParameter<std::string>(OsduSeismicTraceDataSpecificPropertiesLiterals::GatherTypeID);

    seismicTraceDataPropertiesStruct_.timeLapseStruct = getOptionalStructParameter<OsduTimeLapse, OsduTimeLapseStruct>(OsduSeismicTraceDataSpecificPropertiesLiterals::TimeLapse);
}
} // namespace segysdk::metadata::osdu
