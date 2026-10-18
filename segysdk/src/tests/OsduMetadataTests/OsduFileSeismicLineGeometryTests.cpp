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

namespace segysdk::osduMetadata::OsduFileSeismicLineGeometryTests
{
using namespace segysdk::metadata::osdu;

class OsduFileSeismicLineGeometryTests : public OsduMetadataTestBase<OsduFileSeismicLineGeometry, OsduFileSeismicLineGeometryStruct>
{
protected:
    OsduFileSeismicLineGeometryTests()
      : OsduMetadataTestBase(fileSeismicLineGeometryJsonFileName)
    {
        /* Empty */
    }

    virtual void ValidatePartitionPropagation() const override
    {
        const auto fileSeismicLineGeometryStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(fileSeismicLineGeometryStruct_ptr == nullptr);

        fileSeismicLineGeometryStruct_ptr->setPartition(dummyPartitionName_);
        ASSERT_TRUE(fileSeismicLineGeometryStruct_ptr->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(fileSeismicLineGeometryStruct_ptr->commonMetadataStruct.getPartition() == dummyPartitionName_);
        ASSERT_TRUE(fileSeismicLineGeometryStruct_ptr->fileSeismicLineGeometryDataStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(fileSeismicLineGeometryStruct_ptr->fileSeismicLineGeometryDataStruct->commonResourcesStruct->getPartition() == dummyPartitionName_);
        ASSERT_TRUE(fileSeismicLineGeometryStruct_ptr->fileSeismicLineGeometryDataStruct->datasetStruct->getPartition() == dummyPartitionName_);
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto fileSeismicLineGeometryStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(fileSeismicLineGeometryStruct_ptr == nullptr);

        fileSeismicLineGeometryStruct_ptr->fileSeismicLineGeometryDataStruct->contentSchemaKind = invalidIdString_;

        const auto qc_report = fileSeismicLineGeometryStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(fileSeismicLineGeometryStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 3);
    }

private:
    inline static constexpr const char* const fileSeismicLineGeometryJsonFileName = "File.SeismicLineGeometry.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduFileSeismicLineGeometryTests)
} // namespace segysdk::osduMetadata::OsduFileSeismicLineGeometryTests
