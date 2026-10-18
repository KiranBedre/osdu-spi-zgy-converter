/*
Copyright 2021-2024 Schlumberger

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

#include "SegyFile.h"

#include "BinaryHeader.h"
#include "DerivedHeader.h"
#include "HeaderDefinitionProvider.h"
#include "Primitives.h"
#include "SegyTrace.h"
#include "TextualHeader.h"
#include "TraceHeaderReader.h"

#include "../FileIO/GenericFileManager.h"
#include "../SegyFormat/Internal/FileHeaderDefinitionStringConstsBinaryHeader.h"
#include "../SegyFormat/Internal/FileHeaderDefinitionStringConstsTraceHeader.h"
#include "../SegyIO/SegyIOException.h"
#include "../SegyIO/ValueConversion/SampleValueConverter.h"

#include <cmath>
#include <cstring>
#include <limits>
#include <string>
#include <utility>

namespace segysdk::segyio
{

SegyFile::SegyFile(std::shared_ptr<FileHeaderDescription> fileHeaderDescription, fileio::FileReference fileReference)
  : fileHeaderDescription_(std::move(fileHeaderDescription)),
    fileReference_(std::move(fileReference)),
    bufferSize_(0)
{
    initializeHeaderOverrideCache();

    /* TODO Temporary BC Hack
     *
     * This Temporary hack has been there for a long time, from 2016 at least ...
     * It is simply trying to validate that the file has a definition for Inline and Crossline id.
     * So far this has not been an issue for us, but once we start doing 2D SEGY this might not be the case any more.
     * The HACK bit is that it is only checking if the definition of the fields exist, it is not checking that the fields referenced actually contain meaningful values.
     *
     */
    if (getTraceHeaderDefinition(segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Inline) != nullptr)
        inlineHeaderId_ = segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Inline;

    if (getTraceHeaderDefinition(segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Crossline) != nullptr)
        crosslineHeaderId_ = segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Crossline;
}

SegyFile::~SegyFile()
{
    close();
}

std::shared_ptr<fileio::FileReader>
SegyFile::getFile()
{
    if (!isOpen()) {
        open();
    }

    return file_;
}

std::shared_ptr<fileio::FileReader>
SegyFile::getCloneFile(const bool raw) const
{
    return getCloneFile(request::Params{}, raw);
}

std::shared_ptr<fileio::FileReader>
SegyFile::getCloneFile(const request::Params& params, const bool raw) const
{
    decltype(file_) cloneFile;
    open(params, cloneFile, raw);

    return cloneFile;
}

uint64_t
SegyFile::getFileSize()
{
    return getFile()->getFileSize();
}

std::shared_ptr<FileHeaderDescription>
SegyFile::getFileHeaderDescription() const
{
    return fileHeaderDescription_;
}

void
SegyFile::setFileHeaderDescription(std::shared_ptr<FileHeaderDescription> fileHeaderDescription)
{
    fileHeaderDescription_ = fileHeaderDescription;
}

std::shared_ptr<BinaryHeader>
SegyFile::getBinaryHeader()
{
    if (binaryHeader_ == nullptr) {
        binaryHeader_ = std::make_shared<BinaryHeader>(getFile(), *getFileHeaderDescription()->getBinaryHeaders(), *getFileHeaderDescription()->getBinaryHeaderOverrides());
    }

    return binaryHeader_;
}

std::shared_ptr<DerivedHeader>
SegyFile::getDerivedHeader()
{
    if (derivedHeader_ == nullptr) {
        const std::shared_ptr<std::map<std::string, std::shared_ptr<HeaderOverride>>> trh_overrides = fileHeaderDescription_->getTraceHeaderOverrides();

        derivedHeader_ = std::make_shared<DerivedHeader>(getFile(), getBinaryHeader(), trh_overrides);
    }

    return derivedHeader_;
}

uint64_t
SegyFile::getFirstTraceOffset()
{
    const int extendedHeaderCount = getDerivedHeader()->getExtendedTextualHeaderCount();
    return Primitives::getCombinedHeaderSize(extendedHeaderCount);
}

