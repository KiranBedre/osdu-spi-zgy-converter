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

#include "UnitTestDataProvider.h"

#include "../Utilities/Environment.h"
#include "../Utilities/JsonUtilities.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <stdexcept>

namespace segysdk::segyindex::tests
{
using namespace utilities;

UnitTestDataProvider::UnitTestDataProvider()
  : dataFilePath_(evaluateTestDataRootFolder()),
    regular_(evaluateRegularFilesFolder()),
    nonRegular_(evaluateNonRegularFilesFolder())
{
}

Json::Value
UnitTestDataProvider::getDataAsJson(const std::string& jsonFileName, DataGridType gridType) const
{
    const std::filesystem::path filePath = gridType == DataGridType::Regular ? getRegularFilesFolder() /= jsonFileName : getNonRegularFilesFolder() /= jsonFileName;

    if (!exists(filePath))
        throw std::runtime_error("Unable to find unit data file: " + filePath.string());

    const std::string json = getJsonString(filePath);

    return JsonUtilities::getJsonFromString(json);
}

std::string
UnitTestDataProvider::getJsonString(const std::filesystem::path& filePath)
{
    const std::ifstream ifs(filePath);
    std::stringstream buffer;
    buffer << ifs.rdbuf();

    return buffer.str();
}

std::filesystem::path
UnitTestDataProvider::evaluateTestDataRootFolder() const
{

    std::filesystem::path testdata_dir = utilities::Environment::instance().SegyLocalTestDataPath();
    if (testdata_dir.empty()) {
        std::cerr << "Missing environment variable " << testdata_dir << std::endl;
        throw std::runtime_error("Environment variable SEGY_LOCALTESTDATA_PATH has not been set.");
    }

    testdata_dir /= "SegyIndexUnitTestData";

    if (exists(testdata_dir))
        return testdata_dir;

    // This is serious enough to abort
    throw std::runtime_error("Cannot find test data folder: " + testdata_dir.string());
}

std::filesystem::path
UnitTestDataProvider::evaluateRegularFilesFolder() const
{

    std::filesystem::path datapath = evaluateTestDataRootFolder() /= "Regular";
    if (exists(datapath))
        return datapath;

    // This might be because folder has not been populated yet, and should be handled in receiving code
    std::filesystem::path p;
    return p;
}

std::filesystem::path
UnitTestDataProvider::evaluateNonRegularFilesFolder() const
{

    std::filesystem::path datapath = evaluateTestDataRootFolder() /= "NonRegular";

    if (exists(datapath))
        return datapath;

    // This might be because folder has not been populated yet, and should be handled in receiving code
    std::filesystem::path p;
    return p;
}

std::filesystem::path
UnitTestDataProvider::getRegularFilesFolder() const
{
    if (!regular_.empty())
        return regular_;

    // This might be because folder has not been populated yet, and should be handled in receiving code
    std::filesystem::path p;
    return p;
}

std::filesystem::path
UnitTestDataProvider::getNonRegularFilesFolder() const
{
    if (!nonRegular_.empty())
        return nonRegular_;

    // This might be because folder has not been populated yet, and should be handled in receiving code
    std::filesystem::path p;
    return p;
}
} // namespace segysdk::segyindex::tests
