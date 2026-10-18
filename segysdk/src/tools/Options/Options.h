/*
Copyright 2025 Schlumberger

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

#include "Utilities/Containers.h"
#include "tools/Params/Params.h"

#include <memory>

namespace tool
{

namespace keywords
{
constexpr auto configFile = "config-file";
constexpr auto correlationId = "correlation-id";
constexpr auto sdServiceUrl = "sd-service-url";
constexpr auto seismicTraceDataId = "seismic-trace-data-id";
constexpr auto verbosity = "verbosity";
constexpr auto workProductId = "work-product-id";
} // namespace keywords

class ProgramOptionsStore;

class ProgramOptions
{
public:
    ~ProgramOptions();
    ProgramOptions(int argc, char* argv[], const segysdk::utilities::StringPairs& customOptions = {});
    tool::Params getBasicParams() const;
    std::string getValue(const std::string& optionName) const;
    std::string getDescription() const;

private:
    std::unique_ptr<tool::ProgramOptionsStore> store_;
};

} // namespace tool
