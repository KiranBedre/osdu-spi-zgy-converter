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

namespace segysdk::osduMetadata::OsduAncestryTests
{
using namespace segysdk::metadata::osdu;

class OsduAncestryTests : public OsduMetadataTestBase<OsduAncestry, OsduAncestryStruct>
{
protected:
    OsduAncestryTests()
        : OsduMetadataTestBase(fileSeismicHistogramJsonFileName)
    {
        // Retrieve OsduAcl JSON from top-level metadata class
        metadataJson_ = metadataJson_[OsduFileHistogramLiterals::Ancestry];
    }

    virtual void ValidatePartitionPropagation() const override
    {
        // No nested structures
        SUCCEED();
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto ancestryStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(ancestryStruct_ptr == nullptr);

        ancestryStruct_ptr->parents->emplace_back(invalidIdString_);

        auto qc_report = ancestryStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(ancestryStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 2);

        ancestryStruct_ptr->parents->pop_back();

        const std::string dummyReferencePrefix = "namespace:classArea:className:";

        ancestryStruct_ptr->parents->emplace_back(dummyReferencePrefix + "1234");
        ancestryStruct_ptr->parents->emplace_back(dummyReferencePrefix + "5678");

        qc_report = ancestryStruct_ptr->performAttributeValidityScan();
        ASSERT_TRUE(ancestryStruct_ptr->isValid());
        ASSERT_TRUE(qc_report.empty());

        ancestryStruct_ptr->parents->emplace_back(invalidIdString_);

        qc_report = ancestryStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(ancestryStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 2);
    }

private:
    inline static constexpr const char* const fileSeismicHistogramJsonFileName = "File.SeismicHistogram.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduAncestryTests)
} // namespace segysdk::osduMetadata::OsduAncestryTests
