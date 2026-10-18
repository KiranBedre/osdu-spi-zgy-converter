/*
Copyright 2024 Schlumberger

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

#include "../BinGridCalculationPlugin/BinGridCalculationPlugin.h"
#include "../BinGridCalculationPlugin/BinGridCalculationPluginException.h"
#include "../Logger/Logger.h"

namespace segysdk::bin_grid_calculation
{

BinGridCalculationPlugin::BinGridCalculationPlugin(std::unique_ptr<segysdk::library_loader::DynamicLibraryLoader> loader)
  : libraryLoader_(std::move(loader)), pluginInstance_(nullptr, [](auto p) { (void)p; })
{
    if (not initFunctionPointers()) {
        throw BinGridCalculationPluginException("Couldn't initialize function pointers for Bin Grid calculation plugin");
    }
}

std::string
BinGridCalculationPlugin::getBinGridAsFormattedJson(std::unique_ptr<BinGridPluginParams> params)
{
    SEGYLOG_INFO("Start of Bin Grid calculation Plugin... ");
    try {
        auto binGridCalculator = getBinGridCalculatorFn_(params.get());
        if (binGridCalculator == nullptr) {
            throw BinGridCalculationPluginException("Couldn't initialize interface pointer for Bin Grid calculation plugin");
        }
        pluginInstance_ = std::unique_ptr<BinGridCalculationInterface, decltype(destroyBinGridCalculationFn_)>(binGridCalculator, destroyBinGridCalculationFn_);
        return pluginInstance_->getBinGridAsFormattedJson();
    }
    catch (...) {
        throw;
    }
}

bool
BinGridCalculationPlugin::initFunctionPointers()
{
    getBinGridCalculatorFn_ = reinterpret_cast<GetBinGridCalculator_t>(libraryLoader_->getFunction(functionGetBinGridCalculator_));

    destroyBinGridCalculationFn_ = reinterpret_cast<DestroyBinGridCalculator_t>(libraryLoader_->getFunction(functionDestroyBinGridCalculator_));

    return getBinGridCalculatorFn_ != nullptr
           and destroyBinGridCalculationFn_ != nullptr;
}

} // namespace segysdk::bin_grid_calculation
