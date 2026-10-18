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

#include "JsonUnpacker.h"
#include "UnitTestDataProvider.h"

#include "../SegyIndex/IndexBuilderCollatedData.h"
#include "../SegyIndex/IndexBuilderData.h"

#include "../Utilities/EnsureWrapper.h"

#include <cstdint>
#include <filesystem>

namespace segysdk::segyindex::tests
{

using namespace utilities;

std::vector<Json::Value>
JsonUnpacker::getTestDataArrayRegulars()
{

    std::vector<Json::Value> unitTestsJson;

    const UnitTestDataProvider testProvider;
    const std::filesystem::path regularPath = testProvider.getRegularFilesFolder();
    if (regularPath.empty() || !std::filesystem::exists(regularPath))
        return unitTestsJson;

    const std::string ext(".json");
    for (const auto& p : std::filesystem::recursive_directory_iterator(regularPath)) {
        if (p.path().extension() == ext) {

            Json::Value json = testProvider.getDataAsJson(p.path().filename().string(), DataGridType::Regular);
            unitTestsJson.push_back(json);
        }
    }
    return unitTestsJson;
}

std::vector<Json::Value>
JsonUnpacker::getTestDataArrayNonRegulars()
{

    std::vector<Json::Value> unitTestsJson;

    const UnitTestDataProvider testProvider;
    const std::filesystem::path nonRegularPath = testProvider.getNonRegularFilesFolder();
    if (nonRegularPath.empty() || !std::filesystem::exists(nonRegularPath))
        return unitTestsJson;

    const std::string ext(".json");
    for (const auto& p : std::filesystem::recursive_directory_iterator(nonRegularPath)) {
        if (p.path().extension() == ext) {

            Json::Value json = testProvider.getDataAsJson(p.path().filename().string(), DataGridType::NonRegular);
            unitTestsJson.push_back(json);
        }
    }
    return unitTestsJson;
}

std::vector<std::shared_ptr<IndexBuilderCollatedData>>
JsonUnpacker::getIndexBuilderDataFromJson(Json::Value json, int& errors)
{
    std::vector<std::shared_ptr<IndexBuilderCollatedData>> indexBuilderCollatedDataVector;
    if (json.empty())
        return indexBuilderCollatedDataVector;

    int error = 0;
    debugOutputMembers(json[0], error);
    if (error > 0)
        errors += error;

    uint16_t fileNo = 0;

    auto collated = std::make_shared<IndexBuilderCollatedData>();
    auto traceDataManager = std::make_shared<IndexBuilderDataManager>();
    collated->traceDataManager = traceDataManager;

    bool cubeDirectionSet = false;
    bool isFirstTrace = true;
    int prevInlineNo = 0;
    int prevCrosslineNo = 0;
    for (Json::Value::ArrayIndex i = 0; i != json.size(); i++) {

        auto ibd = IndexBuilderData();
        uint16_t fNo = fileNo;
        if (json[i].isMember("Offset"))
            ibd.Offset = json[i]["Offset"].asLargestUInt();
        if (json[i].isMember("FileNo"))
            fNo = static_cast<uint16_t>(json[i]["FileNo"].asUInt());
        if (json[i].isMember("InlineNo"))
            ibd.InlineNo = json[i]["InlineNo"].asInt();
        if (json[i].isMember("CrosslineNo"))
            ibd.CrosslineNo = json[i]["CrosslineNo"].asInt();
        if (json[i].isMember("X"))
            ibd.X = json[i]["X"].asDouble();
        if (json[i].isMember("Y"))
            ibd.Y = json[i]["Y"].asDouble();
        if (json[i].isMember("CMP"))
            ibd.CMP = json[i]["CMP"].asInt();
        if (json[i].isMember("TraceNo"))
            ibd.TraceNo = json[i]["TraceNo"].asUInt();
        if (json[i].isMember("TraceLength"))
            ibd.SampleCount = json[i]["TraceLength"].asUInt();

        if (fNo == fileNo) {
            traceDataManager->WriteData(ibd);
        }
        else // New file
        {
            collated->traceDataManager->CloseWriter();
            indexBuilderCollatedDataVector.push_back(collated);

            collated = std::make_shared<IndexBuilderCollatedData>();

            traceDataManager = std::make_shared<IndexBuilderDataManager>();
            collated->traceDataManager = traceDataManager;

            traceDataManager->WriteData(ibd);

            // Ensure collatedVector gets correctly updated
            fileNo = fNo;

            // Ensure Cube Orientation gets calculated for the new SEG-Y file extract ...
            cubeDirectionSet = false;
            isFirstTrace = true;
        }

        double sampleRate = 0;
        uint16_t sampleCount = 0;
        int tofs;

        collated->InlineNoRange.UpdateMinMax(ibd.InlineNo);
        collated->CrosslineNoRange.UpdateMinMax(ibd.CrosslineNo);

        if (json[i].isMember("TraceLength")) {
            sampleCount = static_cast<uint16_t>(json[i]["TraceLength"].asInt());
            collated->TraceLengthRange.UpdateMinMax(sampleCount);
        }
        if (json[i].isMember("TimeFirstSample")) {
            tofs = json[i]["TimeFirstSample"].asInt();
            collated->TofsRange.UpdateMinMax(tofs);
        }
        else {
            tofs = 0;
            collated->TofsRange.UpdateMinMax(tofs);
        }
        if (json[i].isMember("SampleRate")) {
            sampleRate = json[i]["SampleRate"].asDouble();
            if (sampleRate < 1) {
                // We want SampleRate in SEG-Y style value
                sampleRate *= 1000000;
            }
            collated->SampleRateRange.UpdateMinMax(sampleRate);
        }

        if (json[i].isMember("CoordinateScalar")) {
            const int coordinateScalar = json[i]["CoordinateScalar"].asInt();
            collated->CoordinateScalarRange.UpdateMinMax(coordinateScalar);
        }

        if (json[i].isMember("TraceWeightingFactor")) {
            const auto traceWeighting = static_cast<uint16_t>(json[i]["TraceWeightingFactor"].asUInt());
            collated->TraceWeightingFactorRange.UpdateMinMax(traceWeighting);
        }

        if (std::abs(sampleRate) > 0 && sampleCount > 0) {
            const double traceDepth = tofs + (sampleCount - 1) * (sampleRate / 1000);
            collated->TraceDepthRange.UpdateMinMax(traceDepth);
        }

        if (!cubeDirectionSet) {
            // Set initial default to most common value
            collated->InlineIsPrimary = true;

            if (isFirstTrace) {
                prevInlineNo = ibd.InlineNo;
                prevCrosslineNo = ibd.CrosslineNo;
                isFirstTrace = false;
            }
            else {
                if (ibd.InlineNo == prevInlineNo && ibd.CrosslineNo != prevCrosslineNo) {
                    collated->InlineIsPrimary = true;
                    cubeDirectionSet = true;
                }
                if (ibd.InlineNo != prevInlineNo && ibd.CrosslineNo == prevCrosslineNo) {
                    collated->InlineIsPrimary = false;
                    cubeDirectionSet = true;
                }
            }
        }
    }

    collated->traceDataManager->CloseWriter();
    indexBuilderCollatedDataVector.push_back(collated);

    if (indexBuilderCollatedDataVector.size() > 1) {
        const bool segyCubeLayout = indexBuilderCollatedDataVector[0]->InlineIsPrimary;
        for (const auto& collated : indexBuilderCollatedDataVector) {
            if (collated->InlineIsPrimary != segyCubeLayout) {
                throw std::invalid_argument("JsonUnpacker::getIndexBuilderDataFromJson: Inconsistent cube layout - patches are not all sorted the same way!");
            }
        }
    }

    return indexBuilderCollatedDataVector;
}

Expected
JsonUnpacker::getExpectedResultFromJson(Json::Value json, int& errors)
{
    Expected expected_;
    try {
        EnsureWrapper::isTrue(json.isMember("Expected"), errors);

        Json::Value exp = json["Expected"];
        int error = 0;
        debugOutputMembers(exp, error);
        if (error > 0) {
            errors += error;
        }

        error = 0;
        HorizontalExtent inlineExtent = getHorizontalExtentFromJson(exp["InlineExtent"], error);
        if (error > 0)
            errors += error;
        error = 0;
        HorizontalExtent crosslineExtent = getHorizontalExtentFromJson(exp["CrosslineExtent"], error);
        if (error > 0)
            errors += error;
        error = 0;
        Geometry3D geometry = getGeometry3DFromJson(exp["Geometry"], inlineExtent, crosslineExtent, error);
        if (error > 0)
            errors += error;
        FileLayoutSpecification fileLayout = getFileLayoutFromJson(exp["FileLayoutSpecification"], inlineExtent, crosslineExtent, error);
        if (error > 0)
            errors += error;
        error = 0;
        Parameters params = getParametersFromJson(json["Parameters"], error);
        if (error > 0)
            errors += error;

        geometry.sampleExtent = {
            0,
            (params.SegySampleCount - 1) * params.SegySampleInterval,
            params.SegySampleInterval,
            segyio::VerticalDomain::Time,
            segyio::VerticalUOM::seconds};

        fileLayout.sampleExtent = {
            0,
            (params.SegySampleCount - 1) * params.SegySampleInterval,
            params.SegySampleInterval,
            segyio::VerticalDomain::Time,
            segyio::VerticalUOM::seconds};

        expected_.setGeometry(geometry);
        expected_.setFileLayout(fileLayout);
        expected_.setParameters(params);

        return expected_;
    }
    catch (...) {
        errors++;
        return Expected();
    }
}

HorizontalExtent
JsonUnpacker::getHorizontalExtentFromJson(Json::Value json, int& errors)
{

    int error = 0;
    debugOutputMembers(json, error);
    if (error > 0) {
        errors += error;
        error = 0;
    }
    HorizontalExtent extent{
        json["First"].asInt(),
        json["Last"].asInt(),
        json["Step"].asInt()};
    EnsureWrapper::isTrue(extent.first() != extent.last(), errors);
    EnsureWrapper::isTrue(extent.step() > 0, errors);

    return extent;
}

Geometry3D
JsonUnpacker::getGeometry3DFromJson(Json::Value json, HorizontalExtent jVectorExtent, HorizontalExtent iVectorExtent, int& errors)
{
    int error = 0;
    try {
        debugOutputMembers(json, error);
    }
    catch (...) {
        errors++;
    }
    if (error > 0) {
        errors += error;
        error = 0;
    }
    Geometry3D geometry;
    geometry.JVectorExtent = jVectorExtent;
    geometry.IVectorExtent = iVectorExtent;

    // InlineRotationAngle = Direction of a posted inline => iVector
    // InlineUnitInterval = distance between inlines => jVectorUnitDistance
    // CrosslineRotationAngle = Direction of a posted crossline => jVector
    // CrosslineUnitInterval = Distance between crosslines => iVectorUnitDistance

    geometry.dataGrid.IVectorRotationAngle = json["InlineRotationAngle"].asDouble();
    geometry.dataGrid.IVectorUnitDistance = json["CrosslineUnitInterval"].asDouble();

    geometry.dataGrid.JVectorRotationAngle = json["CrosslineRotationAngle"].asDouble();
    geometry.dataGrid.JVectorUnitDistance = json["InlineUnitInterval"].asDouble();

    geometry.dataGrid.gridCell_11.i = jVectorExtent.first();
    geometry.dataGrid.gridCell_11.j = iVectorExtent.first();
    geometry.dataGrid.gridCell_11.location = {json["FirstInlineFirstCrossline"]["X"].asDouble(), json["FirstInlineFirstCrossline"]["Y"].asDouble()};

    geometry.dataGrid.gridCell_1N.i = jVectorExtent.first();
    geometry.dataGrid.gridCell_1N.j = iVectorExtent.last();
    geometry.dataGrid.gridCell_1N.location = {json["FirstInlineLastCrossline"]["X"].asDouble(), json["FirstInlineLastCrossline"]["Y"].asDouble()};

    geometry.dataGrid.gridCell_N1.i = jVectorExtent.last();
    geometry.dataGrid.gridCell_N1.j = iVectorExtent.first();
    geometry.dataGrid.gridCell_N1.location = {json["LastInlineFirstCrossline"]["X"].asDouble(), json["LastInlineFirstCrossline"]["Y"].asDouble()};

    geometry.dataGrid.gridCell_NN.i = jVectorExtent.last();
    geometry.dataGrid.gridCell_NN.j = iVectorExtent.last();
    geometry.dataGrid.gridCell_NN.location = {json["LastInlineLastCrossline"]["X"].asDouble(), json["LastInlineLastCrossline"]["Y"].asDouble()};

    return geometry;
}

FileLayoutSpecification
JsonUnpacker::getFileLayoutFromJson(Json::Value json, HorizontalExtent jVectorExtent, HorizontalExtent iVectorExtent, int& errors)
{
    int error = 0;
    try {
        debugOutputMembers(json, error);
    }
    catch (...) {
        errors++;
    }
    if (error > 0) {
        errors += error;
        error = 0;
    }

    FileLayoutSpecification result;
    result.JExtent = jVectorExtent;
    result.IExtent = iVectorExtent;

    // InlineRotationAngle = Direction of a posted inline => iVector
    // InlineUnitInterval = distance between inlines => jVectorUnitDistance
    // CrosslineRotationAngle = Direction of a posted crossline => jVector
    // CrosslineUnitInterval = Distance between crosslines => iVectorUnitDistance

    return result;
}

Parameters
JsonUnpacker::getParametersFromJson(Json::Value json, int& errors)
{

    int error = 0;
    debugOutputMembers(json, error);
    if (error > 0)
        errors += error;

    Parameters params;
    params.SegyInitialHeaderOffset = json["SegyInitialHeaderOffset"].asInt();
    params.SegySampleCount = json["SegySampleCount"].asInt();
    params.SegySampleSize = json["SegySampleSize"].asInt();
    params.CoordinateScalar = json["CoordinateScalar"].asInt();
    params.TraceWeightingFactor = json["TraceWeightingFactor"].asInt();

    params.SegySampleInterval = json["SegySampleInterval"].asDouble();
    params.InlineDistanceSlack = json["InlineDistanceSlack"].asDouble();
    params.CrosslineDistanceSlack = json["CrosslineDistanceSlack"].asDouble();

    params.HasVariableTraceSize = json["HasVariableTraceSize"].asBool();
    params.HasVariableCoordinateScalar = json["HasVariableCoordinateScalar"].asBool();
    params.HasVariableTraceWeightingFactor = json["HasVariableTraceWeightingFactor"].asBool();

    params.SkipFullInlines = json["SkipFullInlines"].asBool();
    params.SkipFullCrosslines = json["SkipFullCrosslines"].asBool();
    params.SkipCorner = json["SkipCorner"].asBool();
    params.SkipAreaInside = json["SkipAreaInside"].asBool();

    params.CubeIsInlineSorted = json["CubeIsInlineSorted"].asBool();
    params.InlineSortingIncreasing = json["InlineSortingIncreasing"].asBool();
    params.CrosslineSortingIncreasing = json["CrosslineSortingIncreasing"].asBool();
    const std::string tmpName = json["OutputFileName"].asString();
    const auto pos = tmpName.find_last_of('\\');
    if (pos != std::string::npos) {
        params.FileName = tmpName.substr(pos + 1);
    }
    return params;
}

void
JsonUnpacker::debugOutputMembers(const Json::Value& json, int& errors, bool generateDebugOutput)
{
    if (!generateDebugOutput || !json.empty())
        return;

    const auto members = json.getMemberNames();

    EnsureWrapper::isTrue(!members.empty(), errors);

    for (const auto& member : members) {
        std::cout << member << std::endl;
    }
    std::cout << std::endl;
}

} // namespace segysdk::segyindex::tests
