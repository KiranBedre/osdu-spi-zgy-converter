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

#include "DerivedHeader.h"

#include "BinaryHeader.h"
#include "Primitives.h"
#include "TextualHeader.h"

#include "../FileIO/FileReader.h"
#include "../SegyFormat/External/SegyOverrideField.h"

#include <utility>

namespace segysdk::segyio
{

DerivedHeader::DerivedHeader()
{}

DerivedHeader::DerivedHeader(const std::shared_ptr<fileio::FileReader>& file, const std::vector<std::shared_ptr<HeaderDefinition>>& binaryHeaders, std::shared_ptr<std::map<std::string, std::shared_ptr<HeaderOverride>>> traceHeaderOverrides)
  : fileName_(file->getFileName()),
    fileSize_(file->getFileSize())
{
    initialize(file, std::make_shared<BinaryHeader>(file, binaryHeaders), traceHeaderOverrides);
}

DerivedHeader::DerivedHeader(const std::shared_ptr<fileio::FileReader>& file, const std::shared_ptr<BinaryHeader>& binary, std::shared_ptr<std::map<std::string, std::shared_ptr<HeaderOverride>>> traceHeaderOverrides)
  : fileName_(file->getFileName()),
    fileSize_(file->getFileSize())
{
    initialize(file, binary, traceHeaderOverrides);
}

bool
DerivedHeader::hasTraceLengthOverride(const std::shared_ptr<std::map<std::string, std::shared_ptr<HeaderOverride>>>& traceHeaderOverrides)
{
    const auto it = traceHeaderOverrides->find("TraceLength");
    return it != traceHeaderOverrides->end();
}

void
DerivedHeader::initialize(std::shared_ptr<fileio::FileReader> file, const std::shared_ptr<BinaryHeader>& binary, std::shared_ptr<std::map<std::string, std::shared_ptr<HeaderOverride>>> traceHeaderOverrides)
{
    revisionLevel_ = binary->getRevisionLevel();
    defaultSampleCount_ = binary->getSampleCount();
    sampleFormat_ = binary->getSampleFormat();
    extendedTextualHeaderCount_ = binary->getExtendedTextualHeaderCount();
    isByteSwapped_ = binary->isByteSwapped();
    hasFixedSampleCount_ = binary->hasFixedSampleCount() || hasTraceLengthOverride(traceHeaderOverrides);

    if (extendedTextualHeaderCount_ < 0) {
        TextualHeader textual(std::move(file), -1);
        extendedTextualHeaderCount_ = textual.getExtendedHeaderCount();
    }
}

uint64_t
DerivedHeader::getFirstTraceOffset() const
{
    return Primitives::getCombinedHeaderSize(extendedTextualHeaderCount_);
}

size_t
DerivedHeader::getSampleSize() const
{
    return Primitives::getSampleSize(sampleFormat_);
}

ConversionDetails
DerivedHeader::getConversionDetails() const
{
    return ConversionDetails(isByteSwapped_, sampleFormat_);
}

} // namespace segysdk::segyio
