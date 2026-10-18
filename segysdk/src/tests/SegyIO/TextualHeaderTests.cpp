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

#include "../SegyFormat/External/SegyContext.h"

#include "../SegyIO/BinaryHeader.h"
#include "../SegyIO/FileHeaderDefinitionFactory.h"
#include "../segysdk/SegySessionFactory.h"

#include "../FileIO/DiskFileReader.h"
#include "../SegyIO/TextualHeader.h"
#include "../Utilities/Environment.h"
#include "../Utilities/StringUtils.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <string>

using namespace segysdk;
using namespace segysdk::segyio;

TEST(TextualHeaderTests, Rev0_TextualHeaders)
{
    const std::string filePath = segysdk::utilities::Environment::instance().SegyLocalTestDataPath();
    const auto inputFilename = std::filesystem::path{filePath} / "tiny_with_ascii_header.sgy";
    ASSERT_TRUE(std::filesystem::exists(inputFilename));

    const auto dfr = std::make_shared<fileio::DiskFileReader>(inputFilename.string());

    TextualHeader textualHdr(dfr, -1);

    auto textualHeaders = textualHdr.getTextualHeaderByLine();

    EXPECT_EQ(textualHeaders.size(), 40U);
    for (const auto& hdr : textualHeaders) {
        EXPECT_TRUE(segysdk::utilities::StringUtils::keyBeginsWith(hdr, "C"));
    }

    BinaryHeader binaryHdr(dfr, {}, {});
    ASSERT_TRUE(binaryHdr.getRevisionLevel() == 0); // Should be revision 0

    ASSERT_TRUE(binaryHdr.getExtendedTextualHeaderCount() == 0);
}

TEST(TextualHeaderTests, Rev1_ExtendedTextualHeaders)
{
    const std::string filePath = segysdk::utilities::Environment::instance().SegyLocalTestDataPath();
    const auto inputFilename = std::filesystem::path{filePath} / "CMZ_ExtendedHeaders_Rev1.sgy";
    ASSERT_TRUE(std::filesystem::exists(inputFilename));

    const auto dfr = std::make_shared<fileio::DiskFileReader>(inputFilename.string());

    TextualHeader textualHdr(dfr, -1);

    auto textualHeaders = textualHdr.getTextualHeaderByLine();

    EXPECT_TRUE(textualHeaders.size() == 40U);
    for (auto hdr : textualHeaders) {
        EXPECT_TRUE(segysdk::utilities::StringUtils::keyBeginsWith(hdr, "C"));
    }

    std::vector<std::string> fileList{inputFilename.string()};

    // Need to set up proper session for the rest of this test ...
    segysdk::api::SegySessionFactory sessionFactory(fileList, "{}");
    auto session = sessionFactory.CreateSegySession();
    auto context = std::make_shared<segysdk::segyformat::external::SegyContext>();

    FileHeaderDescriptionFactory fhdFactory(context);
    BinaryHeader binaryHdr(dfr, fhdFactory.Generate_binaryHeaders(), {});
    int revisionLevel = binaryHdr.getRevisionLevel();
    ASSERT_TRUE(revisionLevel == 0x100 || revisionLevel == 1); // Should be revision 1 is interpreted if value is 1, 0x100 or 100 (decimal)

    int headerCount = binaryHdr.getExtendedTextualHeaderCount();
    ASSERT_TRUE(headerCount > 0);

    for (int extHdrNo = 0; extHdrNo < headerCount; extHdrNo++) {
        auto extTextualHeaders = textualHdr.getExtendedHeaderByLine(extHdrNo);
        EXPECT_TRUE(extTextualHeaders.size() == 40U);
        // The headers contain rubbish values so no point testing them for content ...
    }
}
