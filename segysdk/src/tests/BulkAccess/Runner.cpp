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

#include "../Utilities/Environment.h"

#include <gtest/gtest.h>

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <string>

static void
configureSDMS()
{
    std::string token;

    auto const stoken = segysdk::utilities::Environment::instance().sdToken();
    if (!stoken.empty()) {
        token = stoken;
    }
    else {
        auto const fileName = segysdk::utilities::Environment::instance().sdTokenFile();
        if (!fileName.empty()) {
            std::ifstream file(fileName, std::ios::in);
            if (file.good()) {
                file >> token;
            }
            else {
                std::cout << "Failure getting authentication token from " << fileName << std::endl;
            }
        }
    }

    if (token.empty()) {
        std::cout << "Seismic Data Management Services (SDMS) accessor not configured, the authentication token has not been defined." << std::endl;
        std::cout << "Please set it in the SD_SVC_TOKEN environment variable or set SD_SVC_TOKEN_FILE to designate a file containing it to run these tests." << std::endl;
    }
}

int
main(int argc, char** argv)
{
    configureSDMS();

    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
