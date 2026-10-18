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

#include "ByteValuePosition.h"

#include "../SegyIO/SampleFormat.h"

#include <memory>

namespace segysdk::segyio::valueconversion
{

class SEGYSDK_DLL_PUBLIC ByteConverter
{
private: // non-copyable - not implemented
    ByteConverter(const ByteConverter& other) = delete;
    ByteConverter& operator=(const ByteConverter& other) = delete;

protected:
    ByteConverter();

public:
    virtual ~ByteConverter();

    virtual double convert(const uint8_t* bytes) const = 0;
    virtual void convert(const uint8_t* bytes, double* values, const ByteValuePositions& positions) const = 0;
    virtual void convert(const uint8_t* bytes, float* values, size_t count) const = 0;
    virtual void convert(const uint8_t* bytes, float* values, size_t count, float scaleFactor) const = 0;

public:
    static std::unique_ptr<ByteConverter> create(SampleFormat valueType, bool aligned, bool swapBytes);
    static double convertValue(const uint8_t* bytes, SampleFormat valueType, bool aligned, bool swapBytes);
};

} // namespace segysdk::segyio::valueconversion