uint32_t
SegyFile::getTraceHeaderByteSize()
{
    return 240; // No extended trace headers yet
}

ConversionDetails
SegyFile::getConversionDetails()
{
    return getDerivedHeader()->getConversionDetails();
}

SampleFormat
SegyFile::getSampleFormat()
{
    return getDerivedHeader()->getSampleFormat();
}

size_t
SegyFile::getBytesPerSample(SampleFormat sampleFormat)
{
    return Primitives::getSampleSize(sampleFormat);
}

std::string
SegyFile::getLengthUnit()
{
    return getBinaryHeader()->isMetric() ? "m" : "ft";
}

uint64_t
SegyFile::getDefaultTraceLength()
{
    return getDerivedHeader()->getDefaultSampleCount();
}

float
SegyFile::getSampleInterval()
{
    // TODO: May need to specialize this to support scaling and overflow
    const int interval = getBinaryHeader()->getValue(segyformat::internal::FileHeaderDefinitionStringConstsBinaryHeader::SampleInterval, -1);
    if (interval < 0)
        return std::numeric_limits<float>::quiet_NaN();

    return static_cast<float>(interval);
}

size_t
SegyFile::getBufferSize() const
{
    if (bufferSize_ <= 0) {
        return SegyFile::defaultBufferSize_;
    }

    return bufferSize_;
}

// Obsolete prestack access, still in use
int
SegyFile::getMaxTracesPerGather()
{
    return getBinaryHeader()->getValue(segyformat::internal::FileHeaderDefinitionStringConstsBinaryHeader::StackFold, 0);
}

int
SegyFile::getBinaryHeaderField(const std::string& headerId)
{
    return getBinaryHeader()->getValue(headerId);
}

float
SegyFile::getFirstSampleAt()
{
    return static_cast<float>(getScaledTraceHeaderAsDouble(getFirstTraceOffset(), segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ShotToRecordingLagTime));
}

int64_t
SegyFile::getExtendedTextualHeaderCount()
{
    return getDerivedHeader()->getExtendedTextualHeaderCount();
}

std::string
SegyFile::getTextualHeader()
{
    TextualHeader textual = TextualHeader(getFile(), static_cast<int>(getExtendedTextualHeaderCount()));
    return textual.getTextualHeader();
}

std::vector<std::string>
SegyFile::getTextualHeaderByLine()
{
    TextualHeader textual = TextualHeader(getFile(), static_cast<int>(getExtendedTextualHeaderCount()));
    return textual.getTextualHeaderByLine();
}

std::string
SegyFile::getExtendedTextualHeader(int64_t headerIndex)
{
    const auto headerCount = static_cast<int>(getExtendedTextualHeaderCount());
    if (headerIndex < headerCount) {
        TextualHeader textual = TextualHeader(getFile(), headerCount);
        return textual.getExtendedHeader(static_cast<int>(headerIndex));
    }

    return "";
}

std::vector<std::string>
SegyFile::getExtendedTextualHeaderByLine(int64_t headerIndex)
{
    const auto headerCount = static_cast<int>(getExtendedTextualHeaderCount());
    if (headerIndex < headerCount) {
        TextualHeader textual = TextualHeader(getFile(), headerCount);
        return textual.getExtendedHeaderByLine(static_cast<int>(headerIndex));
    }

    return std::vector<std::string>();
}

std::shared_ptr<HeaderDefinition>
SegyFile::getHeaderDefinition(const std::string& headerId)
{
    if (binaryHeaderCache_.empty()) {
        const auto& headers = fileHeaderDescription_->getBinaryHeaders();
        for (const auto& header : *headers) {
            binaryHeaderCache_[header->getHeaderId()] = header;
        }
    }

    const auto header = binaryHeaderCache_.find(headerId);
    if (header != binaryHeaderCache_.end())
        return header->second;

    return nullptr;
}

