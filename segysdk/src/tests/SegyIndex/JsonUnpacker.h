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

#include "Expected.h"
#include "Parameters.h"

#include "../SegyIndex/IndexBuilderCollatedData.h"

#include <memory>
#include <vector>

namespace segysdk::segyindex
{
struct IndexBuilderData;
}

namespace Json
{
class Value;
}

namespace segysdk::segyindex::tests
{

class JsonUnpacker
{

public:
    static Expected getExpectedResultFromJson(Json::Value json, int& errors);

    static HorizontalExtent getHorizontalExtentFromJson(Json::Value json, int& errors);
    static Geometry3D getGeometry3DFromJson(Json::Value json, HorizontalExtent inlineExtent, HorizontalExtent crosslineExtent, int& errors);
    static FileLayoutSpecification getFileLayoutFromJson(Json::Value json, HorizontalExtent inlineExtent, HorizontalExtent crosslineExtent, int& errors);
    static Parameters getParametersFromJson(Json::Value json, int& errors);
    static std::vector<std::shared_ptr<IndexBuilderCollatedData>> getIndexBuilderDataFromJson(Json::Value, int& errors);

    static void debugOutputMembers(const Json::Value& json, int& errors, bool generateDebugOutput = false);

    static std::vector<Json::Value> getTestDataArrayRegulars();
    static std::vector<Json::Value> getTestDataArrayNonRegulars();
};

} // namespace segysdk::segyindex::tests
