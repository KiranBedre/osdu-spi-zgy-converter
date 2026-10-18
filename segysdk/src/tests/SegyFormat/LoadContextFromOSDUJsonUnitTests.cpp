/*
Copyright 2021 Schlumberger

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

#include "JsonLoaderTestDataProvider.h"

#include "../SegyFormat/External/LoadContextFromJson.h"
#include "../SegyFormat/External/LoadContextFromOSDUJson.h"
#include "../SegyFormat/External/OSDUReferenceData.h"
#include "../SegyFormat/External/SegyHeaderField.h"
#include "../SegyFormat/External/TraceHeaderField.h"
#include "../SegyIO/SegyIOException.h"
#include "../Utilities/StringUtils.h"

#include <gtest/gtest.h>

#include <string>

namespace segysdk::segyformat::external::tests
{
using namespace segyformat::external;

class LoadContextFromOSDUJsonUnitTest : public testing::Test
{

protected:
    void SetUp() override
    {

        json = JsonLoaderTestDataProvider::SetupFileCollectionJson();

        fileList.push_back("sd://opendes/sntc/segy/mig.sgy");
    }

    // virtual void TearDown() {}

    Json::Value json;
    std::vector<std::string> fileList;

    std::string VectorHeaderMapping = "VectorHeaderMapping";
};

TEST_F(LoadContextFromOSDUJsonUnitTest, osduKeyNameReferenceDataUnitTestsGood)
{
    const std::string referenceString = "opendes:reference-data--HeaderKeyName:Inline:";
    const auto osduRefData = OSDUKeyNameReference(referenceString);

    ASSERT_TRUE(osduRefData.requiredFieldsPopulated());
    ASSERT_TRUE(osduRefData.isValidReference());

    EXPECT_EQ(osduRefData.getSchema(), "opendes");
    EXPECT_EQ(osduRefData.getReferenceType(), "reference-data--HeaderKeyName");
    EXPECT_EQ(osduRefData.getReferenceValueToUpper(), "INLINE");
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduKeyNameReferenceDataUnitTestsAlmostRight)
{
    const std::string referenceString = "opendes:reference-data--HeaderKeyName:Inline";
    const auto osduRefData = OSDUKeyNameReference(referenceString);

    ASSERT_TRUE(osduRefData.requiredFieldsPopulated());
    ASSERT_TRUE(osduRefData.isValidReference());

    EXPECT_EQ(osduRefData.getSchema(), "opendes");
    EXPECT_EQ(osduRefData.getReferenceType(), "reference-data--HeaderKeyName");
    EXPECT_EQ(osduRefData.getReferenceValueToUpper(), "INLINE");
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduKeyNameReferenceDataUnitTestsAlmostMissingSchema)
{
    const std::string referenceString = ":reference-data--HeaderKeyName:Inline";
    const auto osduRefData = OSDUKeyNameReference(referenceString);

    ASSERT_TRUE(osduRefData.requiredFieldsPopulated());
    ASSERT_TRUE(osduRefData.isValidReference());

    ASSERT_TRUE(osduRefData.getSchema().empty());
    EXPECT_EQ(osduRefData.getReferenceType(), "reference-data--HeaderKeyName");
    EXPECT_EQ(osduRefData.getReferenceValueToUpper(), "INLINE");
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduKeyNameReferenceDataUnitTestsAlmostMissingTypeRef)
{
    const std::string referenceString = "opendes:--HeaderKeyName:Inline";
    const auto osduRefData = OSDUKeyNameReference(referenceString);

    ASSERT_TRUE(osduRefData.requiredFieldsPopulated());
    ASSERT_TRUE(osduRefData.isValidReference());

    EXPECT_EQ(osduRefData.getSchema(), "opendes");
    EXPECT_EQ(osduRefData.getReferenceType(), "--HeaderKeyName");
    EXPECT_EQ(osduRefData.getReferenceValueToUpper(), "INLINE");
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduKeyNameReferenceDataUnitTestsInvalidTypeRef)
{
    const std::string referenceString = "opendes:KeyName:Inline";
    const auto osduRefData = OSDUKeyNameReference(referenceString);

    ASSERT_TRUE(osduRefData.requiredFieldsPopulated());
    ASSERT_FALSE(osduRefData.isValidReference());

    EXPECT_EQ(osduRefData.getSchema(), "opendes");
    EXPECT_EQ(osduRefData.getReferenceType(), "KeyName");
    EXPECT_EQ(osduRefData.getReferenceValueToUpper(), "INLINE");
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduKeyNameReferenceDataUnitTestsBad)
{
    const std::string referenceString = "Inline";

    ASSERT_THROW(auto osduRefData = OSDUKeyNameReference(referenceString), SegyInvalidArgumentException);
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduWordFormatReferenceDataUnitTestsGood)
{
    const std::string referenceString = "opendes:reference-data--HeaderKeyName:Inline:";
    const auto osduRefData = OSDUKeyNameReference(referenceString);

    ASSERT_TRUE(osduRefData.requiredFieldsPopulated());
    ASSERT_TRUE(osduRefData.isValidReference());

    EXPECT_EQ(osduRefData.getSchema(), "opendes");
    EXPECT_EQ(osduRefData.getReferenceType(), "reference-data--HeaderKeyName");
    EXPECT_EQ(osduRefData.getReferenceValueToUpper(), "INLINE");
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduWordFormatReferenceDataUnitTestsAlmostRight)
{
    const std::string referenceString = "opendes:reference-data--HeaderKeyName:Inline";
    const auto osduRefData = OSDUKeyNameReference(referenceString);

    ASSERT_TRUE(osduRefData.requiredFieldsPopulated());
    ASSERT_TRUE(osduRefData.isValidReference());

    EXPECT_EQ(osduRefData.getSchema(), "opendes");
    EXPECT_EQ(osduRefData.getReferenceType(), "reference-data--HeaderKeyName");
    EXPECT_EQ(osduRefData.getReferenceValueToUpper(), "INLINE");
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduWordFormatReferenceDataUnitTestsAlmostMissingSchema)
{
    const std::string referenceString = ":reference-data--HeaderKeyName:Inline";
    const auto osduRefData = OSDUKeyNameReference(referenceString);

    ASSERT_TRUE(osduRefData.requiredFieldsPopulated());
    ASSERT_TRUE(osduRefData.isValidReference());

    ASSERT_TRUE(osduRefData.getSchema().empty());
    EXPECT_EQ(osduRefData.getReferenceType(), "reference-data--HeaderKeyName");
    EXPECT_EQ(osduRefData.getReferenceValueToUpper(), "INLINE");
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduWordFormatReferenceDataUnitTestsAlmostMissingTypeRef)
{
    const std::string referenceString = "opendes:--HeaderKeyName:Inline";
    const auto osduRefData = OSDUKeyNameReference(referenceString);

    ASSERT_TRUE(osduRefData.requiredFieldsPopulated());
    ASSERT_TRUE(osduRefData.isValidReference());

    EXPECT_EQ(osduRefData.getSchema(), "opendes");
    EXPECT_EQ(osduRefData.getReferenceType(), "--HeaderKeyName");
    EXPECT_EQ(osduRefData.getReferenceValueToUpper(), "INLINE");
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduWordFormatReferenceDataUnitTestsInvalidTypeRef)
{
    const std::string referenceString = "opendes:KeyName:Inline";
    const auto osduRefData = OSDUKeyNameReference(referenceString);

    ASSERT_TRUE(osduRefData.requiredFieldsPopulated());
    ASSERT_FALSE(osduRefData.isValidReference());

    EXPECT_EQ(osduRefData.getSchema(), "opendes");
    EXPECT_EQ(osduRefData.getReferenceType(), "KeyName");
    EXPECT_EQ(osduRefData.getReferenceValueToUpper(), "INLINE");
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduWordFormatReferenceDataUnitTestsBad)
{
    const std::string referenceString = "Inline";

    ASSERT_THROW(auto osduRefData = OSDUKeyNameReference(referenceString), SegyInvalidArgumentException);
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduUoMReferenceDataUnitTestsGood)
{
    const std::string referenceString = "opendes:reference-data--UnitOfMeasure:ms:";
    const auto osduRefData = OSDUUnitOfMeasureReference(referenceString);

    ASSERT_TRUE(osduRefData.requiredFieldsPopulated());
    ASSERT_TRUE(osduRefData.isValidReference());

    EXPECT_EQ(osduRefData.getSchema(), "opendes");
    EXPECT_EQ(osduRefData.getReferenceType(), "reference-data--UnitOfMeasure");
    EXPECT_EQ(osduRefData.getReferenceValueToUpper(), utilities::StringUtils::ToUppercase("ms"));
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduUoMReferenceDataUnitTestsAlmostRight)
{
    const std::string referenceString = "opendes:reference-data--UnitOfMeasure:ms";
    const auto osduRefData = OSDUUnitOfMeasureReference(referenceString);

    ASSERT_TRUE(osduRefData.requiredFieldsPopulated());
    ASSERT_TRUE(osduRefData.isValidReference());

    EXPECT_EQ(osduRefData.getSchema(), "opendes");
    EXPECT_EQ(osduRefData.getReferenceType(), "reference-data--UnitOfMeasure");
    EXPECT_EQ(osduRefData.getReferenceValueToUpper(), utilities::StringUtils::ToUppercase("ms"));
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduUoMReferenceDataUnitTestsAlmostMissingSchema)
{
    const std::string referenceString = ":reference-data--UnitOfMeasure:ft";
    const auto osduRefData = OSDUUnitOfMeasureReference(referenceString);

    ASSERT_TRUE(osduRefData.requiredFieldsPopulated());
    ASSERT_TRUE(osduRefData.isValidReference());

    ASSERT_TRUE(osduRefData.getSchema().empty());
    EXPECT_EQ(osduRefData.getReferenceType(), "reference-data--UnitOfMeasure");
    EXPECT_EQ(osduRefData.getReferenceValueToUpper(), utilities::StringUtils::ToUppercase("ft"));
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduUoMReferenceDataUnitTestsAlmostMissingTypeRef)
{
    const std::string referenceString = "opendes:--UnitOfMeasure:ft";
    const auto osduRefData = OSDUUnitOfMeasureReference(referenceString);

    ASSERT_TRUE(osduRefData.requiredFieldsPopulated());
    ASSERT_TRUE(osduRefData.isValidReference());

    EXPECT_EQ(osduRefData.getSchema(), "opendes");
    EXPECT_EQ(osduRefData.getReferenceType(), "--UnitOfMeasure");
    EXPECT_EQ(osduRefData.getReferenceValueToUpper(), utilities::StringUtils::ToUppercase("ft"));
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduUoMReferenceDataUnitTestsInvalidTypeRef)
{
    const std::string referenceString = "opendes:UnitOfMeasure:foot";
    const auto osduRefData = OSDUUnitOfMeasureReference(referenceString);

    ASSERT_TRUE(osduRefData.requiredFieldsPopulated());
    ASSERT_FALSE(osduRefData.isValidReference());

    EXPECT_EQ(osduRefData.getSchema(), "opendes");
    EXPECT_EQ(osduRefData.getReferenceType(), "UnitOfMeasure");
    EXPECT_EQ(osduRefData.getReferenceValueToUpper(), utilities::StringUtils::ToUppercase("foot"));
}

TEST_F(LoadContextFromOSDUJsonUnitTest, osduUoMReferenceDataUnitTestsBad)
{
    const std::string referenceString = "ft";

    ASSERT_THROW(auto osduRefData = OSDUUnitOfMeasureReference(referenceString), SegyInvalidArgumentException);
}

TEST_F(LoadContextFromOSDUJsonUnitTest, testGoodOSDUParameters)
{

    LoadContextFromOSDUJson jsonParser(json);
    ASSERT_TRUE(jsonParser.canGetContext());

    const auto members = json.getMemberNames();

    ASSERT_TRUE(members.size() > 5);

    auto ctxt = jsonParser.getContext(fileList, 0);
    EXPECT_EQ(ctxt->getSegyVersion(), 0);
    EXPECT_EQ(ctxt->getSegyFileNames(), fileList);

    ASSERT_TRUE(JsonLoaderTestDataProvider::ListContains(members, "data"));
    Json::Value jsonData = json["data"];

    auto dataMembers = jsonData.getMemberNames();
    ASSERT_TRUE(JsonLoaderTestDataProvider::ListContains(dataMembers, VectorHeaderMapping));
    Json::Value jsonMapping = jsonData[VectorHeaderMapping];
    ASSERT_TRUE(!jsonMapping.empty());

    std::vector<TraceHeaderField> inputHdrs;
    for (auto& item : jsonMapping) {
        TraceHeaderField headerMapping;
        ASSERT_NO_THROW(headerMapping = JsonLoaderTestDataProvider::getHeaderDefinitionFromJson(item));
        inputHdrs.push_back(headerMapping);
    }

    auto contextHeaders = ctxt->getTraceHeaderDefinitions();
    for (std::shared_ptr<SegyHeaderField>& fld : contextHeaders) {
        auto& thCtxt = dynamic_cast<TraceHeaderField&>(*fld);
        auto inputHdr = JsonLoaderTestDataProvider::getMatchingHeaderFromInputSet(inputHdrs, fld);
        if (inputHdr.getKey().empty())
            continue;

        EXPECT_EQ(inputHdr.getKey(), thCtxt.getKey());
        EXPECT_EQ(inputHdr.getUoM(), thCtxt.getUoM());
        EXPECT_EQ(inputHdr.getOffset(), thCtxt.getOffset());
        EXPECT_EQ(inputHdr.getLength(), thCtxt.getLength());
        EXPECT_EQ(inputHdr.getScalarAttribute(), thCtxt.getScalarAttribute());
        EXPECT_EQ(inputHdr.getNumericFormat(), thCtxt.getNumericFormat());
        EXPECT_EQ(inputHdr.getScalarOverrideValue(), thCtxt.getScalarOverrideValue());
    }
}

TEST_F(LoadContextFromOSDUJsonUnitTest, testIncorrectOSDUParameters)
{
    const auto members = json.getMemberNames();

    ASSERT_TRUE(members.size() > 5);
    ASSERT_TRUE(JsonLoaderTestDataProvider::ListContains(members, "data"));
    Json::Value jsonData = json["data"];

    auto dataMembers = jsonData.getMemberNames();
    ASSERT_TRUE(JsonLoaderTestDataProvider::ListContains(dataMembers, VectorHeaderMapping));

    jsonData[VectorHeaderMapping] = JsonLoaderTestDataProvider::getIncorrectVectorHeaderMappingOSDU();

    Json::Value jsonMapping = jsonData[VectorHeaderMapping];
    ASSERT_TRUE(!jsonMapping.empty());

    LoadContextFromOSDUJson jsonParser(json);
    ASSERT_TRUE(jsonParser.canGetContext());

    auto ctxt = jsonParser.getContext(fileList, 0);
    EXPECT_EQ(ctxt->getSegyVersion(), 0);
    EXPECT_EQ(ctxt->getSegyFileNames(), fileList);

    std::vector<TraceHeaderField> inputHdrs;
    for (auto& item : jsonMapping) {
        TraceHeaderField headerMapping;
        ASSERT_NO_THROW(headerMapping = JsonLoaderTestDataProvider::getHeaderDefinitionFromJson(item));

        inputHdrs.push_back(headerMapping);
    }

    auto contextHeaders = ctxt->getTraceHeaderDefinitions();
    for (std::shared_ptr<SegyHeaderField>& fld : contextHeaders) {
        auto& thCtxt = dynamic_cast<TraceHeaderField&>(*fld);
        auto inputHdr = JsonLoaderTestDataProvider::getMatchingHeaderFromInputSet(inputHdrs, fld);
        if (inputHdr.getKey().empty())
            continue;

        EXPECT_EQ(inputHdr.getKey(), thCtxt.getKey());
        EXPECT_EQ(inputHdr.getUoM(), thCtxt.getUoM());
        EXPECT_EQ(inputHdr.getOffset(), thCtxt.getOffset());
        EXPECT_EQ(inputHdr.getLength(), thCtxt.getLength());
        EXPECT_EQ(inputHdr.getScalarAttribute(), thCtxt.getScalarAttribute());
        EXPECT_EQ(inputHdr.getNumericFormat(), thCtxt.getNumericFormat());
        EXPECT_EQ(inputHdr.getScalarOverrideValue(), thCtxt.getScalarOverrideValue());
    }
}

TEST_F(LoadContextFromOSDUJsonUnitTest, testUnknownNameOSDUParameters)
{
    const auto members = json.getMemberNames();
    ASSERT_TRUE(members.size() > 5);

    ASSERT_TRUE(JsonLoaderTestDataProvider::ListContains(members, "data"));
    const Json::Value jsonData = json["data"];

    const auto dataMembers = jsonData.getMemberNames();
    ASSERT_TRUE(JsonLoaderTestDataProvider::ListContains(dataMembers, VectorHeaderMapping));

    json["data"][VectorHeaderMapping] = JsonLoaderTestDataProvider::illegalHeaderNameMapping();

    const LoadContextFromOSDUJson jsonParser(json);
    ASSERT_TRUE(jsonParser.canGetContext());
}

TEST_F(LoadContextFromOSDUJsonUnitTest, testInvalidOffsetOSDUParameters)
{

    const auto members = json.getMemberNames();
    ASSERT_TRUE(members.size() > 5);

    ASSERT_TRUE(JsonLoaderTestDataProvider::ListContains(members, "data"));
    const Json::Value jsonData = json["data"];

    const auto dataMembers = jsonData.getMemberNames();
    ASSERT_TRUE(JsonLoaderTestDataProvider::ListContains(dataMembers, VectorHeaderMapping));

    json["data"][VectorHeaderMapping] = JsonLoaderTestDataProvider::illegalOffsetMapping();

    ASSERT_NO_THROW(const LoadContextFromOSDUJson jsonParser(json));
    const LoadContextFromOSDUJson jsonParser(json);
    ASSERT_TRUE(jsonParser.canGetContext());
}

} // namespace segysdk::segyformat::external::tests
