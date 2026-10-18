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

namespace segysdk::osduMetadata::OsduAclTests
{
using namespace segysdk::metadata::osdu;

class OsduAclTests : public OsduMetadataTestBase<OsduAcl, OsduAclStruct>
{
protected:
    OsduAclTests()
      : OsduMetadataTestBase(fileSeismicHistogramJsonFileName)
    {
        // Retrieve OsduAcl JSON from top-level metadata class
        metadataJson_ = metadataJson_[OsduFileHistogramLiterals::Acl];
    }

    virtual void ValidatePartitionPropagation() const override
    {
        // No nested structures
        SUCCEED();
    }

    virtual void ValidateAttributesInvalidation() const override
    {
        const auto aclStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(aclStruct_ptr == nullptr);

        aclStruct_ptr->owners.emplace_back(emailRegexTest1);
        aclStruct_ptr->owners.emplace_back(emailRegexTest2);
        aclStruct_ptr->owners.emplace_back(emailRegexTest3);
        aclStruct_ptr->owners.emplace_back(emailRegexTest4);

        aclStruct_ptr->viewers.emplace_back(emailRegexTest1);
        aclStruct_ptr->viewers.emplace_back(emailRegexTest2);
        aclStruct_ptr->viewers.emplace_back(emailRegexTest3);
        aclStruct_ptr->viewers.emplace_back(emailRegexTest4);

        auto qc_report = aclStruct_ptr->performAttributeValidityScan();
        ASSERT_TRUE(aclStruct_ptr->isValid());
        ASSERT_TRUE(qc_report.empty());

        aclStruct_ptr->owners.emplace_back(invalidEmail_);
        aclStruct_ptr->viewers.emplace_back(invalidEmail_);

        qc_report = aclStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(aclStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 3);

        aclStruct_ptr->owners.clear();

        qc_report = aclStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(aclStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 3);

        aclStruct_ptr->viewers.clear();

        qc_report = aclStruct_ptr->performAttributeValidityScan();
        ASSERT_FALSE(aclStruct_ptr->isValid());
        ASSERT_FALSE(qc_report.empty());
        ASSERT_TRUE(qc_report.size() == 3);
    }

private:
    inline static constexpr const char* const fileSeismicHistogramJsonFileName = "File.SeismicHistogram.json";
};

DEFINE_COMMON_MEATADATA_TESTS(OsduAclTests)
} // namespace segysdk::osduMetadata::OsduAclTests
