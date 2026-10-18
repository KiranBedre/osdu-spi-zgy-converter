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

namespace segysdk::osduMetadata::OsduAbstractFileCollectionTests
{
using namespace segysdk::metadata::osdu;

class OsduAbstractFileCollectionTests : public OsduMetadataTestBase<OsduAbstractFileCollection, OsduAbstractFileCollectionStruct>
{
protected:
    OsduAbstractFileCollectionTests()
      : OsduMetadataTestBase(fileCollectionSegyJsonFileName)
    {
        // Retrieve DatasetProperties JSON from top-level metadata class
        Json::Value json = Json::objectValue;
        json[OsduAbstractFileCollectionLiterals::DatasetProperties] = metadataJson_[OsduFileCollectionSegyLiterals::Data][OsduAbstractFileCollectionLiterals::DatasetProperties];
        metadataJson_ = json;
    }

    virtual void ValidatePartitionPropagation() const override
    {
        const auto abstractFileCollectionStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(abstractFileCollectionStruct_ptr == nullptr);

        abstractFileCollectionStruct_ptr->setPartition(dummyPartitionName_);
        ASSERT_TRUE(abstractFileCollectionStruct_ptr->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(abstractFileCollectionStruct_ptr->fileSourceInfoStructs.value()[0].getPartition() == dummyPartitionName_);
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto abstractFileCollectionStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(abstractFileCollectionStruct_ptr == nullptr);

        abstractFileCollectionStruct_ptr->fileCollectionPath.clear();
        abstractFileCollectionStruct_ptr->fileSourceInfoStructs.value()[0].fileSize = invalidNumericString_;
        abstractFileCollectionStruct_ptr->fileSourceInfoStructs.value()[0].encodingFormatTypeID = invalidIdString_;
        abstractFileCollectionStruct_ptr->fileSourceInfoStructs.value()[0].checksum = invalidChecksum_;

        const auto qc_report = abstractFileCollectionStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(abstractFileCollectionStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 6);
    }

private:
    inline static constexpr const char* const fileCollectionSegyJsonFileName = "FileCollection.SEGY.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduAbstractFileCollectionTests)
} // namespace segysdk::osduMetadata::OsduAbstractFileCollectionTests
