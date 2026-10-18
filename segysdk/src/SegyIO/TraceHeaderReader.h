/*
Copyright 2021 Schlumberger

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

#include "../SegyIO/ValueConversion/HeaderValueArrayExtractor.h"
#include "../SegyIO/ConversionDetails.h"
#include "../SegyIO/SegyDefinitionProvider.h"
#include "../FileIO/FileReader.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

namespace segysdk::segyio
{

class SEGYSDK_DLL_PUBLIC TraceHeaderReader
{
private:
    std::shared_ptr<fileio::FileReader> file_;
    std::unique_ptr<valueconversion::HeaderValueArrayExtractor> extractor_;
    std::map<std::string, size_t> indexMap_;
    uint64_t currentOffset_{};
    std::vector<uint8_t> bytes_;
    std::vector<double> values_;

private:
    bool readHeaders(uint64_t traceOffset, double values[]);
    bool readHeaders(uint64_t traceOffset);

public:
    TraceHeaderReader(const TraceHeaderReader& other) = delete;
    TraceHeaderReader& operator=(const TraceHeaderReader& other) = delete;

    TraceHeaderReader(std::shared_ptr<fileio::FileReader> file, SegyDefinitionProvider& headerProvider, const std::vector<std::string>& headers, const ConversionDetails& details);

    int getValueCount() const;
    int getValueIndex(const std::string& headerId) const;

    bool hasValueFor(const std::string& headerId) const { return getValueIndex(headerId) != -1; }

    double getValue(uint64_t traceOffset, const std::string& headerId);
    double getValue(uint64_t traceOffset, int valueIndex);

    const double* getValues(uint64_t traceOffset);
    bool getValues(uint64_t traceOffset, double values[]);
    bool getValues(uint64_t traceOffset, std::vector<double>& values);
};

} // namespace segysdk::segyio
