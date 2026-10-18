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

#include "TraceHeaderResult.h"

namespace segysdk::bulkaccess
{

TraceHeaderResult::TraceHeaderResult(int inline_number, int crossline_number)
  : inlineNumber_(inline_number),
    crosslineNumber_(crossline_number)
{
}

TraceHeaderResult::TraceHeaderResult(int inline_number, int crossline_number, int16_t status_code, uint16_t trace_sample_count)
  : inlineNumber_(inline_number),
    crosslineNumber_(crossline_number),
    traceIdCode_(status_code),
    traceSampleCount_(trace_sample_count)
{
}

int16_t
TraceHeaderResult::getTraceStatus() const
{
    return traceIdCode_;
}

uint16_t
TraceHeaderResult::getTraceSampleCount() const
{
    return traceSampleCount_;
}

size_t
TraceHeaderResult::getFileNo() const
{
    return fileNo_;
}

size_t
TraceHeaderResult::getOffset() const
{
    return offset_;
}

bool 
TraceHeaderResult::isSeismicTrace() const
{
    return traceIdCode_ <= 1;
}

bool
TraceHeaderResult::isDeadTrace() const
{
    return traceIdCode_ == deadTrace or traceIdCode_ == dummyTrace;
}

bool
TraceHeaderResult::isMissingTrace() const
{
    return traceIdCode_ == missingTrace;
}


} // namespace segysdk::bulkaccess
