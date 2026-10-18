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

#include "ConversionDetails.h"
#include "DerivedHeader.h"

#include "../segysdk_details/DLL_EXPORT.h"

#include "../FileIO/FileReader.h"
#include "../SegyIO/BinaryHeader.h"
#include "../SegyIO/HeaderDefinition.h"

#include <memory>
#include <string>
#include <vector>

namespace segysdk {namespace segyformat {namespace external {
class SegyOverrideField;
}}}

namespace segysdk::segyio
{

class SEGYSDK_DLL_PUBLIC DerivedHeader
{
private:
    int revisionLevel_{};
    std::string fileName_{};
    uint64_t fileSize_{};
    uint64_t defaultSampleCount_{};
    SampleFormat sampleFormat_{static_cast<SampleFormat>(0)};
    int extendedTextualHeaderCount_{};
    bool isByteSwapped_{true};
    bool hasFixedSampleCount_{false};

private:
    static bool hasTraceLengthOverride(const std::shared_ptr<std::map<std::string, std::shared_ptr<HeaderOverride>>>& traceHeaderOverrides);
    void initialize(std::shared_ptr<fileio::FileReader> file, const std::shared_ptr<BinaryHeader>& binary, std::shared_ptr<std::map<std::string, std::shared_ptr<HeaderOverride>>> traceHeaderOverrides);

public:
    DerivedHeader();
    DerivedHeader(const std::shared_ptr<fileio::FileReader>& file, const std::vector<std::shared_ptr<HeaderDefinition>>& binaryHeaders, std::shared_ptr<std::map<std::string, std::shared_ptr<HeaderOverride>>> traceHeaderOverrides);
    DerivedHeader(const std::shared_ptr<fileio::FileReader>& file, const std::shared_ptr<BinaryHeader>& binary, std::shared_ptr<std::map<std::string, std::shared_ptr<HeaderOverride>>> traceHeaderOverrides);

    int getRevisionLevel() const { return revisionLevel_; }
    std::string getFileName() const { return fileName_; }
    uint64_t getFileSize() const { return fileSize_; }
    uint64_t getFirstTraceOffset() const;
    uint32_t getTraceHeaderSize() const { return 240; }
    uint64_t getDefaultSampleCount() const { return defaultSampleCount_; }
    SampleFormat getSampleFormat() const { return sampleFormat_; }
    size_t getSampleSize() const;
    int getExtendedTextualHeaderCount() const { return extendedTextualHeaderCount_; }
    ConversionDetails getConversionDetails() const;

    bool isByteSwapped() const { return isByteSwapped_; }
    bool hasFixedSampleCount() const { return hasFixedSampleCount_; }

public:
    // For now, full manual configuration is allowed
    void setRevisionLevel(int level) { revisionLevel_ = level; }
    void setFileName(const std::string& name) { fileName_ = name; }
    void setFileSize(uint64_t size) { fileSize_ = size; }
    void setDefaultSampleCount(uint64_t count) { defaultSampleCount_ = count; }
    void setSampleFormat(SampleFormat format) { sampleFormat_ = format; }
    void setExtendedTextualHeaderCount(int count) { extendedTextualHeaderCount_ = count; }
    void setByteSwapping(bool isSwapped) { isByteSwapped_ = isSwapped; }
    void setFixedSampleCount(bool isFixed) { hasFixedSampleCount_ = isFixed; }
};

} // namespace segysdk::segyio
