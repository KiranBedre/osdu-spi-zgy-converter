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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "HeaderValueExtractor.h"
#include "ScalarValueExtractor.h"

#include "../SegyIO/ConversionDetails.h"
#include "../SegyIO/SegyDefinitionProvider.h"

#include <cstdint>
#include <map>
#include <memory>
#include <vector>

namespace segysdk::segyio::valueconversion
{

// export for testing
class SEGYSDK_DLL_PUBLIC HeaderValueArrayExtractor
{
private:
    class GroupConverter;

private:
    std::vector<std::shared_ptr<ScalarValueExtractor>> scalars_;
    std::vector<std::shared_ptr<HeaderValueExtractor>> headers_;
    std::vector<std::shared_ptr<ValueExtractor>> extractors_;
    std::vector<std::shared_ptr<GroupConverter>> converters_;
    size_t byteOffset_ = 0;
    size_t byteSize_ = 0;
    size_t byteAlignment_ = 1;
    size_t bufferOffset_ = 0;
    size_t valueCount_;
    bool needsByteSwap_;
    bool isPrepared_ = false;

    // These pointers are same to the existing from scalars_ and headers_
    std::vector<ValueExtractor*> fixedValues_;
    std::vector<ScalarValueExtractor*> orphanScalars_;
    std::vector<ScalarValueExtractor*> dynamicScalars_;
    std::vector<HeaderValueExtractor*> dynamicHeaders_;

private:
    bool isPrepared() const { return isPrepared_; }

    void calculateByteExtent();
    void buildConverters();
    void buildTraverses();
    void prepareExtraction();
    void convert(const uint8_t* buffer, double* headerValues);

    std::map<std::string, size_t> getOutputOrderInternal() const;

public:
    HeaderValueArrayExtractor(const HeaderValueArrayExtractor& other) = delete;
    HeaderValueArrayExtractor& operator=(const HeaderValueArrayExtractor& other) = delete;

    HeaderValueArrayExtractor(SegyDefinitionProvider& definitionProvider, const std::vector<std::string>& headers, const ConversionDetails& conversionDetails);
    virtual ~HeaderValueArrayExtractor();

    size_t getByteOffset() const { return byteOffset_; }
    size_t getByteSize() const { return byteSize_; }
    size_t getByteAlignment() const { return byteAlignment_; }
    size_t getBufferOffset() const { return bufferOffset_; }
    size_t getReadSize() const { return getByteOffset() - getBufferOffset() + getByteSize(); }
    size_t getValueCount() const { return valueCount_; }

    std::map<std::string, size_t> getOutputOrder();

    void setBufferOffset(size_t bufferOffset);
    void setFixedHeaderValues(const std::vector<std::string>& headers, const std::vector<double>& values);
    int addOrphanScalars();

    void extract(const uint8_t* buffer, double headerValues[]);
    void extractRaw(const uint8_t* buffer, double headerValues[]);
};

} // namespace segysdk::segyio::valueconversion
