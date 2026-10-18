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

#include "Connector.h"

namespace segysdk::telemetry
{

Connector& getConnector();

SpanScope
startSpan(const SpanContext& context)
{
    return getConnector().startSpan(context);
}

SpanScope
Connector::startSpan(const SpanContext& context) const
{
    return doStartSpan(context);
}

SpanScope
Connector::doStartSpan(const SpanContext&) const
{
    return nullptr;
}

std::string
getCurrentTraceHeader()
{
    return getConnector().getCurrentTraceHeader();
}

std::string
Connector::getCurrentTraceHeader() const
{
    return doGetCurrentTraceHeader();
}

std::string
Connector::doGetCurrentTraceHeader() const
{
    return {};
}

SourceLocation::SourceLocation(std::string_view fileName, std::string_view function)
  : fileName_{fileName}, function_{function}
{
}

SpanContext&
SpanContext::withSourceLocation(const SourceLocation& value)
{
    sourceLocation_ = value;
    return *this;
}

SpanContext&
SpanContext::withAttributes(const SpanAttributes& value)
{
    spanAttributes_ = value;
    return *this;
}

SpanContext&
SpanContext::withTraceHeader(std::string_view value)
{
    traceHeader_ = value;
    return *this;
}

} // namespace segysdk::telemetry