std::shared_ptr<HeaderDefinition>
SegyFile::getTraceHeaderDefinition(const std::string& headerId)
{
    if (traceHeaderCache_.empty()) {
        const auto& headers = fileHeaderDescription_->getTraceHeaders();
        for (const auto& header : *headers) {
            traceHeaderCache_[header->getHeaderId()] = header;
        }

        // Clear any bad scalar references (probably some legacy bug fix
        for (const auto& header : *headers) {
            if (header->needsScalar() && traceHeaderCache_.find(header->getScalarHeaderId()) == traceHeaderCache_.end()) {
                header->setScalarHeaderId("");
            }
        }
    }

    const auto header = traceHeaderCache_.find(headerId);
    if (header != traceHeaderCache_.end())
        return header->second;

    return nullptr;
}

TraceHeaderReader&
SegyFile::getTraceHeaderReader()
{
    if (traceHeaderReader_ == nullptr) {
        std::vector<std::string> headerNames;
        for (const auto& header : *fileHeaderDescription_->getTraceHeaders()) {
            headerNames.push_back(header->getHeaderId());
        }

        const ConversionDetails details = getConversionDetails();
        traceHeaderReader_ = std::make_unique<TraceHeaderReader>(getFile(), *this, headerNames, details);
    }

    return *traceHeaderReader_;
}

TraceHeaderReader&
SegyFile::getRawTraceHeaderReader()
{
    if (rawTraceHeaderReader_ == nullptr) {
        const bool raw = true;
        HeaderDefinitionProvider hdp = HeaderDefinitionProvider(*getFileHeaderDescription()->getTraceHeaders(), raw);
        const ConversionDetails details = getConversionDetails();
        rawTraceHeaderReader_ = std::make_unique<TraceHeaderReader>(getFile(), hdp, hdp.getHeaderNames(), details);
    }

    return *rawTraceHeaderReader_;
}

bool
SegyFile::isByteSwappped()
{
    return getDerivedHeader()->isByteSwapped();
}

bool
SegyFile::hasFixedTraceLength()
{
    return getDerivedHeader()->hasFixedSampleCount();
}

void
SegyFile::setFileHeaderDescription(FileHeaderDescription* newDescription)
{
    fileHeaderDescription_.reset(newDescription);

    // Clear values directly or indirectly derived from definitions
    traceHeaderCache_.clear();
    binaryHeaderCache_.clear();

    binaryHeader_.reset();
    derivedHeader_.reset();
    traceHeaderReader_.reset();
    rawTraceHeaderReader_.reset();

    traceLengthIndex_ = -99;

    initializeHeaderOverrideCache();
}

void
SegyFile::setBufferSize(size_t newBufferSize)
{
    bufferSize_ = newBufferSize;
    if (isOpen()) {
        file_->setBufferSize(newBufferSize);
    }
}

// TODO Not supported yet: for OSDU we get the UOM from the header definitions, but for other implementations we might not get the UOM information the same way.
void
SegyFile::setTraceHeaderStorageUnit(const std::string& headerId, [[maybe_unused]] const std::string& unitMeasurement, [[maybe_unused]] const std::string& storageUnit, [[maybe_unused]] float extraScalar)
{
    const auto header = getTraceHeaderDefinition(headerId);
    if (header != nullptr) {
        // Erik ToDo:
        // header->setUnitDescriptor(unitMeasurement, storageUnit);
        // header->setExtraUnitScalar(extraScalar);
    }
}

// TODO Not supported yet: for OSDU we get the UOM from the header definitions, but for other implementations we might not get the UOM information the same way.
void
SegyFile::setHeaderStorageUnit(const std::string& headerId, [[maybe_unused]] const std::string& unitMeasurement, [[maybe_unused]] const std::string& storageUnit, [[maybe_unused]] float extraScalar)
{
    const std::shared_ptr<HeaderDefinition> header = getHeaderDefinition(headerId);
    if (header != nullptr) {
        // Erik ToDo:
        // header->setUnitDescriptor(unitMeasurement, storageUnit);
        // header->setExtraUnitScalar(extraScalar);
    }
}

