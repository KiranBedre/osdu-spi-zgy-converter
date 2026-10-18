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

#include "../Utilities/BrickSize.h"

#include <openzgy/api.h>

namespace segysdk::utilities
{

enum class SEGYSDK_DLL_PUBLIC ZgySampleSize
{
    Bit8,
    Bit16,
    Bit32
};

class SEGYSDK_DLL_PUBLIC ZgyOutputParams
{
private:
    OpenZGY::SampleDataType _zgySampleSize{OpenZGY::SampleDataType::float32};
    bool _useCompression{false};
    float _zgyCompressionSnR{100};
    bool _useLodCompression{false};
    float _lodCompressionSnR{100};
    BrickSize _brickSize{BrickSize("")};

public:
    static ZgyOutputParams getZgyOutputParameters();

public:
    ZgyOutputParams();
    ZgyOutputParams(OpenZGY::SampleDataType sampleSize, bool useCompression, float zgyCompression);

    ZgyOutputParams(OpenZGY::SampleDataType sampleSize, bool useCompression, float zgyCompression, const std::string& brick_size);
    ZgyOutputParams(OpenZGY::SampleDataType sampleSize, bool useCompression, float zgyCompression, bool useLodCompression, float lodCompression);
    ZgyOutputParams(OpenZGY::SampleDataType sampleSize, bool useCompression, float zgyCompression, bool useLodCompression, float lodCompression, const std::string& brick_size);

    OpenZGY::SampleDataType getZgySampleSize() const { return _zgySampleSize; }
    bool UseCompression() const { return _useCompression; }
    float getZgyCompressionSnR() const { return _zgyCompressionSnR; }
    bool UseLodCompression() const { return _useLodCompression; }
    float getLodCompressionSnR() const { return _lodCompressionSnR; }

    segysdk::utilities::BrickSize getBrickSize() const { return _brickSize; }
};

} // namespace segysdk::utilities
