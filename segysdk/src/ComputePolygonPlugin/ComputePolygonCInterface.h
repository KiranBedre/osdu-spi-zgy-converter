/*
Copyright 2023 Schlumberger

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

#include "../ComputePolygonPlugin/ComputePolygonInterface.h"
#include "../SegyIndex/SegyIndex.h"
#include "../Utilities/BinGrid.h"
#include <openzgy/api.h>

#include "../segysdk_details/DLL_EXPORT.h"

#include <stdint.h>

using namespace segysdk::compute_polygon;
using namespace segysdk::segyindex;
using namespace segysdk::utilities;
using namespace OpenZGY;

/**
 * @brief This is the interface to Compute Polygon Shared Library
 *
 **/

extern "C" {

/**
 * @brief Creates instance of ComputePolygonInterface and computes Polygons for SEGY file based on it's index.
 *
 * @param [IN] pointer to the SegyIndex object
 * @param [IN] pointer to OSDU Bin grid object
 *
 * @return a pointer to the just created and initialized ComputePolygonInteraface object instance on success, NULL otherwise
 *
 **/
SEGYSDK_DLL_PUBLIC ComputePolygonInterface* ComputePolygonSegy(SegyIndex* index, BinGrid* grid);

/**
 * @brief Creates instance of ComputePolygonInterface and computes Polygons for ZGY file using ZGY reader object.
 *
 * @param [IN] pointer to the IZgyReader object
 * 
 * @param [IN] isTimeSeismic  bool if depth seismic = false, else true
 *
 * @param [IN] pointer to OSDU Bin grid object
 *
 * @return a pointer to the just created and initialized ComputePolygonInteraface object instance on success, NULL otherwise
 *
 **/
SEGYSDK_DLL_PUBLIC ComputePolygonInterface* ComputePolygonZgy(IZgyReader* reader, bool isTimeSeismic, segysdk::utilities::BinGrid* grid);

/**
 * @brief Deletes instance of ComputePolygonInterface.
 *
 * @param [IN] pointer to the ComputePolygonInteraface object
 *
 **/
SEGYSDK_DLL_PUBLIC void DestroyPolygon(ComputePolygonInterface* instance);

} /* extern "C" */
