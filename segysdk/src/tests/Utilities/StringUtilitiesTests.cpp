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

#include "../Utilities/StringUtils.h"

#include <gtest/gtest.h>

namespace segysdk::utilities::tests
{

TEST(StringUtilitiesTests, StringWithLettersOnly)
{

    const std::string inputMessage = "Kilroy woz here!";
    const std::string result = StringUtils::ToUppercase(inputMessage);
    EXPECT_EQ(inputMessage, "Kilroy woz here!");
    EXPECT_EQ(result, "KILROY WOZ HERE!");
}

TEST(StringUtilitiesTests, StringWithLettersAndNumbers)
{

    const std::string inputMessage = "1234567890 Kilroy woz here.";
    const std::string result = StringUtils::ToUppercase(inputMessage);
    EXPECT_EQ(inputMessage, "1234567890 Kilroy woz here.");
    EXPECT_EQ(result, "1234567890 KILROY WOZ HERE.");
}

TEST(StringUtilitiesTests, StringWithLettersNumbersAndSpecialChars)
{

    const std::string inputMessage = "1234567890 Kilroy woz here.,/?#[]!£$%^&*()\"";
    const std::string result = StringUtils::ToUppercase(inputMessage);
    EXPECT_EQ(inputMessage, "1234567890 Kilroy woz here.,/?#[]!£$%^&*()\"");
    EXPECT_EQ(result, "1234567890 KILROY WOZ HERE.,/?#[]!£$%^&*()\"");
}

TEST(StringUtilitiesTests, StringWithLettersOnlyToLower)
{

    const std::string inputMessage = "Kilroy WoZ HerE.";
    const std::string result = StringUtils::ToLowercase(inputMessage);
    EXPECT_EQ(inputMessage, "Kilroy WoZ HerE.");
    EXPECT_EQ(result, "kilroy woz here.");
}

TEST(StringUtilitiesTests, ReplaceAllTest)
{

    const std::string sourcePath = R"(D:\Data\Json\Test\Data.sgy)";
    std::string result = sourcePath;
    StringUtils::ReplaceAll(result, "\\", "/");
    EXPECT_EQ(sourcePath, R"(D:\Data\Json\Test\Data.sgy)");
    EXPECT_EQ(result, R"(D:/Data/Json/Test/Data.sgy)");
}

TEST(StringUtilitiesTests, BeginsWithTest)
{

    const std::string key = "gabagaba";
    const std::string test = "gaba";
    const auto result = StringUtils::keyBeginsWith(key, test);

    EXPECT_TRUE(result);
}

TEST(StringUtilitiesTests, NotBeginsWithTest)
{

    const std::string key = "gabagaba";
    const std::string test = "gababa";
    const auto result = StringUtils::keyBeginsWith(key, test);

    EXPECT_FALSE(result);
}

TEST(StringUtilitiesTests, EndsWithTest)
{

    const std::string key = "gabagaba2";
    const std::string test = "aba2";
    const auto result = StringUtils::keyEndsWith(key, test);

    EXPECT_TRUE(result);
}

TEST(StringUtilitiesTests, NotEndsWithTest)
{

    const std::string key = "gabagaba2";
    const std::string test = "aba";
    const auto result = StringUtils::keyEndsWith(key, test);

    EXPECT_FALSE(result);
}

TEST(StringUtilitiesTests, KeyMatchesTest)
{

    const std::string key = "gabagaba2";
    const std::string test = "gaba";
    const auto result = StringUtils::keyMatches(key, test);

    EXPECT_TRUE(result);
}

TEST(StringUtilitiesTests, NotKeyMatchesTest)
{

    const std::string key = "gabagaba";
    const std::string test = "a2ba";
    const auto result = StringUtils::keyMatches(key, test);

    EXPECT_FALSE(result);
}

TEST(StringUtilitiesTests, SplitTest_default)
{

    const std::string key = "ababababa&ebebebebeb&dgdgdgdgdgdg&dkdkdkdkdk";
    const std::vector<std::string> expected{
        "ababababa", "ebebebebeb", "dgdgdgdgdgdg", "dkdkdkdkdk"};
    const std::string delimiter = "&";
    const auto result = StringUtils::split(key, delimiter);

    EXPECT_EQ(result.size(), expected.size());
    for (std::size_t i = 0; i < result.size(); i++) {
        EXPECT_EQ(result.at(i), expected.at(i));
    }
}

TEST(StringUtilitiesTests, SplitTest_KeepDelimiter1)
{

    const std::string key = "ababababa&ebebebebeb&dgdgdgdgdgdg&dkdkdkdkdk";
    const std::vector<std::string> expected{
        "ababababa&", "ebebebebeb&", "dgdgdgdgdgdg&", "dkdkdkdkdk"};

    const std::string delimiter = "&";
    const bool keepDelimiter = true;

    const auto result = StringUtils::split(key, delimiter, keepDelimiter);
    EXPECT_EQ(result.size(), expected.size());
    for (std::size_t i = 0; i < result.size(); i++) {
        EXPECT_EQ(result.at(i), expected.at(i));
    }
}

TEST(StringUtilitiesTests, SplitTest_KeepDelimiter2)
{

    const std::string key = "ababababa&-&ebebebebeb&-&dgdgdgdgdgdg&-&dkdkdkdkdk";
    const std::vector<std::string> expected{
        "ababababa&-&", "ebebebebeb&-&", "dgdgdgdgdgdg&-&", "dkdkdkdkdk"};

    const std::string delimiter = "&-&";
    const bool keepDelimiter = true;

    const auto result = StringUtils::split(key, delimiter, keepDelimiter);
    EXPECT_EQ(result.size(), expected.size());
    for (std::size_t i = 0; i < result.size(); i++) {
        EXPECT_EQ(result.at(i), expected.at(i));
    }
}

} // namespace segysdk::utilities::tests
