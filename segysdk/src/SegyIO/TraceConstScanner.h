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

#include "../segysdk_details/DLL_EXPORT.h"

#include "../FileIO/FileReader.h"
#include "../SegyIO/DerivedHeader.h"
#include "../SegyIO/SegyDefinitionProvider.h"
#include "../SegyIO/TraceScanner.h"

#include <memory>
#include <vector>

namespace segysdk::segyio
{

// TraceConstScanner should be used only to read same size traces.
class SEGYSDK_DLL_PUBLIC TraceConstScanner : public TraceScanner
{
private:
    // non-copyable - not implemented
    TraceConstScanner(const TraceConstScanner& other) = delete;
    TraceConstScanner& operator=(const TraceConstScanner& other) = delete;

protected:
    virtual bool hasVariableSampleCount() const override { return false; }

private:
    bool hasConstTraceSize(const std::shared_ptr<DerivedHeader>& derived);

public:
    TraceConstScanner(const std::shared_ptr<fileio::FileReader>& file, const std::shared_ptr<DerivedHeader>& derived, SegyDefinitionProvider& provider, const std::vector<std::string>& headers);
    virtual ~TraceConstScanner() override;
};

} // namespace segysdk::segyio
