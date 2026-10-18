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

#pragma once

#include "DataGrid.h"
#include "Geometry3D.h"
#include "GeometryCalculationData.h"
#include "GridBuilderData.h"
#include "Range.h"
#include "Vector.h"

#include <map>
#include <memory>
#include <vector>

namespace segysdk::segyindex
{

enum class DirectionType
{
    Inlines,
    Crosslines
};

class CompleteGridHandler
{

private:
    std::map<int, std::vector<GridBuilderData>> InlineTraceMap_;
    std::map<int, std::vector<GridBuilderData>> CrosslineTraceMap_;

    GeometryCalculationData calcData_;

public:
    CompleteGridHandler(const std::vector<std::shared_ptr<IndexBuilderCollatedData>>& indexCollatedData, const GeometryCalculationData& calcData);

private:
    void updateDataVectors(const std::vector<std::shared_ptr<IndexBuilderCollatedData>>& collatedIndexDataVector);

    std::pair<std::pair<int, int>, std::pair<double, double>> getTraceFromIndexData(const int inlineNo, const int crosslineNo);

    void populateInlineTraceMapDataFromIndex(IndexBuilderData& ibd);
    void populateCrosslineTraceMapDataFromIndex(IndexBuilderData& ibd);

    static GridCell populateGridCell(std::pair<std::pair<int, int>, std::pair<double, double>> traceData);
    static GridCell populateGridCell(int i, int j, Coordinate location);

    static bool isValidTraceData(std::pair<std::pair<int, int>, std::pair<double, double>>& traceData);

    static bool isValidTrace(int traceNo, Range<int> valueRange, Range<int> stepRange);
    static int getBestSection(const std::map<int, std::vector<std::shared_ptr<IndexBuilderData>>>& map);

    Vector getBestFitVector(const std::map<int, std::vector<GridBuilderData>>& vectorData, HorizontalExtent extent, DirectionType primaryDirection) const;
    Vector getIVector(const Geometry3D& geometry3D) const;
    Vector getJVector(const Geometry3D& geometry3D) const;

public:
    static inline const int invalidIndex = -999999;
    static inline const double invalidCoordinate = -99999999;

    DataGrid GenerateRegularDataGrid(const Geometry3D& geometry3d);
    DataGrid GenerateIrregularDataGrid(const Geometry3D& geometry3d) const;
};

} // namespace segysdk::segyindex
