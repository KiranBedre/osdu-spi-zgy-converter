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
#include "../OsduMetadataTests/OsduMetadataTestBase.h"

namespace segysdk::osduMetadata::OsduFileCollectionOpenZgyTests
{
using namespace segysdk::metadata::osdu;

class OsduFileCollectionOpenZgyTests : public OsduMetadataTestBase<OsduFileCollectionOpenZgy, OsduFileCollectionOpenZgyStruct>
{
protected:
    OsduFileCollectionOpenZgyTests()
      : OsduMetadataTestBase(fileCollectionOpenZgyJsonFileName)
    {
        /* Empty */
    }

    virtual void ValidatePartitionPropagation() const override
    {
        const auto fileCollectionOpenZgyStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(fileCollectionOpenZgyStruct_ptr == nullptr);

        fileCollectionOpenZgyStruct_ptr->setPartition(dummyPartitionName_);
        ASSERT_TRUE(fileCollectionOpenZgyStruct_ptr->commonMetadataStruct.getPartition() == dummyPartitionName_);
        ASSERT_TRUE(fileCollectionOpenZgyStruct_ptr->dataStruct->commonResourcesStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(fileCollectionOpenZgyStruct_ptr->dataStruct->datasetStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(fileCollectionOpenZgyStruct_ptr->dataStruct->compressionInfoStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(fileCollectionOpenZgyStruct_ptr->dataStruct->precisionStruct->getPartition() == dummyPartitionName_);
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto fileCollectionOpenZgyStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(fileCollectionOpenZgyStruct_ptr == nullptr);

        fileCollectionOpenZgyStruct_ptr->dataStruct->compressionInfoStruct.value().compressionMethodTypeID = invalidIdString_;

        const auto qc_report = fileCollectionOpenZgyStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(fileCollectionOpenZgyStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 4);
    }

private:
    inline static constexpr const char* const fileCollectionOpenZgyJsonFileName = "FileCollection.OpenZgy.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduFileCollectionOpenZgyTests)

} // namespace segysdk::osduMetadata::OsduFileCollectionSegyTests
