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
#include "../LibraryLoader/DynamicLibraryLoader.h"
#include "../SegyIndex/SegyIndex.h"
#include "../Utilities/BinGrid.h"
#include <openzgy/api.h>

#include "../segysdk_details/DLL_EXPORT.h"

#include <memory>

namespace segysdk::compute_polygon
{

class SEGYSDK_DLL_PUBLIC ComputePolygonPlugin
{
public:

    explicit ComputePolygonPlugin(std::unique_ptr<segysdk::library_loader::DynamicLibraryLoader> loader);

    ~ComputePolygonPlugin();

    Json::Value computeForSegy(std::shared_ptr<segysdk::segyindex::SegyIndex> index, std::shared_ptr<segysdk::utilities::BinGrid> grid, bool& isMultiple);

    Json::Value computeForZgy(std::shared_ptr<OpenZGY::IZgyReader> reader, bool isTimeSeismic, std::shared_ptr<segysdk::utilities::BinGrid> grid, bool& isMultiple);

private:
    bool InitFunctionPointers();

private:
    typedef ComputePolygonInterface* (*ComputePolygonSegy_t)(segysdk::segyindex::SegyIndex* index, segysdk::utilities::BinGrid* grid);
    typedef ComputePolygonInterface* (*ComputePolygonZgy_t)(OpenZGY::IZgyReader* reader, bool isTimeSeismic, segysdk::utilities::BinGrid* grid);
    typedef void (*DestroyPolygon_t) (ComputePolygonInterface*);

    static constexpr char const* const functionComputePolygonSegy_ = "ComputePolygonSegy";
    static constexpr char const* const functionComputePolygonZgy_ = "ComputePolygonZgy";
    static constexpr char const* const functionDestroyPolygon_ = "DestroyPolygon";

    std::unique_ptr<segysdk::library_loader::DynamicLibraryLoader> libraryLoader_;

    ComputePolygonSegy_t ComputePolySegyFn_ = nullptr;
    ComputePolygonZgy_t ComputePolyZgyFn_ = nullptr;
    DestroyPolygon_t DestroyPolyFn_ = nullptr;

    std::unique_ptr<ComputePolygonInterface, decltype(DestroyPolyFn_)> pluginInstance_;
};

} // namespace segysdk::compute_polygon

