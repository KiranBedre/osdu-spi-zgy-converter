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

#include "../segysdk_details/DLL_EXPORT.h"

namespace segysdk::segyio
{

/** 
* @class ScalarUsageStrategy
* 
* @brief enum, Used to define ScalarUsage strategy for a defined header literal.
* 
* @param  NoScaling - No scaling operation is performed for the current header literal.
* @param  Default - If the ScalarHeaderId is non empty, the literal matching the ScalarHeaderId is looked up and its value used as scalar according to the SEG-Y revision definition.
* @param  Override - The value specified in the ScalarOverride attribute is used as the scalar for the current literal.
* 
**/
enum class SEGYSDK_DLL_PUBLIC ScalarUsageStrategy
{

    NoScaling = 0,
    Default = 1,
    Override = 2
};

} // namespace segysdk::segyio
