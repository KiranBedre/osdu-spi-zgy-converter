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

#pragma once

#include "../FileIO/FileReference.h"
#include "../SegyIndex/SegyIndexImpl.h"
#include "../segysdk/SegySessionFactory.h"
#include "../segysdk/SegySessionImpl.h"

namespace segysdk::tests
{

class TestSegyFile
{

private:
    std::vector<std::string> segyFileNamesVector_;

    std::string jsonFileName_;
    std::string outputJsonFolder_;
    std::string sd_url_;
    std::string sd_api_key_;

    size_t sd_cache_page_size_{};
    size_t sd_max_pages_{};
    size_t sd_blocks_per_page_{};
    size_t sd_chunk_size_{};

    const std::string emptyJson = "{}";

    bool sessionOK_ = false;
    std::shared_ptr<api::SegySessionImpl> session_;

    int getIntFromString(const std::string& text) const;
    long getLongFromString(const std::string& text) const;
    unsigned long getULongFromString(const std::string& text) const;
    static void getVectorFromString(std::vector<char>* vect, std::string text);
    fileio::FileReference fr(const std::string& fileName) const;

    std::string GetJsonFromInputFile(const std::string& fileName) const;

    void writeJsonTestOutput(const std::string& outputFile, const std::string& result, const std::string& errormsg) const;

    bool testIfCloudSession(const std::string& jsonFile);

    CacheSizeOption getSDCacheSize();

    void initSdArgs(bool isCloudSession);
    void initSession(const bool& isCloudSession, const api::SegySessionFactory& sessionFactory, const std::string& jsonParams);

    std::shared_ptr<segyindex::SegyIndexImpl> getIndex() const;

public:
    TestSegyFile(const std::vector<std::string>& segyFileNamesList, const std::string& jsonFileName, std::string outputJsonFolder);
    TestSegyFile(const std::string& osduFileCollectionJson, std::string outputJsonFolder);

    void ShowSettings() const;
    void SelectInputFile() const;
    void performTests();
    void DisplayMenu() const;

    void ShowTextualHeaders();
    static void DisplayTextualHeaders(const std::string& headerText, const std::string& heading);

    void ShowBinaryHeaders() const;

    void ShowRawTraceHeaders() const;
    void ShowRawTraceHeadersAsJson() const;

    void ShowInterpretedTraceHeaders() const;
    void ShowInterpretedTraceHeadersAsJson() const;
    void ShowTraceSamples() const;
    void ShowSequentialTraceSamples() const;

    void AcceptAndRegisterSettings() const;
    void LoadSettingsFromSavedParameterFile() const;

    void GenerateIndexFile() const;
    void GenerateIndexGeometryInformation() const;
    void SaveIndexAsJson();

    void LoadIndexFile() const;
};

} // namespace segysdk::tests
