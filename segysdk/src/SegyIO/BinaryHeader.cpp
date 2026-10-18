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

#include "BinaryHeader.h"

#include "Primitives.h"

#include "../FileIO/FileReader.h"
#include "../SegyFormat/Internal/FileHeaderDefinitionStringConstsBinaryHeader.h"
#include "../SegyIO/HeaderDefinition.h"
#include "../SegyIO/HeaderOverride.h"
#include "../SegyIO/SegyIOException.h"
#include "../SegyIO/ValueConversion/ByteConverter.h"
#include "../SegyIO/ValueConversion/ValueExtractor.h"

#include <stdexcept>

namespace segysdk::segyio
{

using valueconversion::ByteConverter;
using valueconversion::ValueExtractor;

BinaryHeader::BinaryHeader(const std::shared_ptr<fileio::FileReader>& file,
                           const std::vector<std::shared_ptr<HeaderDefinition>>& headers,
                           const std::map<std::string, std::shared_ptr<HeaderOverride>>& overrides)
{
    const uint64_t binaryOffset = 3200;
    const uint64_t binarySize = 400;

    const auto bytes = file->read(fileio::FileReadParams{}.withOffset(binaryOffset).withSize(binarySize));
    if (bytes.size() < binarySize)
        return;

    // determine byte swapping
    SampleFormat sampleFormat = getSampleFormat(*(uint16_t*)(&bytes[24]));
    if (Primitives::getSampleSize(sampleFormat) != 0) {
        isByteSwapped_ = false;
    }
    else {
        sampleFormat = getSampleFormat(static_cast<int>(ByteConverter::convertValue(&bytes[24], SampleFormat::INT2, true, isByteSwapped_)));
        if (Primitives::getSampleSize(sampleFormat) == 0)
            throw SegyFormatException("Unable to determine byte order, unrecognized sample format. Bytes (decimal): [" + std::to_string(bytes[24]) + ", " + std::to_string(bytes[25]) + "]");
    }

    // extract header values
    for (const auto& header : headers) {
        auto hdrOverride = overrides.find(header->getHeaderId());
        if (hdrOverride != overrides.end()) {
            valueMap_[header->getHeaderId()] = static_cast<int>(hdrOverride->second->getValue());
        }
        else {
            const uint64_t offset = header->getOffset() - binaryOffset; // By legacy rule, binary offsets are absolute
            const bool isAligned = offset % header->getByteCount() == 0;
            const SampleFormat valueType = ValueExtractor::getValueType(header, sampleFormat);

            valueMap_[header->getHeaderId()] = static_cast<int>(ByteConverter::convertValue(&bytes[offset], valueType, isAligned, isByteSwapped_));
        }
    }

    InitializeSegyRevisionLevelStatus(overrides);
}

void
BinaryHeader::InitializeSegyRevisionLevelStatus(const std::map<std::string, std::shared_ptr<HeaderOverride>>& overrides)
{
    // clear revision 1 headers unless explicitly overridden
    if (getRevisionLevel() == 0) {
        std::string header = segyformat::internal::FileHeaderDefinitionStringConstsBinaryHeader::ExtendedTextualHeaderCount;
        if (hasValue(header) && overrides.find(header) == overrides.end()) {
            valueMap_[header] = 0;
        }

        header = segyformat::internal::FileHeaderDefinitionStringConstsBinaryHeader::FixedTraceLengthFlag;
        if (hasValue(header) && overrides.find(header) == overrides.end()) {
            valueMap_[header] = 0;
        }
    }
    else if (getRevisionLevel() != 1) {
        throw SegyFormatException("Unsupported or corrupt revision level.");
    }
}

bool
BinaryHeader::isValid() const
{
    return !valueMap_.empty();
}

bool
BinaryHeader::hasValue(const std::string& header) const
{
    return valueMap_.find(header) != valueMap_.end();
}

bool
BinaryHeader::hasFixedSampleCount() const
{
    return getValue(segyformat::internal::FileHeaderDefinitionStringConstsBinaryHeader::FixedTraceLengthFlag, 0) == 1;
}

bool
BinaryHeader::isMetric() const
{
    // Only assume meter when it is actually defined in the header. Assuming meter when the value is actually undefined might produce incorrect results as often as not.
    return getValue(segyformat::internal::FileHeaderDefinitionStringConstsBinaryHeader::MeasurementSystem, 1) == 1;
}

int
BinaryHeader::getRevisionLevel() const
{
    const int revision = getValue(segyformat::internal::FileHeaderDefinitionStringConstsBinaryHeader::SegyRevision, 0);
    // revision level should be stored in upper byte (but is sometimes in lower)
    return revision >= 256 ? revision / 256 : revision;
}

SampleFormat
BinaryHeader::getSampleFormat() const
{
    return getSampleFormat(getValue(segyformat::internal::FileHeaderDefinitionStringConstsBinaryHeader::SampleDataFormat, 0));
}

int
BinaryHeader::getSampleCount() const
{
    return getValue(segyformat::internal::FileHeaderDefinitionStringConstsBinaryHeader::TraceLength, 0);
}

int
BinaryHeader::getExtendedTextualHeaderCount() const
{
    return getValue(segyformat::internal::FileHeaderDefinitionStringConstsBinaryHeader::ExtendedTextualHeaderCount, 0);
}

int
BinaryHeader::getValue(const std::string& header) const
{
    // TODO dangerous: caller must know that -1 is the "not found" value; failing or using an std::optional would be safer
    return getValue(header, -1);
}

// TODO in callers: is it legal that trace length, sample format... are missing (how?) or are 0?
int
BinaryHeader::getValue(const std::string& header, int defaultValue) const
{
    const auto entry = valueMap_.find(header);
    return (entry != valueMap_.end()) ? entry->second : defaultValue;
}

SampleFormat
BinaryHeader::getSampleFormat(int headerValue)
{
    switch (headerValue) {
    case 1: return SampleFormat::IBMFloat4;
    case 2: return SampleFormat::INT4;
    case 3: return SampleFormat::INT2;
    case 4: return SampleFormat::FixedPointWithGain4;
    case 5: return SampleFormat::IeeeFloat4;
    case 8: return SampleFormat::INT1;
    case 6: return SampleFormat::IeeeFloat8;
    case 7: return SampleFormat::INT3;
    case 10: return SampleFormat::UINT4;
    case 11: return SampleFormat::UINT2;
    case 15: return SampleFormat::UINT3;
    case 16: return SampleFormat::UINT1;

    // TODO why not fail? and if 0 is OK then why isn't is added to the enum (e.g. SampleFormat::Unknown)
    default: return static_cast<SampleFormat>(0);
    }
}

} // namespace segysdk::segyio
