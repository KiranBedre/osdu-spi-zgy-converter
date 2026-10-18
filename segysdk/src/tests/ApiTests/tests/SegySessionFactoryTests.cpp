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

#include "../library/ApiTestFramework/ApiTest.h"
#include "../library/ApiTestFramework/ApiTestBase.h"

#include <Utilities/JsonUtilities.h>
#include <segysdk/SegySessionFactory.h>

namespace segysdk::api::tests
{
using namespace segysdk::api;

class SegySessionFactoryTestBase : public ApiTestBase
{
protected:
    void SetUp() override
    {
        std::vector<std::string> segyFileList{getTestdataPathParameter("dataset").string()};
        std::string jsonParams = getTestdataPathParameter("jsonParameters").string();
        factory_ = std::make_unique<SegySessionFactory>(segyFileList, segysdk::utilities::JsonUtilities::GetJsonStringFromFile(jsonParams));
    }

public:
    std::unique_ptr<SegySessionFactory> factory_;
};

class SegySessionFactoryCtor : public SegySessionFactoryTestBase
{
public:
    auto run()
    {
        std::vector<std::string> filenameList{getTestdataPathParameter("dataset").string()};
        std::string jsonParams = getTestdataPathParameter("jsonParameters").string();
        return std::make_unique<SegySessionFactory>(filenameList, segysdk::utilities::JsonUtilities::GetJsonStringFromFile(jsonParams));
    }
};

TEST_API(SegySessionFactoryCtor, "ApiTests/SegySessionFactoryTests.json")

class SegySessionFactoryCtorOsdu : public ApiTestBase
{
public:
    auto run()
    {
        std::string fileCollection = getTestdataPathParameter("dataset").string();
        return std::make_unique<SegySessionFactory>(segysdk::utilities::JsonUtilities::GetJsonStringFromFile(fileCollection));
    }
};

TEST_API(SegySessionFactoryCtorOsdu, "ApiTests/SegySessionFactoryTests.json")

class SegySessionFactoryCreateSegySession : public SegySessionFactoryTestBase
{
public:
    auto run()
    {
        return factory_->CreateSegySession();
    }
};

TEST_API(SegySessionFactoryCreateSegySession, "ApiTests/SegySessionFactoryTests.json")

class SegySessionFactoryGetFileName : public SegySessionFactoryTestBase
{
public:
    auto run()
    {
        std::filesystem::path filename = std::filesystem::path(factory_->getFileName()).filename();
        return filename.string();
    }
};

TEST_API(SegySessionFactoryGetFileName, "ApiTests/SegySessionFactoryTests.json")
} // namespace segysdk::api::tests