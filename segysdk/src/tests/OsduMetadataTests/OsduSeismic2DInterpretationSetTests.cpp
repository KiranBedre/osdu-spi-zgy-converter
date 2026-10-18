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

namespace segysdk::osduMetadata::OsduSeismic2DInterpretationSetTests
{

using namespace segysdk::metadata::osdu;

class OsduSeismic2DInterpretationSetTests : public OsduMetadataTestBase<OsduSeismic2DInterpretationSet, OsduSeismic2DInterpretationSetStruct>
{
protected:
    OsduSeismic2DInterpretationSetTests()
      : OsduMetadataTestBase(seismic2DInterpretationSetJsonFileName)
    {
        /* Empty */
    }

    virtual void ValidatePartitionPropagation() const override
    {
        const auto seismic2DInterpretationSetStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(seismic2DInterpretationSetStruct_ptr == nullptr);

        seismic2DInterpretationSetStruct_ptr->setPartition(dummyPartitionName_);
        ASSERT_TRUE(seismic2DInterpretationSetStruct_ptr->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismic2DInterpretationSetStruct_ptr->commonMetadataStruct.getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismic2DInterpretationSetStruct_ptr->dataStruct->commonResourcesStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismic2DInterpretationSetStruct_ptr->dataStruct->masterStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismic2DInterpretationSetStruct_ptr->dataStruct->projectStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismic2DInterpretationSetStruct_ptr->dataStruct->activityStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismic2DInterpretationSetStruct_ptr->dataStruct->seismicLineGeometriesStruct.value()[0].getPartition() == dummyPartitionName_);
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto seismic2DInterpretationSetStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(seismic2DInterpretationSetStruct_ptr == nullptr);

        seismic2DInterpretationSetStruct_ptr->dataStruct->masterStruct->technicalAssurancesStruct->front().technicalAssuranceTypeID = invalidIdString_;
        seismic2DInterpretationSetStruct_ptr->dataStruct->seismicLineGeometriesStruct.value()[0].id = invalidIdString_;

        const auto qc_report = seismic2DInterpretationSetStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(seismic2DInterpretationSetStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 7);
    }

private:
    inline static constexpr const char* const seismic2DInterpretationSetJsonFileName = "Seismic2DInterpretationSet.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduSeismic2DInterpretationSetTests)
} // namespace segysdk::osduMetadata::OsduSeismic2DInterpretationSetTests
