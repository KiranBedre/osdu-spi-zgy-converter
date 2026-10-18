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

#include "OsduSeismicBinGrid100.h"

#include "../Utilities/BinGrid.h"
#include "../Utilities/JsonBinGrid.h"

#include <json/json.h>

namespace segysdk::utilities::osdu
{

BinGrid
OsduSeismicBinGrid100::loadBinGridFromJson(const Json::Value& root)
{
    Json::Value bin_grid_node = root["data"];

    // docs:
    // - https://community.opengroup.org/osdu/data/data-definitions/-/blob/master/E-R/abstract/AbstractBinGrid.1.0.0.md
    // - https://community.opengroup.org/osdu/data/data-definitions/-/blob/master/E-R/reference-data/SeismicBinGridType.1.0.0.md
    // example: https://community.opengroup.org/osdu/data/data-definitions/-/blob/master/Examples/work-product-component/SeismicBinGrid.1.0.0.json

    BinGrid bin_grid;
    bin_grid.binGridOriginEasting = bin_grid_node["P6BinGridOriginEasting"].asDouble();
    bin_grid.binGridOriginI = bin_grid_node["P6BinGridOriginI"].asDouble();
    bin_grid.binGridOriginJ = bin_grid_node["P6BinGridOriginJ"].asDouble();
    bin_grid.binGridOriginNorthing = bin_grid_node["P6BinGridOriginNorthing"].asDouble();
    bin_grid.binGridScaleFactor = bin_grid_node["P6ScaleFactorOfBinGrid"].asDouble();
    bin_grid.binNodeIncrementI = bin_grid_node["P6BinNodeIncrementOnIaxis"].asDouble();
    bin_grid.binNodeIncrementJ = bin_grid_node["P6BinNodeIncrementOnJaxis"].asDouble();
    bin_grid.binWidthI = bin_grid_node["P6BinWidthOnIaxis"].asDouble();
    bin_grid.binWidthJ = bin_grid_node["P6BinWidthOnJaxis"].asDouble();
    bin_grid.mapGridBearingJ = bin_grid_node["P6MapGridBearingOfBinGridJaxis"].asDouble();
    bin_grid.transformationMethod = JsonBinGrid::parseTransformationMethodCode(bin_grid_node["P6TransformationMethod"]);
    bin_grid.coordinateReferenceSystemId = bin_grid_node["SpatialArea"]["AsIngestedCoordinates"]["CoordinateReferenceSystemID"].asString();
    bin_grid.persistableReferenceCrs = bin_grid_node["SpatialArea"]["AsIngestedCoordinates"]["persistableReferenceCrs"].asString();
    return bin_grid;
}

} // namespace segysdk::utilities::osdu