void
SegyFile::setCoordinateHeaders(const std::string& xCoordinateHeaderId, const std::string& yCoordinateHeaderId)
{
    xCoordinateHeaderId_ = xCoordinateHeaderId;
    yCoordinateHeaderId_ = yCoordinateHeaderId;
}

void
SegyFile::setLineHeaders(const std::string& inlineHeaderId, const std::string& crosslineHeaderId)
{
    inlineHeaderId_ = inlineHeaderId;
    crosslineHeaderId_ = crosslineHeaderId;
}

void
SegyFile::setUseTraceWeights(bool value)
{
    useTraceWeights_ = value;
    if (useTraceWeights_ && getTraceHeaderDefinition(segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::TraceWeightFactor) == nullptr) {
        useTraceWeights_ = false;
    }
}

bool
SegyFile::isOpen(const FileReaderPtr& file)
{
    if (file != nullptr) {
        return true;
    }

    return false;
}

bool
SegyFile::isOpen() const
{
    return isOpen(file_);
}

void
SegyFile::close()
{
    if (file_ != nullptr) {
        file_.reset();

        // Readers have internal references to the file
        traceHeaderReader_.reset();
        rawTraceHeaderReader_.reset();
    }
}

void
SegyFile::open(const request::Params& params, FileReaderPtr& file, const bool raw) const
{
    if (!isOpen(file)) {
        file.reset();
        const auto managerParams = fileio::FileManagerParams{
            fileio::FileAccessParams{params}
                .withFileName(fileReference_.fileName())}
                                       .withReaderOptions(fileReference_.readerOptions());
        file = raw ? fileio::GenericFileManager::createRawReader(managerParams) : fileio::GenericFileManager::createReader(managerParams);
        file->setBufferSize(getBufferSize());
    }
}

void
SegyFile::open()
{
    open(request::Params{}, file_, false);
}

void
SegyFile::initializeHeaderOverrideCache()
{
    const auto binaryOverrides = fileHeaderDescription_->getBinaryHeaderOverrides();
    if (binaryOverrides != nullptr) {
        for (const auto& [id, bnrOverride] : *binaryOverrides) {
            auto binaryOverride = bnrOverride;

            auto headerDefinition = getHeaderDefinition(binaryOverride->getHeaderId());

            if (headerDefinition == nullptr)
                throw SegyFormatException("SEG-Y override " + binaryOverride->getHeaderId() + " is not part of the binary file headers definition.");

            headerDefinition->setCachedHeaderOverride(*binaryOverride);
        }
    }
    const auto traceOverrides = fileHeaderDescription_->getTraceHeaderOverrides();
    if (traceOverrides != nullptr) {
        for (const auto& [id, trcOverride] : *traceOverrides) {
            auto traceHeaderOverride = trcOverride;

            auto headerDefinition = getTraceHeaderDefinition(traceHeaderOverride->getHeaderId());

            if (headerDefinition == nullptr)
                throw SegyFormatException("SEG-Y override " + traceHeaderOverride->getHeaderId() + " is not part of the trace headers definition.");

            headerDefinition->setCachedHeaderOverride(*traceHeaderOverride);
        }
    }
}

void
SegyFile::primeBulkAccess()
{
    // Ensure necessary initialization to handle multi-threaded access from bulk plugin
    open();
    (void)getSampleFormat();
    (void)getTraceHeaderDefinition("dummy");
}

float
SegyFile::getScaledHeaderFloat(const HeaderDefinition& header)
{
    return static_cast<float>(getBinaryHeaderField(header.getHeaderId()));
}

