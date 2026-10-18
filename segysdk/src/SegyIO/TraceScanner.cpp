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

#include "TraceScanner.h"

#include "DerivedHeader.h"
#include "Primitives.h"
#include "SegyDefinitionProvider.h"

#include "../FileIO/BufferedFileReader.h"
#include "../FileIO/FileReader.h"
#include "../SegyIO/SegyIOException.h"
#include "../SegyIO/ValueConversion/HeaderValueArrayExtractor.h"

#include <algorithm>
#include <stdexcept>

namespace segysdk::segyio
{

using fileio::BufferedFileReader;
using valueconversion::HeaderValueArrayExtractor;

TraceScanner::TraceScanner(const std::shared_ptr<fileio::FileReader>& file, const std::shared_ptr<DerivedHeader>& derived, SegyDefinitionProvider& headerProvider, const std::vector<std::string>& headers)
  : TraceScannerBase(file, derived, headerProvider, headers)
{
    initReader(file, headerProvider);
}

void
TraceScanner::initReader(const std::shared_ptr<fileio::FileReader>& file, SegyDefinitionProvider& headerProvider)
{
    if (reader_ == nullptr) {
        ConversionDetails details = derived_->getConversionDetails();
        extractor_ = std::make_unique<HeaderValueArrayExtractor>(headerProvider, traceHeaders_, details);
        extractor_->addOrphanScalars();

        headerValues_ = std::make_unique<double[]>(extractor_->getValueCount());

        reader_ = std::make_unique<BufferedFileReader>(file);
        reader_->setBufferSize(SegyTraceScanner::MinBufferSize);
    }
}

bool
TraceScanner::findNextTrace(const request::TraceParams& params, double* values)
{
    hasTrace_ = false;
    current_.offset_ = nextOffset_;
    if (current_.offset_ >= fileSize_)
        return false;

    if (hasVariableSampleCount()) {
        current_.size_ = getTraceSize(values[ixSampleCount_]);
    }

    const uint8_t* buffer = reader_->readAligned(
        fileio::FileReadParams{request::Params{params}}
            .withOffset(current_.offset_)
            .withSize(current_.size_)
            .withAlignment(extractor_->getByteAlignment()));
    if (buffer == nullptr)
        return false;

    extractor_->extract(buffer, values);

    auto sampleCount = static_cast<uint64_t>((current_.size_ - headerByteSize_) / sampleSize_);
    samples_.reset(new float[sampleCount]);
    const uint8_t* samples_ptr = buffer + headerByteSize_;
    sampleConverter_->convert(samples_ptr, samples_.get(), sampleCount);

    nextOffset_ = current_.offset_ + current_.size_;
    hasTrace_ = true;

    return true;
}

} // namespace segysdk::segyio
