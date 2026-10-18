/*
Copyright 2021-2022 Schlumberger

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

#include "../FileIO/FileReferenceUtilities.h"
#include "../SegyIO/FileHeaderDefinitionFactory.h"
#include "../Utilities/Environment.h"
#include "../Utilities/TokenSourceFactory.h"

#include <gtest/gtest.h>
#include <string>
#include <vector>

namespace segysdk::segyio::tests
{

class ParameterizedFileHeaderDescriptionTest : public testing::TestWithParam<std::tuple<int, bool>>
{

protected:
    std::vector<std::string> getFileNames(bool isLocal)
    {

        if (isLocal) {
            std::vector<std::string> fileNames{"SegyFile_1.segy", "D:/temp/SegyFile_1.segy"};
            return fileNames;
        }
        else {
            std::vector<std::string> fileNames{"sd://sntc/SegyFile_1.sgy", "sd://sntc/SegyFile_2.sgy"};
            return fileNames;
        }
    }

    static SDCredentials sdmsConfig(const bool isLocal)
    {

        if (isLocal)
            return SDCredentials::none();

        // For tests where we do not know what environment variables are set or not ...
        std::string sdms_token = utilities::Environment::instance().sdToken();
        if (sdms_token.empty()) {
            sdms_token = "JWT token goes here";
            utilities::EnvironmentVariable::setStringEnv(utilities::EnvironmentVariableStringConsts::SD_SVC_TOKEN, sdms_token);
        }
        std::string sdms_url = utilities::Environment::instance().sdUrl();
        if (sdms_url.empty()) {
            sdms_url = "SDMS url goes here";
            utilities::EnvironmentVariable::setStringEnv(utilities::EnvironmentVariableStringConsts::SD_SVC_URL, sdms_url);
        }
        std::string sdms_app_key = utilities::Environment::instance().sdApiKey();
        if (sdms_app_key.empty()) {
            sdms_app_key = "SDMS App Key goes here";
            utilities::EnvironmentVariable::setStringEnv(utilities::EnvironmentVariableStringConsts::SD_SVC_API_KEY, sdms_app_key);
        }

        auto tokenSource = utilities::TokenSourceFactory::getTokenSource();
        if (tokenSource->token().empty()) {
            tokenSource
                = utilities::auth::TokenSource::fromToken("Environment variable SD_SVC_TOKEN not set");
        }

        return {sdms_url, sdms_app_key, std::move(tokenSource)};
    }

    void SetUp() override
    {
        // For tests where we do not know what environment variables are set or not ...
        std::string sdms_token = utilities::Environment::instance().sdToken();

        std::string sdms_url = utilities::Environment::instance().sdUrl();

        std::string sdms_app_key = utilities::Environment::instance().sdApiKey();

        segyVersion = std::get<0>(GetParam());
        isLocalFile = std::get<1>(GetParam());

        segyFileNames = getFileNames(isLocalFile);

        const auto context = std::make_shared<segyformat::external::SegyContext>(segyFileNames, segyVersion);
        const segyio::FileHeaderDescriptionFactory factory(context);

        file_header_description_ = factory.GetFileHeaderDesctription();
    }

    // Test global variables
    std::shared_ptr<segyio::FileHeaderDescription> file_header_description_;
    std::vector<std::string> segyFileNames;
    int segyVersion = 0;
    bool isLocalFile = false;
};

TEST_P(ParameterizedFileHeaderDescriptionTest, getParamFileNameTest)
{
    ASSERT_EQ(segyFileNames.size(), 2);

    const auto fr = fileio::utilities::FileReferenceUtilities::getGenericFileReference(segyFileNames[0], sdmsConfig(isLocalFile), isLocalFile);

    const auto parameterFileName = file_header_description_->getParameterFileName(fr.fileName());
    const std::string expectedName = segyFileNames[0] + ".params.json";
    EXPECT_TRUE(parameterFileName == expectedName);

    const fileio::FileReference result(parameterFileName, fr.readerOptions());

    if (isLocalFile) {
        EXPECT_TRUE(result.readerOptions().sdCredentials.apiKey().empty());
        EXPECT_TRUE(result.readerOptions().sdCredentials.token().empty());
        EXPECT_TRUE(result.readerOptions().sdCredentials.url().empty());
    }
    else {

        if (utilities::Environment::instance().sdApiKey().empty()) {
            EXPECT_EQ(result.readerOptions().sdCredentials.apiKey(), "Environment variable SD_SVC_API_KEY not set");
        }
        else {
            EXPECT_FALSE(result.readerOptions().sdCredentials.apiKey().empty());
        }

        if (utilities::Environment::instance().sdToken().empty()) {
            EXPECT_EQ(result.readerOptions().sdCredentials.token(), "Environment variable SD_SVC_TOKEN not set");
        }
        else {
            EXPECT_FALSE(result.readerOptions().sdCredentials.token().empty());
        }

        if (utilities::Environment::instance().sdUrl().empty()) {
            EXPECT_EQ(result.readerOptions().sdCredentials.url(), "Environment variable SD_SVC_URL not set");
        }
        else {
            EXPECT_FALSE(result.readerOptions().sdCredentials.url().empty());
        }

        EXPECT_TRUE(result.readerOptions().cacheSize.pageSize() > 0);
        EXPECT_TRUE(result.readerOptions().cacheSize.pages() > 0);
    }
}

INSTANTIATE_TEST_SUITE_P(

    FileHeaderDescriptionTests,
    ParameterizedFileHeaderDescriptionTest,
    ::testing::Values(
        std::make_tuple<int, bool>(0, true),
        std::make_tuple<int, bool>(1, true),
        std::make_tuple<int, bool>(0, false),
        std::make_tuple<int, bool>(1, false)));

} // namespace segysdk::segyio::tests
