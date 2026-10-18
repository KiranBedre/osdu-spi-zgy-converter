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

#include "../OsduMetaDataUtilities/OsduFileCollectionSegyLiterals.h"
#include "../OsduMetaDataUtilities/OsduFileCollectionSegyDataLiterals.h"

namespace segysdk::osduMetadata::OsduVectorHeaderMappingTests
{
using namespace segysdk::metadata::osdu;

class OsduVectorHeaderMappingTests : public OsduMetadataTestBase<OsduVectorHeaderMapping, OsduVectorHeaderMappingStruct>
{
protected:
    OsduVectorHeaderMappingTests()
      : OsduMetadataTestBase(fileCollectionSegyJsonFileName)
    {
        // Retrieve VectorHeaderMapping JSON from top-level metadata class
        metadataJson_ = metadataJson_[OsduFileCollectionSegyLiterals::Data][OsduFileCollectionSegyDataLiterals::VectorHeaderMapping];
    }

    virtual void ValidateInputJson() const override
    {
        ASSERT_FALSE(metadataJson_.empty());

        ASSERT_TRUE(metadataJson_.isArray());
        
        const auto members = metadataJson_[0].getMemberNames();
        ASSERT_FALSE(members.empty());
    }

    virtual void ValidatePartitionPropagation() const override
    {
        const auto vectorHeaderMappingStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(vectorHeaderMappingStruct_ptr == nullptr);

        vectorHeaderMappingStruct_ptr->setPartition(dummyPartitionName_);
        ASSERT_TRUE(vectorHeaderMappingStruct_ptr->getPartition() == vectorHeaderMappingStruct_ptr->vectorHeaderMapping_[0].getPartition());
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto vectorHeaderMappingStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(vectorHeaderMappingStruct_ptr == nullptr);

        OsduVectorHeaderMappingElementStruct invalidElementStruct(vectorHeaderMappingStruct_ptr->getPartition());
        invalidElementStruct.keyName = invalidIdString_;
        invalidElementStruct.wordFormatType = invalidIdString_;
        invalidElementStruct.wordWidth = 5;
        invalidElementStruct.position = 300;
        invalidElementStruct.uom = invalidIdString_;

        vectorHeaderMappingStruct_ptr->vectorHeaderMapping_[0].uom = invalidIdString_;
        vectorHeaderMappingStruct_ptr->vectorHeaderMapping_.push_back(invalidElementStruct);

        const auto qc_report = vectorHeaderMappingStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(vectorHeaderMappingStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 9);
    }

private:
    inline static constexpr const char* const fileCollectionSegyJsonFileName = "FileCollection.SEGY.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduVectorHeaderMappingTests)
} // namespace segysdk::osduMetadata::OsduVectorHeaderMappingTests
