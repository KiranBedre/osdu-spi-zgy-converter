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

#include "CompleteGridHandler.h"

#include "GridCalculator.h"
#include "StatsGenerator.h"
#include "../SegyIO/SegyIOException.h"

#include <algorithm>
#include <stdexcept>

namespace segysdk::segyindex
{

CompleteGridHandler::CompleteGridHandler(const std::vector<std::shared_ptr<IndexBuilderCollatedData>>& indexCollatedData, const GeometryCalculationData& calcData)
  : calcData_(calcData)
{
    if (indexCollatedData.empty())
        throw segyio::SegyFormatException("CompleteGridHandler: indexCollatedData is empty!");

    updateDataVectors(indexCollatedData);
}

void
CompleteGridHandler::updateDataVectors(const std::vector<std::shared_ptr<IndexBuilderCollatedData>>& collatedIndexDataVector)
{
    std::map<std::pair<int, int>, std::pair<double, double>> myMap;

    for (const std::shared_ptr<IndexBuilderCollatedData>& collated : collatedIndexDataVector) {
        auto traceDataManager = collated->traceDataManager;
        if (traceDataManager == nullptr || !traceDataManager->IsReaderReady())
            continue;

        if (traceDataManager->IsReaderReady()) {
            while (!traceDataManager->AtReaderEnd()) {
                auto traceData = traceDataManager->ReadNext();
                populateInlineTraceMapDataFromIndex(traceData);
                populateCrosslineTraceMapDataFromIndex(traceData);
            }
            traceDataManager->CloseReader();
        }
    }

    // Make sure the maps are properly ordered
    for (auto& [inlineNo, inlineVector] : InlineTraceMap_) {
        std::sort(inlineVector.begin(), inlineVector.end(),
                  [](GridBuilderData const& a, GridBuilderData const& b) -> bool { return a.Secondary < b.Secondary; });
    }
    for (auto& [crosslineNo, crosslineVector] : CrosslineTraceMap_) {
        std::sort(crosslineVector.begin(), crosslineVector.end(),
                  [](GridBuilderData const& a, GridBuilderData const& b) -> bool { return a.Secondary < b.Secondary; });
    }
}

DataGrid
CompleteGridHandler::GenerateRegularDataGrid(const Geometry3D& geometry3d)
{
    // Initial basic version - probably need to use statistics to work out best fit grid for all data points
    DataGrid dataGrid;
    std::pair<std::pair<int, int>, std::pair<double, double>>
        tracedata = getTraceFromIndexData(geometry3d.JVectorExtent.first(), geometry3d.IVectorExtent.first());
    if (isValidTraceData(tracedata)) {
        dataGrid.gridCell_11 = populateGridCell(tracedata);
    }
    tracedata = getTraceFromIndexData(geometry3d.JVectorExtent.first(), geometry3d.IVectorExtent.last());
    if (isValidTraceData(tracedata)) {
        dataGrid.gridCell_1N = populateGridCell(tracedata);
    }
    tracedata = getTraceFromIndexData(geometry3d.JVectorExtent.last(), geometry3d.IVectorExtent.first());
    if (isValidTraceData(tracedata)) {
        dataGrid.gridCell_N1 = populateGridCell(tracedata);
    }
    tracedata = getTraceFromIndexData(geometry3d.JVectorExtent.last(), geometry3d.IVectorExtent.last());
    if (isValidTraceData(tracedata)) {
        dataGrid.gridCell_NN = populateGridCell(tracedata);
    }
    // Inline vector is vector describing a plotted inline, i.e. all cdp for same inline
    const Vector iVector = Vector::GenerateIVector(dataGrid.gridCell_11, dataGrid.gridCell_1N);
    // Calculate distance between crosslines
    dataGrid.IVectorUnitDistance = iVector.getUnitDistance();
    // Crossline vector is vector describing a plotted crossline, i.e. all inlines for same cdp
    const Vector jVector = Vector::GenerateJVector(dataGrid.gridCell_11, dataGrid.gridCell_N1);
    // Calculate distance between inlines
    dataGrid.JVectorUnitDistance = jVector.getUnitDistance();
    dataGrid.IVectorRotationAngle = iVector.getDegrees();
    dataGrid.JVectorRotationAngle = jVector.getDegrees();
    return dataGrid;
}

std::pair<std::pair<int, int>, std::pair<double, double>>
CompleteGridHandler::getTraceFromIndexData(const int inlineNo, const int crosslineNo)
{
    if (const auto iterator = InlineTraceMap_.find(inlineNo); iterator != InlineTraceMap_.end()) {
        if (!iterator->second.empty()) {
            if (iterator->second[0].Secondary == crosslineNo) {
                return std::make_pair(std::make_pair(inlineNo, crosslineNo), std::make_pair(iterator->second[0].X, iterator->second[0].Y));
            }
            if (iterator->second.size() > 1 && iterator->second[1].Secondary == crosslineNo) {
                return std::make_pair(std::make_pair(inlineNo, crosslineNo), std::make_pair(iterator->second[1].X, iterator->second[1].Y));
            }
            if (calcData_.IsRegular()) {}
            throw segyio::SegyFormatException("getTraceFromIndexData: Regular grid - Requested crossline not found - Requested trace out of range!");
        }
        throw segyio::SegyFormatException("getTraceFromIndexData: Irregular grid: Requested crossline not found.");
    }
    if (calcData_.IsRegular()) {
        throw segyio::SegyFormatException("getTraceFromIndexData: Regular grid - Requested inline not found - Requested trace out of range!");
    }
    throw segyio::SegyFormatException("getTraceFromIndexData: Irregular grid: Requested inline not found.");
}

DataGrid
CompleteGridHandler::GenerateIrregularDataGrid(const Geometry3D& geometry3d) const
{
    DataGrid dataGrid;
    /*
    Several possible strategies that might be used here:
    The best one to use (safest) depends on whether or not we can safely assume that the trace coordinates are
    good (i.e. calculated) or not.
    For calculated surveys, We can expect grid cells to be exactly the same size, and all cells are correctly aligned to the same vectors,
    and the vectors are orthogonal.
    For actual navigation data, this kind of assumption might not be valid, so we might have to perform multiple calculations to determine the best values to use.

    For calculated surveys any to traces along same inline or crossline should give us the desirec result.
    For non-processed navigation, we probably have to use statistics to work out reasonable values based on std deviation and median or mean values.

    As we cannot be sure the trace coordinates are consistent, the best first option is to do it using statistics.
    */

    // Inline vector is a vector describing a plotted inline, i.e. all cdp for same inline
    Vector iVector = getIVector(geometry3d);
    dataGrid.IVectorUnitDistance = iVector.getUnitDistance();
    // Crossline vector is a vector describing a plotted inline, i.e. all inlines for same cdp
    Vector crosslineVector = getJVector(geometry3d);
    dataGrid.JVectorUnitDistance = crosslineVector.getUnitDistance();
    dataGrid.IVectorRotationAngle = iVector.getDegrees();
    dataGrid.JVectorRotationAngle = crosslineVector.getDegrees();
    GridCalculator gridCalc(iVector, crosslineVector);

    dataGrid.gridCell_11 = populateGridCell(
        geometry3d.JVectorExtent.first(),
        geometry3d.IVectorExtent.first(),
        gridCalc.GetXYLocation(geometry3d.JVectorExtent.first(), geometry3d.IVectorExtent.first()));

    dataGrid.gridCell_1N = populateGridCell(
        geometry3d.JVectorExtent.first(),
        geometry3d.IVectorExtent.last(),
        gridCalc.GetXYLocation(geometry3d.JVectorExtent.first(), geometry3d.IVectorExtent.last()));

    dataGrid.gridCell_N1 = populateGridCell(
        geometry3d.JVectorExtent.last(),
        geometry3d.IVectorExtent.first(),
        gridCalc.GetXYLocation(geometry3d.JVectorExtent.last(), geometry3d.IVectorExtent.first()));

    dataGrid.gridCell_NN = populateGridCell(
        geometry3d.JVectorExtent.last(),
        geometry3d.IVectorExtent.last(),
        gridCalc.GetXYLocation(geometry3d.JVectorExtent.last(), geometry3d.IVectorExtent.last()));
    return dataGrid;
}

Vector
CompleteGridHandler::getIVector(const Geometry3D& geometry3D) const
{
    // Primary is Inline
    return getBestFitVector(InlineTraceMap_, geometry3D.IVectorExtent, DirectionType::Inlines);
}

Vector
CompleteGridHandler::getJVector(const Geometry3D& geometry3D) const
{
    // Primary is Crossline
    return getBestFitVector(CrosslineTraceMap_, geometry3D.JVectorExtent, DirectionType::Crosslines);
}

Vector
CompleteGridHandler::getBestFitVector(const std::map<int, std::vector<GridBuilderData>>& vectorData, HorizontalExtent extent, DirectionType primaryDirection) const
{
    std::vector<std::pair<int, Vector>> vectorArray;

    for (auto& [lineNo, lineVector] : vectorData) {
        if (lineVector.size() < 2)
            continue;
        if (lineVector.size() > 2)
            throw segyio::SegyFormatException("GridBuilderData vector contains too many elements!");

        GridBuilderData start = lineVector[0];
        GridBuilderData end = lineVector[1];

        GridCell c1 = primaryDirection == DirectionType::Inlines
                          ? populateGridCell(std::make_pair(std::make_pair(lineNo, start.Secondary), std::make_pair(start.X, start.Y)))
                          : populateGridCell(std::make_pair(std::make_pair(start.Secondary, lineNo), std::make_pair(start.X, start.Y)));
        GridCell c2 = primaryDirection == DirectionType::Inlines
                          ? populateGridCell(std::make_pair(std::make_pair(lineNo, end.Secondary), std::make_pair(end.X, end.Y)))
                          : populateGridCell(std::make_pair(std::make_pair(end.Secondary, lineNo), std::make_pair(end.X, end.Y)));

        // Skip if we haven't got proper indexing data

        if ((primaryDirection == DirectionType::Inlines && c1.j == c2.j) || (primaryDirection == DirectionType::Crosslines && c1.i == c2.i))
            continue;

        HorizontalExtent ext{start.Secondary, end.Secondary, extent.step()};

        Vector v = c1.i == c2.i
                       ? Vector::GenerateIVector(c1, c2)
                       : Vector::GenerateJVector(c1, c2);

        vectorArray.emplace_back(lineNo, v);
    }

    if (vectorArray.empty()) {
        std::string direction = primaryDirection == DirectionType::Inlines ? "J vector" : "I vector";
        throw segyio::SegyFormatException("GridBuilderData: Could not generate useable " + direction + " from generated trace data!");
    }
    StatsGenerator sg(vectorArray);

    return sg.getVector();
}

/*
    Generate a trace map which can be used to easily see where inline sections are complete or whether trace(s) are missing
  */
void
CompleteGridHandler::populateInlineTraceMapDataFromIndex(IndexBuilderData& ibd)
{

    // ibd.InlineNo is our main key
    const auto resIterator = InlineTraceMap_.find(ibd.InlineNo); // Have we got inline in our result?

    if (resIterator != InlineTraceMap_.end()) {
        // Yes - update it if required
        if (resIterator->second.size() == 1) {
            // Only one item, so add new first or last entry
            if (ibd.CrosslineNo < resIterator->second[0].Secondary) {
                const GridBuilderData gbd(ibd.CrosslineNo, ibd.X, ibd.Y);
                resIterator->second.insert(resIterator->second.begin(), gbd);
            }
            else if (ibd.CrosslineNo > resIterator->second[0].Secondary) {
                const GridBuilderData gbd(ibd.CrosslineNo, ibd.X, ibd.Y);
                resIterator->second.push_back(gbd);
            }
        }
        else if (resIterator->second.size() == 2) {
            // Two items already inserted, so update first or second item only if required
            if (ibd.CrosslineNo < resIterator->second[0].Secondary) {
                const GridBuilderData gbd(ibd.CrosslineNo, ibd.X, ibd.Y);
                resIterator->second[0] = gbd;
            }
            else if (ibd.CrosslineNo > resIterator->second[1].Secondary) {
                const GridBuilderData gbd(ibd.CrosslineNo, ibd.X, ibd.Y);
                resIterator->second[1] = gbd;
            }
        }
    }
    else {
        std::vector<GridBuilderData> gbdVector;
        const GridBuilderData gbd(ibd.CrosslineNo, ibd.X, ibd.Y);
        gbdVector.push_back(gbd);
        InlineTraceMap_.try_emplace(ibd.InlineNo, gbdVector);
    }
}

void
CompleteGridHandler::populateCrosslineTraceMapDataFromIndex(IndexBuilderData& ibd)
{
    // ibd.CrosslineNo is our main key

    const auto resIterator = CrosslineTraceMap_.find(ibd.CrosslineNo); // Have we got crossline in our result?

    if (resIterator != CrosslineTraceMap_.end()) {
        // Yes - update it if required
        if (resIterator->second.size() == 1) {
            // Only one item, so add new first or last entry
            if (ibd.InlineNo < resIterator->second[0].Secondary) {
                const GridBuilderData gbd(ibd.InlineNo, ibd.X, ibd.Y);
                resIterator->second.insert(resIterator->second.begin(), gbd); // Insert new first record
            }
            else if (ibd.InlineNo > resIterator->second[0].Secondary) {
                const GridBuilderData gbd(ibd.InlineNo, ibd.X, ibd.Y);
                resIterator->second.push_back(gbd); // Add a second record to the end
            }
        }
        else if (resIterator->second.size() == 2) {
            // Two items already inserted, so update first or second item only if required
            if (ibd.InlineNo < resIterator->second[0].Secondary) {
                // Update first record
                const GridBuilderData gbd(ibd.InlineNo, ibd.X, ibd.Y);
                resIterator->second[0] = gbd;
            }
            else if (ibd.InlineNo > resIterator->second[1].Secondary) {
                // Update second record
                const GridBuilderData gbd(ibd.InlineNo, ibd.X, ibd.Y);
                resIterator->second[1] = gbd;
            }
        }
    }
    else {
        std::vector<GridBuilderData> gbdVector;
        const GridBuilderData gbd(ibd.InlineNo, ibd.X, ibd.Y);
        gbdVector.push_back(gbd);

        CrosslineTraceMap_.try_emplace(ibd.CrosslineNo, gbdVector);
    }
}

int
CompleteGridHandler::getBestSection(const std::map<int, std::vector<std::shared_ptr<IndexBuilderData>>>& map)
{
    size_t maxTraces = 0;
    size_t useKey = 0;
    for (auto& [inlineNo, inlineVector] : map) {
        if (inlineVector.size() > maxTraces) {
            maxTraces = static_cast<int>(inlineVector.size());
            useKey = inlineNo;
        }
    }

    return useKey;
}

bool
CompleteGridHandler::isValidTrace(int traceNo, Range<int> valueRange, Range<int> stepRange)
{

    if (traceNo < valueRange.Min || traceNo > valueRange.Max)
        return false;

    const int result = traceNo - valueRange.Min;

    return result % stepRange.Min == 0;
}

GridCell
CompleteGridHandler::populateGridCell(int i, int j, Coordinate location)
{

    GridCell corner;

    corner.i = i;
    corner.j = j;
    corner.location = location;

    return corner;
}

GridCell
CompleteGridHandler::populateGridCell(std::pair<std::pair<int, int>, std::pair<double, double>> traceData)
{

    GridCell corner;

    corner.i = traceData.first.first;
    corner.j = traceData.first.second;
    corner.location = {traceData.second.first, traceData.second.second};

    return corner;
}

bool
CompleteGridHandler::isValidTraceData(std::pair<std::pair<int, int>, std::pair<double, double>>& traceData)
{
    return traceData.first.first != invalidIndex && traceData.first.second != invalidIndex && traceData.second.first != invalidCoordinate && traceData.second.second != invalidCoordinate;
}

} // namespace segysdk::segyindex
