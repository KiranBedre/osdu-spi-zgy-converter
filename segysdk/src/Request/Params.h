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

#include "../Utilities/Containers.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <string>
#include <string_view>

namespace segysdk::request
{

struct SEGYSDK_DLL_PUBLIC Params
{
    template<class Derived>
    explicit Params(const Derived& derived)
      : correlationId_{derived.correlationId_}, traceHeader_{derived.traceHeader_}
    {
    }
    Params() = default;
    StringsPair getCorrelationId() const;
    Params& withCorrelationId(std::string_view value);
    Params& withTraceHeader(std::string_view value);
    std::string correlationId_{};
    std::string traceHeader_{};
};

struct SEGYSDK_DLL_PUBLIC TraceParams : public request::Params
{
    TraceParams() = default;
    explicit TraceParams(const std::size_t startTrace);
    explicit TraceParams(const request::Params& params);
    TraceParams& withStartTrace(const std::size_t value);
    TraceParams& withTracesToDump(const std::size_t value);
    TraceParams& withSampleOffset(const std::size_t value);
    TraceParams& withSamplesToDump(const std::size_t value);
    /**
     * Requests to return trace header values as they are seen by internal methods. This means scaling factors and/or user defined overrides are applied for each trace.
     */
    TraceParams& withScaledHeaders();
    std::size_t startTrace_{};
    std::size_t tracesToDump_{};
    std::size_t sampleOffset_{};
    std::size_t samplesToDump_{};
    bool scaledHeaders_{};
};

} // namespace segysdk::request
