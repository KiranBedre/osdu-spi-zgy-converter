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

#include "../OsduMetaDataUtilities/OsduMetafileApi.h"
#include "OsduMetadataTestBase.h"

namespace segysdk::osduMetadata::OsduSeismicLineGeometryTests
{
using namespace segysdk::metadata::osdu;

class OsduSeismicLineGeometryTests : public OsduMetadataTestBase<OsduSeismicLineGeometry, OsduSeismicLineGeometryStruct>
{
protected:
    OsduSeismicLineGeometryTests()
      : OsduMetadataTestBase(seismicLineGeometryJsonFileName)
    {
        /* Empty */
    }

    virtual void ValidatePartitionPropagation() const override
    {
        const auto seismicLineGeometryStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(seismicLineGeometryStruct_ptr == nullptr);

        seismicLineGeometryStruct_ptr->setPartition(dummyPartitionName_);
        ASSERT_TRUE(seismicLineGeometryStruct_ptr->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicLineGeometryStruct_ptr->commonMetadataStruct.getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicLineGeometryStruct_ptr->seismicLineGeometryDataStruct->commonResourcesStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicLineGeometryStruct_ptr->seismicLineGeometryDataStruct->wpcGroupTypeStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicLineGeometryStruct_ptr->seismicLineGeometryDataStruct->workProductComponentStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicLineGeometryStruct_ptr->seismicLineGeometryDataStruct->firstLocation->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicLineGeometryStruct_ptr->seismicLineGeometryDataStruct->lastLocation->getPartition() == dummyPartitionName_);
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto seismicLineGeometryStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(seismicLineGeometryStruct_ptr == nullptr);

        seismicLineGeometryStruct_ptr->seismicLineGeometryDataStruct->notionalSeismicLineID = invalidIdString_;
        seismicLineGeometryStruct_ptr->seismicLineGeometryDataStruct->preferred2DInterpretationSetID = invalidIdString_;
        seismicLineGeometryStruct_ptr->seismicLineGeometryDataStruct->horizontalCRSID = invalidIdString_;

        const auto qc_report = seismicLineGeometryStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(seismicLineGeometryStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 5);
    }

private:
    inline static constexpr const char* const seismicLineGeometryJsonFileName = "SeismicLineGeometry.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduSeismicLineGeometryTests)
} // namespace segysdk::osduMetadata::OsduSeismicLineGeometryTests
