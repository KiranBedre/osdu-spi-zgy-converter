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

namespace segysdk::osduMetadata::OsduContentSeismicHistogramTests
{
using namespace segysdk::metadata::osdu;

class OsduContentSeismicHistogramTests : public OsduMetadataTestBase<OsduContentSeismicHistogram, OsduContentSeismicHistogramStruct>
{
protected:
    OsduContentSeismicHistogramTests()
      : OsduMetadataTestBase(seismicHistogramJsonFileName)
    {
        /* Empty */
    }

    virtual void ValidatePartitionPropagation() const override
    {
        const auto seismicHistogramStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(seismicHistogramStruct_ptr == nullptr);

        seismicHistogramStruct_ptr->setPartition(dummyPartitionName_);
        ASSERT_TRUE(seismicHistogramStruct_ptr->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(seismicHistogramStruct_ptr->abstractContentStruct.getPartition() == dummyPartitionName_);
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto seismicHistogramStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(seismicHistogramStruct_ptr == nullptr);

        seismicHistogramStruct_ptr->seismicRecordID = invalidIdString_;
        seismicHistogramStruct_ptr->amplitudeUnitOfMeasureID = invalidIdString_;

        const auto qc_report = seismicHistogramStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(seismicHistogramStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 3);
    }

private:
    inline static constexpr const char* const seismicHistogramJsonFileName = "SeismicHistogram.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduContentSeismicHistogramTests)
} // namespace segysdk::osduMetadata::OsduContentSeismicHistogramTests
