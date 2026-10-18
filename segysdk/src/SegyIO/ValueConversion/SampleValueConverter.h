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

#include "../SegyIO/ConversionDetails.h"
#include "../SegyIO/ValueConverter.h"

#include <memory>

namespace segysdk::segyio::valueconversion
{

class SEGYSDK_DLL_PUBLIC SampleValueConverter : public ValueConverter
{
private:
    ConversionDetails details_;

private:
    SampleValueConverter(SampleFormat valueType, bool needsByteSwap);

    static void convertInt8ToFloat(uint8_t const* bytes, float* out, size_t count);
    void convertInt16ToFloat(uint8_t const* bytes, float* out, size_t count) const;
    void convertUInt16ToFloat(uint8_t const* bytes, float* out, size_t count) const;
    void convertInt32ToFloat(uint8_t const* bytes, float* out, size_t count) const;

    static void convertFloatToFloat(uint8_t const* bytes, float* out, size_t count);
    static void convertSwappedFloatToFloat(uint8_t const* bytes, float* out, size_t count);

public:
    const ConversionDetails& getConversionDetails() const { return details_; }
    SampleFormat getValueType() const { return details_.getSampleValueType(); }
    size_t getRequiredAlignment() const;

    // the input is kept unchanged and may need to be aligned...
    void convert(uint8_t const* bytes, float* samples, size_t sampleCount) const;

    // the input is kept unchanged and may need to be aligned...
    void convertScaled(uint8_t const* bytes, float* samples, size_t sampleCount, float scaleFactor) const;

public:
    static std::unique_ptr<SampleValueConverter> create(SampleFormat format, bool needsByteSwap);
};

} // namespace segysdk::segyio::valueconversion
