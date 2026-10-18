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

#include "BulkAccess/SegyBulkReader.h"
#include "Utilities/JsonUtilities.h"
#include "segysdk/SegySession.h"
#include "segysdk/SegySessionFactory.h"

namespace segysdk::api::tests
{

class SegyBulkReaderTestBase : public ApiTestBase
{
protected:
    void SetUp() override
    {
        ApiTestBase::SetUp();

        std::vector<std::string> segyFileList{getTestdataPathParameter("dataset").string()};
        std::string jsonParams = getTestdataPathParameter("jsonParameters").string();
        auto factory = std::make_unique<segysdk::api::SegySessionFactory>(segyFileList, segysdk::utilities::JsonUtilities::GetJsonStringFromFile(jsonParams));
        session_ = factory->CreateSegySession();
        session_->generateIndexFile();
        index_ = session_->getIndex();
        reader_ = session_->createBulkReader();
    }

    void TearDown() override
    {
        ApiTestBase::TearDown();

        index_->deleteIndex();
    }

protected:
    std::unique_ptr<segysdk::api::SegySession> session_;
    std::shared_ptr<segysdk::bulkaccess::SegyBulkReader> reader_;
    std::shared_ptr<segysdk::segyindex::SegyIndex> index_;
};

class SegyBulkReaderGetBufferSize : public SegyBulkReaderTestBase
{
public:
    auto run()
    {
        return reader_->getBufferSize();
    }
};
TEST_API(SegyBulkReaderGetBufferSize, "ApiTests/SegyBulkReaderTests.json")

class SegyBulkReaderReadUncached : public SegyBulkReaderTestBase
{
public:
    auto run()
    {
        size_t inlineIndex = getParameter<size_t>("inlineIndex");
        size_t xlineIndex = getParameter<size_t>("xlineIndex");
        size_t sampleIndex = getParameter<size_t>("sampleIndex");
        size_t inlineCount = getParameter<size_t>("inlineCount");
        size_t xlineCount = getParameter<size_t>("xlineCount");
        size_t sampleCount = getParameter<size_t>("sampleCount");

        const size_t brickSize = inlineCount * xlineCount * sampleCount;
        std::vector<float> brickBuffer(brickSize, 0);
        reader_->read(inlineIndex, xlineIndex, sampleIndex, inlineCount, xlineCount, sampleCount, brickBuffer.data());
        return brickBuffer;
    }
};
TEST_API(SegyBulkReaderReadUncached, "ApiTests/SegyBulkReaderTests.json")

class SegyBulkReaderLoadDataToCache : public SegyBulkReaderTestBase
{
public:
    auto run()
    {
        size_t offset = getParameter<size_t>("fileOffset");
        size_t cacheSize = getParameter<size_t>("cacheSize");

        reader_->LoadDataToCache(offset, cacheSize);
    }
};
TEST_API(SegyBulkReaderLoadDataToCache, "ApiTests/SegyBulkReaderTests.json")

class SegyBulkReaderReadCached : public SegyBulkReaderTestBase
{
public:
    auto run()
    {
        size_t inlineIndex = getParameter<size_t>("inlineIndex");
        size_t xlineIndex = getParameter<size_t>("xlineIndex");
        size_t sampleIndex = getParameter<size_t>("sampleIndex");
        size_t inlineCount = getParameter<size_t>("inlineCount");
        size_t xlineCount = getParameter<size_t>("xlineCount");
        size_t sampleCount = getParameter<size_t>("sampleCount");

        size_t offset = getParameter<size_t>("fileOffset");
        size_t cacheSize = getParameter<size_t>("cacheSize");

        reader_->LoadDataToCache(offset, cacheSize);
        const size_t brickSize = inlineCount * xlineCount * sampleCount;
        std::vector<float> brickBuffer(brickSize, 0);
        reader_->read(inlineIndex, xlineIndex, sampleIndex, inlineCount, xlineCount, sampleCount, brickBuffer.data());
        return brickBuffer;
    }
};
TEST_API(SegyBulkReaderReadCached, "ApiTests/SegyBulkReaderTests.json")

} // namespace segysdk::api::tests

int
main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    segysdk::api::tests::ApiEnvironment::instantiateFromCLI(argc, argv);
    return RUN_ALL_TESTS();
}
