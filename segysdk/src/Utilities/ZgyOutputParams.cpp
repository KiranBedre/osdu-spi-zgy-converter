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



#include "../Utilities/ZgyOutputParams.h"

#include "../Logger/Logger.h"

#include "../Utilities/Adapters.h"
#include "../Utilities/Environment.h"
#include "../Utilities/Options.h"

#include "../segysdk_details/SegyException.h"

#include <algorithm>
#include <string>


namespace segysdk::utilities
{

static bool
is_digit(const unsigned char& c)
{
    return std::isdigit(c);
}

static bool
looks_like_a_number(const std::string& str)
{
    return !str.empty() && str.size() < 8 && std::all_of(str.begin(), str.end(), is_digit);
}

void
ValidateCompression(const std::string& kind, float compression)
{
    if (compression < 0 || compression > 100) {
        throw segysdk::SegyInvalidArgumentException(kind + ": should be an integer between 0 and 100");
    }
}

void
ValidateBrickSize(const std::string& brick_size)
{
    if (brick_size.empty())
        return;

    if (!looks_like_a_number(brick_size) && brick_size.find_last_of('X') <= 0 && brick_size.find_last_of('x') <= 0) {
        throw segysdk::SegyInvalidArgumentException("BrickSize: should be an empty string or in the form \"<inline Count>X<Crossline count>X<Sample count>\". specified value [" + brick_size + "]");
    }
}

ZgyOutputParams::ZgyOutputParams()
{}

ZgyOutputParams::ZgyOutputParams(OpenZGY::SampleDataType sampleSize, bool useCompression, float zgyCompression)
  : _zgySampleSize(sampleSize),
    _useCompression(useCompression),
    _zgyCompressionSnR(zgyCompression),
    _useLodCompression(useCompression),
    _lodCompressionSnR(zgyCompression)
{
    ValidateCompression("ZFP_SNR", zgyCompression);
}

ZgyOutputParams::ZgyOutputParams(OpenZGY::SampleDataType sampleSize, bool useCompression, float zgyCompression, const std::string& brick_size)
  : _zgySampleSize(sampleSize),
    _useCompression(useCompression),
    _zgyCompressionSnR(zgyCompression),
    _useLodCompression(useCompression),
    _lodCompressionSnR(zgyCompression),
    _brickSize(segysdk::utilities::BrickSize(brick_size))
{
    ValidateCompression("ZFP_SNR", zgyCompression);
    ValidateBrickSize(brick_size);
}

ZgyOutputParams::ZgyOutputParams(OpenZGY::SampleDataType sampleSize, bool useCompression, float zgyCompression, bool useLodCompression, float lodCompression)
  : _zgySampleSize(sampleSize),
    _useCompression(useCompression),
    _zgyCompressionSnR(zgyCompression),
    _useLodCompression(useLodCompression),
    _lodCompressionSnR(lodCompression)
{
    ValidateCompression("ZFP_SNR", zgyCompression);
    ValidateCompression("ZFP_LOD_SNR", lodCompression);
}

ZgyOutputParams::ZgyOutputParams(OpenZGY::SampleDataType sampleSize, bool useCompression, float zgyCompression, bool useLodCompression, float lodCompression, const std::string& brick_size)
  : _zgySampleSize(sampleSize),
    _useCompression(useCompression),
    _zgyCompressionSnR(zgyCompression),
    _useLodCompression(useCompression),
    _lodCompressionSnR(lodCompression),
    _brickSize(BrickSize(brick_size))
{
    (void)useLodCompression;
    ValidateCompression("ZFP_SNR", zgyCompression);
    ValidateBrickSize(brick_size);
    ValidateCompression("ZFP_LOD_SNR", lodCompression);
}


ZgyOutputParams
ZgyOutputParams::getZgyOutputParameters()
{
    const auto outputParams = Environment::instance().zgyOutputParameters();

    const bool isCloudBased = isSDDataset(outputParams);

    if (isCloudBased && !outputParams.empty()) {
        SEGYLOG_DEBUG("zgyWriterArgs: Get output parameters from json : [" + outputParams + "]");
    }
    else if (!isCloudBased && !outputParams.empty() && std::filesystem::exists(outputParams)) {
        SEGYLOG_DEBUG("zgyWriterArgs: Get output parameters from json : [" + outputParams + "]");
    }
    else {
        SEGYLOG_DEBUG("zgyWriterArgs: Taking output parameters from environment variables");
    }

    const ZgyOutputParams zgyOutputParams = (!outputParams.empty() && isCloudBased) || std::filesystem::exists(outputParams)
                                                                    ? Options::getOutputParamsFromJson(outputParams)
                                                                    : ZgyOutputParams();

    return zgyOutputParams;
}

} // namespace segysdk::utilities
