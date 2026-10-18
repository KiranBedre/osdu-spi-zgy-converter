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

#include "ValueExtractor.h"

#include "ByteConverter.h"
#include "../SegyIO/ConversionDetails.h"
#include "../FileIO/FileIOException.h"
#include "../SegyIO/HeaderDefinition.h"
#include "../SegyIO/HeaderOverride.h"

#include <utility>
#include <limits>

namespace segysdk::segyio::valueconversion
{

ValueExtractor::ValueExtractor(const std::shared_ptr<const HeaderDefinition>& definition, const ConversionDetails& conversionDetails)
  : name_(definition->getHeaderId()),
    valueType_(getValueType(definition, conversionDetails.getSampleValueType())),
    byteOffset_(definition->getOffset()),
    byteSize_(definition->getByteCount()),
    bufferOffset_(definition->getOffset()),
    fixedValue_(0),
    needsByteSwap_(conversionDetails.needsByteSwap()),
    isAligned_(false),
    hasFixedValue_(false)
{
    if (valueType_ == SampleFormat::INT1) // unsupported format
    {
        hasFixedValue_ = true;
        fixedValue_ = std::numeric_limits<double>::quiet_NaN();
        byteSize_ = 0;
        return;
    }
    if (definition->getScalarUsageStrategy() == ScalarUsageStrategy::Override) {
        hasScalarOverrideValue_ = true;
        scalarOverrideValue_ = definition->getScalarOverride();
    }
    if (definition->isOverridden()) {
        hasFixedValue_ = true;
        fixedValue_ = definition->getCachedHeaderOverride()->getValue();
    }
}

ValueExtractor::ValueExtractor(std::string headerId, int valueOffset)
  : name_(std::move(headerId)),
    valueOffset_(valueOffset),
    fixedValue_(std::numeric_limits<double>::quiet_NaN()),
    isAligned_(true),
    hasFixedValue_(true),
    hasValueOffset_(valueOffset >= 0),
    converter_(nullptr)
{
    // Dummy NaN producer, only used on error conditions
}

ValueExtractor::~ValueExtractor()
{}

void
ValueExtractor::setBufferOffset(size_t bufferOffset)
{
    bufferOffset_ = bufferOffset;
    if (byteSize_ > 0) {
        isAligned_ = (bufferOffset % getByteSize()) == 0;
    }
}

void
ValueExtractor::setValueOffset(int valueOffset)
{
    valueOffset_ = valueOffset;
    hasValueOffset_ = valueOffset >= 0; // for now
}

void
ValueExtractor::setFixedValue(double fixedValue)
{
    fixedValue_ = fixedValue;
    hasFixedValue_ = true;
}

void
ValueExtractor::prepareExtraction()
{
    if (hasFixedValue())
        return;

    // Ensure alignment is properly set
    setBufferOffset(bufferOffset_);
    converter_ = ByteConverter::create(getValueType(), isAligned(), needsByteSwap());
}

SampleFormat
ValueExtractor::getValueType(const std::shared_ptr<const HeaderDefinition>& headerDefinition, SampleFormat sampleValueType)
{
    switch (headerDefinition->getHeaderDataType()) {
    case HeaderDataType::IBMFloat:
        if (headerDefinition->getByteCount() == 4)
            return SampleFormat::IBMFloat4;
        else if (headerDefinition->getByteCount() == 8)
            return SampleFormat::IeeeFloat8; // This is probably incorrect, but the only floating point option for 8 bytes
        else {
            // TODO: exception?
        }
        break;
    case HeaderDataType::IEEEFloat:
        if (headerDefinition->getByteCount() == 4)
            return SampleFormat::IeeeFloat4;
        else if (headerDefinition->getByteCount() == 8)
            return SampleFormat::IeeeFloat8;
        else {
            // TODO: exception?
        }
        break;
    case HeaderDataType::Float:
        if (headerDefinition->getByteCount() == 8) {
            return SampleFormat::IeeeFloat8;
        }

        if (headerDefinition->getByteCount() == 4) {
            switch (sampleValueType) {
            case SampleFormat::FixedPointWithGain4:
            case SampleFormat::IBMFloat4:
            case SampleFormat::IeeeFloat4:
                return sampleValueType;

            default:
                return SampleFormat::IeeeFloat4; // best guess
            }
        }
        break;

    case HeaderDataType::Integer:
        switch (headerDefinition->getByteCount()) {
        case 2:
            return SampleFormat::INT2;

        case 4:
            return SampleFormat::INT4;
        default:
            throw SegyInvalidArgumentException("Internal error in ValueExtractor::getValueType");
        }
        break;

    case HeaderDataType::UnsignedInteger:
        switch (headerDefinition->getByteCount()) {
        case 2:
            return SampleFormat::UINT2;

        case 4:
            return SampleFormat::UINT4;
        default:
            throw SegyInvalidArgumentException("Internal error in ValueExtractor::getValueType");
        }
        break;
    default: return SampleFormat::INT1;
    }
    return SampleFormat::INT1;
}

} // namespace segysdk::segyio::valueconversion
