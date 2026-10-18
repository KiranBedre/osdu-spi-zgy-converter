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

#include "../OsduMetaDataUtilities/OsduFileSeismicLineGeometryDataLiterals.h"
#include "../OsduMetaDataUtilities/OsduAbstractFileLiterals.h"

namespace segysdk::osduMetadata::OsduAbstractFileSourceInfoTests
{
using namespace segysdk::metadata::osdu;

class OsduAbstractFileSourceInfoTests : public OsduMetadataTestBase<OsduAbstractFileSourceInfo, OsduAbstractFileSourceInfoStruct>
{
protected:
    OsduAbstractFileSourceInfoTests()
      : OsduMetadataTestBase(fileSeismicLineGeometryJsonFileName)
    {
        // Retrieve VectorHeaderMapping JSON from top-level metadata class
        metadataJson_ = metadataJson_[OsduFileSeismicLineGeometryLiterals::Data][OsduAbstractFileLiterals::DatasetProperties][OsduAbstractFileLiterals::FileSourceInfo];
    }

    virtual void ValidatePartitionPropagation() const override
    {
        SUCCEED();
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto abstractFileSourceInfoStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(abstractFileSourceInfoStruct_ptr == nullptr);

        abstractFileSourceInfoStruct_ptr->fileSource.clear();
        abstractFileSourceInfoStruct_ptr->fileSize = invalidNumericString_;
        abstractFileSourceInfoStruct_ptr->encodingFormatTypeID = invalidIdString_;
        abstractFileSourceInfoStruct_ptr->checksum = invalidChecksum_;

        const auto qc_report = abstractFileSourceInfoStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(abstractFileSourceInfoStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 5);
    }

private:
    inline static constexpr const char* const fileSeismicLineGeometryJsonFileName = "File.SeismicLineGeometry.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduAbstractFileSourceInfoTests)
} // namespace segysdk::osduMetadata::OsduAbstractFileSourceInfoTests
