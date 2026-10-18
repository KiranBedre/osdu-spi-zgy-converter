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

namespace segysdk::osduMetadata::OsduFileCollectionSegyTests
{
using namespace segysdk::metadata::osdu;

class OsduFileCollectionSegyTests : public OsduMetadataTestBase<OsduFileCollectionSegy, OsduFileCollectionSegyStruct>
{
protected:
    OsduFileCollectionSegyTests()
      : OsduMetadataTestBase(fileCollectionSegyJsonFileName)
    {
        /* Empty */
    }

    virtual void ValidatePartitionPropagation() const override
    {
        const auto fileCollectionSegyStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(fileCollectionSegyStruct_ptr == nullptr);

        fileCollectionSegyStruct_ptr->setPartition(dummyPartitionName_);
        ASSERT_TRUE(fileCollectionSegyStruct_ptr->commonMetadataStruct.getPartition() == dummyPartitionName_);
        ASSERT_TRUE(fileCollectionSegyStruct_ptr->dataStruct->commonResourcesStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(fileCollectionSegyStruct_ptr->dataStruct->datasetStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(fileCollectionSegyStruct_ptr->dataStruct->vectorHeaderMappingStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(fileCollectionSegyStruct_ptr->dataStruct->binaryHeaderOverridesStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(fileCollectionSegyStruct_ptr->dataStruct->traceHeaderOverridesStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(fileCollectionSegyStruct_ptr->dataStruct->precisionStruct->getPartition() == dummyPartitionName_);
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto fileCollectionSegyStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(fileCollectionSegyStruct_ptr == nullptr);

        fileCollectionSegyStruct_ptr->dataStruct->storageFacilityID = invalidIdString_;

        const auto qc_report = fileCollectionSegyStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(fileCollectionSegyStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 3);
    }

private:
    inline static constexpr const char* const fileCollectionSegyJsonFileName = "FileCollection.SEGY.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduFileCollectionSegyTests)
} // namespace segysdk::osduMetadata::OsduFileCollectionSegyTests
