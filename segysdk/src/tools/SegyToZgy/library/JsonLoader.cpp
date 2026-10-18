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

#include "JsonLoader.h"

#include <json/json.h>

#include <fstream>
#include <sstream>

Json::Value
JsonLoader::fromFile(const std::string& fileName)
{
    std::ifstream stream(fileName, std::ios::binary);
    return fromStream(stream);
}

Json::Value
JsonLoader::fromStream(std::istream& stream)
{
    std::string errors;
    Json::CharReaderBuilder builder;
    builder["collectComments"] = false;
    Json::Value root;

    if (!Json::parseFromStream(builder, stream, &root, &errors)) {
        throw std::runtime_error("Error parsing JSON: " + errors);
    }

    return root;
}

Json::Value
JsonLoader::fromString(const std::string& content)
{
    std::istringstream stream(content, std::ios::binary);
    return fromStream(stream);
}
