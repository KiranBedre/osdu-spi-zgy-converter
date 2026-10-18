/*
Copyright 2022 Schlumberger

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

#include <cstddef>
#include <cstdint>

namespace segysdk::segyindex
{
namespace trace_range_map
{

/**
 * @brief struct TraceRangeRecord Implements the core unit to describe the trace range
 *
 * The TraceRangeRecord instances are used to store the trace index map contents to the binary index file.
 **/

using PrimaryIndex = int64_t;
using SecondaryIndex = int64_t;
using IndexStep = int64_t;

#pragma pack(push, 1)
struct SEGYSDK_DLL_PUBLIC TraceRangeRecord
{
    uint8_t exists_;
    PrimaryIndex primaryNo_;
    SecondaryIndex secondaryNo_;
    size_t fileNo_;
    size_t samplesCount_;
    size_t rangeCount_;
    size_t fileOffset_;
};
#pragma pack(pop)

} // namespace trace_range_map
} // namespace segysdk::segyindex
