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

#include "ByteConverter.h"

#include "../SegyIO/HeaderDefinition.h"
#include "../SegyIO/ConversionDetails.h"
#include "../SegyIO/ValueConversion/ByteConverter.h"
#include "../SegyIO/SampleFormat.h"

#include <cstddef>
#include <memory>
#include <string>

namespace segysdk::segyio::valueconversion
{

class SEGYSDK_DLL_PUBLIC ValueExtractor
{
private:
    std::string name_;
    SampleFormat valueType_ = SampleFormat::INT1;
    size_t byteOffset_ = 0;
    size_t byteSize_ = 0;
    size_t bufferOffset_ = 0;
    int valueOffset_ = -1;
    double fixedValue_;
    bool needsByteSwap_ = false;
    bool isAligned_;
    bool hasFixedValue_;
    bool hasValueOffset_ = false;
    bool hasScalarOverrideValue_ = false;
    double scalarOverrideValue_ = 1.0;


protected:
    std::unique_ptr<ByteConverter> converter_ = nullptr;

protected:
    ValueExtractor(const std::shared_ptr<const HeaderDefinition>&, const ConversionDetails& conversionDetails);
    ValueExtractor(std::string headerId, int valueOffset);

protected:
    virtual void prepareExtraction();

public:
    ValueExtractor(const ValueExtractor& other) = delete;
    ValueExtractor& operator=(const ValueExtractor& other) = delete;

    virtual ~ValueExtractor();

    const std::string& getName() const { return name_; }
    SampleFormat getValueType() const { return valueType_; }
    size_t getByteOffset() const { return byteOffset_; }
    size_t getByteSize() const { return byteSize_; }
    size_t getBufferOffset() const { return bufferOffset_; }
    int getValueOffset() const { return valueOffset_; }
    double getFixedValue() const { return fixedValue_; }
    double getScalarOverrideValue() const { return scalarOverrideValue_; }
    bool needsByteSwap() const { return needsByteSwap_; }
    bool isAligned() const { return isAligned_; }
    bool hasFixedValue() const { return hasFixedValue_; }
    bool hasScalarOverrideValue() const { return hasScalarOverrideValue_; }
    bool hasValueOffset() const { return hasValueOffset_; }

    void setBufferOffset(size_t bufferOffset);
    void setValueOffset(int valueOffset);
    void setFixedValue(double fixedValue);

public:
    static SampleFormat getValueType(const std::shared_ptr<const HeaderDefinition>& headerDefinition, SampleFormat sampleValueType);
};

} // namespace segysdk::segyio::valueconversion
