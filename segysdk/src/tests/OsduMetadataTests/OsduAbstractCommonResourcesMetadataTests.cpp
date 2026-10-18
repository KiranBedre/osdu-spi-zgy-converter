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

namespace segysdk::osduMetadata::OsduAbstractCommonResourcesTests
{
using namespace segysdk::metadata::osdu;

class OsduAbstractCommonResourcesTests : public OsduMetadataTestBase<OsduAbstractCommonResources, OsduAbstractCommonResourcesStruct>
{
protected:
    OsduAbstractCommonResourcesTests()
      : OsduMetadataTestBase(fileSeismicLineGeometryJsonFileName)
    {
        // Retrieve OsduAbstractCommonResourcesTests JSON from top-level metadata class
        // Additionally, filter unnecessary fields
        Json::Value json{Json::objectValue};
        json[OsduAbstractCommonResourcesLiterals::ResourceHomeRegionID] = metadataJson_[OsduFileCollectionSegyLiterals::Data][OsduAbstractCommonResourcesLiterals::ResourceHomeRegionID];
        json[OsduAbstractCommonResourcesLiterals::ResourceHostRegionIDs] = metadataJson_[OsduFileCollectionSegyLiterals::Data][OsduAbstractCommonResourcesLiterals::ResourceHostRegionIDs];
        json[OsduAbstractCommonResourcesLiterals::ResourceCurationStatus] = metadataJson_[OsduFileCollectionSegyLiterals::Data][OsduAbstractCommonResourcesLiterals::ResourceCurationStatus];
        json[OsduAbstractCommonResourcesLiterals::ResourceLifecycleStatus] = metadataJson_[OsduFileCollectionSegyLiterals::Data][OsduAbstractCommonResourcesLiterals::ResourceLifecycleStatus];
        json[OsduAbstractCommonResourcesLiterals::Source] = metadataJson_[OsduFileCollectionSegyLiterals::Data][OsduAbstractCommonResourcesLiterals::Source];
        json[OsduAbstractCommonResourcesLiterals::ExistenceKind] = metadataJson_[OsduFileCollectionSegyLiterals::Data][OsduAbstractCommonResourcesLiterals::ExistenceKind];

        metadataJson_ = json;
    }

    virtual void ValidatePartitionPropagation() const override
    {
        // No nested structures
        SUCCEED();
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto commonResourcesStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(commonResourcesStruct_ptr == nullptr);

        commonResourcesStruct_ptr->resourceHomeRegionID = invalidIdString_;
        commonResourcesStruct_ptr->resourceHostRegionIDs->emplace_back(invalidIdString_);
        commonResourcesStruct_ptr->resourceCurationStatus = invalidIdString_;
        commonResourcesStruct_ptr->resourceLifecycleStatus = invalidIdString_;
        commonResourcesStruct_ptr->existenceKind = invalidIdString_;

        const auto qc_report = commonResourcesStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(commonResourcesStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 6);
    }

private:
    inline static constexpr const char* const fileSeismicLineGeometryJsonFileName = "File.SeismicLineGeometry.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduAbstractCommonResourcesTests)
} // namespace segysdk::osduMetadata::OsduAbstractCommonResourcesTests
