/*
Copyright 2021-2022 Schlumberger

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

#include "../BulkAccess/BufferedTraceHeaderReader.h"

#include "../FileIO/BufferedFileReader.h"
#include "../SegyIO/Primitives.h"
#include "../SegyIO/SegyDefinitionProvider.h"
#include "../SegyIO/ValueConversion/HeaderValueArrayExtractor.h"

#include <limits>
#include <utility>

namespace segysdk::bulkaccess
{
using namespace fileio;
using namespace segyio;
using namespace valueconversion;

template<typename READER>
BufferedTraceHeaderReader<READER>::BufferedTraceHeaderReader(
    std::shared_ptr<READER> file,
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

template<typename READER>
bool
BufferedTraceHeaderReader<READER>::readHeaders(uint64_t traceOffset, double values[]) const
{
    const uint8_t* bytes = file_->readAligned(traceOffset, extractor_->getReadSize(), extractor_->getByteAlignment());
    if (bytes == nullptr)
        return false;

    // extract the header values from
    extractor_->extract(bytes, values);

    return true;
}

template<typename READER>
bool
BufferedTraceHeaderReader<READER>::readHeaders(uint64_t traceOffset)
{
    if (currentOffset_ == traceOffset)
        return true;

    if (!readHeaders(traceOffset, values_.data()))
        return false;

    currentOffset_ = traceOffset;

    return true;
}

template<typename READER>
int
BufferedTraceHeaderReader<READER>::getValueCount() const
{
    return extractor_->getValueCount();
}

template<typename READER>
int
BufferedTraceHeaderReader<READER>::getValueIndex(const std::string& headerId) const
{
    const auto indexEntry = indexMap_.find(headerId);
    if (indexEntry != indexMap_.end())
        return static_cast<int>(indexEntry->second);

    return -1;
}

template<typename READER>
double
BufferedTraceHeaderReader<READER>::getValue(uint64_t traceOffset, const std::string& headerId)
{
    const auto indexEntry = indexMap_.find(headerId);
    if (indexEntry != indexMap_.end())
        return getValue(traceOffset, static_cast<int>(indexEntry->second));

    return std::numeric_limits<float>::quiet_NaN();
}

template<typename READER>
double
BufferedTraceHeaderReader<READER>::getValue(uint64_t traceOffset, int valueIndex)
{
    if ((valueIndex >= 0 && valueIndex < getValueCount()) && readHeaders(traceOffset))
        return values_[valueIndex];

    return std::numeric_limits<float>::quiet_NaN();
}

template<typename READER>
const double*
BufferedTraceHeaderReader<READER>::getValues(uint64_t traceOffset)
{
    if (readHeaders(traceOffset))
        return values_.data();

    return nullptr;
}

template<typename READER>
bool
BufferedTraceHeaderReader<READER>::getValues(uint64_t traceOffset, double values[]) const
{
    return readHeaders(traceOffset, values);
}

template<typename READER>
bool
BufferedTraceHeaderReader<READER>::getValues(uint64_t traceOffset, std::vector<double>& values) const
{
    values.resize(getValueCount());
    if (readHeaders(traceOffset, values.data()))
        return true;

    values.clear();
    return false;
}

} // namespace segysdk::bulkaccess
