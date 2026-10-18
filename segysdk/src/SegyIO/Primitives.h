/*
Copyright 2021-2022 Schlumberger

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

#include "../SegyIO/SampleFormat.h"

#include <cstddef>

namespace segysdk::segyio
{

class SEGYSDK_DLL_PUBLIC Primitives
{
public:
    constexpr static size_t getBinaryHeaderSize() { return 400U; }
    constexpr static size_t getTextualHeaderSize() { return 3200U; }
    constexpr static size_t getCombinedHeaderSize(int extendedTextualHeaderCount) { return 3200ULL + 400ULL + 3200ULL * extendedTextualHeaderCount; }
    constexpr static size_t getTraceHeaderSize() { return 240U; }
    constexpr static size_t getTraceSize(SampleFormat sampleFormat, size_t sampleCount);
    constexpr static size_t getTraceSize(size_t sampleSize, size_t sampleCount);
    constexpr static size_t getSampleSize(SampleFormat sampleFormat);
    constexpr static bool isFloatFormat(SampleFormat sampleFormat);
};

} // namespace segysdk::segyio

//-----------------------------------------------------------------------------
//    Primitives class implementation
//-----------------------------------------------------------------------------

namespace segysdk::segyio
{

SEGYSDK_DLL_PUBLIC size_t inline constexpr Primitives::getSampleSize(SampleFormat sampleFormat)
{
    switch (sampleFormat) {
    case SampleFormat::IeeeFloat8:
        return 8;

    case SampleFormat::FixedPointWithGain4:
    case SampleFormat::IBMFloat4:
    case SampleFormat::INT4:
    case SampleFormat::UINT4:
    case SampleFormat::IeeeFloat4:
        return 4;

    case SampleFormat::INT2:
    case SampleFormat::UINT2:
        return 2;

    case SampleFormat::INT3:
    case SampleFormat::UINT3:
        return 3;

    case SampleFormat::INT1:
    case SampleFormat::UINT1:
        return 1;

    default: return 0;
    }
}

SEGYSDK_DLL_PUBLIC bool inline constexpr Primitives::isFloatFormat(SampleFormat sampleFormat)
{
    switch (sampleFormat) {
    case SampleFormat::IeeeFloat8:
    case SampleFormat::FixedPointWithGain4:
    case SampleFormat::IBMFloat4:
    case SampleFormat::IeeeFloat4:
        return true;

    case SampleFormat::INT4:
    case SampleFormat::INT2:
    case SampleFormat::INT1:
    case SampleFormat::INT3:
    case SampleFormat::UINT4:
    case SampleFormat::UINT2:
    case SampleFormat::UINT3:
    case SampleFormat::UINT1:
        return false;
    }

    return false;
}

SEGYSDK_DLL_PUBLIC size_t inline constexpr Primitives::getTraceSize(SampleFormat sampleFormat, size_t sampleCount)
{
    return getTraceSize(getSampleSize(sampleFormat), sampleCount);
}

SEGYSDK_DLL_PUBLIC size_t inline constexpr Primitives::getTraceSize(size_t sampleSize, size_t sampleCount)
{
    return getTraceHeaderSize() + sampleCount * sampleSize;
}

} // namespace segysdk::segyio
