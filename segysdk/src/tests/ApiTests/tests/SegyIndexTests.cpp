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

#include "../library/JsonSerializers/IndexFieldJson.h"
#include "../library/JsonSerializers/JsonSerializer.h"

#include "SegyIndex/SegyIndex.h"
#include "SegyIndex/SegyIndexImpl.h"
#include "Utilities/JsonUtilities.h"
#include "segysdk/SegySession.h"
#include "segysdk/SegySessionFactory.h"

namespace segysdk::api::tests
{

class SegyIndexTestBase : public ApiTestBase
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
    }

    void TearDown() override
    {
        ApiTestBase::TearDown();

        index_->deleteIndex();
    }

protected:
    std::unique_ptr<segysdk::api::SegySession> session_;
    std::shared_ptr<segysdk::segyindex::SegyIndex> index_;
};

class SegyHasIndexFile : public SegyIndexTestBase
{
public:
    auto run()
    {
        auto storageFileCheck = getParameter<bool>("storageFileCheck");
        return index_->hasIndexFile(storageFileCheck);
    }
};
TEST_API(SegyHasIndexFile, "ApiTests/SegyIndexTests.json")

class SegyIndexDelete : public SegyIndexTestBase
{
public:
    auto run()
    {
        index_->deleteIndex();
        return index_->hasIndexFile(true);
    }
};
TEST_API(SegyIndexDelete, "ApiTests/SegyIndexTests.json")

class SegyGenerateIndex : public SegyIndexTestBase
{
public:
    auto run()
    {
        const bool forceGenerate = getParameter<bool>("forceGenerate");
        return index_->GenerateIndex(forceGenerate);
    }
};
TEST_API(SegyGenerateIndex, "ApiTests/SegyIndexTests.json")

class SegyIndexSaveAsJson : public SegyIndexTestBase
{
public:
    auto run()
    {
        const std::string filename = GetParam().getExpectedFile().string();
        index_->GenerateIndex(true);
        index_->SaveAsJson(filename);
    }
};
TEST_API(SegyIndexSaveAsJson, "ApiTests/SegyIndexTests.json")

class SegyIndexGetIndexAt : public SegyIndexTestBase
{
public:
    auto run()
    {
        index_->GenerateIndex(true);
        index_->Load();

        const int inlineNo = getParameter<int>("inline");
        const int crosslineNo = getParameter<int>("crossline");
        return JsonSerializer::toJson<IndexFieldJson>(index_->getIndexAt(inlineNo, crosslineNo));
    }
};
TEST_API(SegyIndexGetIndexAt, "ApiTests/SegyIndexTests.json")

class SegyIndexLoad : public SegyIndexTestBase
{
public:
    void SetUp() override
    {
        SegyIndexTestBase::SetUp();
        index_->Load();
    }
};

class SegyIndexGetGeometry3D : public SegyIndexLoad
{
public:
    auto run()
    {
        auto geometry = index_->getGeometry3D();
        auto geometryJson = segysdk::segyindex::SegyIndexImpl::getGeometry3DAsJson(geometry);
        return geometryJson.toStyledString();
    }
};
TEST_API(SegyIndexGetGeometry3D, "ApiTests/SegyIndexTests.json")

class SegyIndexGetFileLayout : public SegyIndexLoad
{
public:
    auto run()
    {
        return segysdk::segyindex::SegyIndexImpl::getFileLayoutAsJson(index_->getFileLayout()).toStyledString();
    }
};
TEST_API(SegyIndexGetFileLayout, "ApiTests/SegyIndexTests.json")

class SegyIndexIsRegular : public SegyIndexLoad
{
public:
    auto run()
    {
        return index_->isRegular();
    }
};
TEST_API(SegyIndexIsRegular, "ApiTests/SegyIndexTests.json")

class SegyIndexIsValid : public SegyIndexLoad
{
public:
    auto run()
    {
        return index_->isValid();
    }
};
TEST_API(SegyIndexIsValid, "ApiTests/SegyIndexTests.json")

class SegyIndexGetTraceSize : public SegyIndexLoad
{
public:
    auto run()
    {
        return index_->getTraceSize();
    }
};
TEST_API(SegyIndexGetTraceSize, "ApiTests/SegyIndexTests.json")

class SegyIndexGetFirstTraceOffset : public SegyIndexLoad
{
public:
    auto run()
    {
        constexpr const uint16_t fileId = 0;
        return index_->getFirstTraceOffset(fileId);
    }
};
TEST_API(SegyIndexGetFirstTraceOffset, "ApiTests/SegyIndexTests.json")

class SegyIndexIsInlineOrdered : public SegyIndexLoad
{
public:
    auto run()
    {
        return index_->isInlineOrdered();
    }
};
TEST_API(SegyIndexIsInlineOrdered, "ApiTests/SegyIndexTests.json")

class SegyIndexHasConstantCoordinateScalar : public SegyIndexLoad
{
public:
    auto run()
    {
        return index_->hasConstantCoordinateScalar();
    }
};
TEST_API(SegyIndexHasConstantCoordinateScalar, "ApiTests/SegyIndexTests.json")

class SegyIndexHasConstantTraceLength : public SegyIndexLoad
{
public:
    auto run()
    {
        return index_->hasConstantTraceLength();
    }
};
TEST_API(SegyIndexHasConstantTraceLength, "ApiTests/SegyIndexTests.json")

class SegyIndexGetConstantCoordinateScalar : public SegyIndexLoad
{
public:
    auto run()
    {
        return index_->getConstantCoordinateScalar();
    }
};
TEST_API(SegyIndexGetConstantCoordinateScalar, "ApiTests/SegyIndexTests.json")

class SegyIndexGetConstantTraceLength : public SegyIndexLoad
{
public:
    auto run()
    {
        return index_->getConstantTraceLength();
    }
};
TEST_API(SegyIndexGetConstantTraceLength, "ApiTests/SegyIndexTests.json")

class SegyIndexGetConstantTraceWeightingFactor : public SegyIndexLoad
{
public:
    auto run()
    {
        return index_->getConstantTraceWeightingFactor();
    }
};
TEST_API(SegyIndexGetConstantTraceWeightingFactor, "ApiTests/SegyIndexTests.json")

class SegyIndexGetIndexedTraceCount : public SegyIndexLoad
{
public:
    auto run()
    {
        return index_->getIndexedTraceCount();
    }
};
TEST_API(SegyIndexGetIndexedTraceCount, "ApiTests/SegyIndexTests.json")

} // namespace segysdk::api::tests

int
main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    segysdk::api::tests::ApiEnvironment::instantiateFromCLI(argc, argv);
    return RUN_ALL_TESTS();
}
