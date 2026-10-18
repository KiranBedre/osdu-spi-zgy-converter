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

#include "JsonBinGrid.h"

#include "JsonUtilities.h"

#include "../Utilities/delfi/DelfiBinGrid.h"
#include "../Utilities/osdu/OsduSeismicBinGrid100.h"

#include <json/json.h>

#include <exception>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>

namespace segysdk::utilities
{

static BinGrid
loadBinGridFromStream(std::istream& stream)
{
    const auto root = JsonUtilities::getJsonFromStream(stream);
    if (root["data"].isMember("binGridOperation")) {
        // Load Delfi bin grid
        return delfi::DelfiBinGrid::loadBinGridFromJson(root);
    }

    // Load OSDU bin grid
    return osdu::OsduSeismicBinGrid100::loadBinGridFromJson(root);
}

BinGrid
JsonBinGrid::loadBinGridFromFile(const std::string& binGridFileName)
{
    if (!std::filesystem::exists(binGridFileName)) {
        const std::string msg = "Unable to locate specified input file [" + binGridFileName + "]";
        std::cerr << msg << std::endl;
        throw std::invalid_argument(msg);
    }
    std::ifstream stream(binGridFileName, std::ios::binary);

    return loadBinGridFromStream(stream);
}

BinGrid
JsonBinGrid::loadBinGridFromContent(const std::string& binGridContent)
{
    if (binGridContent.empty() || binGridContent == "{}") {
        return {};
    }

    std::istringstream stream(binGridContent, std::ios::binary);
    return loadBinGridFromStream(stream);
}

BinGrid::TransformationMethod
JsonBinGrid::parseTransformationMethodCode(const Json::Value& jsonValue)
{
    switch (jsonValue.asInt()) {
    case 1049:
        return BinGrid::TransformationMethod::LeftHanded;
    case 9666:
        return BinGrid::TransformationMethod::RightHanded;
    default:
        throw std::runtime_error("Unknown transformationMethod: " + jsonValue.asString());
    }
}

} // namespace segysdk::utilities
