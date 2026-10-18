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

namespace segysdk::osduMetadata::OsduSeismicTraceNavigationTests
{
using namespace segysdk::metadata::osdu;

class OsduSeismicTraceNavigationTests : public OsduMetadataTestBase<OsduSeismicTraceNavigation, OsduSeismicTraceNavigationStruct>
{
protected:
    OsduSeismicTraceNavigationTests()
      : OsduMetadataTestBase(seismicTraceNavigationJsonFileName)
    {
        /* Empty */
    }

    virtual void ValidatePartitionPropagation() const override
    {
        const auto seismicTraceNavigationStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(seismicTraceNavigationStruct_ptr == nullptr);

        seismicTraceNavigationStruct_ptr->setPartition(dummyPartitionName_);
        ASSERT_TRUE(seismicTraceNavigationStruct_ptr->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicTraceNavigationStruct_ptr->contentStruct.getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicTraceNavigationStruct_ptr->lineSegmentsStruct.value()[0].getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicTraceNavigationStruct_ptr->spatialLocationStruct->getPartition() == dummyPartitionName_);
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto seismicTraceNavigationStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(seismicTraceNavigationStruct_ptr == nullptr);

        seismicTraceNavigationStruct_ptr->originalSeismicLineGeometryId = invalidIdString_;

        const auto qc_report = seismicTraceNavigationStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(seismicTraceNavigationStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
    }

private:
    inline static constexpr const char* const seismicTraceNavigationJsonFileName = "SeismicTraceNavigation.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduSeismicTraceNavigationTests)
} // namespace segysdk::osduMetadata::OsduSeismicTraceNavigationTests
