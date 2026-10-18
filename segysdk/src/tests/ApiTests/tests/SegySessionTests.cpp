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

#include "../library/ApiTestFramework/ApiEnvironment.h"
#include "../library/ApiTestFramework/ApiTest.h"
#include "../library/ApiTestFramework/ApiTestBase.h"
#include "../library/JsonSerializers/HeaderDataStructJson.h"
#include "../library/JsonSerializers/JsonSerializer.h"
#include "../library/JsonSerializers/TraceJson.h"

#include "Utilities/JsonUtilities.h"
#include "segysdk/SegySession.h"
#include "segysdk/SegySessionFactory.h"

namespace segysdk::api::tests
{
using namespace segysdk::api;
using namespace segysdk::utilities;

class SegySessionTestBase : public ApiTestBase
{
protected:
    void SetUp() override
    {
        ApiTestBase::SetUp();

        std::vector<std::string> segyFileList{getTestdataPathParameter("dataset").string()};
        std::string jsonParams = getTestdataPathParameter("jsonParameters").string();
        auto factory = std::make_unique<SegySessionFactory>(segyFileList, JsonUtilities::GetJsonStringFromFile(jsonParams));
        session_ = factory->CreateSegySession();
    }

protected:
    std::unique_ptr<SegySession> session_;
};

class SegySessionIndexTestBase : public SegySessionTestBase
{
protected:
    void SetUp() override
    {
        SegySessionTestBase::SetUp();

        std::filesystem::path segyFile = session_->getSegyFileName();
        std::string extension = segyFile.extension().string();

        // Save index filenames to cleanup
        std::filesystem::path indexFile = segyFile;
        indexFile.replace_extension(extension + ".idx");
        std::filesystem::path indexBinFile = segyFile;
        indexBinFile.replace_extension(extension + ".idx.bin");

        addToCleanup(indexFile);
        addToCleanup(indexBinFile);

        auto indexFileExists = getOptionalParameter<bool>("indexFileExists");
        if (indexFileExists.has_value() && indexFileExists.value() == true) {
            // Force generate index file first then reinitialize session
            session_->generateIndexFile(true, true);
            session_.reset();
            SegySessionTestBase::SetUp();
        }
    }
};

class SegySessionAcceptAndRegisterSettings : public SegySessionTestBase
{
public:
    auto run()
    {
        return session_->acceptAndRegisterSettings();
    }
};

TEST_API(SegySessionAcceptAndRegisterSettings, "ApiTests/SegySessionTests.json")

class SegySessionLoadStoredSettings : public SegySessionTestBase
{
public:
    auto run()
    {
        return session_->loadStoredSettings();
    }
};

TEST_API(SegySessionLoadStoredSettings, "ApiTests/SegySessionTests.json")

class SegySessionGetSampleFormat : public SegySessionTestBase
{
public:
    auto run()
    {
        return session_->getSampleFormat();
    }
};

TEST_API(SegySessionGetSampleFormat, "ApiTests/SegySessionTests.json")

class SegySessionGetIndex : public SegySessionTestBase
{
public:
    auto run()
    {
        return session_->getIndex();
    }
};

TEST_API(SegySessionGetIndex, "ApiTests/SegySessionTests.json")

class SegySessionGenerateIndexFile : public SegySessionIndexTestBase
{
public:
    auto run()
    {
        return session_->generateIndexFile();
    }
};

TEST_API(SegySessionGenerateIndexFile, "ApiTests/SegySessionTests.json")

class SegySessionGenerateIndexFileP : public SegySessionIndexTestBase
{
public:
    auto run()
    {
        bool forceGenerateIndex = getParameter<bool>("forceGenerateIndex");
        bool forceOverwrite = getParameter<bool>("forceOverwrite");
        return session_->generateIndexFile(forceGenerateIndex, forceOverwrite);
    }
};

TEST_API(SegySessionGenerateIndexFileP, "ApiTests/SegySessionTests.json")

class SegySessionLoadIndexFile : public SegySessionIndexTestBase
{
public:
    auto run()
    {
        return session_->loadIndexFile();
    }
};

TEST_API(SegySessionLoadIndexFile, "ApiTests/SegySessionTests.json")

class SegySessionGetSegyFileName : public SegySessionTestBase
{
public:
    auto run()
    {
        std::filesystem::path path = session_->getSegyFileName();
        return path.filename().string();
    }
};

TEST_API(SegySessionGetSegyFileName, "ApiTests/SegySessionTests.json")

class SegySessionGetSegyVersion : public SegySessionTestBase
{
public:
    auto run()
    {
        return session_->getSegyVersion();
    }
};

TEST_API(SegySessionGetSegyVersion, "ApiTests/SegySessionTests.json")

class SegySessionIsSegy3D : public SegySessionTestBase
{
public:
    auto run()
    {
        return session_->isSegy3D();
    }
};

TEST_API(SegySessionIsSegy3D, "ApiTests/SegySessionTests.json")

class SegySessionGetSelectedSegyField : public SegySessionTestBase
{
public:
    auto run()
    {
        return session_->getSelectedSegyFileId();
    }
};

TEST_API(SegySessionGetSelectedSegyField, "ApiTests/SegySessionTests.json")

class SegySessionGetAsciiHeaders : public SegySessionTestBase
{
public:
    auto run()
    {
        return session_->getAsciiHeaders();
    }
};

TEST_API(SegySessionGetAsciiHeaders, "ApiTests/SegySessionTests.json")

class SegySessionGetAsciiHeadersAsJson : public SegySessionTestBase
{
public:
    auto run()
    {
        return session_->getAsciiHeadersAsJson();
    }
};

TEST_API(SegySessionGetAsciiHeadersAsJson, "ApiTests/SegySessionTests.json")

class SegySessionGetAsciiHeadersByLineAsJson : public SegySessionTestBase
{
public:
    auto run()
    {
        return session_->getAsciiHeadersByLineAsJson();
    }
};

TEST_API(SegySessionGetAsciiHeadersByLineAsJson, "ApiTests/SegySessionTests.json")

class SegySessionGetExtendedAsciiHeadersAsJson : public SegySessionTestBase
{
public:
    auto run()
    {
        return session_->getExtendedAsciiHeadersAsJson();
    }
};

TEST_API(SegySessionGetExtendedAsciiHeadersAsJson, "ApiTests/SegySessionTests.json")

class SegySessionGetBinaryHeadersAsJson : public SegySessionTestBase
{
public:
    auto run()
    {
        return session_->getBinaryHeadersAsJson();
    }
};

TEST_API(SegySessionGetBinaryHeadersAsJson, "ApiTests/SegySessionTests.json")

class SegySessionSetVectorHeaderMapping : public SegySessionTestBase
{
public:
    auto run()
    {
        auto vectorHeaderMapping = JsonUtilities::GetJsonStringFromFile(getTestdataPathParameter("vectorHeaderMapping").string());
        auto forceRemapping = getParameter<bool>("forceRemapping");
        return session_->setVectorHeaderMapping(vectorHeaderMapping, forceRemapping);
    }
};

TEST_API(SegySessionSetVectorHeaderMapping, "ApiTests/SegySessionTests.json")

class SegySessionGetRawTraceHeadersAsJson : public SegySessionTestBase
{
public:
    auto run()
    {
        auto startTrace = getParameter<long>("startTrace");
        auto tracesToDump = getParameter<int>("tracesToDump");
        return session_->getRawTraceHeadersAsJson(startTrace, tracesToDump);
    }
};

TEST_API(SegySessionGetRawTraceHeadersAsJson, "ApiTests/SegySessionTests.json")

class SegySessionGetTracesAsJson : public SegySessionTestBase
{
public:
    auto run()
    {
        auto startTrace = getParameter<long>("startTrace");
        auto tracesToDump = getParameter<int>("tracesToDump");
        return session_->getTracesAsJson(startTrace, tracesToDump);
    }
};

TEST_API(SegySessionGetTracesAsJson, "ApiTests/SegySessionTests.json")

class SegySessionGetExtendedAsciiHeadersByLineAsJson : public SegySessionTestBase
{
public:
    auto run()
    {
        return session_->getExtendedAsciiHeadersByLineAsJson();
    }
};

TEST_API(SegySessionGetExtendedAsciiHeadersByLineAsJson, "ApiTests/SegySessionTests.json")

class SegySessionGetScaledTraceHeadersAsJson : public SegySessionTestBase
{
public:
    auto run()
    {
        auto startTrace = getParameter<long>("startTrace");
        auto tracesToDump = getParameter<int>("tracesToDump");
        return session_->getScaledTraceHeadersAsJson(startTrace, tracesToDump);
    }
};

TEST_API(SegySessionGetScaledTraceHeadersAsJson, "ApiTests/SegySessionTests.json")

class SegySessionGetAsciiHeadersByLine : public SegySessionTestBase
{
public:
    auto run()
    {
        return JsonSerializer::toJson("asciiHeadersByLine", session_->getAsciiHeadersByLine());
    }
};

TEST_API(SegySessionGetAsciiHeadersByLine, "ApiTests/SegySessionTests.json")

class SegySessionGetExtendedAsciiHeaders : public SegySessionTestBase
{
public:
    auto run()
    {
        return JsonSerializer::toJson("extendedAsciiHeaders", session_->getExtendedAsciiHeaders());
    }
};

TEST_API(SegySessionGetExtendedAsciiHeaders, "ApiTests/SegySessionTests.json")

class SegySessionGetExtendedAsciiHeadersByLine : public SegySessionTestBase
{
public:
    auto run()
    {
        return JsonSerializer::toJson("extendedAsciiHeadersByLine", session_->getExtendedAsciiHeadersByLine());
    }
};

TEST_API(SegySessionGetExtendedAsciiHeadersByLine, "ApiTests/SegySessionTests.json")

class SegySessionGetBinaryHeaders : public SegySessionTestBase
{
public:
    auto run()
    {
        return JsonSerializer::toJson<HeaderDataStructJson>("binaryHeaders", session_->getBinaryHeaders());
    }
};

TEST_API(SegySessionGetBinaryHeaders, "ApiTests/SegySessionTests.json")

class SegySessionGetTraceHeaderFieldCount : public SegySessionTestBase
{
public:
    auto run()
    {
        return session_->getTraceHeaderFieldCount();
    }
};

TEST_API(SegySessionGetTraceHeaderFieldCount, "ApiTests/SegySessionTests.json")

class SegySessionGetRawTraceHeaders : public SegySessionTestBase
{
public:
    auto run()
    {
        auto startTrace = getParameter<size_t>("startTrace");
        auto tracesToDump = getParameter<size_t>("tracesToDump");
        return JsonSerializer::toJson<HeaderDataStructJson>("rawTraceHeaders", session_->getRawTraceHeaders(startTrace, tracesToDump));
    }
};

TEST_API(SegySessionGetRawTraceHeaders, "ApiTests/SegySessionTests.json")

class SegySessionGetTraces : public SegySessionTestBase
{
public:
    auto run()
    {
        auto startTrace = getParameter<size_t>("startTrace");
        auto tracesToDump = getParameter<size_t>("tracesToDump");
        return JsonSerializer::toJson<TraceJson>("traces", session_->getTraces(startTrace, tracesToDump));
    }
};

TEST_API(SegySessionGetTraces, "ApiTests/SegySessionTests.json")

class SegySessionGetScaledTraceHeaders : public SegySessionTestBase
{
public:
    auto run()
    {
        auto startTrace = getParameter<size_t>("startTrace");
        auto tracesToDump = getParameter<size_t>("tracesToDump");
        return JsonSerializer::toJson<HeaderDataStructJson>("scaledTraceHeaders", session_->getScaledTraceHeaders(startTrace, tracesToDump));
    }
};

TEST_API(SegySessionGetScaledTraceHeaders, "ApiTests/SegySessionTests.json")

class SegySessionGetSelectedScaledTraceHeaderValues : public SegySessionTestBase
{
public:
    auto run()
    {
        auto startTrace = getParameter<size_t>("startTrace");
        auto tracesToDump = getParameter<size_t>("tracesToDump");
        auto headerIds = getParameter<std::vector<std::string>>("headerIds");
        return JsonSerializer::toJson("scaledTraceHeaderValues", session_->getSelectedScaledTraceHeaderValues(startTrace, tracesToDump, headerIds));
    }
};

TEST_API(SegySessionGetSelectedScaledTraceHeaderValues, "ApiTests/SegySessionTests.json")

class SegySessionCreateBulkReader : public SegySessionIndexTestBase
{
public:
    auto run()
    {
        return session_->createBulkReader();
    }
};

TEST_API(SegySessionCreateBulkReader, "ApiTests/SegySessionTests.json")

class SegySessionCreateMultiBulkReader : public SegySessionTestBase
{
public:
    auto run()
    {
        auto segyIndex = session_->getIndex();
        auto numberOfTraces = getParameter<size_t>("numberOfTraces");
        auto bulksCount = getParameter<size_t>("bulksCount");
        return session_->createMultiBulkReader(segyIndex, numberOfTraces, bulksCount);
    }
};

TEST_API(SegySessionCreateMultiBulkReader, "ApiTests/SegySessionTests.json")

class SegySessionCreateSectionBulkReader : public SegySessionIndexTestBase
{
public:
    auto run()
    {
        auto readersCount = getParameter<size_t>("readersCount");
        return session_->createSectionBulkReader(readersCount);
    }
};

TEST_API(SegySessionCreateSectionBulkReader, "ApiTests/SegySessionTests.json")
} // namespace segysdk::api::tests

int
main(int argc, char** argv)
{
    testing::InitGoogleTest(&argc, argv);
    segysdk::api::tests::ApiEnvironment::instantiateFromCLI(argc, argv);
    return RUN_ALL_TESTS();
}