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

#include "../SegyIO/DerivedHeader.h"
#include "../SegyIO/HeaderDefinition.h"
#include "../SegyIO/SegyTraceScanner.h"

#include "../segysdk_details/DLL_EXPORT.h"

#include <memory>
#include <string>

namespace segysdk::bin_grid_calculation
{

class SEGYSDK_DLL_PUBLIC BinGridPluginParams
{
public:
    explicit BinGridPluginParams(std::shared_ptr < segyio::SegyTraceScanner> scanner, const std::shared_ptr<segyio::DerivedHeader> derived, const std::vector<std::shared_ptr<segyio::HeaderDefinition>> headers);
    BinGridPluginParams(const BinGridPluginParams& other);
    BinGridPluginParams(BinGridPluginParams&& other) noexcept;
    BinGridPluginParams& operator=(const BinGridPluginParams& rhs);
    BinGridPluginParams& operator=(BinGridPluginParams&& rhs) noexcept;
    ~BinGridPluginParams() = default;

    std::shared_ptr<segyio::SegyTraceScanner> getScanner() const;
    std::shared_ptr<segyio::DerivedHeader> getDerivedHeader() const;
    std::vector<std::shared_ptr<segyio::HeaderDefinition>> getHeaderDefinitions() const;

private:
    std::shared_ptr<segyio::SegyTraceScanner> scanner_;
    std::shared_ptr<segyio::DerivedHeader> derivedHeaders_;
    std::vector<std::shared_ptr<segyio::HeaderDefinition>> headerDefinitions_;
};

} // namespace segysdk::bin_grid_calculation