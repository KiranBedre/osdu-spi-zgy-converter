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

#include "../Utilities/Environment.h"
#include "../Utilities/JsonUtilities.h"

#include <gtest/gtest.h>

namespace segysdk::osduMetadata
{
using namespace segysdk::metadata::osdu;

template <typename T, typename Struct>
class OsduMetadataTestBase : public testing::Test
{
protected:
    OsduMetadataTestBase(const std::string& jsonFileName)
      : jsonPath_(getFileName(jsonFileName))
    {
        if (!jsonPath_.empty()) {
            metadataJsonStr_ = segysdk::utilities::JsonUtilities::GetJsonStringFromFile(jsonPath_);
            metadataJson_ = segysdk::utilities::JsonUtilities::getJsonFromString(metadataJsonStr_);
        }
    }

    auto getMetadataStructFromJson() const
    {
        T object(metadataJson_, partitionName_);
        return std::reinterpret_pointer_cast<Struct>(object.getMetaDataStruct());
    }

protected:
    virtual void ValidateInputJson() const
    {
        ASSERT_FALSE(metadataJson_.empty());

        const auto members = metadataJson_.getMemberNames();
        ASSERT_FALSE(members.empty());
    }

    virtual void ValidateInputMetadata() const
    {
        const auto metadataStruct_ptr = getMetadataStructFromJson();
        ASSERT_FALSE(metadataStruct_ptr == nullptr);

        const auto results = metadataStruct_ptr->performAttributeValidityScan();
        int lineNo = 0;
        for (const auto& result : results) {
            std::cerr << "Result: " << std::to_string(++lineNo) << " : " << result << '\n';
        }

        ASSERT_TRUE(metadataStruct_ptr->isValid());
        ASSERT_TRUE(results.empty());
    }

    virtual void ValidatePartitionPropagation() const
    {
        FAIL();
    }

    virtual void ValidateAttributesInvalidation() const
    {
        FAIL();
    }

    virtual void ValidateCopyingMetadata() const
    {
        const auto metadataStruct_ptr = getMetadataStructFromJson();

        T objectCopy(*metadataStruct_ptr);

        const auto json_string1 = metadataJson_.toStyledString();
        const auto json_string2 = objectCopy.getAsFormattedString();
        ASSERT_TRUE(json_string1 == json_string2);
    }

    std::string jsonPath_;
    std::string metadataJsonStr_;
    Json::Value metadataJson_;

    inline static constexpr const char* const partitionName_ = "namespace";
    inline static constexpr const char* const dummyPartitionName_ = "partition";
    inline static constexpr const char* const invalidIdString_ = "invalid ID";
    inline static constexpr const char* const invalidKindString_ = "invalid kind";
    inline static constexpr const char* const invalidNumericString_ = "abcde";
    inline static constexpr const char* const invalidChecksum_ = "ghijk";
    inline static constexpr const char* const invalidEmail_ = "invalid email";
    inline static constexpr const char* const invalidCountryCode_ = "123";
    inline static constexpr const char* const invalidCompliance_ = "invalid compliance";
    inline static constexpr const char* const emailRegexTest1 = "owners@company.com";
    inline static constexpr const char* const emailRegexTest2 = "data-owners@namespace.enterprisedata.cloud.company.com";
    inline static constexpr const char* const emailRegexTest3 = "data.owners@namespace.enterprisedata.cloud.company-ds.com";
    inline static constexpr const char* const emailRegexTest4 = "data.default.owners@namespace.enterprisedata.cloud.company-ds.com";

private:
    static std::string getFileName(const std::string& fileName)
    {
        if (std::filesystem::exists(fileName))
            return fileName;

        auto const filePath = utilities::Environment::instance().SegyLocalTestDataPath();
        if (filePath.empty()) {
            throw std::runtime_error("Required environment variable missing. Please specify its location by the SEGY_LOCALTESTDATA_PATH environment variable to run this test.");
        }

        std::filesystem::path myPath(filePath);
        myPath.append("OSDU");
        myPath.append("Metadata");
        myPath.append(fileName);
        if (std::filesystem::exists(myPath))
            return myPath.string();

        throw std::runtime_error("File " + myPath.string() + " not found or not accessible.");
    }
};

#define DEFINE_COMMON_MEATADATA_TESTS(MetadataTestClass) \
TEST_F(MetadataTestClass, ValidateInputJson) \
{ \
    ValidateInputJson(); \
} \
 \
TEST_F(MetadataTestClass, ValidateInputMetadata) \
{ \
    ValidateInputMetadata(); \
} \
 \
TEST_F(MetadataTestClass, ValidatePartitionPropagation) \
{ \
    ValidatePartitionPropagation(); \
} \
 \
TEST_F(MetadataTestClass, ValidateAttributesInvalidation) \
{ \
    ValidateAttributesInvalidation(); \
} \
 \
TEST_F(MetadataTestClass, ValidateCopyingMetadata)        \
{ \
    ValidateCopyingMetadata(); \
}

} // namespace segysdk::osduMetadata
