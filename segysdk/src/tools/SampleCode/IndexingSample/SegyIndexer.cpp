/*
Copyright 2022 Schlumberger

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

#include "../tools/SampleCode/Common/SessionUtilities.h"

#include "../SegyIO/SegyIOException.h"

#include "../Utilities/EnvironmentVariableStringConsts.h"
#include "../Utilities/StringUtils.h"
#include "../Utilities/InputParser.h"

#include <iostream>

void
ShowGeometry(std::shared_ptr<segysdk::segyindex::SegyIndex>& index)
{
    if (index->hasIndexFile()) {
        segysdk::segyindex::SeismicCubeExtents const seismicCubeExtents
            = index->getFileLayoutExtents();

        std::cout << "Successfully generated SEG-Y index." << std::endl;
        std::cout << "Geometry:" << std::endl;
        std::cout << "First inline:     " << seismicCubeExtents.getJVectorExtent().first() << "   Last inline:     " << seismicCubeExtents.getJVectorExtent().last() << "  Step: " << seismicCubeExtents.getJVectorExtent().step() << std::endl;
        std::cout << "First crossline: " << seismicCubeExtents.getIVectorExtent().first() << "   Last crossline: " << seismicCubeExtents.getIVectorExtent().last() << "  Step: " << seismicCubeExtents.getIVectorExtent().step() << std::endl;
        std::cout << "First sample at:     " << seismicCubeExtents.getSampleExtent().first() << "   Last sample at:  " << seismicCubeExtents.getSampleExtent().last() << "  Step: " << seismicCubeExtents.getSampleExtent().step() << std::endl;
    }
}

void
print_usage(const char* exec_name, int argc)
{
    std::cout << exec_name << " called with " << std::to_string(argc - 1) << " parameters." << std::endl;

    std::cout << "Usage: " << std::endl;
    std::cout << "\tFor OSDU Data Platform: " << std::endl;
    std::cout << "\t\t" << exec_name << " -osdu <FileCollection.SEGY.json> [-verbose=0|1] " << std::endl;

    std::cout << "To access data in cloud SDMS the following environment variables must be defined:" << std::endl;
    std::cout << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_URL << std::endl;
    std::cout << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_API_KEY << std::endl;
    std::cout << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_TOKEN << std::endl;
}

int
main(int argc, char** argv)
{
    const segysdk::utilities::InputParser input(argc, argv);

    std::vector<std::string> segyFileList;
    std::string segyFile;

    std::string jsonFile;

    // Check if user has provided valid runtime parameters. If not, provide usage information
    if (input.cmdOptionExists("-h") || input.cmdOptionExists("-help") || argc < 2) {
        print_usage(argv[0], argc);
        return -1;
    }

    std::string osduFileCollection;
    const std::string& osduFileName = input.getCmdOption("-osdu");
    if (!osduFileName.empty()) {

        osduFileCollection = osduFileName;
    }
    else {
        std::cout << "Input FileCollection.SEGY parameter must be a valid file name!" << std::endl;
        return -1;
    }

    try {
        const auto session = SampleCode::Common::SessionUtilities::GetSegySessionFromFileCollectionJson(osduFileCollection);
        if (session == nullptr) {
            std::cout << "SegyIndexer: Unable to create a valid SEG-Y session, exiting!" << std::endl;
            return -10;
        }

        std::shared_ptr<segysdk::segyindex::SegyIndex> index = SampleCode::Common::SessionUtilities::GetSegyIndex(session);

        if (index == nullptr) {
            std::cout << "SegyIndexer: Could not get index object!" << std::endl;
            return -15;
        }

        bool forceOverwrite = false;
        if (!index->hasIndexFile(true)) {
            std::cout << "Segy file does not have an index. Will generate it ... " << std::endl;
        }
        else {
            std::cout << "Segy file already has an index. Do you really want to generate a new one? [y/n] ";

            std::string response;
            std::cin >> response;

            if (response != "y" && response != "Y") {
                std::cout << "Segy indexing process cancelled by user." << std::endl;

                ShowGeometry(index);
                return 0;
            }
            forceOverwrite = true;
        }

        constexpr bool forceGenerate = false; // Forces generation of map off

        session->generateIndexFile(forceGenerate, forceOverwrite);

        ShowGeometry(index);
    }
    catch (segysdk::SegyInvalidArgumentException& ex) {
        std::cout << std::endl
                  << "Invalid argument passed to generateIndexFile! Error: " << ex.what() << std::endl;
        return -70;
    }
    catch (segysdk::segyio::SegyRuntimeException& ex) {
        std::cout << std::endl
                  << "Segy Runtime exception! Error: " << ex.what() << std::endl;
        return -180;
    }
    catch (segysdk::SegyException& ex) {
        std::cout << std::endl
                  << "Exception when executing generateIndexFile! Error: " << ex.what() << std::endl;
        return -90;
    }
    catch (std::exception& ex) {
        std::cout << std::endl
                  << "Error generating SEG-Y Index! Error: " << ex.what() << std::endl;
        return -100;
    }

    return 0;
}
