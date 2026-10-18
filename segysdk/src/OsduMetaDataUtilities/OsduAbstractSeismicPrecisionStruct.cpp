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

#include "../OsduMetaDataUtilities/OsduAbstractSeismicPrecisionStruct.h"
#include "../Logger/Logger.h"

namespace segysdk::metadata::osdu
{
OsduAbstractSeismicPrecisionStruct::OsduAbstractSeismicPrecisionStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition)),
    helper_(getPartition())
{
    /* Empty */
}

OsduAbstractSeismicPrecisionStruct::OsduAbstractSeismicPrecisionStruct(const OsduAbstractSeismicPrecisionStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    wordWidth(other.wordWidth),
    wordFormat(other.wordFormat),
    helper_(other.helper_)
{
    /* Empty */
}

OsduAbstractSeismicPrecisionStruct::OsduAbstractSeismicPrecisionStruct(OsduAbstractSeismicPrecisionStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    wordWidth(other.wordWidth),
    wordFormat(std::move(other.wordFormat)),
    helper_(std::move(other.helper_))
{
    /* Empty */
}

OsduAbstractSeismicPrecisionStruct&
OsduAbstractSeismicPrecisionStruct::operator=(const OsduAbstractSeismicPrecisionStruct& rhs)
{
    return *this = OsduAbstractSeismicPrecisionStruct(rhs);
}

OsduAbstractSeismicPrecisionStruct&
OsduAbstractSeismicPrecisionStruct::operator=(OsduAbstractSeismicPrecisionStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    wordWidth = rhs.wordWidth;
    wordFormat = std::move(rhs.wordFormat);
    helper_ = std::move(rhs.helper_);

    return *this;
}

bool
OsduAbstractSeismicPrecisionStruct::isValid() const
{
    return (wordWidth.has_value() && wordFormat.has_value()) || (!wordWidth.has_value() && !wordFormat.has_value());
}

std::vector<std::string>
OsduAbstractSeismicPrecisionStruct::performAttributeValidityScan() const
{
    if (!isValid()) {
        return {"OsduAbstractSeismicPrecisionStruct::isValid() failed."};
    }
    return {};
}

int
OsduAbstractSeismicPrecisionStruct::getSampleSize(const OpenZGY::SampleDataType sampleSize) const
{
    switch (sampleSize) {
    case OpenZGY::SampleDataType::int8:
        return 1;
    case OpenZGY::SampleDataType::int16:
        return 2;
    case OpenZGY::SampleDataType::float32:
    case OpenZGY::SampleDataType::unknown:
        return 4;
    }

    return 4;
}

int
OsduAbstractSeismicPrecisionStruct::getSampleSize(const segysdk::segyio::SampleFormat sampleFormat) const
{
    switch (sampleFormat) {
    case segysdk::segyio::SampleFormat::UINT1:
    case segysdk::segyio::SampleFormat::INT1:
        return 1;

    case segysdk::segyio::SampleFormat::UINT2:
    case segysdk::segyio::SampleFormat::INT2:
        return 2;

    case segysdk::segyio::SampleFormat::UINT4:
    case segysdk::segyio::SampleFormat::INT4:
    case segysdk::segyio::SampleFormat::FixedPointWithGain4:
    case segysdk::segyio::SampleFormat::IBMFloat4:
    case segysdk::segyio::SampleFormat::IeeeFloat4:
        return 4;

    case segysdk::segyio::SampleFormat::IeeeFloat8:
        return 8;

    default:
        return 4;
    }

    return 4;
}

std::string
OsduAbstractSeismicPrecisionStruct::getPrecisionWord(OpenZGY::SampleDataType sampleType) const
{
    switch (sampleType) {
    case OpenZGY::SampleDataType::int8:
    case OpenZGY::SampleDataType::int16:
        return helper_.osduReferenceLookupString(WordFormatTypeReference, WordFormatINT);

    case OpenZGY::SampleDataType::float32:
    case OpenZGY::SampleDataType::unknown:        
    default:
        return helper_.osduReferenceLookupString(WordFormatTypeReference, WordFormatIEEEFLOAT);
    }

    return helper_.osduReferenceLookupString(WordFormatTypeReference, WordFormatIEEEFLOAT);
}

std::string
OsduAbstractSeismicPrecisionStruct::getPrecisionWord(const segysdk::segyio::SampleFormat sampleFormat) const
{
    switch (sampleFormat) {
    case segysdk::segyio::SampleFormat::INT1:
    case segysdk::segyio::SampleFormat::INT2:
    case segysdk::segyio::SampleFormat::INT4:
        return helper_.osduReferenceLookupString(WordFormatTypeReference, WordFormatINT);

    case segysdk::segyio::SampleFormat::IBMFloat4:
        return helper_.osduReferenceLookupString(WordFormatTypeReference, WordFormatIBMFLOAT);

    case segysdk::segyio::SampleFormat::IeeeFloat4:
    default:
        return helper_.osduReferenceLookupString(WordFormatTypeReference, WordFormatIEEEFLOAT);
    }

    return helper_.osduReferenceLookupString(WordFormatTypeReference, WordFormatIEEEFLOAT);
}

void
OsduAbstractSeismicPrecisionStruct::setPrecision(const segysdk::segyio::SampleFormat sampleFormat)
{
    wordFormat = getPrecisionWord(sampleFormat);
    wordWidth = getSampleSize(sampleFormat);
}

void
OsduAbstractSeismicPrecisionStruct::setPrecision(const OpenZGY::SampleDataType zgySampleType)
{
    wordFormat = getPrecisionWord(zgySampleType);
    wordWidth = getSampleSize(zgySampleType);
}

} // namespace segysdk::metadata::osdu
