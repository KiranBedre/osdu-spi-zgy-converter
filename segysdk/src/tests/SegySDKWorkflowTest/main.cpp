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
#include <SegySdkProject.h>

#include "../../FileIO/GenericFileManager.h"
#include "../../Utilities/Environment.h"
#include "../../Utilities/InputParser.h"
#include "../../Utilities/JsonUtilities.h"
#include "../../Utilities/StringUtils.h"
#include "../../Utilities/TokenSourceFactory.h"

#include <iostream>
#include <string>

using namespace segysdk::tests;
using namespace segysdk::utilities;
using namespace segysdk::segyio;
using namespace segysdk::fileio;

segysdk::SDCredentials
sdmsConfig(const std::string& fileName)
{

    if (!GenericFileManager::isSDDataset(fileName)) {
        return segysdk::SDCredentials::none();
    }

    auto tokenSource = TokenSourceFactory::getTokenSource();
    const std::string sdmsUrl = Environment::instance().sdUrl();
    const std::string sdmsAppKey = Environment::instance().sdApiKey();

    if (!tokenSource->token().empty() && !sdmsUrl.empty() && !sdmsAppKey.empty()) {
        std::cout << "Seismic Data Management Services (SDMS) configured to access " << sdmsUrl << std::endl;
        std::cout << "SDMS configured with app key " << sdmsAppKey << std::endl;
        // std::cout << "SDMS configured with token " << stoken << std::endl;
    }
    else {
        std::cout << "SDMS accessor not configured." << sdmsUrl << std::endl;
    }

    return {sdmsUrl, sdmsAppKey, std::move(tokenSource)};
}

FileReaderOptions
getFileReaderOptions(const std::string& fileName)
{
    if (!GenericFileManager::isSDDataset(fileName)) {
        return FileReaderOptions::localOnly();
    }

    const segysdk::SDCredentials credentials = sdmsConfig(fileName);
    const segysdk::CacheSizeOption cacheSize{2 * 1024 * 1024, 32, 1, 1024U * 1024U};

    FileReaderOptions readerOptions(credentials, cacheSize);

    return readerOptions;
}

FileReference
getFileReference(const std::string& fileName)
{
    const FileReaderOptions readerOptions = getFileReaderOptions(fileName);
    FileReference fileReference(fileName, readerOptions);

    return fileReference;
}

std::string
GetJsonFromInputFile(const FileReference& fileReference)
{
    std::string result = "{}";

    try {
        if (GenericFileManager::getSize(fileReference) == 0)
            return result;

        auto [success, contents] = GenericFileManager::readTextFile(segysdk::fileio::FileManagerParams{fileReference});
        if (success) {
            return contents;
        }
    }
    catch (std::exception& ex) {
        std::cerr << "Error reading input file list." << std::endl;
        std::cerr << ex.what() << std::endl;
    }

    return result;
}

int
main(int argc, char** argv)
{

    InputParser input(argc, argv);

    std::vector<std::string> segyFileList;
    std::string segyFile;
    std::string osduFile;

    std::string outputJSONFolder;
    std::string jsonFile;

    if (input.cmdOptionExists("-h") || input.cmdOptionExists("-help")) {
        std::cout << "Usage: " << std::endl;
        std::cout << "\tFor DELFI hosted data: " << std::endl;
        std::cout << "\t\t" << argv[0] << " -f <SEG-Y file> -c <JSON config file>  [-o  <Folder for generated Json files>]" << std::endl;
        std::cout << "\t\t" << argv[0] << " -flist <json file listing input segy files> -c <JSON config file>  [-o  <Folder for generated Json files>]" << std::endl;
        std::cout << "\tFor OSDU Data Platform: " << std::endl;
        std::cout << "\t\t" << argv[0] << " -osdu <OSDU FileCollection.SEGY json file> [-o  <Folder for generated Json files>]" << std::endl;
        return 0;
    }

    const std::string& osduFileName = input.getCmdOption("-osdu");
    if (!osduFileName.empty()) {
        osduFile = osduFileName;
    }

    const std::string& filename = input.getCmdOption("-f");
    if (!filename.empty()) {
        segyFile = filename;
    }

    const std::string& outputFolder = input.getCmdOption("-o");
    if (!outputFolder.empty()) {
        outputJSONFolder = (StringUtils::keyEndsWith(outputFolder, "\\")) ? outputFolder : outputFolder + "\\";
    }
    else {
        outputJSONFolder = "D:\\Temp\\";
    }

    const std::string& configFile = input.getCmdOption("-c");
    if (!configFile.empty()) {
        jsonFile = configFile;
    }
    else
        jsonFile = "";

    std::vector<std::string> inputFileList;

    auto fr = getFileReference(segyFile);
    if (!segyFile.empty() && (GenericFileManager::isSDDataset(segyFile) || GenericFileManager::exists(fr))) {
        inputFileList.push_back(segyFile);
    }

    const std::string& filelistJson = input.getCmdOption("-flist");
    if (!filelistJson.empty() && GenericFileManager::exists(getFileReference(filelistJson))) {
        std::string json = GetJsonFromInputFile(getFileReference(filelistJson));
        Json::Value root = JsonUtilities::getJsonFromString(json);
        if (!root.isMember("InputSegyFiles")) {
            std::cout << "No input files specified! Exit program." << std::endl;
            return -1;
        }
        Json::Value fListNode = root["InputSegyFiles"];
        for (Json::Value::ArrayIndex i = 0; i != fListNode.size(); i++) {
            if (fListNode[i].isMember("FileName")) {
                std::string fname = fListNode[i]["FileName"].asString();
                if (GenericFileManager::exists(getFileReference(fname)))
                    inputFileList.push_back(fname);
            }
        }
    }

    std::cout << "Segy SDK: Version " << SegySdkProject_VERSION << std::endl;

    while (true) {
        TestSegyFile unitTester = osduFile.empty()
                                      ? TestSegyFile(inputFileList, jsonFile, outputJSONFolder)
                                      : TestSegyFile(osduFile, outputJSONFolder);

        unitTester.ShowSettings();
        unitTester.performTests();

        std::cout << "Segy SDK: Really quit? [Y/n] : ";
        char option;
        std::cin >> option;
        if (option == ' ' or option == 'y' or option == 'Y') {
            break;
        }
    }

    std::cout << "Segy SDK: Exit and close session ...  " << std::endl;
}
