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

#include <cstdint>
#include <string>
#include <utility>
#include <vector>

namespace segysdk::bulkaccess
{

class SEGYSDK_DLL_PUBLIC TraceHeaderResult
{
public:
    TraceHeaderResult() = default;
    TraceHeaderResult(int inlineNumber, int crosslineNumber);
    TraceHeaderResult(int inlineNumber, int crosslineNumber, int16_t statusCode, uint16_t trace_sample_count);

public:
    int getInline() const { return inlineNumber_; }
    void setInline(const int value) { inlineNumber_ = value; }

    int getCrossline() const { return crosslineNumber_; }
    void setCrossline(const int value) { crosslineNumber_ = value; }

    int16_t getTraceStatus() const;
    void setTraceIdCode(const int16_t value) { traceIdCode_ = value; }

    uint16_t getTraceSampleCount() const;
    void setTraceSampleCount(const uint16_t value) { traceSampleCount_ = value; }

    size_t getFileNo() const;
    void setFileNo(const size_t value) { fileNo_ = value; }

     size_t getOffset() const;
    void setOffset(const size_t value) { offset_ = value; }

    bool isSeismicTrace() const;
    bool isDeadTrace() const;
    bool isMissingTrace() const;
    

private:
    int inlineNumber_{};
    int crosslineNumber_{};

    int16_t traceIdCode_{seismicTrace};
    uint16_t traceSampleCount_{0};
    size_t fileNo_{0};
    size_t offset_{0};

public:
    static constexpr int16_t seismicTrace = 1;
    static constexpr int16_t deadTrace = 2;
    static constexpr int16_t dummyTrace = 3;
    static constexpr int16_t constantZeroTrace = 99;
    static constexpr int16_t missingTrace = 999;
};

}
