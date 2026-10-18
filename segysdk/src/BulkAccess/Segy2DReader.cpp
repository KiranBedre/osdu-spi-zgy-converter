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

#include "Segy2DReader.h"

#include "Telemetry/Connector.h"

namespace segysdk::bulkaccess
{

Samples
Segy2DReader::read(const Segy2DReaderParams& params)
{
    const auto& scope = START_TRACE_SPAN(SpanContext{});
    return doRead(params);
}

Segy2DReaderParams&
Segy2DReaderParams::withSampleStart(const std::size_t value)
{
    sampleStart_ = value;
    return *this;
}

Segy2DReaderParams&
Segy2DReaderParams::withSampleEnd(const std::size_t value)
{
    sampleEnd_ = value;
    return *this;
}

Segy2DReaderParams&
Segy2DReaderParams::withTraceStart(const std::size_t value)
{
    traceStart_ = value;
    return *this;
}

Segy2DReaderParams&
Segy2DReaderParams::withTraceEnd(const std::size_t value)
{
    traceEnd_ = value;
    return *this;
}

} // namespace segysdk::bulkaccess
