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
 * @brief SampleFormat.h contains the declaration of the SampleFormat class which describes the value type of the SEG-Y trace samples.
 *
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

namespace segysdk::segyio
{

/**
 * @enum SampleFormat
 *
 * @brief enum class, Used to define Override numeric format of a data fields. This will typically be trace header values and trace sample values.
 *
 **/
enum class SEGYSDK_DLL_PUBLIC SampleFormat
{

    IBMFloat4 = 1,           /**< 32-bit IBM floating point representation */
    INT4 = 2,                /**< 32-bit twos complement integer representation */
    INT2 = 3,                /**< 16-bit twos complement integer representation */
    FixedPointWithGain4 = 4, /**< Fixed point integer with gain representation (Obsolete, not supported) */
    IeeeFloat4 = 5,          /**< 32-bit IEEE floating point representation */
    INT1 = 8,                /**< 8-bit twos complement integer representation */

    IeeeFloat8 = 6, /**< 64-bit IEEE floating point representation ( SEG-Y rev 2 definition NOT currently supported) */
    INT3 = 7,       /**< INT3  ( SEG-Y rev 2 definition NOT currently supported) */
    UINT4 = 10,     /**< UINT4 ( SEG-Y rev 2 definition NOT currently supported) */
    UINT2 = 11,     /**< UINT2 ( SEG-Y rev 2 definition NOT currently supported) */
    UINT3 = 15,     /**< UINT3 ( SEG-Y rev 2 definition NOT currently supported) */
    UINT1 = 16      /**< UINT1 ( SEG-Y rev 2 definition NOT currently supported) */
};

} // namespace segysdk::segyio
