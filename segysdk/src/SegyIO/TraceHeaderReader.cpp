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

#include "TraceHeaderReader.h"

#include "Primitives.h"
#include "SegyDefinitionProvider.h"

#include "../FileIO/FileReader.h"
#include "../SegyIO/ValueConversion/HeaderValueArrayExtractor.h"

#include <limits>
#include <utility>

namespace segysdk::segyio
{

using valueconversion::HeaderValueArrayExtractor;

TraceHeaderReader::TraceHeaderReader(
    std::shared_ptr<fileio::FileReader> file,
    SegyDefinitionProvider& definitionProvider,
    const std::vector<std::string>& headers,
    const ConversionDetails& conversionDetails)
  : file_(std::move(file)),
    extractor_(std::make_unique<HeaderValueArrayExtractor>(definitionProvider, headers, conversionDetails))
{
    extractor_->addOrphanScalars();
    indexMap_ = extractor_->getOutputOrder();
    values_.resize(extractor_->getValueCount());
}

bool
TraceHeaderReader::readHeaders(uint64_t traceOffset, double values[])
{
    bytes_ = file_->read(fileio::FileReadParams{}.withOffset(traceOffset).withSize(Primitives::getTraceHeaderSize()));
    if (bytes_.size() != Primitives::getTraceHeaderSize())
        return false;

    // extract the header values from
    extractor_->extract(bytes_.data(), values);

    return true;
}

bool
TraceHeaderReader::readHeaders(uint64_t traceOffset)
{
    if (currentOffset_ == traceOffset)
        return true;

    if (!readHeaders(traceOffset, values_.data()))
        return false;

    currentOffset_ = traceOffset;

    return true;
}

int
TraceHeaderReader::getValueCount() const
{
    return extractor_->getValueCount();
}

int
TraceHeaderReader::getValueIndex(const std::string& headerId) const
{
    const auto indexEntry = indexMap_.find(headerId);
    if (indexEntry != indexMap_.end())
        return static_cast<int>(indexEntry->second);

    return -1;
}

double
segyio::TraceHeaderReader::getValue(uint64_t traceOffset, const std::string& headerId)
{
    const auto indexEntry = indexMap_.find(headerId);
    if (indexEntry != indexMap_.end())
        return getValue(traceOffset, static_cast<int>(indexEntry->second));

    return std::numeric_limits<float>::quiet_NaN();
}

double
TraceHeaderReader::getValue(uint64_t traceOffset, int valueIndex)
{
    if (valueIndex >= 0 && valueIndex < getValueCount() && readHeaders(traceOffset)) {
        return values_[valueIndex];
    }

    return std::numeric_limits<float>::quiet_NaN();
}

const double*
TraceHeaderReader::getValues(uint64_t traceOffset)
{
    if (readHeaders(traceOffset))
        return values_.data();

    return nullptr;
}

bool
TraceHeaderReader::getValues(uint64_t traceOffset, double values[])
{
    return readHeaders(traceOffset, values);
}

bool
TraceHeaderReader::getValues(uint64_t traceOffset, std::vector<double>& values)
{
    values.resize(getValueCount());
    if (readHeaders(traceOffset, values.data()))
        return true;

    values.clear();
    return false;
}

} // namespace segysdk::segyio
