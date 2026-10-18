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

#include "HeaderValueArrayExtractor.h"

#include "ByteConverter.h"
#include "ByteValuePosition.h"
#include "HeaderValueExtractor.h"
#include "ScalarValueExtractor.h"

#include "../SegyIO/SegyIOException.h"

#include <algorithm>
#include <list>
#include <string>

namespace segysdk::segyio::valueconversion
{

class HeaderValueArrayExtractor::GroupConverter
{
private:
    std::unique_ptr<ByteConverter> converter_;
    ByteValuePositions positions_;

public:
    GroupConverter(SampleFormat valueType, bool aligned, bool needsByteSwap)
    {
        converter_ = ByteConverter::create(valueType, aligned, needsByteSwap);
    }

    ~GroupConverter() {}

    void
    addPosition(size_t byteOffset, size_t valueOffset)
    {
        positions_.emplace_back(byteOffset, valueOffset);
    }

    void
    convert(const uint8_t* buffer, double* headerValues) const
    {
        converter_->convert(buffer, headerValues, positions_);
    }
};

HeaderValueArrayExtractor::HeaderValueArrayExtractor(SegyDefinitionProvider& definitionProvider, const std::vector<std::string>& headerNames, const ConversionDetails& conversionDetails)
  : valueCount_(headerNames.size()),
    needsByteSwap_(conversionDetails.needsByteSwap())
{
    std::vector<std::string> scalarNames;
    std::vector isScalar(headerNames.size(), false);
    std::vector<std::shared_ptr<const HeaderDefinition>> definitions(headerNames.size(), nullptr);
    std::vector<ScalarValueExtractor*> headerScalars(headerNames.size(), nullptr);

    extractors_.reserve(headerNames.size());

    for (size_t i = 0; i < headerNames.size(); ++i) {
        if (isScalar[i])
            continue;

        std::shared_ptr<const HeaderDefinition> definition = definitionProvider.getTraceHeaderDefinition(headerNames[i]);
        if (definition == nullptr)
            continue; // ERROR

        definitions[i] = definition;
        if (!definition->needsScalar())
            continue;

        const std::string& scalarName = definition->getScalarHeaderId();
        auto scalar_it = std::find(scalarNames.begin(), scalarNames.end(), scalarName);
        if (scalar_it != scalarNames.end()) {
            size_t pos = scalar_it - scalarNames.begin();
            headerScalars[i] = scalars_[pos].get();
            continue;
        }

        std::shared_ptr<const HeaderDefinition> scalarDefinition = definitionProvider.getTraceHeaderDefinition(scalarName);
        if (scalarDefinition == nullptr)
            continue; // ERROR

        auto scalar = std::make_shared<ScalarValueExtractor>(scalarDefinition, conversionDetails);
        scalars_.emplace_back(scalar);
        extractors_.emplace_back(scalar);
        scalarNames.emplace_back(scalarName);
        headerScalars[i] = scalar.get();

        auto header_it = std::find(headerNames.begin(), headerNames.end(), scalarName);
        if (header_it != headerNames.end()) {
            size_t pos = header_it - headerNames.begin();
            isScalar[pos] = true;
            scalar->setValueOffset(static_cast<int>(pos));
        }
    }

    headers_.reserve(headerNames.size());

    for (size_t i = 0; i < headerNames.size(); ++i) {
        if (isScalar[i])
            continue;

        auto definition = definitions[i];
        if (definition == nullptr) {
            headers_.emplace_back(std::make_shared<HeaderValueExtractor>(headerNames[i], static_cast<int>(i)));
            continue;
        }

        auto header = std::make_shared<HeaderValueExtractor>(definition, conversionDetails);
        header->setValueOffset(static_cast<int>(i));
        if (!header->hasScalarOverride()) {
            header->setScalar(headerScalars[i]);
        }
        
        headers_.emplace_back(header);
        extractors_.emplace_back(header);
    }

    calculateByteExtent();
}

HeaderValueArrayExtractor::~HeaderValueArrayExtractor()
{
}

void
HeaderValueArrayExtractor::setBufferOffset(size_t bufferOffset)
{
    bufferOffset_ = bufferOffset;
    for (auto it = extractors_.begin(); it != extractors_.end(); ++it) {
        (*it)->setBufferOffset((*it)->getByteOffset() - bufferOffset);
    }
}

void
HeaderValueArrayExtractor::setFixedHeaderValues(const std::vector<std::string>& headerNames, const std::vector<double>& values)
{
    if (isPrepared())
        throw segyio::SegyRuntimeException("Cannot set fixed headers at this stage.");

    const size_t count = headerNames.size();
    for (size_t i = 0; i < count; ++i) {
        const std::string& headerName = headerNames[i];

        auto it = std::find_if(extractors_.begin(), extractors_.end(),
                               [&](const auto& ext_ptr) { return ext_ptr->getName() == headerName; });
        if (it != extractors_.end() && !(*it)->hasFixedValue()) {
            (*it)->setFixedValue(values[i]);
        }
    }

    calculateByteExtent();
}

int
HeaderValueArrayExtractor::addOrphanScalars()
{
    if (isPrepared())
        throw segyio::SegyRuntimeException("Cannot add orphan scalars at this stage.");

    if (scalars_.empty())
        return 0;

    // Reserve space in output array for orphan scalars
    // May have a slight benign performance effect in current implementation
    const size_t oldValueCount = valueCount_;
    const size_t count = scalars_.size();
    for (size_t i = 0; i < count; ++i) {
        std::shared_ptr<ScalarValueExtractor> scalar = scalars_[i];
        if (!scalar->hasValueOffset() && !scalar->hasFixedValue()) {
            scalar->setValueOffset(static_cast<int>(valueCount_));
            valueCount_++;
        }
    }

    return getValueCount() - oldValueCount;
}

void
HeaderValueArrayExtractor::calculateByteExtent()
{
    size_t minOffset = std::numeric_limits<size_t>::max();
    size_t maxOffset = std::numeric_limits<size_t>::min();
    size_t maxSize = 1;
    size_t maxSizeOffset = 0;

    for (auto it = extractors_.begin(); it != extractors_.end(); ++it) {
        const auto& extractor = *(*it);
        if (extractor.hasFixedValue())
            continue;

        minOffset = std::min(minOffset, extractor.getByteOffset());
        maxOffset = std::max(maxOffset, extractor.getByteOffset() + extractor.getByteSize());
        if (extractor.getByteSize() > maxSize) {
            maxSize = extractor.getByteSize();
            maxSizeOffset = extractor.getByteOffset();
        }
    }

    byteAlignment_ = maxSize;

    if (minOffset > maxOffset) {
        byteOffset_ = 0;
        byteSize_ = 0;
    }
    else {
        byteOffset_ = minOffset;
        byteSize_ = maxOffset - minOffset;

        // Try to align values
        if (maxSize > 1 && ((maxSizeOffset - minOffset) % maxSize) != 0) {
            const size_t offset = ((maxSizeOffset - minOffset) + (maxSize - 1)) % maxSize;
            if (offset <= maxSizeOffset) {
                byteOffset_ = maxSizeOffset - offset;
                byteSize_ = maxOffset - byteOffset_;
            }
        }
    }
}

void
HeaderValueArrayExtractor::buildConverters()
{
    std::list<ValueExtractor*> candidates;

    for (auto it = extractors_.begin(); it != extractors_.end(); ++it) {
        if ((*it)->hasValueOffset() && !(*it)->hasFixedValue()) {
            candidates.push_back(it->get());
        }
    }

    while (!candidates.empty()) {
        const ValueExtractor* extractor = candidates.front();
        auto group = std::make_shared<GroupConverter>(extractor->getValueType(), extractor->isAligned(), needsByteSwap_);
        group->addPosition(extractor->getBufferOffset(), extractor->getValueOffset());
        converters_.emplace_back(std::move(group));
        candidates.pop_front();

        for (auto it = candidates.begin(); it != candidates.end();) {
            if ((*it)->getValueType() == extractor->getValueType() && (*it)->isAligned() == extractor->isAligned()) {
                converters_.back()->addPosition((*it)->getBufferOffset(), (*it)->getValueOffset());
                it = candidates.erase(it);
            }
            else {
                ++it;
            }
        }
    }
}

void
HeaderValueArrayExtractor::buildTraverses()
{
    // Scanning scalar definitions.
    for (const auto& scalar_ptr : scalars_) {
        if (!scalar_ptr->hasValueOffset() && !scalar_ptr->hasFixedValue()) {
            orphanScalars_.push_back(scalar_ptr.get());
        }

        else if (scalar_ptr->hasFixedValue()) {
            fixedValues_.push_back(scalar_ptr.get());
        }
        else {
            dynamicScalars_.push_back(scalar_ptr.get());
        }
    }
    // Scanning trace header definitions
    for (const auto& header_ptr : headers_) {
        if (header_ptr->hasValueOffset() && header_ptr->hasFixedValue()) {
            fixedValues_.push_back(header_ptr.get());
        }
        else if (header_ptr->hasProcessing() or header_ptr->hasScalarOverride()) {
            dynamicHeaders_.push_back(header_ptr.get());
        }
    }
}

void
HeaderValueArrayExtractor::prepareExtraction()
{
    if (!isPrepared()) {
        for (auto it = scalars_.begin(); it != scalars_.end(); ++it) {
            (*it)->prepareExtraction();
        }

        for (auto it = headers_.begin(); it != headers_.end(); ++it) {
            (*it)->prepareExtraction();
        }

        buildConverters();
        buildTraverses();
        isPrepared_ = true;
    }
}

void
HeaderValueArrayExtractor::convert(const uint8_t* buffer, double* headerValues)
{
    for (const auto& converter_ptr : converters_) {
        converter_ptr->convert(buffer, headerValues);
    }
}

void
HeaderValueArrayExtractor::extract(const uint8_t* buffer, double headerValues[])
{
    if (!isPrepared()) {
        prepareExtraction();
    }

    convert(buffer, headerValues);

    /**
     *  Order of operation
     *   1. convert() (above) takes raw bytes and converts them into doubles.
     *   2. extract scalar values
     *   3. apply any Fixed values (overrides)
     *   4. apply any Scalars
     *   6. apply any unit scalars and unit conversion
     **/

    for (const auto& extract_ptr : orphanScalars_) {
        extract_ptr->extractValue(buffer);
    }

    for (const auto& dynamic_scalar_ptr : dynamicScalars_) {
            dynamic_scalar_ptr->setScaleValue(headerValues[dynamic_scalar_ptr->getValueOffset()]);
    }

    for (const auto& fixed_val_ptr : fixedValues_) {
        headerValues[fixed_val_ptr->getValueOffset()] = fixed_val_ptr->getFixedValue();
    }

    // split the dynamic header activity into two
    // 1. apply any scalars
    // 2. apply any unit conversion
    // this means that the calculated headers are correctly applied to the scaled values BEFORE unit conversion
    for (const auto& dynamic_header_ptr : dynamicHeaders_) {
        dynamic_header_ptr->applyScalar(headerValues[dynamic_header_ptr->getValueOffset()]);
    }

    for (const auto& dynamic_header_ptr : dynamicHeaders_) {
        dynamic_header_ptr->applyUnitConversion(headerValues[dynamic_header_ptr->getValueOffset()]);
    }
}

void
HeaderValueArrayExtractor::extractRaw(const uint8_t* buffer, double headerValues[])
{
    if (!isPrepared()) {
        prepareExtraction();
    }

    convert(buffer, headerValues);

    /**
     *  No further corrections applied to the RAW trace header values.
     **/

    for (const auto& dynamic_header_ptr : dynamicHeaders_) {
        dynamic_header_ptr->applyUnitConversion(headerValues[dynamic_header_ptr->getValueOffset()]);
    }
}

std::map<std::string, size_t>
HeaderValueArrayExtractor::getOutputOrder()
{
    if (!isPrepared()) {
        prepareExtraction();
    }

    return getOutputOrderInternal();
}

std::map<std::string, size_t>
HeaderValueArrayExtractor::getOutputOrderInternal() const
{
    std::map<std::string, size_t> ordering;
    std::list<size_t> values;
    for (const auto& ext : extractors_) {
        if (ext->getValueOffset() >= 0) {
            std::string attrName = ext->getName();
            int valueOffset = ext->getValueOffset();
            ordering.try_emplace(attrName, valueOffset);
            values.push_back(valueOffset);
        }
    }
    if (ordering.size() != valueCount_ || values.size() != ordering.size())
        throw segyio::SegyFormatException("Expected header ordering size to match the number of values");
    // verify that the values 0 to getValueCount() - 1 are represented in the ordering
    if (std::any_of(values.begin(), values.end(), [&](const auto value) { return (value > ordering.size()); })) {
        throw segyio::SegyFormatException("Expected header output values to be self-consistent");
    }
    return ordering;
}

} // namespace segysdk::segyio::valueconversion
