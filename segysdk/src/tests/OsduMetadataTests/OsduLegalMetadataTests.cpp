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

namespace segysdk::osduMetadata::OsduLegalTests
{
using namespace segysdk::metadata::osdu;

class OsduLegalTests : public OsduMetadataTestBase<OsduLegal, OsduLegalStruct>
{
protected:
    OsduLegalTests()
      : OsduMetadataTestBase(fileSeismicHistogramJsonFileName)
    {
        // Retrieve OsduAcl JSON from top-level metadata class
        metadataJson_ = metadataJson_[OsduFileHistogramLiterals::Legal];
    }

    virtual void ValidatePartitionPropagation() const override
    {
        // No nested structures
        SUCCEED();
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto legalStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(legalStruct_ptr == nullptr);

        legalStruct_ptr->otherRelevantDataCountries.emplace_back(invalidCountryCode_);
        legalStruct_ptr->status = invalidCompliance_;

        auto qc_report = legalStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(legalStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 3);

        legalStruct_ptr->otherRelevantDataCountries.clear();

        qc_report = legalStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(legalStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 3);
    }

private:
    inline static constexpr const char* const fileSeismicHistogramJsonFileName = "File.SeismicHistogram.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduLegalTests)
} // namespace segysdk::osduMetadata::OsduLegalTests
