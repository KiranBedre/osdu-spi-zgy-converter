/*
Copyright 2022 Schlumberger

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

#include "../SegyIO/TraceScannerBase.h"
#include "../FileIO/FileReader.h"
#include "../SegyIO/DerivedHeader.h"
#include "../SegyIO/SegyDefinitionProvider.h"

#include <memory>
#include <vector>

namespace segysdk::segyio
{

class SEGYSDK_DLL_PUBLIC TraceSequentialScanner final : public TraceScannerBase
{
private:
    // non-copyable - not implemented
    TraceSequentialScanner(const TraceSequentialScanner& other) = delete;
    TraceSequentialScanner& operator=(const TraceSequentialScanner& other) = delete;

protected:
    virtual bool findNextTrace(const request::TraceParams& params, double* values) override;

private:
    void initReader(std::vector<std::shared_ptr<fileio::FileReader>>&& files, SegyDefinitionProvider& headerProvider);

public:
    TraceSequentialScanner(std::vector<std::shared_ptr<fileio::FileReader>>&& files, const std::shared_ptr<DerivedHeader>& derived, SegyDefinitionProvider& provider, const std::vector<std::string>& headers);
    virtual ~TraceSequentialScanner() override;
};

} // namespace segysdk::segyio
