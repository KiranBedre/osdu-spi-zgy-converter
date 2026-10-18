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

#include "../BinGridCalculationPlugin/BinGridPluginParams.h"

namespace segysdk::bin_grid_calculation
{

BinGridPluginParams::BinGridPluginParams(std::shared_ptr<segyio::SegyTraceScanner> scanner, const std::shared_ptr<segyio::DerivedHeader> derived, const std::vector<std::shared_ptr<segyio::HeaderDefinition>> headers)
  : scanner_(scanner), derivedHeaders_(derived), headerDefinitions_(headers)
{
}

BinGridPluginParams::BinGridPluginParams(const BinGridPluginParams& other)
  : scanner_(other.scanner_),
    derivedHeaders_(other.derivedHeaders_),
    headerDefinitions_(other.headerDefinitions_)
{
}

BinGridPluginParams::BinGridPluginParams(BinGridPluginParams&& other) noexcept
  : scanner_(other.scanner_),
    derivedHeaders_(std::move(other.derivedHeaders_)),
    headerDefinitions_(std::move(other.headerDefinitions_))
{
}

BinGridPluginParams&
BinGridPluginParams::operator=(const BinGridPluginParams& rhs)
{
    return *this = BinGridPluginParams(rhs);
}

BinGridPluginParams&
BinGridPluginParams::operator=(BinGridPluginParams&& rhs) noexcept
{
    scanner_ = std::move(rhs.scanner_);
    derivedHeaders_ = std::move(rhs.derivedHeaders_);
    headerDefinitions_ = std::move(rhs.headerDefinitions_);

    return *this;
}

std::shared_ptr<segyio::SegyTraceScanner>
BinGridPluginParams::getScanner() const
{
    return scanner_;
}

std::shared_ptr<segyio::DerivedHeader>
BinGridPluginParams::getDerivedHeader() const
{
    return derivedHeaders_;
}

std::vector<std::shared_ptr<segyio::HeaderDefinition>>
BinGridPluginParams::getHeaderDefinitions() const
{
    return headerDefinitions_;
}

} // namespace segysdk::bin_grid_calculation
