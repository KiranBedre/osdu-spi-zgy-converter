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

#include <opentelemetry/context/propagation/global_propagator.h>
#include <opentelemetry/trace/context.h>
#include <opentelemetry/trace/provider.h>

#include <filesystem>

namespace segysdk::telemetry
{

class SpanScopeWrapper
{
public:
    explicit SpanScopeWrapper(opentelemetry::trace::Scope scope_)
      : scope{std::move(scope_)}
    {
    }

private:
    opentelemetry::trace::Scope scope;
};

namespace live
{

using OtelStringView = opentelemetry::nostd::string_view;

class Carrier final : public opentelemetry::context::propagation::TextMapCarrier
{
public:
    Carrier() = default;
    explicit Carrier(std::string_view traceHeader);
    OtelStringView Get(OtelStringView key) const noexcept override;
    void Set(OtelStringView key, OtelStringView value) noexcept override;
    std::string getTraceHeader() const;

private:
    std::string traceHeader_;
};

OtelStringView
Carrier::Get(OtelStringView key) const noexcept
{
    (void)key;
    return traceHeader_;
}

void
Carrier::Set(OtelStringView key, OtelStringView value) noexcept
{
    (void)key;
    traceHeader_ = std::string{value};
}

class Connector : public telemetry::Connector
{
private:
    SpanScope doStartSpan(const SpanContext& context) const override;
    std::string doGetCurrentTraceHeader() const override;
};

static auto
makeSpanOptions(const opentelemetry::context::Context& context)
{
    auto result = opentelemetry::trace::StartSpanOptions{};
    result.parent = opentelemetry::trace::GetSpan(context)->GetContext();
    result.kind = opentelemetry::trace::SpanKind::kClient;
    return result;
}

Carrier::Carrier(std::string_view traceHeader)
  : traceHeader_{traceHeader}
{
}

static auto
makeSpanName(const SourceLocation& location)
{
    return std::string{std::filesystem::path{location.fileName_}.stem().string()}.append(":").append(location.function_);
}

SpanScope
Connector::doStartSpan(const SpanContext& context) const
{
    const auto tracer = opentelemetry::trace::Provider::GetTracerProvider()->GetTracer("segysdk");
    auto ctx = opentelemetry::context::RuntimeContext::GetCurrent();
    if (!context.traceHeader_.empty())
    {
        auto propagator = opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
        ctx = propagator->Extract(Carrier{context.traceHeader_}, ctx);
    }
    auto span = tracer->StartSpan(makeSpanName(context.sourceLocation_), context.spanAttributes_, makeSpanOptions(ctx));
    return std::make_shared<SpanScopeWrapper>(opentelemetry::trace::Tracer::WithActiveSpan(span));
}

std::string
Connector::doGetCurrentTraceHeader() const
{
    auto carrier = Carrier{};
    auto propagator = opentelemetry::context::propagation::GlobalTextMapPropagator::GetGlobalPropagator();
    propagator->Inject(carrier, opentelemetry::context::RuntimeContext::GetCurrent());
    return carrier.getTraceHeader();
}

std::string
Carrier::getTraceHeader() const
{
    return traceHeader_;
}

struct Globals
{
    inline static auto result = live::Connector{};
};

} // namespace live

Connector&
getConnector()
{
    return live::Globals::result;
}

} // namespace segysdk::telemetry
