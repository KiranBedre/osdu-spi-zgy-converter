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

namespace segysdk::osduMetadata::OsduAbstractDatasetTests
{
using namespace segysdk::metadata::osdu;

class OsduAbstractDatasetTests : public OsduMetadataTestBase<OsduAbstractDataset, OsduAbstractDatasetStruct>
{
protected:
    OsduAbstractDatasetTests()
      : OsduMetadataTestBase(fileSeismicLineGeometryJsonFileName)
    {
        // Retrieve OsduAbstractDatasetTests JSON from top-level metadata class
        // Additionally, filter unnecessary fields
        Json::Value json{Json::objectValue};
        json[OsduAbstractDatasetLiterals::Description] = metadataJson_[OsduFileCollectionSegyLiterals::Data][OsduAbstractDatasetLiterals::Description];
        json[OsduAbstractDatasetLiterals::EncodingFormatTypeID] = metadataJson_[OsduFileCollectionSegyLiterals::Data][OsduAbstractDatasetLiterals::EncodingFormatTypeID];
        json[OsduAbstractDatasetLiterals::Endian] = metadataJson_[OsduFileCollectionSegyLiterals::Data][OsduAbstractDatasetLiterals::Endian];
        json[OsduAbstractDatasetLiterals::Name] = metadataJson_[OsduFileCollectionSegyLiterals::Data][OsduAbstractDatasetLiterals::Name];
        json[OsduAbstractDatasetLiterals::SchemaFormatTypeID] = metadataJson_[OsduFileCollectionSegyLiterals::Data][OsduAbstractDatasetLiterals::SchemaFormatTypeID];
        json[OsduAbstractDatasetLiterals::TotalSize] = metadataJson_[OsduFileCollectionSegyLiterals::Data][OsduAbstractDatasetLiterals::TotalSize];
        
        metadataJson_ = json;
    }

    virtual void ValidatePartitionPropagation() const override
    {
        // No nested structures
        SUCCEED();
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto datasetStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(datasetStruct_ptr == nullptr);

        datasetStruct_ptr->totalSize = invalidNumericString_;
        datasetStruct_ptr->encodingFormatTypeID = invalidIdString_;
        datasetStruct_ptr->schemaFormatTypeID = invalidIdString_;
        datasetStruct_ptr->endian = "MEDIUM";

        const auto qc_report = datasetStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(datasetStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 5);
    }

private:
    inline static constexpr const char* const fileSeismicLineGeometryJsonFileName = "File.SeismicLineGeometry.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduAbstractDatasetTests)
}
// namespace segysdk::osduMetadata::OsduAbstractDatasetTests
