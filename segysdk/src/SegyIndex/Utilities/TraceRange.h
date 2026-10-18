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

#include "../SegyIndex/Utilities/TraceRangeRecord.h"
#include "../SegyIO/SampleFormat.h"

#include <json/json.h>

#include <cstddef>
#include <cstdint>
#include <limits>
#include <string>

namespace segysdk::segyindex
{
namespace trace_range_map
{

/**
 * @brief struct TraceRange Implements the index map value and describes the range of sequential similar traces including present or absent ones
 **/
struct SEGYSDK_DLL_PUBLIC TraceRange final : protected TraceRangeRecord
{
    static constexpr size_t const missingFileNo_ = std::numeric_limits<size_t>::max();
    static constexpr size_t const missingFileOffset_ = std::numeric_limits<size_t>::max();

    TraceRange() noexcept;
    explicit TraceRange(TraceRangeRecord const& record) noexcept;
    TraceRange(bool exists,
               PrimaryIndex primaryNo,
               SecondaryIndex secondaryNo,
               size_t fileNo,
               size_t samplesCount,
               size_t rangeCount,
               size_t fileOffset) noexcept;

    TraceRange(TraceRange const& other) noexcept;
    TraceRange& operator=(TraceRange const& other) noexcept;
    TraceRange(TraceRange&& other) noexcept;
    TraceRange& operator=(TraceRange&& other) noexcept;

    /**
     * @brief The equality operator
     *
     * @param other - represents another instance to compare with
     *
     * @return the boolean value if the the current instance equals to other
     **/
    bool operator==(TraceRange const& other) const noexcept;

    /**
     * @brief The not equality operator
     *
     * @param other - represents another instance to compare with
     *
     * @return the boolean value if the the current instance does not equal to other
     **/
    bool operator!=(TraceRange const& other) const noexcept;

    /**
     * @brief Returns true if the range describes the present traces or false otherwise
     **/
    bool isExist() const noexcept;

    /**
     * @brief Returns the primary index value for the range
     **/
    PrimaryIndex getPrimaryNo() const noexcept;

    /**
     * @brief Returns the secondary index value for the range
     **/
    SecondaryIndex getSecondaryNo() const noexcept;

    /**
     * @brief Returns the input SEGY file ID number for the range
     **/
    size_t getFileNo() const noexcept;

    /**
     * @brief Returns the number of the sample per trace for the range
     **/
    size_t getSamplesCount() const noexcept;

    /**
     * @brief Returns the number of traces that the current range describes
     **/
    size_t getRangeCount() const noexcept;

    /**
     * @brief Returns the file offset of the first traces of the range
     **/
    size_t getFileOffset() const noexcept;

    /**
     * @brief Returns the constant reference to the basic trace range record which is to be kept in the index file
     **/
    TraceRangeRecord const& getRecord() const;

    /**
    * @brief Returns the JSON object.
    **/
    Json::Value toJsonValue() const;

    /**
    * @brief Returns the JSON formatted string.
    **/
    std::string toJsonString() const;

    /**
     * @brief Checks if the specified trace is described by the current range
     *
     * @param primaryNo - the required trace's primary index
     * @param secondaryNo - the required trace's secondary index
     * @param secStep - the required trace's secondary index step/increment value
     *
     * @return true if the specified trace is described by the current range
     **/
    bool isHit(PrimaryIndex primaryNo,
               SecondaryIndex secondaryNo,
               IndexStep secStep) const noexcept;

    /**
     * @brief Checks if the specified trace is marked as present and described by the current range
     *
     * @param primaryNo - the required trace's primary index
     * @param secondaryNo - the required trace's secondary index
     * @param secStep - the required trace's secondary index step/increment value
     *
     * @return true if the specified trace is marked as present and described by the current range
     **/
    bool isLiveTrace(PrimaryIndex primaryNo,
                     SecondaryIndex secondaryNo,
                     IndexStep secStep) const noexcept;

    /**
     * @brief Gets file offset for the specified trace
     *
     * @param primaryNo - the required trace's primary index
     * @param secondaryNo - the required trace's secondary index
     * @param secStep - the required trace's secondary index step/increment value
     * @param sampleFormat - the required trace's sample value format
     *
     * @return file offset for the specified trace if the trace there is, missingFileOffset_ otherwise
     **/
    size_t getFileOffset(PrimaryIndex primaryNo,
                         SecondaryIndex secondaryNo,
                         IndexStep secStep,
                         segysdk::segyio::SampleFormat sampleFormat) const noexcept;

    /**
     * @brief Gets file offset for the specified trace
     *
     * @param primaryNo - the required trace's primary index
     * @param secondaryNo - the required trace's secondary index
     * @param secStep - the required trace's secondary index step/increment value
     * @param sampleSize - the required trace's sample value size in bytes
     *
     * @return file offset for the specified trace if the trace there is, missingFileOffset_ otherwise
     **/
    size_t getFileOffset(PrimaryIndex primaryNo,
                         SecondaryIndex secondaryNo,
                         IndexStep secStep,
                         size_t sampleSize) const noexcept;

private:
    static constexpr char const* const fieldExists_ = "Exists";
    static constexpr char const* const fieldPrimaryNo_ = "PrimaryNo";
    static constexpr char const* const fieldSecondaryNo_ = "SecondaryNo";
    static constexpr char const* const fieldFileNo_ = "FileNo";
    static constexpr char const* const fieldSamplesCount_ = "SamplesCount";
    static constexpr char const* const fieldRangeCount_ = "RangeCount";
    static constexpr char const* const fieldFileOffset_ = "FileOffset";
};

} // namespace trace_range_map
} // namespace segysdk::segyindex
