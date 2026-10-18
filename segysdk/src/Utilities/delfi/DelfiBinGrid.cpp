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

#include "DelfiBinGrid.h"

#include "../Utilities/JsonBinGrid.h"

#include <json/json.h>

namespace segysdk::utilities::delfi
{

BinGrid
DelfiBinGrid::loadBinGridFromJson(const Json::Value& root)
{
    // example: https://slb-swt.visualstudio.com/data-management/_git/wke-schema?path=%2Fdomains%2Fgeophysics%2Fjson_data%2Fslb_wke_binGrid.data.json

    auto const& bin_grid_node = root["data"]["binGridOperation"];

    BinGrid bin_grid;
    bin_grid.binGridOriginEasting = bin_grid_node["binGridOriginEasting"].asDouble();
    bin_grid.binGridOriginI = bin_grid_node["binGridOriginI"].asDouble();
    bin_grid.binGridOriginJ = bin_grid_node["binGridOriginJ"].asDouble();
    bin_grid.binGridOriginNorthing = bin_grid_node["binGridOriginNorthing"].asDouble();
    bin_grid.binGridScaleFactor = bin_grid_node["binGridScaleFactor"].asDouble();
    bin_grid.binNodeIncrementI = bin_grid_node["binNodeIncrementI"].asDouble();
    bin_grid.binNodeIncrementJ = bin_grid_node["binNodeIncrementJ"].asDouble();
    bin_grid.binWidthI = bin_grid_node["binWidthI"].asDouble();
    bin_grid.binWidthJ = bin_grid_node["binWidthJ"].asDouble();
    bin_grid.mapGridBearingJ = bin_grid_node["mapGridBearingJ"].asDouble();
    bin_grid.transformationMethod = JsonBinGrid::parseTransformationMethodCode(bin_grid_node["transformationMethodNumber"]);
    bin_grid.persistableReferenceCrs = bin_grid_node["persistableReferenceCrs"].asString();
    return bin_grid;
}

} // namespace segysdk::utilities::delfi
