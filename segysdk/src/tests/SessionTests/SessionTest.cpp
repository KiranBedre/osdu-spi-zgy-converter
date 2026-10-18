/*
Copyright 2023-2025 Schlumberger

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
#include "../Utilities/Environment.h"
#include "../Utilities/JsonUtilities.h"
#include "../Utilities/TokenSourceFactory.h"
#include "../Utilities/osdu/OsduFileCollectionSegy100.h"
#include "../segysdk/SegySession.h"
#include "../segysdk/SegySessionFactory.h"
#include "../segysdk_public/segy_c_api.h"

#include <gtest/gtest.h>

#include <filesystem>
#include <string>

using namespace segysdk;
using namespace segyio;

std::string
getSmall2DFileCollectionSegy(const std::string& partition)
{
    segysdk::metadata::osdu::OsduMetadataHelper osdu_helper(partition);
    auto testPathStr = utilities::Environment::instance().SegyLocalTestDataPath();

    if (testPathStr.empty()) {
        return testPathStr;
    }

    auto segyFileCollection = segysdk::metadata::osdu::OsduFileCollectionSegyStruct(partition);

    auto commonMetadataStruct = segysdk::metadata::osdu::OsduCommonMetadataStruct(partition);
    commonMetadataStruct.kind = osdu_helper.osduWksReferenceLookupString("dataset--FileCollection.SEGY", "1.3.0");
    auto myAcl = segysdk::metadata::osdu::OsduAclStruct(partition);
    myAcl.owners = {"data.default.owners@partition.enterprisedata.cloud.slb.com"};
    myAcl.viewers = {"data.default.viewers@partition.enterprisedata.cloud.slb.com"};
    commonMetadataStruct.aclStruct = myAcl;

    auto myLegal = segysdk::metadata::osdu::OsduLegalStruct(partition);
    myLegal.otherRelevantDataCountries = {"US"};
    myLegal.legalTags = {"partition-default-legal"};
    myLegal.status = "compliant";
    commonMetadataStruct.legalStruct = myLegal;

    auto myMetaArray = segysdk::metadata::osdu::OsduMetaArrayStruct(partition);
    commonMetadataStruct.metaStruct = myMetaArray;

    auto myTagArray = segysdk::metadata::osdu::OsduTagArrayStruct(partition);
    commonMetadataStruct.tagsStruct = myTagArray;

    segyFileCollection.commonMetadataStruct = commonMetadataStruct;

    auto segyFileCollectionData = segysdk::metadata::osdu::OsduFileCollectionSegyDataStruct(partition);
    auto dsp = segysdk::metadata::osdu::OsduAbstractDatasetStruct(partition);
    dsp.description = "Test 2D data set";
    dsp.endian = "BIG";
    dsp.schemaFormatTypeID = osdu_helper.osduReferenceLookupString("reference-data--SchemaFormatType", "SEGY");
    dsp.totalSize = "354144";
    segyFileCollectionData.datasetStruct = dsp;

    auto commonResources = segysdk::metadata::osdu::OsduAbstractCommonResourcesStruct(partition);
    commonResources.existenceKind = osdu_helper.osduReferenceLookupString("reference-data--ExistenceKind", "Actual");
    commonResources.source = "Testdata set";
    segyFileCollectionData.commonResourcesStruct = commonResources;

    auto fileCollection = segysdk::metadata::osdu::OsduAbstractFileCollectionStruct(partition);
    fileCollection.checksum = "16416625e1c0446eca8a38a0156ff5e2";

    auto fsp = std::filesystem::path(testPathStr) /= "small2D.sgy";

    fileCollection.fileCollectionPath = fsp.parent_path().string();
    // We want a trailing folder separator here...
    fileCollection.fileCollectionPath += std::filesystem::path::preferred_separator;

    auto fsi = segysdk::metadata::osdu::OsduAbstractFileSourceInfoStruct(partition);
    fsi.checksum = "16416625e1c0446eca8a38a0156ff5e2";
    fsi.checksumAlgorithm = "MD5";
    fsi.fileSize = "354144";

    fsi.fileSource = fsp.string();
    fileCollection.fileSourceInfoStructs = {fsi};
    segyFileCollectionData.fileCollectionStruct = fileCollection;

    auto vhm = segysdk::metadata::osdu::OsduVectorHeaderMappingStruct(partition);
    auto vhmCmp = segysdk::metadata::osdu::OsduVectorHeaderMappingElementStruct(partition);
    vhmCmp.keyName = osdu_helper.osduReferenceLookupString("reference-data--HeaderKeyName", "CMP");
    vhmCmp.position = 21;
    vhmCmp.scalarIndicator = "NOSCALE";
    vhmCmp.scalarOverride = 0;
    vhmCmp.uom = osdu_helper.osduUnitReferenceString("Euc");
    vhmCmp.wordFormatType = osdu_helper.osduReferenceLookupString("reference-data--WordFormatType", "INT");
    vhmCmp.wordWidth = 4;
    vhm.vectorHeaderMapping_.emplace_back(vhmCmp);

    auto vhmSP = segysdk::metadata::osdu::OsduVectorHeaderMappingElementStruct(partition);
    vhmSP.keyName = osdu_helper.osduReferenceLookupString("reference-data--HeaderKeyName", "SP");
    vhmSP.position = 17;
    vhmSP.scalarIndicator = "NOSCALE";
    vhmSP.scalarOverride = 0;
    vhmSP.uom = osdu_helper.osduUnitReferenceString("Euc");
    vhmSP.wordFormatType = osdu_helper.osduReferenceLookupString("reference-data--WordFormatType", "INT");
    vhmSP.wordWidth = 4;
    vhm.vectorHeaderMapping_.emplace_back(vhmSP);

    auto vhmCmpx = segysdk::metadata::osdu::OsduVectorHeaderMappingElementStruct(partition);
    vhmCmpx.keyName = osdu_helper.osduReferenceLookupString("reference-data--HeaderKeyName", "CMPX");
    vhmCmpx.position = 73;
    vhmCmpx.scalarIndicator = "STANDARD";
    vhmCmpx.scalarOverride = 0;
    vhmCmpx.uom = osdu_helper.osduUnitReferenceString("m");
    vhmCmpx.wordFormatType = osdu_helper.osduReferenceLookupString("reference-data--WordFormatType", "INT");
    vhmCmpx.wordWidth = 4;
    vhm.vectorHeaderMapping_.emplace_back(vhmCmpx);

    auto vhmCmpy = segysdk::metadata::osdu::OsduVectorHeaderMappingElementStruct(partition);
    vhmCmpy.keyName = osdu_helper.osduReferenceLookupString("reference-data--HeaderKeyName", "CMPY");
    vhmCmpy.position = 77;
    vhmCmpy.scalarIndicator = "STANDARD";
    vhmCmpy.scalarOverride = 0;
    vhmCmpy.uom = osdu_helper.osduUnitReferenceString("m");
    vhmCmpy.wordFormatType = osdu_helper.osduReferenceLookupString("reference-data--WordFormatType", "INT");
    vhmCmpy.wordWidth = 4;
    vhm.vectorHeaderMapping_.emplace_back(vhmCmpy);

    // XYScalar, ZScalar and STARTTIME are OK as defaults
    segyFileCollectionData.vectorHeaderMappingStruct = vhm;
    segyFileCollection.dataStruct = segyFileCollectionData;

    auto fc_json = segysdk::metadata::osdu::OsduFileCollectionSegy(segyFileCollection);
    return fc_json.getAsFormattedString();
}

bool
spAndCmpHdrsFound(const std::vector<std::vector<HeaderDataStruct>>& traceHeaders)
{
    bool cmpFound = false;
    bool spFound = false;

    if (traceHeaders.empty()) {
        return true;
    }
    for (const auto& hdr : traceHeaders.at(0)) {
        if (hdr.id == "CMP") {
            cmpFound = true;
        };
        if (hdr.id == "SP") {
            spFound = true;
        };
    }

    return cmpFound and spFound;
}

bool
spAndCmpHdrsFoundInJson(const std::string& traceHeadersJson)
{
    auto myJson = segysdk::utilities::JsonUtilities::getJsonFromString(traceHeadersJson);
    bool cmpFound = false;
    bool spFound = false;

    if (myJson.empty()) {
        return true;
    }
    for (const auto& hdr : myJson["Metadata"]["ColumnHeaders"]) {
        const auto literal = hdr["Id"].asString();
        if (literal == "CMP") {
            cmpFound = true;
        };
        if (literal == "SP") {
            spFound = true;
        };
    }

    return cmpFound and spFound;
}

TEST(SessionTests, PrimitiveLocalSessionTest)
{
    const std::string filePath = utilities::Environment::instance().SegyLocalTestDataPath();
    const auto inputFilename = std::filesystem::path{filePath} / "tiny_with_ascii_header.sgy";
    ASSERT_TRUE(std::filesystem::exists(inputFilename));

    const std::string loadingParameterJson = "{}";

    std::vector<std::string> segyFileList;
    segyFileList.emplace_back(inputFilename.string());
    const api::SegySessionFactory factory(segyFileList, loadingParameterJson);

    using tokenFactory = utilities::TokenSourceFactory;
    auto tokenSource = tokenFactory::getTokenSource();

    const auto sdCredentials = SDCredentials("", "", std::move(tokenSource));
    factory.CreateSegySession(sdCredentials, CacheSizeOption::none());
}

using SessionPtr = std::unique_ptr<api::SegySession>;

using IndexCleanupSessionPtr = std::unique_ptr<api::SegySession, void (*)(api::SegySession*)>;

static IndexCleanupSessionPtr
wrapSession(SessionPtr session)
{
    return IndexCleanupSessionPtr(session.release(), [](api::SegySession* session) { session->getIndex()->deleteIndex(); delete session; });
}

TEST(SessionTests, OSDULocalSessionTest)
{
    std::string osdu_fc_segy = R"({"data": {"DatasetProperties": {"Checksum": "92D719208302E627516D98FD59BC271", "FileSourceInfos": [{"FileSize": "1050849760", "FileSource": "tiny.sgy", "Name": "CVX_DEPTH"}]}, "Endian": "BIG", "SchemaFormatTypeID": "partition:reference-data--SchemaFormatType:SEGY:", "TotalSize": 5000, "VectorHeaderMapping": [{"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:Inline:", "Position": 189, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:Euc:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:Crossline:", "Position": 193, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:Euc:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:CMPX:", "Position": 181, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:m:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:CMPY:", "Position": 185, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:m:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}]}, "id": "partition:dataset--FileCollection.SEGY:test1", "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0", "version": 1562066009929332})";
    const std::string testPath = utilities::Environment::instance().SegyLocalTestDataPath();

    ASSERT_FALSE(testPath.empty());
    std::vector<std::string> segyFileList;

    const auto json = utilities::JsonUtilities::getJsonFromString(osdu_fc_segy);
    const auto fc100 = utilities::osdu::OsduFileCollectionSegy100(json);
    for (const auto& fs : fc100.segyFileNames()) {
        auto fs_path = std::filesystem::path(testPath);
        fs_path /= fs;
        segyFileList.emplace_back(fs_path.string());
    }

    for (const auto& file : segyFileList) {
        ASSERT_TRUE(std::filesystem::exists(file));
    }

    const api::SegySessionFactory factory(segyFileList, osdu_fc_segy);

    using tokenFactory = utilities::TokenSourceFactory;
    auto tokenSource = tokenFactory::getTokenSource();

    const auto sdCredentials = SDCredentials("", "", std::move(tokenSource));
    auto session = wrapSession(factory.CreateSegySession(sdCredentials, CacheSizeOption::none()));
    ASSERT_FALSE(session == nullptr);
}

TEST(SessionTests, OSDULocalGetTraceSamplesTest1)
{
    std::string osdu_fc_segy = R"({"data": {"DatasetProperties": {"Checksum": "92D719208302E627516D98FD59BC271", "FileSourceInfos": [{"FileSize": "1050849760", "FileSource": "tiny.sgy", "Name": "CVX_DEPTH"}]}, "Endian": "BIG", "SchemaFormatTypeID": "partition:reference-data--SchemaFormatType:SEGY:", "TotalSize": 5000, "VectorHeaderMapping": [{"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:Inline:", "Position": 189, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:Euc:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:Crossline:", "Position": 193, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:Euc:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:CMPX:", "Position": 181, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:m:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:CMPY:", "Position": 185, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:m:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}]}, "id": "partition:dataset--FileCollection.SEGY:test1", "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0", "version": 1562066009929332})";
    const std::string testPath = utilities::Environment::instance().SegyLocalTestDataPath();

    ASSERT_FALSE(testPath.empty());
    std::vector<std::string> segyFileList;

    const auto json = utilities::JsonUtilities::getJsonFromString(osdu_fc_segy);
    const auto fc100 = utilities::osdu::OsduFileCollectionSegy100(json);
    for (const auto& fs : fc100.segyFileNames()) {
        auto fs_path = std::filesystem::path(testPath);
        fs_path /= fs;
        segyFileList.emplace_back(fs_path.string());
    }

    for (const auto& file : segyFileList) {
        ASSERT_TRUE(std::filesystem::exists(file));
    }

    const api::SegySessionFactory factory(segyFileList, osdu_fc_segy);

    using tokenFactory = utilities::TokenSourceFactory;
    auto tokenSource = tokenFactory::getTokenSource();

    const auto sdCredentials = SDCredentials("", "", std::move(tokenSource));
    auto session = wrapSession(factory.CreateSegySession(sdCredentials, CacheSizeOption::none()));
    ASSERT_FALSE(session == nullptr);

    if (!session->getIndex()->hasIndexFile()) {
        session->generateIndexFile();
    }

    size_t start_trace = 0;
    size_t requested_traces_count = 100;
    size_t sample_offset = 0;
    size_t requested_samples_count = 10;
    std::vector<std::vector<float>> traceSamples(requested_traces_count);
    bool success = session->getTraceSamples(traceSamples, start_trace, requested_traces_count, sample_offset, requested_samples_count);
    ASSERT_TRUE(success);
    ASSERT_FALSE(traceSamples.empty());
    ASSERT_EQ(traceSamples.size(), 40);
    ASSERT_EQ(traceSamples.at(0).size(), 10);
}

TEST(SessionTests, OSDULocalGetTraceSamplesTest2)
{
    std::string osdu_fc_segy = R"({"data": {"DatasetProperties": {"Checksum": "92D719208302E627516D98FD59BC271", "FileSourceInfos": [{"FileSize": "1050849760", "FileSource": "tiny.sgy", "Name": "CVX_DEPTH"}]}, "Endian": "BIG", "SchemaFormatTypeID": "partition:reference-data--SchemaFormatType:SEGY:", "TotalSize": 5000, "VectorHeaderMapping": [{"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:Inline:", "Position": 189, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:Euc:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:Crossline:", "Position": 193, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:Euc:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:CMPX:", "Position": 181, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:m:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:CMPY:", "Position": 185, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:m:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}]}, "id": "partition:dataset--FileCollection.SEGY:test1", "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0", "version": 1562066009929332})";
    const std::string testPath = utilities::Environment::instance().SegyLocalTestDataPath();

    ASSERT_FALSE(testPath.empty());
    std::vector<std::string> segyFileList;

    const auto json = utilities::JsonUtilities::getJsonFromString(osdu_fc_segy);
    const auto fc100 = utilities::osdu::OsduFileCollectionSegy100(json);
    for (const auto& fs : fc100.segyFileNames()) {
        auto fs_path = std::filesystem::path(testPath);
        fs_path /= fs;
        segyFileList.emplace_back(fs_path.string());
    }

    for (const auto& file : segyFileList) {
        ASSERT_TRUE(std::filesystem::exists(file));
    }

    const api::SegySessionFactory factory(segyFileList, osdu_fc_segy);

    using tokenFactory = utilities::TokenSourceFactory;
    auto tokenSource = tokenFactory::getTokenSource();

    const auto sdCredentials = SDCredentials("", "", std::move(tokenSource));
    auto session = wrapSession(factory.CreateSegySession(sdCredentials, CacheSizeOption::none()));
    ASSERT_FALSE(session == nullptr);

    if (!session->getIndex()->hasIndexFile()) {
        session->generateIndexFile();
    }

    size_t start_trace = 10;
    size_t requested_traces_count = 100;
    size_t sample_offset = 0;
    size_t requested_samples_count = 10;
    std::vector<std::vector<float>> traceSamples(requested_traces_count);
    bool success = session->getTraceSamples(traceSamples, start_trace, requested_traces_count, sample_offset, requested_samples_count);
    ASSERT_TRUE(success);
    ASSERT_FALSE(traceSamples.empty());
    ASSERT_EQ(traceSamples.size(), 30);
    ASSERT_EQ(traceSamples.at(0).size(), 10);
}

TEST(SessionTests, OSDULocalGetTraceHeadersTest)
{
    std::string osdu_fc_segy = R"({"data": {"DatasetProperties": {"Checksum": "92D719208302E627516D98FD59BC271", "FileSourceInfos": [{"FileSize": "1050849760", "FileSource": "tiny.sgy", "Name": "CVX_DEPTH"}]}, "Endian": "BIG", "SchemaFormatTypeID": "partition:reference-data--SchemaFormatType:SEGY:", "TotalSize": 5000, "VectorHeaderMapping": [{"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:Inline:", "Position": 189, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:Euc:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:Crossline:", "Position": 193, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:Euc:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:CMPX:", "Position": 181, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:m:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:CMPY:", "Position": 185, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:m:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}]}, "id": "partition:dataset--FileCollection.SEGY:test1", "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0", "version": 1562066009929332})";
    const std::string testPath = utilities::Environment::instance().SegyLocalTestDataPath();

    ASSERT_FALSE(testPath.empty());
    std::vector<std::string> segyFileList;

    const auto json = utilities::JsonUtilities::getJsonFromString(osdu_fc_segy);
    const auto fc100 = utilities::osdu::OsduFileCollectionSegy100(json);
    for (const auto& fs : fc100.segyFileNames()) {
        auto fs_path = std::filesystem::path(testPath);
        fs_path /= fs;
        segyFileList.emplace_back(fs_path.string());
    }

    for (const auto& file : segyFileList) {
        ASSERT_TRUE(std::filesystem::exists(file));
    }

    const api::SegySessionFactory factory(segyFileList, osdu_fc_segy);

    using tokenFactory = utilities::TokenSourceFactory;
    auto tokenSource = tokenFactory::getTokenSource();

    const auto sdCredentials = SDCredentials("", "", std::move(tokenSource));
    auto session = wrapSession(factory.CreateSegySession(sdCredentials, CacheSizeOption::none()));
    ASSERT_FALSE(session == nullptr);

    if (!session->getIndex()->hasIndexFile()) {
        session->generateIndexFile();
    }

    std::vector<std::string> exportedHeaders;
    std::vector<std::vector<double>> traceHeaderValues;
    bool success = session->getTraceHeaderValues(exportedHeaders, traceHeaderValues, 0, 100);
    ASSERT_TRUE(success);

    ASSERT_FALSE(exportedHeaders.empty());

    ASSERT_EQ(traceHeaderValues.size(), 40);

    // verify that each trace has the expected number of headers exported
    for (const auto& hdrs : traceHeaderValues) {
        ASSERT_EQ(hdrs.size(), exportedHeaders.size());
    }
}

TEST(SessionTests, OSDULocalGetTracesCInterfaceTest)
{
    std::string osdu_fc_segy = R"({"data": {"DatasetProperties": {"Checksum": "92D719208302E627516D98FD59BC271", "FileSourceInfos": [{"FileSize": "1050849760", "FileSource": "tiny.sgy", "Name": "CVX_DEPTH"}]}, "Endian": "BIG", "SchemaFormatTypeID": "partition:reference-data--SchemaFormatType:SEGY:", "TotalSize": 5000, "VectorHeaderMapping": [{"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:Inline:", "Position": 189, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:Euc:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:Crossline:", "Position": 193, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:Euc:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:CMPX:", "Position": 181, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:m:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:CMPY:", "Position": 185, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:m:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}]}, "id": "partition:dataset--FileCollection.SEGY:test1", "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0", "version": 1562066009929332})";
    const std::string testPath = utilities::Environment::instance().SegyLocalTestDataPath();

    ASSERT_FALSE(testPath.empty());
    std::vector<std::string> segyFileList;

    const auto json = utilities::JsonUtilities::getJsonFromString(osdu_fc_segy);
    const auto fc100 = utilities::osdu::OsduFileCollectionSegy100(json);
    for (const auto& fs : fc100.segyFileNames()) {
        auto fs_path = std::filesystem::path(testPath);
        fs_path /= fs;
        segyFileList.emplace_back(fs_path.string());
    }

    for (const auto& file : segyFileList) {
        ASSERT_TRUE(std::filesystem::exists(file));
    }

    const api::SegySessionFactory factory(segyFileList, osdu_fc_segy);

    using tokenFactory = utilities::TokenSourceFactory;
    auto tokenSource = tokenFactory::getTokenSource();

    const auto sdCredentials = SDCredentials("", "", std::move(tokenSource));
    auto session = wrapSession(factory.CreateSegySession(sdCredentials, CacheSizeOption::none()));
    ASSERT_FALSE(session == nullptr);

    if (!session->getIndex()->hasIndexFile()) {
        session->generateIndexFile();
    }

    std::shared_ptr<segysdk::segyindex::SegyIndex> index = nullptr;
    ASSERT_NO_THROW(index = session->getIndex());

    long startTrace = 0;
    long tracesToDump = 100;

    segy_api_error error;
    const auto json_string = std::string{segy_get_traces_as_json(session.get(), startTrace, tracesToDump, &error)};
    auto resultJson = segysdk::utilities::JsonUtilities::getJsonFromString(json_string);

    ASSERT_FALSE(error.is_error);
    ASSERT_TRUE(resultJson.isMember("Metadata"));
    ASSERT_TRUE(resultJson["Metadata"].isMember("ColumnHeaders"));
    ASSERT_TRUE(resultJson.isMember("TraceData"));
    ASSERT_TRUE(resultJson.isMember("metadata"));

    size_t returnedTraces = resultJson["TraceData"].size();
    ASSERT_EQ(resultJson["TraceData"].size(), 40) << "Actual value: " << std::to_string(returnedTraces);

    const auto returnedHeaders = resultJson["Metadata"]["ColumnHeaders"].size();

    ASSERT_EQ(returnedHeaders, 86);
}

TEST(SessionTests, OSDULocalGetTraceHeadersCInterfaceTest3D)
{
    std::string osdu_fc_segy = R"({"data": {"DatasetProperties": {"Checksum": "92D719208302E627516D98FD59BC271", "FileSourceInfos": [{"FileSize": "1050849760", "FileSource": "tiny.sgy", "Name": "CVX_DEPTH"}]}, "Endian": "BIG", "SchemaFormatTypeID": "partition:reference-data--SchemaFormatType:SEGY:", "TotalSize": 5000, "VectorHeaderMapping": [{"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:Inline:", "Position": 189, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:Euc:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:Crossline:", "Position": 193, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:Euc:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:CMPX:", "Position": 181, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:m:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:CMPY:", "Position": 185, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:m:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}]}, "id": "partition:dataset--FileCollection.SEGY:test1", "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0", "version": 1562066009929332})";
    const std::string testPath = utilities::Environment::instance().SegyLocalTestDataPath();

    ASSERT_FALSE(testPath.empty());
    std::vector<std::string> segyFileList;

    const auto json = utilities::JsonUtilities::getJsonFromString(osdu_fc_segy);
    const auto fc100 = utilities::osdu::OsduFileCollectionSegy100(json);
    for (const auto& fs : fc100.segyFileNames()) {
        auto fs_path = std::filesystem::path(testPath);
        fs_path /= fs;
        segyFileList.emplace_back(fs_path.string());
    }

    for (const auto& file : segyFileList) {
        ASSERT_TRUE(std::filesystem::exists(file));
    }

    const api::SegySessionFactory factory(segyFileList, osdu_fc_segy);

    using tokenFactory = utilities::TokenSourceFactory;
    auto tokenSource = tokenFactory::getTokenSource();

    const auto sdCredentials = SDCredentials("", "", std::move(tokenSource));
    auto session = wrapSession(factory.CreateSegySession(sdCredentials, CacheSizeOption::none()));
    ASSERT_FALSE(session == nullptr);

    std::shared_ptr<segysdk::segyindex::SegyIndex> index = nullptr;
    ASSERT_NO_THROW(index = session->getIndex());

    long startTrace = 0;
    long tracesToDump = 100;
    int headerIdsCount = 10; // Usually expect 7 headers to be returned

    std::vector<char*> myHeaders(headerIdsCount);
    auto buffer = std::vector<double>(headerIdsCount * tracesToDump);

    size_t allocated_bytes = buffer.size() * sizeof(double);
    int returnedHeaders = 0;
    int returnedTraces = 0;
    segy_api_error error;
    segy_get_trace_header_values(session.get(), startTrace, tracesToDump, allocated_bytes, &returnedHeaders, &returnedTraces, myHeaders.data(), buffer.data(), &error);
    ASSERT_FALSE(error.is_error);

    std::vector<std::string> returnedHeadersNames;
    for (int ix = 0; ix < returnedHeaders; ix++) {
        returnedHeadersNames.emplace_back(std::string(myHeaders[ix]));
        delete[] myHeaders[ix];
        myHeaders[ix] = nullptr;
    }

    ASSERT_EQ(returnedHeaders, returnedHeadersNames.size());
    ASSERT_EQ(returnedTraces, 40) << "Actual value: " << std::to_string(returnedTraces) << " Expected value: " << std::to_string(40);

    buffer.resize(returnedHeaders * returnedTraces);

    ASSERT_EQ(buffer.size(), 280);

    bool il_found = false;
    bool xl_found = false;

    for (const auto& headerName : returnedHeadersNames) {
        if (headerName.find("Inline") != std::string::npos) {
            il_found = true;
        }
        if (headerName.find("Crossline") != std::string::npos) {
            xl_found = true;
        }
    }
    ASSERT_TRUE(il_found) << "Missing Inline Header for 3D";
    ASSERT_TRUE(xl_found) << "Missing Crossline Header for 3D";
}

TEST(SessionTests, OSDULocalGetTraceHeadersCInterfaceTest2D)
{
    std::string osdu_fc_segy = R"({"data": {"DatasetProperties": {"Checksum": "92D719208302E627516D98FD59BC271", "FileSourceInfos": [{"FileSize": "1050849760", "FileSource": "tiny.sgy", "Name": "CVX_DEPTH"}]}, "Endian": "BIG", "SchemaFormatTypeID": "partition:reference-data--SchemaFormatType:SEGY:", "TotalSize": 5000, "VectorHeaderMapping": [{"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:Inline:", "Position": 189, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:Euc:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:Crossline:", "Position": 193, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:Euc:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:CMPX:", "Position": 181, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:m:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:CMPY:", "Position": 185, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:m:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}]}, "id": "partition:dataset--FileCollection.SEGY:test1", "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0", "version": 1562066009929332})";
    const std::string testPath = utilities::Environment::instance().SegyLocalTestDataPath();

    ASSERT_FALSE(testPath.empty());
    std::vector<std::string> segyFileList;

    const auto json = utilities::JsonUtilities::getJsonFromString(osdu_fc_segy);
    const auto fc100 = utilities::osdu::OsduFileCollectionSegy100(json);
    for (const auto& fs : fc100.segyFileNames()) {
        auto fs_path = std::filesystem::path(testPath);
        fs_path /= fs;
        segyFileList.emplace_back(fs_path.string());
    }

    for (const auto& file : segyFileList) {
        ASSERT_TRUE(std::filesystem::exists(file));
    }

    const api::SegySessionFactory factory(segyFileList, osdu_fc_segy);

    using tokenFactory = utilities::TokenSourceFactory;
    auto tokenSource = tokenFactory::getTokenSource();

    const auto sdCredentials = SDCredentials("", "", std::move(tokenSource));
    auto session = wrapSession(factory.CreateSegySession(sdCredentials, CacheSizeOption::none()));
    ASSERT_FALSE(session == nullptr);

    session->setSegy2DFlag(true);
    std::shared_ptr<segysdk::segyindex::SegyIndex> index = nullptr;

    ASSERT_NO_THROW(index = session->getIndex());
    bool reloadIndexRequired = false;
    if (index->hasIndexFile()) {
        index->deleteIndex();
        reloadIndexRequired = true;
    }
    if (reloadIndexRequired) {
        index = nullptr;
        session->setSegy2DFlag(true);
        ASSERT_NO_THROW(index = session->getIndex());
    }

    long startTrace = 0;
    long tracesToDump = 100;
    int headerIdsCount = 10; // Usually expect 7 headers to be returned

    std::vector<char*> myHeaders(headerIdsCount);
    auto buffer = std::vector<double>(headerIdsCount * tracesToDump);

    size_t allocated_bytes = buffer.size() * sizeof(double);
    int returnedHeaders = 0;
    int returnedTraces = 0;
    segy_api_error error;
    segy_get_trace_header_values(session.get(), startTrace, tracesToDump, allocated_bytes, &returnedHeaders, &returnedTraces, myHeaders.data(), buffer.data(), &error);
    ASSERT_FALSE(error.is_error);

    std::vector<std::string> returnedHeadersNames;
    for (int ix = 0; ix < returnedHeaders; ix++) {
        returnedHeadersNames.emplace_back(std::string(myHeaders[ix]));
        delete[] myHeaders[ix];
        myHeaders[ix] = nullptr;
    }

    ASSERT_EQ(returnedHeaders, returnedHeadersNames.size());
    ASSERT_EQ(returnedTraces, 40) << "Actual value: " << std::to_string(returnedTraces) << " Expected value: " << std::to_string(40);

    buffer.resize(returnedHeaders * returnedTraces);

    ASSERT_EQ(buffer.size(), 280);
    bool cmp_found = false;
    bool sp_found = false;
    bool cmpx_found = false;
    bool cmpy_found = false;

    for (const auto& headerName : returnedHeadersNames) {
        if (headerName.find("SP") != std::string::npos or headerName.find("SPScaled") != std::string::npos) {
            sp_found = true;
        }
        else if (headerName.find("CMPX") != std::string::npos) {
            cmpx_found = true;
        }
        else if (headerName.find("CMPY") != std::string::npos) {
            cmpy_found = true;
        }
        else if (headerName.find("CMP") != std::string::npos) {
            cmp_found = true;
        }
    }
    ASSERT_TRUE(cmp_found) << "Missing CMP Header for 2D";
    ASSERT_TRUE(sp_found) << "Missing SP Header for 2D";
    ASSERT_TRUE(cmpx_found) << "Missing CMPX Header for 2D";
    ASSERT_TRUE(cmpy_found) << "Missing CMPY Header for 2D";
}

TEST(SessionTests, OSDULocalGetScaledTraceHeadersCInterfaceTest2D)
{
    std::string osdu_fc_segy = R"({"data": {"DatasetProperties": {"Checksum": "92D719208302E627516D98FD59BC271", "FileSourceInfos": [{"FileSize": "1050849760", "FileSource": "tiny.sgy", "Name": "SEIS_DEPTH"}]}, "Endian": "BIG", "SchemaFormatTypeID": "partition:reference-data--SchemaFormatType:SEGY:", "TotalSize": 5000, "VectorHeaderMapping": [{"KeyName": "partition:reference-data--HeaderKeyName:Inline:", "Position": 189, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "partition:reference-data--UnitOfMeasure:Euc:", "WordFormat": "partition:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "partition:reference-data--HeaderKeyName:Crossline:", "Position": 193, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "partition:reference-data--UnitOfMeasure:Euc:", "WordFormat": "partition:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "partition:reference-data--HeaderKeyName:CMPX:", "Position": 181, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "partition:reference-data--UnitOfMeasure:m:", "WordFormat": "partition:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "partition:reference-data--HeaderKeyName:CMPY:", "Position": 185, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "partition:reference-data--UnitOfMeasure:m:", "WordFormat": "partition:reference-data--WordFormatType:INT:", "WordWidth": 4}]}, "id": "partition:dataset--FileCollection.SEGY:test1", "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0", "version": 1562066009929332})";
    const std::string testPath = utilities::Environment::instance().SegyLocalTestDataPath();

    ASSERT_FALSE(testPath.empty());
    std::vector<std::string> segyFileList;

    const auto json = utilities::JsonUtilities::getJsonFromString(osdu_fc_segy);
    const auto fc100 = utilities::osdu::OsduFileCollectionSegy100(json);
    for (const auto& fs : fc100.segyFileNames()) {
        auto fs_path = std::filesystem::path(testPath);
        fs_path /= fs;
        segyFileList.emplace_back(fs_path.string());
    }

    for (const auto& file : segyFileList) {
        ASSERT_TRUE(std::filesystem::exists(file));
    }

    const api::SegySessionFactory factory(segyFileList, osdu_fc_segy);

    using tokenFactory = utilities::TokenSourceFactory;
    auto tokenSource = tokenFactory::getTokenSource();

    const auto sdCredentials = SDCredentials("", "", std::move(tokenSource));
    auto session = wrapSession(factory.CreateSegySession(sdCredentials, CacheSizeOption::none()));
    ASSERT_FALSE(session == nullptr);

    session->setSegy2DFlag(true);
    std::shared_ptr<segysdk::segyindex::SegyIndex> index = nullptr;

    ASSERT_NO_THROW(index = session->getIndex());
    bool reloadIndexRequired = false;
    if (index->hasIndexFile()) {
        index->deleteIndex();
        reloadIndexRequired = true;
    }
    if (reloadIndexRequired) {
        index = nullptr;
        session->setSegy2DFlag(true);
        ASSERT_NO_THROW(index = session->getIndex());
    }

    segy_api_error error;
    const auto trace_header_json = utilities::JsonUtilities::getJsonFromString(segy_get_scaled_trace_headers_as_json(session.get(), 0, 1, &error));
    
    ASSERT_FALSE(error.is_error);
    
    ASSERT_FLOAT_EQ(trace_header_json["TraceData"][0]["Traces"][0].asFloat(), 0.0f);
    ASSERT_FLOAT_EQ(trace_header_json["TraceData"][0]["Traces"][1].asFloat(), 3850.0f);
    ASSERT_FLOAT_EQ(trace_header_json["TraceData"][0]["Traces"][2].asFloat(), -100.0f);
    ASSERT_FLOAT_EQ(trace_header_json["TraceData"][0]["Traces"][3].asFloat(), 0.0f);
    ASSERT_FLOAT_EQ(trace_header_json["TraceData"][0]["Traces"][4].asFloat(), 256.0f);
    ASSERT_FLOAT_EQ(trace_header_json["TraceData"][0]["Traces"][5].asFloat(), 32000.0f);
    ASSERT_FLOAT_EQ(trace_header_json["TraceData"][0]["Traces"][6].asFloat(), 3432733.04f);
    ASSERT_FLOAT_EQ(trace_header_json["TraceData"][0]["Traces"][7].asFloat(), 267490.5f);
}

TEST(SessionTests, OSDULocalGetTraceSamplesCInterfaceTest)
{
    std::string osdu_fc_segy = R"({"data": {"DatasetProperties": {"Checksum": "92D719208302E627516D98FD59BC271", "FileSourceInfos": [{"FileSize": "1050849760", "FileSource": "tiny.sgy", "Name": "CVX_DEPTH"}]}, "Endian": "BIG", "SchemaFormatTypeID": "partition:reference-data--SchemaFormatType:SEGY:", "TotalSize": 5000, "VectorHeaderMapping": [{"KeyName": "partition:reference-data--HeaderKeyName:Inline:", "Position": 189, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "partition:reference-data--UnitOfMeasure:Euc:", "WordFormat": "partition:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "partition:reference-data--HeaderKeyName:Crossline:", "Position": 193, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "partition:reference-data--UnitOfMeasure:Euc:", "WordFormat": "partition:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "partition:reference-data--HeaderKeyName:CMPX:", "Position": 181, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "partition:reference-data--UnitOfMeasure:m:", "WordFormat": "partition:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "partition:reference-data--HeaderKeyName:CMPY:", "Position": 185, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "partition:reference-data--UnitOfMeasure:m:", "WordFormat": "partition:reference-data--WordFormatType:INT:", "WordWidth": 4}]}, "id": "partition:dataset--FileCollection.SEGY:test1", "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0", "version": 1562066009929332})";
    const std::string testPath = utilities::Environment::instance().SegyLocalTestDataPath();

    ASSERT_FALSE(testPath.empty());
    std::vector<std::string> segyFileList;

    const auto json = utilities::JsonUtilities::getJsonFromString(osdu_fc_segy);
    const auto fc100 = utilities::osdu::OsduFileCollectionSegy100(json);
    for (const auto& fs : fc100.segyFileNames()) {
        auto fs_path = std::filesystem::path(testPath);
        fs_path /= fs;
        segyFileList.emplace_back(fs_path.string());
    }

    for (const auto& file : segyFileList) {
        ASSERT_TRUE(std::filesystem::exists(file));
    }

    const api::SegySessionFactory factory(segyFileList, osdu_fc_segy);

    using tokenFactory = utilities::TokenSourceFactory;
    auto tokenSource = tokenFactory::getTokenSource();

    const auto sdCredentials = SDCredentials("", "", std::move(tokenSource));
    auto session = wrapSession(factory.CreateSegySession(sdCredentials, CacheSizeOption::none()));
    ASSERT_FALSE(session == nullptr);

    std::shared_ptr<segysdk::segyindex::SegyIndex> index = nullptr;
    ASSERT_NO_THROW(index = session->getIndex());

    long startTrace = 0;
    long tracesToDump = 20;
    long sampleOffset = 10;
    long samplesToDump = 100;
    auto buffer = std::vector<float>(tracesToDump * samplesToDump);

    size_t allocated_bytes = buffer.size() * sizeof(float);
    int returnedTraces = 0;
    int returnedSamples = 0;

    segy_api_error error;
    segy_get_trace_samples(session.get(), startTrace, tracesToDump, sampleOffset, samplesToDump, allocated_bytes, &returnedTraces, &returnedSamples, buffer.data(), &error);
    ASSERT_FALSE(error.is_error);

    ASSERT_EQ(returnedTraces, static_cast<int>(tracesToDump)) << "Actual value: " << std::to_string(returnedTraces) << " Expected: " << std::to_string(static_cast<int>(tracesToDump));

    buffer.resize(returnedTraces * returnedSamples);

    ASSERT_EQ(buffer.size(), 2000);
}

TEST(SessionTests, GetSegy3DFileExtentsTest)
{
    std::string osdu_fc_segy = R"({"data": {"DatasetProperties": {"Checksum": "92D719208302E627516D98FD59BC271", "FileSourceInfos": [{"FileSize": "1050849760", "FileSource": "tiny.sgy", "Name": "CVX_DEPTH"}]}, "Endian": "BIG", "SchemaFormatTypeID": "partition:reference-data--SchemaFormatType:SEGY:", "TotalSize": 5000, "VectorHeaderMapping": [{"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:Inline:", "Position": 189, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:Euc:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:Crossline:", "Position": 193, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:Euc:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:CMPX:", "Position": 181, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:m:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "stage-chevron-corporation:reference-data--HeaderKeyName:CMPY:", "Position": 185, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "stage-chevron-corporation:reference-data--UnitOfMeasure:m:", "WordFormat": "stage-chevron-corporation:reference-data--WordFormatType:INT:", "WordWidth": 4}]}, "id": "partition:dataset--FileCollection.SEGY:test1", "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0", "version": 1562066009929332})";
    const std::string testPath = utilities::Environment::instance().SegyLocalTestDataPath();

    ASSERT_FALSE(testPath.empty());
    std::vector<std::string> segyFileList;

    const auto json = utilities::JsonUtilities::getJsonFromString(osdu_fc_segy);
    const auto fc100 = utilities::osdu::OsduFileCollectionSegy100(json);
    for (const auto& fs : fc100.segyFileNames()) {
        auto fs_path = std::filesystem::path(testPath);
        fs_path /= fs;
        segyFileList.emplace_back(fs_path.string());
    }

    for (const auto& file : segyFileList) {
        ASSERT_TRUE(std::filesystem::exists(file));
    }

    const api::SegySessionFactory factory(segyFileList, osdu_fc_segy);

    using tokenFactory = utilities::TokenSourceFactory;
    auto tokenSource = tokenFactory::getTokenSource();

    const auto sdCredentials = SDCredentials("", "", std::move(tokenSource));
    auto session = wrapSession(factory.CreateSegySession(sdCredentials, CacheSizeOption::none()));
    ASSERT_FALSE(session == nullptr);

    long firstInline = 0;
    int inlineStep = 0;
    long inline_count = 0;
    long firstCrossline = 0;
    int crosslineStep = 0;
    long crossline_count = 0;
    double firstSampleDepth = 0;
    double sampleInterval = 0;
    int sample_count = 0;

    if (!session->getIndex()->hasIndexFile()) {
        session->generateIndexFile();
    }

    segy_api_error error;
    segy_get_3D_file_extents(session.get(), &firstInline, &inlineStep, &inline_count, &firstCrossline, &crosslineStep, &crossline_count, &firstSampleDepth, &sampleInterval, &sample_count, &error);
    ASSERT_FALSE(error.is_error);

    std::shared_ptr<segysdk::segyindex::SegyIndex> index = nullptr;
    ASSERT_NO_THROW(index = session->getIndex());

    ASSERT_EQ(firstInline, index->getFileLayoutExtents().getIVectorExtent().first());
    ASSERT_EQ(inlineStep, index->getFileLayoutExtents().getIVectorExtent().step());
    ASSERT_EQ(inline_count, index->getFileLayoutExtents().getIVectorExtent().count());
    ASSERT_EQ(firstCrossline, index->getFileLayoutExtents().getJVectorExtent().first());
    ASSERT_EQ(crosslineStep, index->getFileLayoutExtents().getJVectorExtent().step());
    ASSERT_EQ(crossline_count, index->getFileLayoutExtents().getJVectorExtent().count());
    ASSERT_EQ(firstSampleDepth, index->getFileLayoutExtents().getSampleExtent().first());
    ASSERT_EQ(sampleInterval, index->getFileLayoutExtents().getSampleExtent().step());
    ASSERT_EQ(sample_count, index->getFileLayoutExtents().getSampleExtent().count());
}

TEST(SessionTests, CreateSessionTest)
{
    const std::string small2d_json = "{}";
    const std::string testPathStr = utilities::Environment::instance().SegyLocalTestDataPath();

    ASSERT_FALSE(testPathStr.empty());
    auto small2d_path = std::filesystem::path(testPathStr) /= "small2D.sgy";
    ASSERT_TRUE(std::filesystem::exists(small2d_path));
    auto small2d_string = small2d_path.string();
    auto token = std::string{};
    segy_api_error error;
    auto session = wrapSession(SessionPtr{segy_create_session(small2d_string.c_str(), small2d_json.c_str(), token.c_str(), &error)});
    ASSERT_FALSE(session == nullptr);

    ASSERT_FALSE(error.is_error);
    session->setSegy2DFlag(true);

    const auto traceHeaders = session->getRawTraceHeaders(0, 1);
    ASSERT_TRUE(spAndCmpHdrsFound(traceHeaders));

    const auto traceHeadersJson = session->getRawTraceHeadersAsJson(0, 1);
    ASSERT_TRUE(spAndCmpHdrsFoundInJson(traceHeadersJson));

    ASSERT_FALSE(session->isSegy3D());
    if (!session->getIndex()->hasIndexFile()) {
        session->generateIndexFile();
    }
    ASSERT_TRUE(session->getIndex()->hasIndexFile());
    ASSERT_TRUE(session->validateSession());
}

TEST(SessionTests, CreateOsduSessionTest)
{
    const std::string osdu_partition = "partition";
    const std::string small2d_json = getSmall2DFileCollectionSegy(osdu_partition);
    ASSERT_FALSE(small2d_json.empty());

    auto token = std::string{};
    segy_api_error error;
    auto session = wrapSession(SessionPtr{segy_create_osdu_session(small2d_json.c_str(), token.c_str(), &error)});
    ASSERT_FALSE(session == nullptr);

    ASSERT_FALSE(error.is_error);
    session->setSegy2DFlag(true);
    ASSERT_FALSE(session->isSegy3D());

    const auto traceHeaders = session->getRawTraceHeaders(0, 1);
    ASSERT_TRUE(spAndCmpHdrsFound(traceHeaders));

    const auto traceHeadersJson = session->getRawTraceHeadersAsJson(0, 1);
    ASSERT_TRUE(spAndCmpHdrsFoundInJson(traceHeadersJson));

    if (!session->getIndex()->hasIndexFile()) {
        session->generateIndexFile();
    }
    ASSERT_TRUE(session->getIndex()->hasIndexFile());
    ASSERT_TRUE(session->validateSession());
}

TEST(SessionTests, GetSegy2DLineExtentsTest)
{
    std::string osdu_fc_segy = R"({"data": {"DatasetProperties": {"Checksum": "16416625e1c0446eca8a38a0156ff5e2", "FileSourceInfos": [{"FileSize": "356352 ", "FileSource": "small2D.sgy", "Name": "SMALL_2D"}]}, "Endian": "BIG", "SchemaFormatTypeID": "partition:reference-data--SchemaFormatType:SEGY:", "TotalSize": 356352, "VectorHeaderMapping": [{"KeyName": "partition:reference-data--HeaderKeyName:SP:", "Position": 17, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "partition:reference-data--UnitOfMeasure:Euc:", "WordFormat": "partition:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "partition:reference-data--HeaderKeyName:CMP:", "Position": 21, "ScalarIndicator": "NOSCALE", "ScalarOverride": 0, "UoM": "partition:reference-data--UnitOfMeasure:Euc:", "WordFormat": "partition:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "partition:reference-data--HeaderKeyName:CMPX:", "Position": 73, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "partition:reference-data--UnitOfMeasure:m:", "WordFormat": "partition:reference-data--WordFormatType:INT:", "WordWidth": 4}, {"KeyName": "partition:reference-data--HeaderKeyName:CMPY:", "Position": 77, "ScalarIndicator": "STANDARD", "ScalarOverride": 0, "UoM": "partition:reference-data--UnitOfMeasure:m:", "WordFormat": "partition:reference-data--WordFormatType:INT:", "WordWidth": 4}]}, "id": "partition:dataset--FileCollection.SEGY:test2", "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0", "version": 1738150783929332})";
    const std::string testPath = utilities::Environment::instance().SegyLocalTestDataPath();

    ASSERT_FALSE(testPath.empty());
    std::vector<std::string> segyFileList;

    const auto json = utilities::JsonUtilities::getJsonFromString(osdu_fc_segy);
    const auto fc100 = utilities::osdu::OsduFileCollectionSegy100(json);
    for (const auto& fs : fc100.segyFileNames()) {
        auto fs_path = std::filesystem::path(testPath);
        fs_path /= fs;
        segyFileList.emplace_back(fs_path.string());
    }

    for (const auto& file : segyFileList) {
        ASSERT_TRUE(std::filesystem::exists(file));
    }

    const api::SegySessionFactory factory(segyFileList, osdu_fc_segy);

    using tokenFactory = utilities::TokenSourceFactory;
    auto tokenSource = tokenFactory::getTokenSource();

    const auto sdCredentials = SDCredentials("", "", std::move(tokenSource));
    auto session = wrapSession(factory.CreateSegySession(sdCredentials, CacheSizeOption::none()));
    ASSERT_FALSE(session == nullptr);

    long firstCMP = 0;
    int cmpStep = 0;
    long cmp_count = 0;
    double firstSampleDepth = 0;
    double sampleInterval = 0;
    int sample_count = 0;

    if (!session->getIndex()->hasIndexFile()) {
        session->setSegy2DFlag(true);
        session->generateIndexFile();
    }

    segy_api_error error;
    segy_get_2D_line_extents(session.get(), &firstCMP, &cmpStep, &cmp_count, &firstSampleDepth, &sampleInterval, &sample_count, &error);
    ASSERT_FALSE(error.is_error);

    std::shared_ptr<segysdk::segyindex::SegyIndex> index = nullptr;
    ASSERT_NO_THROW(index = session->getIndex());

    ASSERT_EQ(firstCMP, index->getLineExtents().getCMPExtent().first());
    ASSERT_EQ(cmpStep, index->getLineExtents().getCMPExtent().step());
    ASSERT_EQ(cmp_count, index->getLineExtents().getCMPExtent().count());
    ASSERT_EQ(firstSampleDepth, index->getLineExtents().getSampleExtent().first());
    ASSERT_EQ(sampleInterval, index->getLineExtents().getSampleExtent().step());
    ASSERT_EQ(sample_count, index->getLineExtents().getSampleExtent().count());
}

TEST(SessionTests, ReadTextFileTest)
{
    const std::string filePath = utilities::Environment::instance().SegyLocalTestDataPath();
    const auto inputSegyFilename = std::filesystem::path{filePath} / "tiny_with_ascii_header.sgy";
    ASSERT_TRUE(std::filesystem::exists(inputSegyFilename));

    const std::string loadingParameterJson = "{}";

    std::vector<std::string> segyFileList;
    segyFileList.emplace_back(inputSegyFilename.string());
    const api::SegySessionFactory factory(segyFileList, loadingParameterJson);

    using tokenFactory = utilities::TokenSourceFactory;
    auto tokenSource = tokenFactory::getTokenSource();

    const auto sdCredentials = SDCredentials("", "", std::move(tokenSource));
    const auto session = factory.CreateSegySession(sdCredentials, CacheSizeOption::none());

    const auto inputTextFilename = std::filesystem::path{filePath} / "FileCollection.SEGY-Tiny.json";
    ASSERT_TRUE(std::filesystem::exists(inputTextFilename));
    const auto result = session->readTextFile(inputTextFilename.string());

    ASSERT_FALSE(result.empty());
    ASSERT_TRUE(result.find("VectorHeaderMapping") != std::string::npos);
    ASSERT_TRUE(segysdk::utilities::JsonUtilities::isValidJson(result));
}