uint64_t
SegyFile::getActualTraceLength(SegyTrace& trace)
{
    if (!hasFixedTraceLength() && traceLengthIndex_ != -1) {
        if (traceLengthIndex_ < 0) {
            traceLengthIndex_ = getTraceHeaderReader().getValueIndex("TraceLength");
        }

        if (traceLengthIndex_ >= 0) {
            const double value = getTraceHeaderReader().getValue(trace.getTraceFileOffset(), traceLengthIndex_);
            if (!std::isnan(value) && value > 0.5)
                return static_cast<uint64_t>(value + 0.5);
        }
    }

    return getDefaultTraceLength();
}

double
SegyFile::getScaledTraceHeaderAsDouble(uint64_t traceOffset, const std::string& headerId)
{
    return getTraceHeaderReader().getValue(traceOffset, headerId);
}

double
SegyFile::getRawTraceHeaderAsDouble(uint64_t traceOffset, const std::string& headerId)
{
    return getRawTraceHeaderReader().getValue(traceOffset, headerId);
}

double
SegyFile::getScaledTraceHeaderAsDouble(const HeaderDefinition& header, const SegyTrace& trace)
{
    return getScaledTraceHeaderAsDouble(trace.getTraceFileOffset(), header.getHeaderId());
}

double
SegyFile::getRawTraceHeaderAsDouble(const HeaderDefinition& header, const SegyTrace& trace)
{
    return getRawTraceHeaderAsDouble(trace.getTraceFileOffset(), header.getHeaderId());
}

void
SegyFile::readSamples(const SegyTrace& trace, size_t offsetInTrace, float* samples, size_t desiredSampleCount, size_t samplesOffset)
{
    const uint64_t sampleCount = trace.getSamplesCount();
    const uint64_t bytesPerSample = trace.getBytesPerSample();

    const size_t firstSampleOffset = trace.getSamplesFileOffset() + (bytesPerSample * offsetInTrace);

    // zero fill extra samples
    if (offsetInTrace + desiredSampleCount > sampleCount) {
        float* buffer = samples + samplesOffset;
        if (offsetInTrace >= sampleCount) {
            memset(samples + samplesOffset, 0, desiredSampleCount * sizeof(float));
            return;
        }
        else if (offsetInTrace + desiredSampleCount >= sampleCount) {
            const uint64_t oldDesiredSampleCount = desiredSampleCount;
            desiredSampleCount = sampleCount - offsetInTrace;
            const size_t firstSampleToClear = offsetInTrace + desiredSampleCount;
            memset(buffer + firstSampleToClear, 0, (oldDesiredSampleCount - desiredSampleCount) * sizeof(float));
        }
    }

    const size_t rawBufferSize = desiredSampleCount * bytesPerSample;
    auto rawData = getFile()->read(fileio::FileReadParams{}.withOffset(firstSampleOffset).withSize(rawBufferSize));

    float twf = 1.0f;
    bool twfEnabled = false;
    if (useTraceWeights_) {
        const int twfValue = static_cast<int>(getScaledTraceHeaderAsDouble(trace.getTraceFileOffset(), "TraceWeightFactor"));
        if (twfValue > 0 && twfValue < 32768) {
            twfEnabled = true;
            twf = static_cast<float>(pow(2.f, -twfValue));
        }
    }

    const auto details = getConversionDetails();
    const auto converter = valueconversion::SampleValueConverter::create(details.getSampleValueType(), details.needsByteSwap());

    if (twfEnabled) {
        converter->convertScaled(&rawData[0], samples, desiredSampleCount, twf);
    }
    else {
        converter->convert(&rawData[0], samples, desiredSampleCount);
    }

    for (size_t i = 0; i < desiredSampleCount; ++i) {
        if (static_cast<int32_t>(std::abs(samples[i])) > std::numeric_limits<int32_t>::max() || std::isnan(samples[i]))
            samples[i] = 0;
    }
}

StorageAccessParams&
StorageAccessParams::withSDCredentials(const SDCredentials& value)
{
    sdCredentials_ = value;
    return *this;
}

StorageAccessParams&
StorageAccessParams::withCacheSizeOption(const CacheSizeOption& value)
{
    cacheSizeOption_ = value;
    return *this;
}

} // namespace segysdk::segyio
