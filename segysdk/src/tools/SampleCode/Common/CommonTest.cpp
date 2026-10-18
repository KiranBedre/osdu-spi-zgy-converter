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

#include "EnvironmentVariables.h"
#include "FileCollectionUtilities.h"
#include "JsonToolKit.h"

#include "../FileIO/GenericFileReader.h"
#include "../segysdk_details/SegyException.h"
#include "../Utilities/EnvironmentVariableStringConsts.h"
#include "../Utilities/StringUtils.h"
#include "../Utilities/InputParser.h"

#include <iostream>

using namespace segysdk::fileio;
using namespace segysdk::utilities;

int
main(int argc, char** argv)
{
    segysdk::utilities::InputParser input(argc, argv);

    std::vector<std::string> segyFileList;
    std::string segyFile;
    std::string osduFileCollection;

    std::string outputJSONFolder;
    std::string jsonFile;

    if (input.cmdOptionExists("-h") || input.cmdOptionExists("-help") || argc < 2) {
        std::cout << argv[0] << " called with " << std::to_string(argc - 1) << " parameters." << std::endl;

        std::cout << "Usage: " << std::endl;
        std::cout << "\tFor OSDU Data Platform: " << std::endl;
        std::cout << "\t\t" << argv[0] << " -osdu <FileCollection.SEGY.json -outputFolder Path/To/Output/" << std::endl;

        std::cout << "To access data in cloud SDMS the following environment variables must be defined:" << std::endl;
        std::cout << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_URL << std::endl;
        std::cout << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_API_KEY << std::endl;
        std::cout << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_TOKEN << std::endl;

        return -1;
    }

    const std::string& osduFileName = input.getCmdOption("-osdu");
    if (!osduFileName.empty()) {
        osduFileCollection = osduFileName;
    }
    else {
        throw segysdk::SegyInvalidArgumentException("Input fileCollection.SEGY parameter must be a valid file name!");
    }

    const std::string& outputFolder = input.getCmdOption("-outputFolder");
    if (!outputFolder.empty()) {
        outputJSONFolder = (StringUtils::keyEndsWith(outputFolder, "\\")) ? outputFolder : outputFolder + "\\";
    }
    else {
        outputJSONFolder = "D:\\Temp\\";
    }

    std::vector<std::string> inputFileList;
    SampleCode::Common::FileCollectionUtilities::PopulateFileListFromFileCollection(osduFileCollection, inputFileList);

    if (inputFileList.empty()) {
        std::cout << "Parsing provided FileCollection.SEGY file failed" << std::endl;
        return -1;
    }

    std::cout << "Parsed input FileCollection.SEGY" << std::endl;
    for (const auto& f : inputFileList) {
        std::cout << "Input File: " << f << std::endl;
    }
}
