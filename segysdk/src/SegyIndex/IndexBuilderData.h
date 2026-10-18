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

#include <cstddef>
#include <cstdint>
#include <string>

namespace Json
{
class Value;
}

namespace segysdk::segyindex
{

/**
* @struct IndexBuilderData 
* 
* @brief Used to hold intermediate data extracted for a single trace. This is collated into the main index, and then discarded.
**/
struct IndexBuilderData
{

    /**
    * @brief File Index of the input SEGY files list.
    **/
    size_t FileNo;

    /**
    * @brief File offset.
    **/
    size_t Offset;

    /**
    * @brief Inline number of the current trace.
    **/
    int InlineNo;

    /**
    * @brief Crossline number of the current trace.
    **/
    int CrosslineNo;

    /**
     * @brief CMP of the current trace.
    **/
    int CMP;

    /**
     * @brief Trace number of the current trace.
    **/
    size_t TraceNo;

    /**
    * @brief X-Coordinate value of the current trace extracted from the trace header coordinate.
    **/
    double X;

    /**
    * @brief Y-Coordinate value of the current trace extracted from the trace header coordinate.
    **/
    double Y;

    /**
    * @brief Number of Samples in the current trace.
    **/
    size_t SampleCount;

    /**
    * brief Minimum sample amplitude in current trace.
    **/
    float MinimumAmplitude;

    /**
     * brief Maximum sample amplitude in current trace.
     **/
    float MaximumAmplitude;

    /**
    * @brief Returns the JSON object.
    **/
    Json::Value toJsonValue() const;

    /**
    * @brief Returns the JSON formatted string.
    **/
    std::string toJsonString() const;

private:
    static constexpr char const* const fieldInlineNo_ = "InlineNo";
    static constexpr char const* const fieldCrosslineNo_ = "CrosslineNo";
    static constexpr char const* const fieldCMP_ = "CMP";
    static constexpr char const* const fieldTraceNo_ = "TraceNo";
    static constexpr char const* const fieldFileNo_ = "FileNo";
    static constexpr char const* const fieldOffset_ = "Offset";
    static constexpr char const* const fieldSampleCount_ = "SampleCount";
    static constexpr char const* const fieldX_ = "X";
    static constexpr char const* const fieldY_ = "Y";
};

} // namespace segysdk::segyindex

