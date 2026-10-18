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

#include "../SegyIO/AsyncActivity.h"

#include <cstdint>
#include <memory>

namespace segysdk::request
{

struct TraceParams;

} // namespace segysdk::request

namespace segysdk::segyio
{

struct SEGYSDK_DLL_PUBLIC SegyTraceScanner
{
    static constexpr size_t const MinBufferSize = 1024U * 1024U;
    static constexpr size_t const MaxBufferSize = 128U * 1024U * 1024U;

    struct TraceInfo
    {
        uint64_t index_;
        uint64_t offset_;
        uint64_t size_;
    };

    SegyTraceScanner();
    virtual ~SegyTraceScanner();

    virtual void setAsyncActivity(const std::shared_ptr<AsyncActivity>& activity) = 0;
    virtual void setReaderBufferSize(size_t bufferSize) const = 0;

    virtual bool moveNext(const request::TraceParams& params) = 0;
    virtual void reset() = 0;
    virtual void reset(const request::TraceParams& params) = 0;

    virtual bool readAt(uint64_t traceIndex) = 0;

    virtual uint64_t getTraceIndex() const = 0;
    virtual const TraceInfo& getTraceInfo() const = 0;
    virtual const double* getHeaderValues() const = 0;
    virtual const float* getTraceSamples() const = 0;
    virtual uint64_t getTraceSampleCount() const = 0;
    virtual uint64_t getHeaderValueCount() const = 0;

private: // non-copyable - not implemented
    SegyTraceScanner(const SegyTraceScanner& other) = delete;
    SegyTraceScanner& operator=(const SegyTraceScanner& other) = delete;
};

} // namespace segysdk::segyio
