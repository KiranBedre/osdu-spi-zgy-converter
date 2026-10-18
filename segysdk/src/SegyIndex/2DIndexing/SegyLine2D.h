/*
Copyright 2024 Schlumberger

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
#include "../SegyIndex/IndexBuilderData.h"

#include <json/json.h>

#include <deque>
#include <limits>
#include <map>
#include <string>
#include <utility>

namespace segysdk::segyindex
{

namespace index_2d
{

struct SEGYSDK_DLL_PUBLIC Line2DRecord
{
    size_t traceLastNo_;
    int cmpStart_;
    int cmpEnd_;
    size_t samplesCount_;
    size_t traceCount_;
    size_t fileOffset_;
};

struct SEGYSDK_DLL_PUBLIC SegyLine2D final
{
    SegyLine2D() noexcept;

    SegyLine2D(SegyLine2D const& other) noexcept = default;
    SegyLine2D& operator=(SegyLine2D const& other) noexcept = default;
    SegyLine2D(SegyLine2D&& other) noexcept = default;
    SegyLine2D& operator=(SegyLine2D&& other) noexcept = default;

    ~SegyLine2D() noexcept = default;

    void append(const IndexBuilderData& data);

    /**
     * @brief Returns the number of the sample per trace for the range
     **/
    size_t getSamplesCount() const noexcept;

    /**
     * @brief Returns the file offset of the first traces of the range
     **/
    size_t getFileOffset() const noexcept;

    size_t getFileOffsetForCMP(int CMP) const;

    size_t getFileOffsetForTrace(size_t traceNo) const;

    size_t getTraceNoForCMP(int CMP, bool findNextClosest = false) const;

    std::deque<Line2DRecord> const& getRecords() const;

    /**
     * @brief Returns the JSON object.
     **/
    Json::Value toJsonValue() const;

    size_t getIndexedTraceCount() const;

    bool isRegular() const;
    size_t size() const;
    bool isValid() const;

    void clear();

    void addRecord(Line2DRecord&& record);

    void setSampleSize(size_t sampleSize);
    void setTraceHeaderSize(size_t traceHeaderSize);

private:
    typedef std::pair<int, int> CMPRange;

    static bool compare(const CMPRange& a, const CMPRange& b);

    typedef std::map<CMPRange, size_t, decltype(&SegyLine2D::compare)> CMPRangeMap;

    size_t getTraceSize(size_t samplesCount) const;

    void populateCmpMap() const;

private:
    std::deque<Line2DRecord> records_;
    bool isRegular_{true};
    size_t sampleSize_{0};
    size_t traceHeaderSize_{0};

    mutable CMPRangeMap cmpMap_;
};

} // namespace index_2d
} // namespace segysdk::segyindex