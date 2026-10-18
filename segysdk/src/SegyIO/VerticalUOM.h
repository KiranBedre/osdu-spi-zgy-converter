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
 * @brief VerticalUOM.h declares the enum class VerticalUOM, which is used to specify unit of measurement for the vertical domain of a SEG-Y volume.
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

namespace segysdk::segyio
{

/**
 * @enum VerticalUOM
 *
 * @brief Enum class, used to specify the Unit of measurement for the sample interval in a SEG-Y volume.
 *
 **/
enum class SEGYSDK_DLL_PUBLIC VerticalUOM
{
    seconds = 0, /**< Sample interval specified in fractions of one second. E.g. default 4us is represented with a sample interval of 0.0000004. */
    meter,       /**< SEG-Y sample intervals are specified in depth using meter as unit of measurement. */
    feet         /**< SEG-Y sample intervals are specified in depth using feet as unit of measurement. */
};

} // namespace segysdk::segyio
