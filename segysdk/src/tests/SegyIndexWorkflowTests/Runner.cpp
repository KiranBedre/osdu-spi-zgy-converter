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

#include "SegyIndexWorkflowTest.h"

#include "../Utilities/InputParser.h"

#include <fstream>
#include <iostream>
#include <string>

using namespace segysdk::segyindex::tests;

void
showInfoMessage(char** argv)
{
    const std::string me(argv[0]);
    std::cout << "Usage: \n\t" + me + " -w <workflow.json> | -d to produce sample workflow json | -h to produce this message." << std::endl;
}

int
main(int argc, char** argv)
{
    const segysdk::utilities::InputParser input(argc, argv);

    if (input.cmdOptionExists("-h") || input.cmdOptionExists("-?")) {
        showInfoMessage(argv);
        return 0;
    }

    if (input.cmdOptionExists("-d")) {
        std::string dataPath = input.getCmdOption("-d");
        if (dataPath.empty())
            dataPath = "D:\\TestDataFolder";
        SegyIndexWorkflowTests::generateDemoWorkflowJson(dataPath);
        return 0;
    }

    std::string workFlow;
    bool useWorkflow = false;

    if (input.cmdOptionExists("-w")) {
        workFlow = input.getCmdOption("-w");
        useWorkflow = true;
    }
    if (useWorkflow && workFlow.empty()) {
        std::cout << "-w option must be used with the path to a valid workflow json file." << std::endl;
        return 99;
    }

    if (workFlow.empty()) {
        showInfoMessage(argv);
        return 90;
    }

    SegyIndexWorkflowTests::runAllTests(workFlow);

    return 0;
}
