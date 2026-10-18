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

#include "TestSegyFile.h"

#include "../FileIO/GenericFileManager.h"
#include "../Logger/Logger.h"
#include "../Utilities/Environment.h"
#include "../Utilities/JsonUtilities.h"
#include "../Utilities/TokenSourceFactory.h"

#include "../tests/UtilityLibrary/TestBackdoor.h"

#include <cassert>
#include <cmath>
#include <iostream>
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace segysdk::tests
{

TestSegyFile::TestSegyFile(const std::vector<std::string>& segyFileNamesList, const std::string& jsonFileName, std::string outputJsonFolder)
  : segyFileNamesVector_(segyFileNamesList),
    jsonFileName_(jsonFileName),
    outputJsonFolder_(std::move(outputJsonFolder))
{
    if (segyFileNamesList.empty()) {
        std::cout << std::endl
                  << " *** Input file list is empty. Exit program! *** " << std::endl
                  << std::endl;
        return;
    }

    const bool isCloudSession = fileio::GenericFileManager::isSDDataset(segyFileNamesVector_[0]) || fileio::GenericFileManager::isSDDataset(jsonFileName);

    initSdArgs(isCloudSession);

    const std::string jsonParams = (!jsonFileName_.empty()) ? GetJsonFromInputFile(jsonFileName_) : emptyJson;

    const api::SegySessionFactory sessionFactory(segyFileNamesVector_, jsonParams);
    initSession(isCloudSession, sessionFactory, jsonParams);
}

TestSegyFile::TestSegyFile(const std::string& osduFileCollectionJson, std::string outputJsonFolder)
  : outputJsonFolder_(std::move(outputJsonFolder))
{
    if (osduFileCollectionJson.empty()) {
        std::cout << std::endl
                  << " *** Input OSDU FileCollection parameter is empty. Exit program! *** " << std::endl
                  << std::endl;
        return;
    }

    const bool isCloudSession = testIfCloudSession(osduFileCollectionJson);

    initSdArgs(isCloudSession);

    const std::string jsonParams = GetJsonFromInputFile(osduFileCollectionJson);
    const api::SegySessionFactory sessionFactory(jsonParams);
    initSession(isCloudSession, sessionFactory, jsonParams);
}

void
TestSegyFile::initSdArgs(const bool isCloudSession)
{
    // SD_SVC_URL=https://seistore.p4d.subsurface.cloud.slb-ds.com/seistore-svc/api/v3
    // SD_SVC_API_KEY=<valid app key to use SD service>
    // SD_SVC_TOKEN=<valid service or user token to use SD service>

    sd_url_ = isCloudSession ? utilities::Environment::instance().sdUrl() : "";
    sd_api_key_ = isCloudSession ? utilities::Environment::instance().sdApiKey() : "";
}

void
TestSegyFile::initSession(const bool& isCloudSession, const api::SegySessionFactory& sessionFactory, const std::string& jsonParams)
{
    const SDCredentials sdCredentials(sd_url_, sd_api_key_, utilities::TokenSourceFactory::getTokenSource());

    const CacheSizeOption cacheSize = isCloudSession ? getSDCacheSize() : CacheSizeOption::none();

    session_ = TestBackdoor::getImpl(
        isCloudSession ? sessionFactory.CreateSegySession(sdCredentials, cacheSize) : sessionFactory.CreateSegySession());
    sessionOK_ = true;

    if (jsonParams.empty() || jsonParams == emptyJson) {
        session_->loadStoredSettings();
        std::cout << std::endl
                  << " *** Using stored header definitions *** " << std::endl
                  << std::endl;
    }
}

std::shared_ptr<segyindex::SegyIndexImpl>
TestSegyFile::getIndex() const
{
    return TestBackdoor::getImpl(session_->getIndex());
}

CacheSizeOption
TestSegyFile::getSDCacheSize()
{
    sd_cache_page_size_ = utilities::Environment::instance().sdReadCachePageSize();
    sd_max_pages_ = utilities::Environment::instance().sdReadCacheMaxPages();
    sd_blocks_per_page_ = utilities::Environment::instance().sdReadCacheBlocksPerPage();
    sd_chunk_size_ = utilities::Environment::instance().sdReadCachePageSize();

    size_t pageSize = 2U * 1024U * 1024U;
    size_t maxPages = 32U;
    size_t blocksPerPage = 1;
    size_t chunkSize = 1024U * 1024U;

    if (sd_cache_page_size_) {
        pageSize = sd_cache_page_size_;
    }

    if (sd_max_pages_) {
        maxPages = sd_max_pages_;
    }

    if (sd_blocks_per_page_) {
        blocksPerPage = sd_blocks_per_page_;
    }

    if (sd_chunk_size_) {
        chunkSize = sd_chunk_size_;
    }

    return CacheSizeOption{pageSize, maxPages, blocksPerPage, chunkSize};
}

bool
TestSegyFile::testIfCloudSession(const std::string& jsonFile)
{

    bool is_CloudSession = fileio::GenericFileManager::isSDDataset(jsonFile);

    if (!is_CloudSession) {
        Json::Value root = utilities::JsonUtilities::getJsonFromString(GetJsonFromInputFile(jsonFile));
        if (root.empty()) {
            std::cout << std::endl
                      << " *** Unable to parse OSDU FileCollection parameters. Exit program! *** " << std::endl
                      << std::endl;
            sessionOK_ = false;
            return is_CloudSession;
        }

        // Extract input file list from OSDU FileCollection parameters to see if they are located in the cloud or not
        if (root.isMember("data")) {
            Json::Value dataNode = root["data"];
            if (dataNode.isMember("DatasetProperties")) {
                Json::Value dsNode = dataNode["DatasetProperties"];
                if (dsNode.isMember("FileSourceInfos")) {
                    Json::Value fsNode = dsNode["FileSourceInfos"];
                    for (Json::Value::ArrayIndex i = 0; i != fsNode.size(); i++) {
                        if (fsNode[i].isMember("FileSource")) {
                            std::string fileName = fsNode[i]["FileSource"].asString();
                            segyFileNamesVector_.push_back(fileName);
                        }
                    }
                    if (!segyFileNamesVector_.empty()) {
                        is_CloudSession = fileio::GenericFileManager::isSDDataset(segyFileNamesVector_[0]);
                    }
                }
            }
        }
    }
    return is_CloudSession;
}

void
TestSegyFile::ShowSettings() const
{
    std::cout << "Using the following settings:" << std::endl;
    std::cout << "Input SEG-Y file name: " << session_->getSegyFileName() << std::endl
              << std::endl;
}

void
TestSegyFile::SelectInputFile() const
{
    if (!segyFileNamesVector_.empty() && segyFileNamesVector_.size() > 1) {
        std::cout << "Select input file number: ";
        std::string fileNoString;
        std::cin >> fileNoString;
        std::string::size_type sz; // alias of size_t
        const size_t fNo = std::stoul(fileNoString, &sz);
        if (fNo < segyFileNamesVector_.size()) {
            session_->setSelectedSegyFile(fNo);
        }
        else {
            std::cout << "Please a select a file number between 0 and " << std::to_string(segyFileNamesVector_.size() - 1) << " ..." << std::endl;
        }
    }
}

void
TestSegyFile::performTests()
{

    while (sessionOK_) {

        DisplayMenu();
        char option;
        std::cin >> option;

        switch (option) {
        case '1':
            {
                ShowTextualHeaders();
            }
            break;
        case '2':
            {
                ShowBinaryHeaders();
            }
            break;
        case '3':
            {
                ShowRawTraceHeaders();
            }
            break;
        case '4':
            {
                ShowRawTraceHeadersAsJson();
            }
            break;
        case '5':
            {
                ShowInterpretedTraceHeaders();
            }
            break;
        case '6':
            {
                ShowInterpretedTraceHeadersAsJson();
            }
            break;
        case '7':
            {
                ShowTraceSamples();
            }
            break;
        case '8':
            {
                ShowSequentialTraceSamples();
            }
            break;
        case 's':
        case 'S':
            {
                AcceptAndRegisterSettings();
            }
            break;
        case 'l':
        case 'L':
            {
                LoadSettingsFromSavedParameterFile();
            }
            break;
        case 'i':
        case 'I':
            {
                GenerateIndexFile();
            }
            break;
        case 'f':
        case 'F':
            {
                SelectInputFile();
            }
            break;
        case 'g':
        case 'G':
            {
                GenerateIndexGeometryInformation();
            }
            break;
        case 'j':
        case 'J':
            {
                SaveIndexAsJson();
            }
            break;
        case 'r':
        case 'R':
            {
                LoadIndexFile();
            }
            break;
        case '0':
        case 'q':
        case 'Q':
            return;
        default:
            std::cout << "Selected item has not been implemented ... " << std::endl;
        }
    }
}

int
TestSegyFile::getIntFromString(const std::string& text) const
{
    try {
        std::string::size_type sz; // alias of size_t
        const long value = std::stoi(text, &sz);

        return value;
    }
    catch (...) {
        std::cout << "Illegal integer value entered!" << std::endl;
        return 0;
    }
}

long
TestSegyFile::getLongFromString(const std::string& text) const
{
    try {
        std::string::size_type sz; // alias of size_t
        const long value = std::stol(text, &sz);

        return value;
    }
    catch (...) {
        std::cout << "Illegal integer value entered!" << std::endl;
        return -1;
    }
}

unsigned long
TestSegyFile::getULongFromString(const std::string& text) const
{
    try {
        std::string::size_type sz; // alias of size_t
        const unsigned long value = std::stoul(text, &sz);

        return value;
    }
    catch (...) {
        std::cout << "Illegal integer value entered!" << std::endl;
        return 0;
    }
}

void
TestSegyFile::DisplayMenu() const
{

    std::cout << "=====================================================================" << std::endl;
    std::cout << "=====   SEG-Y file: " << session_->getSegyFileName() << std::endl;
    std::cout << "=====================================================================" << std::endl
              << std::endl;
    std::cout << std::endl;
    if (!segyFileNamesVector_.empty() && segyFileNamesVector_.size() > 1) {
        std::cout << "F. Select input file to use" << std::endl
                  << std::endl;
    }
    std::cout << "1. Display textual headers" << std::endl;
    std::cout << "2. Display binary headers" << std::endl;
    std::cout << "3. Display raw trace header values" << std::endl;
    std::cout << "4. Dump raw trace header values to Json file" << std::endl;
    std::cout << "5. Display interpreted (with scaling and overrides applied) trace header values" << std::endl;
    std::cout << "6. Dump interpreted trace header values to Json file" << std::endl;
    std::cout << "7  Dump trace samples (index required)" << std::endl;
    std::cout << "8  Dump Sequential trace samples" << std::endl;
    std::cout << "S  Save Session parameter with input SEG-Y file" << std::endl;
    std::cout << "L. Load Session parameters from input SEG-Y file parameter file" << std::endl;
    std::cout << std::endl;
    std::cout << "I. Generate index file" << std::endl;
    if (getIndex()->isValid()) {
        std::cout << "g. Display computed Geometry from Index" << std::endl;
        std::cout << "j. Save index to Json file" << std::endl;
        std::cout << std::endl;
    }
    std::cout << "R. Read Index from Saved Index file" << std::endl;
    std::cout << std::endl;
    std::cout << "Q. Quit tests" << std::endl;
    std::cout << "---------------------------------------------------------------------" << std::endl
              << std::endl;
    std::cout << "Please enter function choice: ";
}

void
TestSegyFile::writeJsonTestOutput(const std::string& outputFile, const std::string& result, const std::string& errormsg) const
{
    try {
        const bool success = fileio::GenericFileManager::writeTextFile(fr(outputFile), result, true);
        if (!success) {
            std::cout << std::endl
                      << "Nothing written to Json file " << outputFile << std::endl;
            return;
        }
    }
    catch (const std::exception& ex) {
        std::cerr << errormsg << outputFile << std::endl
                  << ex.what() << std::endl;
        return;
    }

    std::cout << std::endl
              << "Json data dump can be found here: " << outputFile << std::endl;
}

void
TestSegyFile::ShowTextualHeaders()
{
    const std::string txtHeaders = session_->getAsciiHeaders();
    DisplayTextualHeaders(txtHeaders, "Textual header");

    const std::vector<std::string> extendedHeaders = session_->getExtendedAsciiHeaders();
    const auto numExtHdrs = extendedHeaders.size();

    auto result = session_->getAsciiHeadersAsJson();
    std::string outputFileName = outputJsonFolder_ + "AsciiHeaderOutput.json";

    writeJsonTestOutput(outputFileName, result, "TestSegyFile::ShowTextualHeaders: Error writing text header data to Json file: ");

    if (numExtHdrs == 0) {
        std::cout << "---------------------------------------------------------------------" << std::endl;
        std::cout << "   *** SEG-Y file has no extended textual headers.  *** " << std::endl;
    }
    else {
        std::cout << "---------------------------------------------------------------------" << std::endl;
        for (size_t ix = 0; ix < numExtHdrs; ix++) {
            const std::string headerText = "Extended header[" + std::to_string(ix) + "] for SEG-Y file";
            DisplayTextualHeaders(extendedHeaders[ix], headerText);
            std::cout << "---------------------------------------------------------------------" << std::endl;
        }
    }

    if (numExtHdrs > 0) {
        result = session_->getExtendedAsciiHeadersAsJson();
        outputFileName = outputJsonFolder_ + "ExtendedAsciiHeaderOutput.json";
        writeJsonTestOutput(outputFileName, result, "TestSegyFile::ShowTextualHeaders: Error writing Extended textual header data to Json file: ");

        std::cout << std::endl
                  << std::endl;
    }
}

void
TestSegyFile::ShowBinaryHeaders() const
{
    std::cout << "=====================================================================" << std::endl;
    std::cout << "=====                      Binary Header                        =====" << std::endl;
    std::cout << "=====================================================================" << std::endl;
    const std::string dots = " ..........................................................";
    constexpr int width = 35;

    const std::vector<api::HeaderDataStruct> binaryHeaders = session_->getBinaryHeaders();

    for (const auto& hdr : binaryHeaders) {

        std::string displayHdrId = hdr.id;
        const auto offset = hdr.offset;
        const auto end = hdr.end;
        std::cout << "[" << offset + 1 << " - " << end << "]   ";
        const auto strLen = displayHdrId.size();
        std::string myDots = dots.substr(0, width - strLen);

        std::cout << " " + displayHdrId << myDots << " : ";

        const auto value = static_cast<int>(hdr.value);
        std::cout << value << std::endl;
    }

    const auto result = session_->getBinaryHeadersAsJson();
    const std::string outputFileName = outputJsonFolder_ + "BinaryHeaderOutput.json";
    writeJsonTestOutput(outputFileName, result, "TestSegyFile::ShowBinaryHeaders: Error writing binary header data to Json file: ");
}

void
TestSegyFile::DisplayTextualHeaders(const std::string& headerText, const std::string& heading)
{
    constexpr int TextHeaderSize = 3200;

    if (headerText.size() == TextHeaderSize) {
        std::cout << "=====================================================================" << std::endl;
        std::cout << "=====                       " << heading << std::endl;
        std::cout << "=====================================================================" << std::endl;

        bool isDone = false;
        size_t offset = 0;
        while (!isDone) {
            std::string line = headerText.substr(offset, 80U);
            std::cout << line << std::endl;
            offset += 80U;
            isDone = (offset >= TextHeaderSize);
        }
    }
}

void
TestSegyFile::ShowRawTraceHeaders() const
{

    std::cout << "=====================================================================" << std::endl;
    std::cout << "=====                  Raw Trace Headers                        =====" << std::endl;
    std::cout << "=====================================================================" << std::endl
              << std::endl;

    long startTrace = 0;
    int traceCount = 1000;
    std::cout << "Start scan from trace#   [0]               : ";
    std::string answer;
    std::cin >> answer;
    if (!answer.empty()) {
        startTrace = getLongFromString(answer);
        if (startTrace == -1) {
            return;
        }
    }
    std::cout << std::endl
              << "Number of traces to scan? [1000]: ";
    std::cin >> answer;
    if (!answer.empty()) {
        traceCount = getIntFromString(answer);
        if (traceCount == -1) {
            return;
        }
    }

    try {
        const auto result = session_->getRawTraceHeaders(static_cast<size_t>(startTrace), static_cast<size_t>(traceCount));

        long traceNo = startTrace;
        typedef std::numeric_limits<double> dbl;
        for (const auto& traceData : result) { // all requested traces
            std::cout << "TraceNo: " << traceNo++ << std::endl;
            for (const auto& td : traceData) { // single trace fields
                std::cout.precision(dbl::max_digits10);
                std::cout << "   Id:" << td.id << "  Offset: " << td.offset << " End:" << td.end << "   Value: " << td.value << std::endl;
            }
        }
    }
    catch (const std::logic_error& le) {
        std::cerr << "Logical error: " << le.what() << std::endl;
    }
}

void
TestSegyFile::ShowRawTraceHeadersAsJson() const
{

    std::cout << "=====================================================================" << std::endl;
    std::cout << "=====                  Raw Trace Headers As Json                =====" << std::endl;
    std::cout << "=====================================================================" << std::endl
              << std::endl;

    long startTrace = 0;
    int traceCount = 1000;
    std::cout << "Start scan from trace#   [0]               : ";
    std::string answer;
    std::cin >> answer;
    if (!answer.empty()) {
        startTrace = getLongFromString(answer);
        if (startTrace == -1) {
            return;
        }
    }
    std::cout << std::endl
              << "Number of traces to scan? [1000]: ";
    std::cin >> answer;
    if (!answer.empty()) {
        traceCount = getIntFromString(answer);
        if (traceCount == -1) {
            return;
        }
    }

    try {
        const auto result = session_->getRawTraceHeadersAsJson(startTrace, traceCount);
        const std::string outputFileName = outputJsonFolder_ + "RawTraceHeadersOutput.json";
        writeJsonTestOutput(outputFileName, result, "TestSegyFile::ShowRawTraceHeadersAsJson: Error writing raw trace header data to Json file: ");
    }
    catch (const std::logic_error& le) {
        std::cerr << "Logical error: " << le.what() << std::endl;
    }
}

void
TestSegyFile::ShowInterpretedTraceHeaders() const
{

    std::cout << "=====================================================================" << std::endl;
    std::cout << "=====           Interpreted Trace Headers                       =====" << std::endl;
    std::cout << "=====================================================================" << std::endl
              << std::endl;

    long startTrace = 0;
    int traceCount = 1000;
    std::cout << "Start scan from trace#   [0]               : ";
    std::string answer;
    std::cin >> answer;
    if (!answer.empty()) {
        startTrace = getLongFromString(answer);
        if (startTrace == -1) {
            return;
        }
    }
    std::cout << std::endl
              << "Number of traces to scan? [1000]: ";
    std::cin >> answer;
    if (!answer.empty()) {
        traceCount = getIntFromString(answer);
        if (traceCount == -1) {
            return;
        }
    }

    try {
        const auto result = session_->getScaledTraceHeaders(static_cast<size_t>(startTrace), static_cast<size_t>(traceCount));

        typedef std::numeric_limits<double> dbl;

        long traceNo = startTrace;
        for (const auto& traceData : result) { // all requested traces
            std::cout << "TraceNo: " << traceNo++ << std::endl;
            for (const auto& td : traceData) { // single trace fields
                std::cout.precision(dbl::digits10);
                const double roundedValue = round(td.value * 1000.0) / 1000.0; // Round to show only 3 digits after decimal point

                std::cout << "   Id:" << td.id << "  Offset: " << td.offset << " End:" << td.end << "   Value: " << roundedValue << std::endl;
            }
        }
    }
    catch (const std::logic_error& le) {
        std::cerr << "Logical error: " << le.what() << std::endl;
    }
}

void
TestSegyFile::ShowInterpretedTraceHeadersAsJson() const
{

    std::cout << "=====================================================================" << std::endl;
    std::cout << "=====           Interpreted Trace Headers As Json               =====" << std::endl;
    std::cout << "=====================================================================" << std::endl
              << std::endl;

    long startTrace = 0;
    int traceCount = 1000;
    std::cout << "Start scan from trace#   [0]               : ";
    std::string answer;
    std::cin >> answer;
    if (!answer.empty()) {
        startTrace = getLongFromString(answer);
        if (startTrace == -1) {
            return;
        }
    }
    std::cout << std::endl
              << "Number of traces to scan? [1000]: ";
    std::cin >> answer;
    if (!answer.empty()) {
        traceCount = getIntFromString(answer);
        if (traceCount == -1) {
            return;
        }
    }

    try {
        const auto result = session_->getScaledTraceHeadersAsJson(startTrace, traceCount);
        const std::string outputFileName = outputJsonFolder_ + "ScaledTraceHeadersOutput.json";
        writeJsonTestOutput(outputFileName, result, "TestSegyFile::ShowInterpretedTraceHeadersAsJson: Error writing scaled trace header data to Json file: ");
    }
    catch (const std::logic_error& le) {
        std::cerr << "Logical error: " << le.what() << std::endl;
    }
}

void
TestSegyFile::ShowTraceSamples() const
{

    std::cout << "=====================================================================" << std::endl;
    std::cout << "=====           Dump of Trace Samples                           =====" << std::endl;
    std::cout << "=====================================================================" << std::endl
              << std::endl;

    size_t startTrace{};
    std::cout << "Enter trace#   [0]               : ";
    std::cin >> startTrace;
    if (std::cin.fail()) {
        return;
    }

    try {
        const auto result = session_->getScaledTraceHeaders(startTrace, 1);

        typedef std::numeric_limits<double> dbl;

        int inlineNo{};
        int crossLineNo{};

        size_t traceNo = startTrace;
        for (const auto& traceData : result) { // all requested traces
            std::cout << "TraceNo: " << traceNo++ << std::endl;
            for (const auto& td : traceData) { // single trace fields
                std::cout.precision(dbl::digits10);
                const double roundedValue = round(td.value * 1000.0) / 1000.0; // Round to show only 3 digits after decimal point

                std::cout << "   Id:" << td.id << "  Offset: " << td.offset << " End:" << td.end << "   Value: " << roundedValue << std::endl;

                if (td.id == "Inline") {
                    inlineNo = static_cast<int>(roundedValue);
                }
                else if (td.id == "Crossline") {
                    crossLineNo = static_cast<int>(roundedValue);
                }
            }
        }

        assert(dynamic_cast<api::SegySession*>(session_.get()) != nullptr);
        auto bulkReader = dynamic_cast<api::SegySession*>(session_.get())->createBulkReader();
        assert(bulkReader.get());

        segyindex::SeismicCubeExtents const seismicCubeExtents
            = session_->getIndex()->getFileLayoutExtents();

        int const firstInlineNo = seismicCubeExtents.getJVectorExtent().first();
        int const firstCrosslineNo = seismicCubeExtents.getIVectorExtent().first();

        size_t const inlineIdx = std::abs(inlineNo - firstInlineNo);
        size_t const crosslineIdx = std::abs(crossLineNo - firstCrosslineNo);

        std::vector<float> buffer(static_cast<size_t>(seismicCubeExtents.getSampleExtent().count()), 0.0f);

        if (bulkReader->read(inlineIdx,
                             crosslineIdx,
                             0U,
                             1U,
                             1U,
                             static_cast<size_t>(seismicCubeExtents.getSampleExtent().count()),
                             buffer.data())) {
            for (size_t i = 0; i < buffer.size(); ++i) {
                std::cout << "Sample[" << i << "] = " << buffer[i] << std::endl;
            }
        }
    }
    catch (const std::logic_error& le) {
        std::cerr << "Logical error: " << le.what() << std::endl;
    }
}

void
TestSegyFile::ShowSequentialTraceSamples() const
{

    std::cout << "=====================================================================" << std::endl;
    std::cout << "=====           Dump of Sequential Trace Samples                =====" << std::endl;
    std::cout << "=====================================================================" << std::endl
              << std::endl;

    size_t startTrace{};
    std::cout << "Enter trace#   [0]               : ";
    std::cin >> startTrace;
    if (std::cin.fail()) {
        return;
    }

    try {
        const auto result = session_->getTraces(startTrace, 1);

        typedef std::numeric_limits<double> dbl;

        size_t traceNo = startTrace;
        for (const auto& traceData : result) { // all requested traces
            std::cout << "TraceNo: " << traceNo++ << std::endl;
            for (const auto& td : traceData.first) { // single trace fields
                std::cout.precision(dbl::digits10);
                const double roundedValue = round(td.value * 1000.0) / 1000.0; // Round to show only 3 digits after decimal point

                std::cout << "   Id:" << td.id << "  Offset: " << td.offset << " End:" << td.end << "   Value: " << roundedValue << std::endl;
            }
            for (size_t i = 0; i < traceData.second.size(); ++i) {
                std::cout << "Sample[" << i << "] = " << traceData.second[i] << std::endl;
            }
        }

        const std::string outputFileName = outputJsonFolder_ + "SequentialTraceSamplesOutput.json";
        writeJsonTestOutput(outputFileName, session_->getTracesAsJson(startTrace, 1), "TestSegyFile::ShowSequentialTraceSamples: Error writing trace data to Json file: ");
    }
    catch (const std::logic_error& le) {
        std::cerr << "Logical error: " << le.what() << std::endl;
    }
}

void
TestSegyFile::getVectorFromString(std::vector<char>* vect, std::string text)
{

    vect->clear();
    std::string::iterator it = text.begin();
    while (it < text.end()) {
        vect->push_back(*it++);
    }
}

fileio::FileReference
TestSegyFile::fr(const std::string& fileName) const
{
    const SDCredentials sdCredentials{sd_url_, sd_api_key_, utilities::TokenSourceFactory::getTokenSource()};
    // Max 64 * 16 Mb buffers
    constexpr size_t pageSize = 16 * 1024 * 1014;
    constexpr size_t maxPageCount = 64;
    constexpr size_t blocksPerPage = 1;
    constexpr size_t chunkSize = 1024 * 1014;
    CacheSizeOption cacheSizeOption{pageSize, maxPageCount, blocksPerPage, chunkSize};
    const fileio::FileReaderOptions readerOptions(sdCredentials, cacheSizeOption);
    return fileio::FileReference(fileName, readerOptions);
}

std::string
TestSegyFile::GetJsonFromInputFile(const std::string& fileName) const
{
    std::string result = emptyJson;

    try {
        const size_t filesize = fileio::GenericFileManager::getSize(fr(fileName));
        if (filesize == 0)
            return result;

        auto [success, contents] = fileio::GenericFileManager::readTextFile(fileio::FileManagerParams{fr(fileName)});
        if (success) {
            return contents;
        }
    }
    catch (const char* msg) {
        std::cerr << "Error reading context file." << std::endl;
        std::cerr << msg << std::endl;
    }

    return result;
}

void
TestSegyFile::AcceptAndRegisterSettings() const
{
    session_->acceptAndRegisterSettings();
}

void
TestSegyFile::LoadSettingsFromSavedParameterFile() const
{
    session_->loadStoredSettings();
}

void
TestSegyFile::GenerateIndexFile() const
{

    bool forceOverwrite = false;
    constexpr bool forceGenerateIndex = false;

    if (getIndex()->hasIndexFile()) {
        SEGYLOG_INFO("Current SEG-Y file already has a saved index file.");
        SEGYLOG_INFO("Would you like to overwrite it and create a new one? (y/N)");
        char answer;
        std::cin >> answer;
        if (answer != 'Y' && answer != 'y') {
            return;
        }

        // Force overwrite
        forceOverwrite = true;
    }

    const std::chrono::steady_clock::time_point begin = std::chrono::steady_clock::now();

    SEGYLOG_INFO("Building up index generation data ... ");

    session_->generateIndexFile(forceGenerateIndex, forceOverwrite);

    const std::chrono::steady_clock::time_point endRead = std::chrono::steady_clock::now();

    SEGYLOG_INFO("Elapsed Read time: " + std::to_string(std::chrono::duration_cast<std::chrono::seconds>(endRead - begin).count()) + "[s]");
}

void
TestSegyFile::GenerateIndexGeometryInformation() const
{
    const auto g = getIndex()->getGeometry3D();
    const auto f = getIndex()->getFileLayout();

    SEGYLOG_INFO("-----------------------------------------------------------------------------------------------");
    SEGYLOG_INFO("Extents:  \t\tFirst\tLast\tStep");
    SEGYLOG_INFO("\tInlines:\t" + std::to_string(f.JExtent.first()) + "\t" + std::to_string(f.JExtent.last()) + "\t" + std::to_string(f.JExtent.step()));
    SEGYLOG_INFO("\tCrosslines:\t" + std::to_string(f.IExtent.first()) + "\t" + std::to_string(f.IExtent.last()) + "\t" + std::to_string(f.IExtent.step()));
    SEGYLOG_INFO("-----------------------------------------------------------------------------------------------");

    if (getIndex()->isRegular()) {

        SEGYLOG_INFO("Grid corners: ");
        SEGYLOG_INFO("\tInline\t\tXline\t\tX\t\t\tY");

        typedef std::numeric_limits<double> dbl;
        std::cout.precision(dbl::max_digits10);

        SEGYLOG_INFO("11\t" + std::to_string(g.dataGrid.gridCell_11.i) + "\t\t" + std::to_string(g.dataGrid.gridCell_11.j) + "\t" + std::to_string(g.dataGrid.gridCell_11.location.X) + "\t" + std::to_string(g.dataGrid.gridCell_11.location.Y));
        SEGYLOG_INFO("1N\t" + std::to_string(g.dataGrid.gridCell_1N.i) + "\t\t" + std::to_string(g.dataGrid.gridCell_1N.j) + "\t" + std::to_string(g.dataGrid.gridCell_1N.location.X) + "\t" + std::to_string(g.dataGrid.gridCell_1N.location.Y));
        SEGYLOG_INFO("N1\t" + std::to_string(g.dataGrid.gridCell_N1.i) + "\t\t" + std::to_string(g.dataGrid.gridCell_N1.j) + "\t" + std::to_string(g.dataGrid.gridCell_N1.location.X) + "\t" + std::to_string(g.dataGrid.gridCell_N1.location.Y));
        SEGYLOG_INFO("NN\t" + std::to_string(g.dataGrid.gridCell_NN.i) + "\t\t" + std::to_string(g.dataGrid.gridCell_NN.j) + "\t" + std::to_string(g.dataGrid.gridCell_NN.location.X) + "\t" + std::to_string(g.dataGrid.gridCell_NN.location.Y));

        SEGYLOG_INFO("Intervals\t\t\t" + std::to_string(g.dataGrid.JVectorUnitDistance) + "\t" + std::to_string(g.dataGrid.IVectorUnitDistance));
        SEGYLOG_INFO("Rotation degrees\t\t" + std::to_string(g.dataGrid.JVectorRotationAngle) + "\t" + std::to_string(g.dataGrid.IVectorRotationAngle));
        SEGYLOG_INFO("-----------------------------------------------------------------------------------------------");
    }
    SEGYLOG_INFO("Vertical geometry:");
    SEGYLOG_INFO("Time/depth top:\t\t" + std::to_string(f.sampleExtent.first()));
    SEGYLOG_INFO("Sample interval:\t" + std::to_string(f.sampleExtent.step()));
    SEGYLOG_INFO("Time/depth bottom : \t" + std::to_string(f.sampleExtent.last()));
    SEGYLOG_INFO("-----------------------------------------------------------------------------------------------");
}

void
TestSegyFile::SaveIndexAsJson()
{
    if (!getIndex()->isValid())
        return;
    std::string segyFileBase = segyFileNamesVector_[0];

    const size_t last_slash_idx = segyFileBase.find_last_of("\\/");
    if (std::string::npos != last_slash_idx) {
        segyFileBase.erase(0, last_slash_idx + 1);
    }

    const std::string outputFileName = outputJsonFolder_ + segyFileBase + ".Index.json";
    getIndex()->SaveAsJson(outputFileName);

    SEGYLOG_INFO("Json dump of index file saved as : ");
}

void
TestSegyFile::LoadIndexFile() const
{
    try {
        session_->loadIndexFile();
    }
    catch (const std::invalid_argument& ex) {
        const std::string exceptionString = ex.what();
        const std::string msg = "TestSegyFile::LoadIndexFile: Unable to load index file: Is input SEG-Y file specified correctly? " + exceptionString;
        SEGYLOG_INFO(msg);
    }
    catch (const std::runtime_error& ex) {
        const std::string exceptionString = ex.what();
        const std::string msg = "TestSegyFile::LoadIndexFile: Unable to load index file: Has an index been generated and saved for the current SEG-Y file? " + exceptionString;
        SEGYLOG_INFO(msg);
    }
}

} // namespace segysdk::tests
