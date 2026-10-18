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

#include "../BulkAccess/AsyncReader.h"
#include "../Telemetry/Connector.h"

namespace segysdk::bulkaccess::async
{

ExtentSpan::ExtentSpan(const std::size_t start, const std::size_t count)
  : start_(start),
    count_(count)
{
}

ExtentSpan&
ExtentSpan::withStep(const std::size_t value)
{
    step_ = value;
    return *this;
}

ReaderParams&
ReaderParams::withInline(const ExtentSpan& value)
{
    inline_ = value;
    return *this;
}

ReaderParams&
ReaderParams::withXline(const ExtentSpan& value)
{
    xline_ = value;
    return *this;
}

ReaderParams&
ReaderParams::withSamples(const ExtentSpan& value)
{
    samples_ = value;
    return *this;
}

ReaderParams&
ReaderParams::withNumParallelTasks(const int value)
{
    numParallelTasks_ = value;
    return *this;
}

Samples
Reader::read(const ReaderParams& params)
{
    const auto& scope = START_TRACE_SPAN(SpanContext{});
    return doRead(params);
}

} // namespace segysdk::bulkaccess::async
