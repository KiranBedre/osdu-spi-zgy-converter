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

#include "SampleValueConverter.h"

#include "../SegyIO/SegyIOException.h"
#include <cstring>
#include <type_traits>

#ifndef _MSC_VER
#define _byteswap_ulong __builtin_bswap32
#endif

namespace segysdk::segyio::valueconversion
{

SampleValueConverter::SampleValueConverter(SampleFormat format, bool needsByteSwap)
  : ValueConverter(needsByteSwap), details_(needsByteSwap, format)
{
}

size_t
SampleValueConverter::getRequiredAlignment() const
{
    // Depends on algorithm, for now just use the value type
    switch (getValueType()) {
    case SampleFormat::IBMFloat4:
        return std::alignment_of<int32_t>::value;

    case SampleFormat::FixedPointWithGain4:
        return std::alignment_of<int32_t>::value;

    case SampleFormat::IeeeFloat4:
        return std::alignment_of<float>::value;

    case SampleFormat::INT1:
        return std::alignment_of<int8_t>::value;

    case SampleFormat::INT2:
        return std::alignment_of<int16_t>::value;

    case SampleFormat::INT4:
        return std::alignment_of<int32_t>::value;

    default:
        return std::alignment_of<int8_t>::value;
    }
}

void
SampleValueConverter::convertInt8ToFloat(uint8_t const* bytes, float* out, size_t count)
{
    const auto in = reinterpret_cast<int8_t const*>(bytes);
    for (size_t i = 0; i < count; ++i) {
        out[i] = static_cast<float>(in[i]);
    }
}

void
SampleValueConverter::convertInt16ToFloat(uint8_t const* bytes, float* out, size_t count) const
{
    // sizeof(int16_t) is less then sizeof(float) so
    // let's put the swap results to the output buffer
    // the tail of the output buffer will not be used obviously
    SwapBytes2(bytes, reinterpret_cast<uint16_t*>(out), count);

    // process the output buffer inplace starting from its end which is safe because
    // no data is overlapped due to input and output data type sizes
    const auto in = reinterpret_cast<int16_t const*>(out);
    if (count > 0) {
        for (size_t i = count - 1; i != 0; --i) {
            out[i] = static_cast<float>(in[i]);
        }
        out[0] = static_cast<float>(in[0]);
    }
}

void
SampleValueConverter::convertUInt16ToFloat(uint8_t const* bytes, float* out, size_t count) const
{
    // sizeof(int16_t) is less then sizeof(float) so
    // let's put the swap results to the output buffer
    // the tail of the output buffer will not be used obviously
    SwapBytes2(bytes, reinterpret_cast<uint16_t*>(out), count);

    // process the output buffer inplace starting from its end which is safe because
    // no data is overlapped due to input and output data type sizes
    const auto in = reinterpret_cast<uint16_t const*>(out);
    if (count > 0) {
        for (size_t i = count - 1; i != 0; --i) {
            out[i] = static_cast<float>(in[i]);
        }
        out[0] = static_cast<float>(in[0]);
    }
}

void
SampleValueConverter::convertInt32ToFloat(uint8_t const* bytes, float* out, size_t count) const
{
    // sizeof(int32_t) is same as sizeof(float) so
    // let's put the swap results to the output buffer
    SwapBytes4(bytes, reinterpret_cast<uint32_t*>(out), count);

    // processing inplace is safe because
    // no data is overlapped due to input and output data type sizes
    const auto in = reinterpret_cast<int32_t const*>(out);
    for (size_t i = 0; i < count; ++i) {
        out[i] = static_cast<float>(in[i]);
    }
}

void
SampleValueConverter::convertFloatToFloat(uint8_t const* bytes, float* out, size_t count)
{
    const size_t byteCount = count * sizeof(float);
    memcpy(out, bytes, byteCount);
}

void
SampleValueConverter::convertSwappedFloatToFloat(uint8_t const* bytes, float* out, size_t count)
{
    const auto src = reinterpret_cast<uint32_t const*>(bytes);
    const auto des = reinterpret_cast<uint32_t*>(out);
    for (size_t i = 0; i < count; ++i) {
        des[i] = _byteswap_ulong(src[i]);
    }
}

void
SampleValueConverter::convert(uint8_t const* bytes, float* samples, size_t sampleCount) const
{
    switch (getValueType()) {
    case SampleFormat::IBMFloat4:
        IBM4BytesToFloat(bytes, samples, sampleCount);
        break;

    case SampleFormat::FixedPointWithGain4:
        FPGToFloat(bytes, samples, sampleCount);
        break;

    case SampleFormat::IeeeFloat4:
        if (needsSwap)
            convertSwappedFloatToFloat(bytes, samples, sampleCount);
        else
            convertFloatToFloat(bytes, samples, sampleCount);
        break;

    case SampleFormat::INT1:
        convertInt8ToFloat(bytes, samples, sampleCount);
        break;

    case SampleFormat::INT2:
        convertInt16ToFloat(bytes, samples, sampleCount);
        break;
    case SampleFormat::UINT2:
        convertUInt16ToFloat(bytes, samples, sampleCount);
        break;
    case SampleFormat::INT4:
        convertInt32ToFloat(bytes, samples, sampleCount);
        break;

    default:
        throw SegyFormatException("No valid numeric format for trace samples found in the binary header. Please supply a valid value as a binary header override setting");
    }
}

void
SampleValueConverter::convertScaled(uint8_t const* bytes, float* samples, size_t sampleCount, float scaleFactor) const
{
    convert(bytes, samples, sampleCount);

    for (size_t i = 0; i < sampleCount; ++i)
        samples[i] *= scaleFactor;
}

std::unique_ptr<SampleValueConverter>
SampleValueConverter::create(SampleFormat format, bool needsByteSwap)
{
    return std::make_unique<SampleValueConverter>(SampleValueConverter(format, needsByteSwap));
}

} // namespace segysdk::segyio::valueconversion
