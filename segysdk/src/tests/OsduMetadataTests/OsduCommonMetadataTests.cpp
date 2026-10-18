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
#include "../OsduMetaDataUtilities/OsduCommonMetadata.h"
#include "../OsduMetaDataUtilities/OsduCommonMetadataLiterals.h"
#include "../OsduMetaDataUtilities/OsduCommonMetadataDataLiterals.h"

#include "OsduMetadataTestBase.h"

namespace segysdk::osduMetadata::OsduCommonMetadataTests
{
using namespace segysdk::metadata::osdu;

class OsduCommonMetadataTests : public OsduMetadataTestBase<OsduCommonMetadata, OsduCommonMetadataStruct>
{
protected:
    OsduCommonMetadataTests()
      : OsduMetadataTestBase(workProductJsonFileName)
    {
        // Retrieve CommonMetadata JSON from top-level metadata class and filter out necessary fields
        Json::Value extensionPropertiesJson = metadataJson_[OsduCommonMetadataLiterals::Data][OsduCommonMetadataDataLiterals::ExtensionProperties];
        metadataJson_.removeMember(OsduCommonMetadataLiterals::Data);
        metadataJson_[OsduCommonMetadataLiterals::Data][OsduCommonMetadataDataLiterals::ExtensionProperties] = extensionPropertiesJson;
    }

    virtual void ValidatePartitionPropagation() const override
    {
        const auto commonMetadataStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(commonMetadataStruct_ptr == nullptr);

        commonMetadataStruct_ptr->setPartition(dummyPartitionName_);
        ASSERT_TRUE(commonMetadataStruct_ptr->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(commonMetadataStruct_ptr->aclStruct.getPartition() == dummyPartitionName_);
        ASSERT_TRUE(commonMetadataStruct_ptr->legalStruct.getPartition() == dummyPartitionName_);
        ASSERT_TRUE(commonMetadataStruct_ptr->tagsStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(commonMetadataStruct_ptr->ancestryStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(commonMetadataStruct_ptr->metaStruct.getPartition() == dummyPartitionName_);
        ASSERT_TRUE(commonMetadataStruct_ptr->dataStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(commonMetadataStruct_ptr->dataStruct->extensionPropertiesStruct.getPartition() == dummyPartitionName_);
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto commonMetadataStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(commonMetadataStruct_ptr == nullptr);

        commonMetadataStruct_ptr->id = invalidIdString_;
        commonMetadataStruct_ptr->kind = invalidKindString_;

        const auto qc_report = commonMetadataStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(commonMetadataStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
    }

private:
    inline static constexpr const char* const workProductJsonFileName = "WorkProduct.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduCommonMetadataTests)
} // namespace segysdk::osduMetadata::OsduCommonMetadataTests
