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

namespace segysdk::osduMetadata::OsduWorkProductTests
{
using namespace segysdk::metadata::osdu;

class OsduWorkProductTests : public OsduMetadataTestBase<OsduWorkProduct, OsduWorkProductStruct>
{
protected:
    OsduWorkProductTests()
      : OsduMetadataTestBase(workProductJsonFileName)
    {
        /* Empty */
    }

    virtual void ValidatePartitionPropagation() const override
    {
        const auto workProductStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(workProductStruct_ptr == nullptr);

        workProductStruct_ptr->setPartition(dummyPartitionName_);
        ASSERT_TRUE(workProductStruct_ptr->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(workProductStruct_ptr->commonMetadataStruct.getPartition() == dummyPartitionName_);
        ASSERT_TRUE(workProductStruct_ptr->dataStruct->commonResources->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(workProductStruct_ptr->dataStruct->spatialPointStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(workProductStruct_ptr->dataStruct->spatialAreaStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(workProductStruct_ptr->dataStruct->lineageAssertionsStruct.value()[0].getPartition() == dummyPartitionName_);
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto workProductStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(workProductStruct_ptr == nullptr);

        workProductStruct_ptr->commonMetadataStruct.id = invalidIdString_;
        workProductStruct_ptr->dataStruct->components->emplace_back(invalidIdString_);

        const auto qc_report = workProductStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(workProductStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 5);
    }

private:
    inline static constexpr const char* const workProductJsonFileName = "WorkProduct.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduWorkProductTests)
} // namespace segysdk::osduMetadata::OsduWorkProductTests
