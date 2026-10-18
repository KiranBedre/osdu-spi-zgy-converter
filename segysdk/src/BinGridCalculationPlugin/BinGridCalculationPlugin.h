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

#pragma once

#include "../BinGridCalculationPlugin/BinGridCalculationInterface.h"

#include "../LibraryLoader/DynamicLibraryLoader.h"
#include "../BinGridCalculationPlugin/BinGridPluginParams.h"

#include "../segysdk_details/DLL_EXPORT.h"

#include <memory>
#include <vector>

namespace segysdk::bin_grid_calculation
{

class SEGYSDK_DLL_PUBLIC BinGridCalculationPlugin
{
public:

    explicit BinGridCalculationPlugin(std::unique_ptr<segysdk::library_loader::DynamicLibraryLoader> loader);
    ~BinGridCalculationPlugin() = default;

    std::string getBinGridAsFormattedJson(std::unique_ptr<BinGridPluginParams> params);   

private:
    bool initFunctionPointers();

private:
    typedef BinGridCalculationInterface* (*GetBinGridCalculator_t)(BinGridPluginParams* params);
    typedef void (*DestroyBinGridCalculator_t)(BinGridCalculationInterface*);

    static constexpr char const* const functionGetBinGridCalculator_ = "GetBinGridCalculator";
    static constexpr char const* const functionDestroyBinGridCalculator_ = "DestroyBinGridCalculator";

    std::unique_ptr<segysdk::library_loader::DynamicLibraryLoader> libraryLoader_;

    GetBinGridCalculator_t getBinGridCalculatorFn_ = nullptr;
    DestroyBinGridCalculator_t destroyBinGridCalculationFn_ = nullptr;

    std::unique_ptr<BinGridCalculationInterface, decltype(destroyBinGridCalculationFn_)> pluginInstance_;
};

} // namespace segysdk::navigation_extractor

