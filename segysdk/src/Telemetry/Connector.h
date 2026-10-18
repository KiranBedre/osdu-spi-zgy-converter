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

#include <map>
#include <memory>
#include <string>
#include <string_view>

#define START_TRACE_SPAN(context) segysdk::telemetry::startSpan((context).withSourceLocation({__FILE__, __func__}))

namespace segysdk
{

namespace telemetry
{

class SpanScopeWrapper;

using SpanScope = std::shared_ptr<SpanScopeWrapper>;

using SpanAttributes = std::map<std::string, std::string>;

struct SourceLocation
{
    SourceLocation(std::string_view fileName, std::string_view function);
    std::string fileName_;
    std::string function_;
};

struct SpanContext
{
    SpanContext& withSourceLocation(const SourceLocation& value);
    SpanContext& withAttributes(const SpanAttributes& value);
    SpanContext& withTraceHeader(std::string_view value);
    SourceLocation sourceLocation_{"", ""};
    SpanAttributes spanAttributes_{};
    std::string traceHeader_{};
};

class Connector
{
public:
    virtual ~Connector() = default;
    [[nodiscard]] SpanScope startSpan(const SpanContext& context) const;
    std::string getCurrentTraceHeader() const;

private:
    virtual SpanScope doStartSpan(const SpanContext& context) const;
    virtual std::string doGetCurrentTraceHeader() const;
};

[[nodiscard]] SpanScope startSpan(const SpanContext& context);
std::string getCurrentTraceHeader();

} // namespace telemetry

using telemetry::SpanContext;

} // namespace segysdk
