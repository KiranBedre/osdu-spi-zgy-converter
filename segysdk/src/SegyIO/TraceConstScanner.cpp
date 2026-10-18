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

#include "TraceConstScanner.h"

#include "../SegyIO/Primitives.h"
#include "../SegyIO/SegyIOException.h"

namespace segysdk::segyio
{

bool
TraceConstScanner::hasConstTraceSize(const std::shared_ptr<DerivedHeader>& derived)
{
    if (derived->hasFixedSampleCount()) {
        return true;
    }
    else {
        const size_t traceSize = segyio::Primitives::getTraceSize(derived_->getSampleFormat(), derived_->getDefaultSampleCount());

        // This is a simple assumption. Cannot be better until reads are performed
        if ((derived_->getFileSize() - derived_->getFirstTraceOffset()) % traceSize != 0) {
            return false;
        }
    }
    return true;
}

TraceConstScanner::TraceConstScanner(const std::shared_ptr<fileio::FileReader>& file, const std::shared_ptr<DerivedHeader>& derived, SegyDefinitionProvider& headerProvider, const std::vector<std::string>& headers)
  : TraceScanner(file, derived, headerProvider, headers)
{
    if (not hasConstTraceSize(derived)) {
        const std::string error = "File has a variable trace length size: " + file->getFileName();
        throw segyio::SegyFileIOException(error);
    }
}

TraceConstScanner::~TraceConstScanner()
{
}

} // namespace segysdk::segyio

