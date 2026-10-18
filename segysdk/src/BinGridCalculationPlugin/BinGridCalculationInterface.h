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

#include "../segysdk_details/DLL_EXPORT.h"

#include <memory>
#include <string>

namespace segysdk::bin_grid_calculation
{

/**
 * @class BinGridCalculationInterface
 *
 * @brief The Bin Grid Calculation Interface
 *
 * Interface to be used by BinGridCalculationPlugin. Intended to provide interface of calculation of a Bin Grid.
 *
 */
class SEGYSDK_DLL_PUBLIC BinGridCalculationInterface
{
public:
    BinGridCalculationInterface() = default;

    virtual ~BinGridCalculationInterface() = default;

    virtual std::string getBinGridAsFormattedJson() = 0;

private:
    BinGridCalculationInterface(BinGridCalculationInterface&&) = delete;
    BinGridCalculationInterface(const BinGridCalculationInterface&) = delete;
};

} // namespace segysdk::bin_grid_calculation
