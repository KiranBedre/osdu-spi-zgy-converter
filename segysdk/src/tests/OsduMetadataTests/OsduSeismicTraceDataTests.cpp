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

#include "../OsduMetaDataUtilities/OsduMetafileApi.h"
#include "OsduMetadataTestBase.h"

namespace segysdk::osduMetadata::OsduSeismicTraceDataTests
{
using namespace segysdk::metadata::osdu;

class OsduSeismicTraceDataTests : public OsduMetadataTestBase<OsduSeismicTraceData, OsduSeismicTraceDataStruct>
{
protected:
    OsduSeismicTraceDataTests()
      : OsduMetadataTestBase(seismicTraceDataJsonFileName)
    {
        /* Empty */
    }

    virtual void ValidatePartitionPropagation() const override
    {
        const auto seismicTraceDataStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(seismicTraceDataStruct_ptr == nullptr);

        seismicTraceDataStruct_ptr->setPartition(dummyPartitionName_);
        ASSERT_TRUE(seismicTraceDataStruct_ptr->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicTraceDataStruct_ptr->commonMetadata.getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->commonResources->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->wpcGroupTypeStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->workProductComponentStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->liveTraceOutline->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->precisionStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->processingParametersVector.value()[0].getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->stackAngleRangesStructVector.value()[0].getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->stackAzimuthRangesStructVector.value()[0].getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->stackOffsetRangesStructVector.value()[0].getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->timeLapseStruct->getPartition() == dummyPartitionName_);
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto seismicTraceDataStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(seismicTraceDataStruct_ptr == nullptr);

        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->binGridID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->horizontalCRSID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->seismicLineGeometryID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->traceRelationFileID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->principalAcquisitionProjectID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->processingProjectID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->preferred3DInterpretationSetID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->preferred2DInterpretationSetID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->seismicTraceDataDimensionalityTypeID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->seismicDomainTypeID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->seismicMigrationTypeID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->seismicStackingTypeID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->seismicAttributeTypeID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->seismicFilteringTypeID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->seismicPhaseID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->seismicPolarityID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->seismicProcessingStageTypeID = invalidIdString_;

        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->sampleInterval = -1;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->sampleCount = -1;

        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->traceDomainUOM = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->verticalMeasurementTypeID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->seismicWaveTypeID = invalidIdString_;
        seismicTraceDataStruct_ptr->seismicTraceDataSpecificProperties->sortOrderID = invalidIdString_;

        const auto qc_report = seismicTraceDataStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(seismicTraceDataStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 25);
    }

private:
    inline static constexpr const char* const seismicTraceDataJsonFileName = "SeismicTraceData.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduSeismicTraceDataTests)
} // namespace segysdk::osduMetadata::OsduSeismicTraceDataTests
