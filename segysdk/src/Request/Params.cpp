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

#include "Params.h"

namespace segysdk::request
{

Params&
Params::withCorrelationId(std::string_view value)
{
    correlationId_ = value;
    return *this;
}

StringsPair
Params::getCorrelationId() const
{
    return {"correlation_id", correlationId_};
}

Params&
Params::withTraceHeader(std::string_view value)
{
    traceHeader_ = value;
    return *this;
}

TraceParams::TraceParams(const std::size_t startTrace)
  : startTrace_{startTrace}
{
}

TraceParams::TraceParams(const request::Params& params)
  : request::Params{params}
{
}

TraceParams&
TraceParams::withStartTrace(const std::size_t value)
{
    startTrace_ = value;
    return *this;
}

TraceParams&
TraceParams::withTracesToDump(const std::size_t value)
{
    tracesToDump_ = value;
    return *this;
}

TraceParams&
TraceParams::withSampleOffset(const std::size_t value)
{
    sampleOffset_ = value;
    return *this;
}

TraceParams&
TraceParams::withSamplesToDump(const std::size_t value)
{
    samplesToDump_ = value;
    return *this;
}

TraceParams&
TraceParams::withScaledHeaders()
{
    scaledHeaders_ = true;
    return *this;
}

} // namespace segysdk::request
