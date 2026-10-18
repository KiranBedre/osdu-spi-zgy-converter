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

#include "../ComputePolygonPlugin/ComputePolygonPlugin.h"
#include "../ComputePolygonPlugin/ComputePolygonPluginException.h"
#include "../Logger/Logger.h"

namespace segysdk::compute_polygon
{

ComputePolygonPlugin::ComputePolygonPlugin(std::unique_ptr<segysdk::library_loader::DynamicLibraryLoader> loader)
  : libraryLoader_(std::move(loader)), pluginInstance_(nullptr, [](auto p) { (void)p; })
{
    if (not InitFunctionPointers()) {
        throw ComputePolygonPluginException("Couldn't initialize function pointers for Compute polygon plugin");
    }
}

ComputePolygonPlugin::~ComputePolygonPlugin()
{
}

Json::Value
ComputePolygonPlugin::computeForSegy(std::shared_ptr<segysdk::segyindex::SegyIndex> index, std::shared_ptr<segysdk::utilities::BinGrid> grid, bool& isMultiple)
{
    SEGYLOG_INFO("Start of Polygon Computation");
    if (index->hasMissingTraceIndex() and !index->IsLiveTracesIndexLoaded()) {
        index->loadMissingTraceIndex(segysdk::request::Params());
    }

    auto pInterface = ComputePolySegyFn_(index.get(), grid.get());
    if (pInterface == nullptr) {
        // @TODO: better to update for more error messages from library
        throw ComputePolygonPluginException("Could not initialize Compute Polygon instance");
    }
    pluginInstance_ = std::unique_ptr<ComputePolygonInterface, decltype(DestroyPolyFn_)>(pInterface, DestroyPolyFn_);
    SEGYLOG_INFO("End of Polygon Computation");
    return pluginInstance_->getJsonPolygons(isMultiple);
}

Json::Value
ComputePolygonPlugin::computeForZgy(std::shared_ptr<OpenZGY::IZgyReader> reader, bool isTimeSeismic, std::shared_ptr<segysdk::utilities::BinGrid> grid, bool& isMultiple)
{
    SEGYLOG_INFO("Start of Polygon Computation"); 
    auto pInterface = ComputePolyZgyFn_(reader.get(), isTimeSeismic, grid.get());
    if (pInterface == nullptr) {
        // @TODO: better to update for more error messages from library
        throw ComputePolygonPluginException("Could not initialize Compute Polygon instance");
    }
    pluginInstance_ = std::unique_ptr<ComputePolygonInterface, decltype(DestroyPolyFn_)>(pInterface, DestroyPolyFn_);
    SEGYLOG_INFO("End of Polygon Computation");
    return pluginInstance_->getJsonPolygons(isMultiple);
}

bool
ComputePolygonPlugin::InitFunctionPointers()
{
    ComputePolySegyFn_ = reinterpret_cast<ComputePolygonSegy_t>(libraryLoader_->getFunction(functionComputePolygonSegy_));
    ComputePolyZgyFn_ = reinterpret_cast<ComputePolygonZgy_t>(libraryLoader_->getFunction(functionComputePolygonZgy_));
    DestroyPolyFn_ = reinterpret_cast<DestroyPolygon_t>(libraryLoader_->getFunction(functionDestroyPolygon_));

    return ComputePolySegyFn_ != nullptr
           and ComputePolyZgyFn_ != nullptr
           and DestroyPolyFn_ != nullptr;
}

} // namespace segysdk::compute_polygon
