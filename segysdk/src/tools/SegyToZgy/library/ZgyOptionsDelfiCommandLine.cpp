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

#include "ZgyOptionsDelfiCommandLine.h"

#include "../Utilities/Adapters.h"
#include "../Utilities/BinGrid.h"
#include "../Utilities/JsonBinGrid.h"

#include "../Utilities/Environment.h"
#include <openzgy/api.h>
#include <openzgy/iocontext.h>

#include <filesystem>
#include <fstream>
#include <sstream>

static std::string
readFile(const std::string& fileName)
{
    std::ostringstream string;
    const std::ifstream file(fileName);
    string << file.rdbuf();
    return string.str();
}

ZgyOptionsDelfiCommandLine::ZgyOptionsDelfiCommandLine(int argc, const char** argv, segysdk::utilities::OptionsFileType oft)
  : ZgyOptions(oft)
{
    if (argc != 5) {
        help(argv[0]);
        exit(1);
    }

    sourceFile_ = argv[1];
    const std::string segyConfigFile = argv[2];
    binGridFile_ = argv[3];
    targetFile_ = argv[4];

    if (not segysdk::utilities::isSDDataset(sourceFile_)
        and not std::filesystem::exists(sourceFile_)) {
        throw std::runtime_error("Source file does not exist: " + sourceFile_);
    }

    if (!binGridFile_.empty()
        and not segysdk::utilities::isSDDataset(binGridFile_)
        and not std::filesystem::exists(binGridFile_)) {
        throw std::runtime_error("Bin grid file does not exist: " + binGridFile_);
    }

    if (not std::filesystem::exists(segyConfigFile)) {
        throw std::runtime_error("SEG-Y config file does not exist: " + segyConfigFile);
    }

    segyConfig_ = readFile(segyConfigFile);
}

std::vector<std::string>
ZgyOptionsDelfiCommandLine::sourceFiles() const
{
    return std::vector{sourceFile_};
}
std::string
ZgyOptionsDelfiCommandLine::segyConfig() const
{
    return segyConfig_;
}

segysdk::utilities::BinGrid
ZgyOptionsDelfiCommandLine::binGrid() const
{
    return segysdk::utilities::JsonBinGrid::loadBinGridFromFile(binGridFile_);
}

std::string
ZgyOptionsDelfiCommandLine::targetFile() const
{
    return targetFile_;
}

void
ZgyOptionsDelfiCommandLine::finalize() const
{
    // no action needed
}

void
ZgyOptionsDelfiCommandLine::help(const std::string& programName)
{
    std::cerr << "Usage: " << programName << " input_file.sgy config_file.json bin_grid_file.json output_file.zgy" << std::endl;
}
