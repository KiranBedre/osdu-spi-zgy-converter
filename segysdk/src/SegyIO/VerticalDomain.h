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

/** @file
 * @brief VerticalDomain.h declares the enum class VerticalDomain, used to specify domain of a SEG-Y volume.
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

namespace segysdk::segyio
{

/**
 * @enum VerticalDomain
 *
 * @brief enum class, Used to specify domain of a SEG-Y volume.
 *
 **/
enum class SEGYSDK_DLL_PUBLIC VerticalDomain
{
    Time = 0, /**< SEG-Y sample intervals are specified in time (microseconds) */
    Depth     /**< SEG-Y sample intervals are specified in depth (scaled meter or feet) */
};

} // namespace segysdk::segyio
