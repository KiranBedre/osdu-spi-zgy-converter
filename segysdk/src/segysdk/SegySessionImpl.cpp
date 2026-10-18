/*
Copyright 2021-2025 Schlumberger

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

#include "../segysdk/SegySessionImpl.h"
#include "../BinGridCalculationPlugin/BinGridCalculationPlugin.h"
#include "../BulkAccess/AsyncReaderImpl.h"
#include "../BulkAccess/ReaderConnectorLive.h"
#include "../BulkAccess/SegyReaderFactory.h"
#include "../FileIO/GenericFileManager.h"
#include "../Logger/Logger.h"
#include "../SegyFormat/External/LoadContextFromOSDUJson.h"
#include "../SegyFormat/External/SegyTraceHeaderStringConsts.h"

#include "../SegyFormat/Internal/FileHeaderDefinitionStringConstsBinaryHeader.h"
#include "../SegyFormat/Internal/FileHeaderDefinitionStringConstsTraceHeader.h"
#include "../SegyIO/FileHeaderDefinitionFactory.h"
#include "../SegyIO/HeaderDefinitionProvider.h"
#include "../SegyIO/TraceConstScanner.h"
#include "../SegyIndex/SegyIndexFactory.h"
#include "../Telemetry/Connector.h"
#include "../Utilities/JsonUtilities.h"
#include "../segysdk_details/JSONNumericalFormatStrings.h"
#include "../segysdk_details/JSONScalingEnums.h"
#include "../segysdk_details/JSONTraceHeaderDefStrings.h"
#include "../Utilities/StringUtils.h"

#include <algorithm>
#include <iostream>
#include <map>
#include <optional>
#include <stdexcept>
#include <utility>

namespace segysdk::api
{
SegySessionImpl::SegySessionImpl(const SessionImplParams& params)
  : context_(params.segyContext_),
    fileHeaderDescr_(std::make_shared<segyio::FileHeaderDescription>(params.binaryHeaders_, params.traceHeaders_)),
    binaryHeaders_(params.binaryHeaders_),
    traceHeaders_(params.traceHeaders_),
    bhOverrides_(params.binaryHeaderOverrides_),
    thOverrides_(params.traceHeaderOverrides_),
    sdCredentials_(params.sdCredentials_),
    cacheSize_(params.cacheSizeOption_),
    isByteSwapped_(true)
{
    if (!bhOverrides_.empty()) {
        fileHeaderDescr_->setBinaryHeaderOverrides(params.binaryHeaderOverrides_);
    }
    if (!thOverrides_.empty()) {
        fileHeaderDescr_->setTraceHeaderOverrides(params.traceHeaderOverrides_);
    }

    {
        fileio::FileReaderOptions const readerOptions(sdCredentials_, cacheSize_);

        for (const std::string& fname : context_->getSegyFileNames()) {
            auto fileReference = fileio::FileReference(fname, readerOptions);
            auto segyFile = std::make_shared<segyio::SegyFile>(fileHeaderDescr_, fileReference);
            segyFilesVector_.push_back(segyFile);
        }
    }

    const auto segyFilePtr = segyFilesVector_[0];
    if (segyFilePtr != nullptr) {
        segyRevision_ = segyFilePtr->getBinaryHeader()->getRevisionLevel();
        isByteSwapped_ = segyFilePtr->getBinaryHeader()->isByteSwapped();
    }
    //
    // Initialize the legal tag
    //
    {
        std::optional<std::string> legalTag(context_->getLegalTag());

        if (not legalTag.has_value()) {
            bool const is_sd_dataset = fileio::GenericFileManager::isSDDataset(segyFilePtr->getFileName());
            // If the SD Legal Tag was not set on the creation of the SegyIndexImpl from the Context (OSDU json file)
            // use the one which is set on the original file SEGY file.
            if (is_sd_dataset) {
                legalTag = fileio::GenericFileManager::getSDLegalTag({params, segyFilePtr->getFileReference()});
                // It will be always set in SD storage but nevertheless
                if (not legalTag.has_value()) {
                    throw segysdk::SegyInvalidArgumentException("GenerateIndex: No SD legal tag set on the SEGY file in the SD storage.");
                }
            }
        }

        fileio::FileReaderOptions const readerOptions(sdCredentials_, cacheSize_, legalTag);

        segyIndex_ = segysdk::api::SegyIndexFactory().CreateSegyIndex(segyFilesVector_, context_->getSegyFileNames(), readerOptions);
    }
}

SegySessionImpl::SegySessionImpl(SegySessionImpl&& other) noexcept
  : context_(std::move(other.context_)),
    fileHeaderDescr_(std::move(other.fileHeaderDescr_)),
    segyFilesVector_(std::move(other.segyFilesVector_)),
    selectedSegyFile_(std::move(other.selectedSegyFile_)),
    segyIndex_(std::move(other.segyIndex_)),
    binaryHeaders_(std::move(other.binaryHeaders_)),
    traceHeaders_(std::move(other.traceHeaders_)),
    bhOverrides_(std::move(other.bhOverrides_)),
    thOverrides_(std::move(other.thOverrides_)),
    sdCredentials_(std::move(other.sdCredentials_)),
    cacheSize_(std::move(other.cacheSize_)),
    isByteSwapped_(std::move(other.isByteSwapped_)),
    segyRevision_(std::move(other.segyRevision_))
{}

SegySessionImpl&
SegySessionImpl::operator=(const SegySessionImpl& rhs)
{
    return *this = SegySessionImpl(rhs);
}

SegySessionImpl&
SegySessionImpl::operator=(SegySessionImpl&& rhs) noexcept
{
    context_ = std::move(rhs.context_);
    fileHeaderDescr_ = std::move(rhs.fileHeaderDescr_);
    segyFilesVector_ = std::move(rhs.segyFilesVector_);
    selectedSegyFile_ = rhs.selectedSegyFile_;
    segyIndex_ = std::move(rhs.segyIndex_);
    binaryHeaders_ = std::move(rhs.binaryHeaders_);
    traceHeaders_ = std::move(rhs.traceHeaders_);
    bhOverrides_ = std::move(rhs.bhOverrides_);
    thOverrides_ = std::move(rhs.thOverrides_);
    sdCredentials_ = rhs.sdCredentials_;
    cacheSize_ = rhs.cacheSize_;
    isByteSwapped_ = rhs.isByteSwapped_;
    segyRevision_ = rhs.segyRevision_;

    return *this;
}

SegySessionImpl::~SegySessionImpl()
{
    for (auto i = 0U; i < segyFilesVector_.size(); i++) {
        segysdk::fileio::GenericFileManager::eraseCache(segyFilesVector_[i]->getFileReference());
    }
}

std::string
SegySessionImpl::getAsciiHeaders() const
{

    return segyFilesVector_[selectedSegyFile_]->getTextualHeader();
}

std::vector<std::string>
SegySessionImpl::getAsciiHeadersByLine() const
{

    return segyFilesVector_[selectedSegyFile_]->getTextualHeaderByLine();
}

std::vector<std::string>
SegySessionImpl::getExtendedAsciiHeaders() const
{
    const auto numExtHdrs = static_cast<int>(segyFilesVector_[selectedSegyFile_]->getExtendedTextualHeaderCount());

    std::vector<std::string> results;

    if (!isSegyRev1() || numExtHdrs == 0)
        return results;

    for (int ix = 0; ix < numExtHdrs; ix++) {
        results.push_back(segyFilesVector_[selectedSegyFile_]->getExtendedTextualHeader(ix));
    }

    return results;
}

std::vector<std::vector<std::string>>
SegySessionImpl::getExtendedAsciiHeadersByLine() const
{
    const auto numExtHdrs = static_cast<int>(segyFilesVector_[selectedSegyFile_]->getExtendedTextualHeaderCount());

    std::vector<std::vector<std::string>> results;

    if (!isSegyRev1() || numExtHdrs == 0)
        return results;

    for (int ix = 0; ix < numExtHdrs; ix++) {
        results.push_back(segyFilesVector_[selectedSegyFile_]->getExtendedTextualHeaderByLine(ix));
    }

    return results;
}

std::vector<HeaderDataStruct>
SegySessionImpl::getBinaryHeaders() const
{

    std::vector<HeaderDataStruct> results;

    for (const auto& hdr : binaryHeaders_) {
        std::string internalHdrId = hdr->getHeaderId();
        if (!isSegyRev1() && (internalHdrId == segyformat::internal::FileHeaderDefinitionStringConstsBinaryHeader::FixedTraceLengthFlag || internalHdrId == segyformat::internal::FileHeaderDefinitionStringConstsBinaryHeader::ExtendedTextualHeaderCount)) {
            continue;
        }

        const int value = segyFilesVector_[selectedSegyFile_]->getBinaryHeaderField(internalHdrId);

        HeaderDataStruct bh(hdr->getExternalHeaderId(), static_cast<size_t>(hdr->getOffset()), hdr->getByteCount(), value);
        results.push_back(bh);
    }

    return results;
}

std::shared_ptr<segyindex::SegyIndex>
SegySessionImpl::getIndex() const
{
    return getIndex(request::Params{});
}

std::shared_ptr<segyindex::SegyIndex>
SegySessionImpl::getIndex(const request::Params& params) const
{
    if (segyIndex_->hasIndexFile()) {
        segyIndex_->Load(params);
    }
    return segyIndex_;
}

bool
SegySessionImpl::setVectorHeaderMapping(std::string vectorHeaderMapping, bool forceRemapping)
{
    if (segyIndex_ != nullptr && segyIndex_->hasIndexFile() && !forceRemapping) {
        return false;
    }

    auto json = utilities::JsonUtilities::getJsonFromString(vectorHeaderMapping);
    if (json.empty() || !json.isMember("data") || !json["data"].isMember("VectorHeaderMapping")) {
        return false;
    }

    auto contextLoader = segyformat::external::LoadContextFromOSDUJson(json);
    if (!contextLoader.canGetContext()) {
        return false;
    }

    const auto newContext = contextLoader.getContext(context_->getSegyFileNames(), context_->getSegyVersion());
    const auto fhdFactory = segyio::FileHeaderDescriptionFactory(newContext);

    binaryHeaders_ = fhdFactory.Generate_binaryHeaders();
    traceHeaders_ = fhdFactory.Generate_traceHeaders();
    bhOverrides_ = fhdFactory.GetBinaryHeaderOverrides();
    thOverrides_ = fhdFactory.GetTraceHeaderOverrides();

    fileHeaderDescr_ = std::make_shared<segyio::FileHeaderDescription>(binaryHeaders_, traceHeaders_);

    for (const auto& segyFile : segyFilesVector_) {
        segyFile->setFileHeaderDescription(fileHeaderDescr_);
    }

    return true;
}

std::vector<std::vector<HeaderDataStruct>>
SegySessionImpl::getRawTraceHeaders(size_t startTrace, size_t tracesToDump) const
{

    std::vector<std::vector<HeaderDataStruct>> result;

    if (traceHeaders_.empty())
        return result;

    constexpr bool raw = true;
    segyio::HeaderDefinitionProvider hdp(traceHeaders_, raw);
    segyio::TraceScanner scanner(segyFilesVector_[selectedSegyFile_]->getFile(),
                                 segyFilesVector_[selectedSegyFile_]->getDerivedHeader(), hdp, hdp.getHeaderNames());

    // Initialize scanner
    scanner.reset(request::TraceParams{startTrace});

    size_t extracted_traces = 0;

    while (scanner.moveNext({}) && extracted_traces < tracesToDump) {

        const auto values = scanner.getHeaderValues();
        std::vector<HeaderDataStruct> traceData;
        int ix = 0;
        traceData.reserve(traceHeaders_.size());
        for (const auto& hdr : traceHeaders_) {
            traceData.emplace_back(hdr->getExternalHeaderId(), hdr->getOffset(), hdr->getByteCount(), values[ix++]);
        }
        result.push_back(traceData);
        extracted_traces++;
    }

    return result;
}

std::vector<SegySession::Trace_t>
SegySessionImpl::getTraces(size_t startTrace, size_t tracesToDump) const
{
    return getTraces(request::TraceParams{startTrace}.withTracesToDump(tracesToDump));
}

std::vector<SegySession::Trace_t>
SegySessionImpl::getTraces(const request::TraceParams& params) const
{
    const auto& scope = START_TRACE_SPAN(SpanContext{});
    std::vector<segysdk::api::SegySession::Trace_t> result;

    if (traceHeaders_.empty())
        return result;

    segyio::HeaderDefinitionProvider hdp(traceHeaders_, !params.scaledHeaders_);
    segyio::TraceConstScanner scanner(segyFilesVector_[selectedSegyFile_]->getFile(),
                                 segyFilesVector_[selectedSegyFile_]->getDerivedHeader(), hdp, hdp.getHeaderNames());

    // Initialize scanner
    scanner.reset(params);

    size_t extracted_traces = 0;

    while (scanner.moveNext(params) && extracted_traces < params.tracesToDump_) {

        const auto values = scanner.getHeaderValues();
        std::vector<HeaderDataStruct> traceHeader;
        int ix = 0;
        traceHeader.reserve(traceHeaders_.size());
        for (const auto& hdr : traceHeaders_) {
            traceHeader.emplace_back(hdr->getExternalHeaderId(), static_cast<int>(hdr->getOffset()), hdr->getByteCount(), values[ix++]);
        }

        const auto rawSamples = scanner.getTraceSamples();
        const auto sampleCount = scanner.getTraceSampleCount();
        std::vector<float> samples;

        for (uint64_t i = 0; i < sampleCount; i++) {
            samples.emplace_back(rawSamples[i]);
        }

        result.emplace_back(traceHeader, samples);
        extracted_traces++;
    }

    return result;
}

bool
SegySessionImpl::getTraceSamples(std::vector<std::vector<float>>& result, size_t startTrace, size_t tracesToDump, size_t sampleOffset, size_t samplesToDump) const
{
    return getTraceSamples(result, request::TraceParams{startTrace}.withTracesToDump(tracesToDump).withSampleOffset(sampleOffset).withSamplesToDump(samplesToDump));
}

bool
SegySessionImpl::getTraceSamples(std::vector<std::vector<float>>& result, const request::TraceParams& params) const
{
    const auto& scope = START_TRACE_SPAN(SpanContext{});

    if (traceHeaders_.empty()) {
        return false;
    }

    try {

        segyio::HeaderDefinitionProvider hdp(traceHeaders_, !params.scaledHeaders_);
        segyio::TraceScanner scanner(segyFilesVector_[selectedSegyFile_]->getFile(),
                                     segyFilesVector_[selectedSegyFile_]->getDerivedHeader(), hdp, hdp.getHeaderNames());

        // Initialize scanner
        scanner.reset(params);

        size_t extracted_traces = 0;
        const size_t sampleOffset = params.sampleOffset_;
        const size_t samplesToDump = params.samplesToDump_;
        size_t traceNo = 0;
        if (result.size() < params.tracesToDump_) {
            std::string msg = "getTraceSamples: Result buffer not large enough to hold requested number of traces [" + std::to_string(params.tracesToDump_) + "]";
            throw segyio::SegyFileIOException(msg);
        }
        while (scanner.moveNext(params) && extracted_traces < params.tracesToDump_) {

            const auto rawSamples = scanner.getTraceSamples();
            const auto traceSampleCount = scanner.getTraceSampleCount();

            if (sampleOffset > traceSampleCount) {
                return false;
            }
            size_t extractSampleCount = sampleOffset + samplesToDump <= traceSampleCount
                                            ? samplesToDump
                                            : traceSampleCount - sampleOffset;

            std::vector<float> samples(extractSampleCount);
            size_t source_ix = sampleOffset;
            size_t target_ix = 0;

            for (size_t ix = 0; ix < extractSampleCount; ix++) {
                samples[target_ix + ix] = rawSamples[source_ix + ix];
            }

            result[traceNo++] = samples;
            extracted_traces++;
        }
        result.resize(extracted_traces);
        return true;
    }
    catch (segysdk::segyio::SegyIOException& e) {
        std::string msg = "getTraceSamples: SegyIO exception occured. \n" + std::string(e.what());
        SEGYLOG_REPORT_ERROR(msg);
    }

    return false;
}

std::string
makeHeaderNamesJson(const std::vector<std::string>& headerNames)
{
    Json::Value root;
    Json::Value heders;
    for (const auto& hdrName : headerNames) {
        heders.append(hdrName);
    }
    root["ExtractedHeaders"] = heders;

    return root.toStyledString();
}

std::vector<std::string>
remapToExternalLiterals(const std::vector<std::string>& inputHeaders, const std::map<std::string, std::string>& headerNameMap)
{
    // try to remap back to input trace header literals
    std::vector<std::string> remappedHeaders;
    for (const auto& hdr : inputHeaders) {
        auto pos = headerNameMap.find(hdr);
        if (pos == headerNameMap.end() or pos->second.empty()) {
            remappedHeaders.emplace_back(hdr);
        }
        else {
            remappedHeaders.emplace_back(pos->second);
        }
    }

    return remappedHeaders;
}

bool
SegySessionImpl::getTraceHeaderValues(std::vector<std::string>& headerKeys, std::vector<std::vector<double>>& headerValues, size_t startTrace, size_t tracesToDump) const
{
    return getTraceHeaderValues(headerKeys, headerValues, request::TraceParams{startTrace}.withTracesToDump(tracesToDump));
}

bool
SegySessionImpl::getTraceHeaderValues(std::vector<std::string>& headerKeys, std::vector<std::vector<double>>& headerValues, const request::TraceParams& params) const
{
    const auto& scope = START_TRACE_SPAN(SpanContext{});

    std::vector<std::string> extractedHeaders;
    if (traceHeaders_.empty()) {
        return false;
    }

    std::map<std::string, std::string> headerNameMap;
    bool ilFound = false;
    bool xlFound = false;
    bool cmpFound = false;
    bool scaledSPFound = false;
    bool spScalarFound = false;
    bool energySourcePointFound = false;
    bool sourcexFound = false;
    bool sourceyFound = false;
    bool receiverxFound = false;
    bool receiveryFound = false;
    bool cmpxFound = false;
    bool cmpyFound = false;
    bool firstSampleDepthFound = false;
    bool sampleRateFound = false;
    bool sampleCountFound = false;

    for (const auto& hdr : traceHeaders_) {
        if (hdr->getHeaderId() == segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CMP) {
            headerNameMap.try_emplace(hdr->getHeaderId(), hdr->getExternalHeaderId());
            cmpFound = true;
        }
        if (hdr->getHeaderId() == segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::EnergySourcePointNumber) {
            headerNameMap.try_emplace(hdr->getHeaderId(), hdr->getExternalHeaderId());
            energySourcePointFound = true;
        }
        if (hdr->getHeaderId() == segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ShotNumber) {
            headerNameMap.try_emplace(hdr->getHeaderId(), hdr->getExternalHeaderId());
            scaledSPFound = true;
        }
        if (hdr->getHeaderId() == segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ShotpointScalar) {
            headerNameMap.try_emplace(hdr->getHeaderId(), hdr->getExternalHeaderId());
            spScalarFound = true;
        }
        if (hdr->getHeaderId() == segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SourceXCoordinate) {
            headerNameMap.try_emplace(hdr->getHeaderId(), hdr->getExternalHeaderId());
            sourcexFound = true;
        }
        if (hdr->getHeaderId() == segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SourceYCoordinate) {
            headerNameMap.try_emplace(hdr->getHeaderId(), hdr->getExternalHeaderId());
            sourceyFound = true;
        }
        if (hdr->getHeaderId() == segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ReceiverXCoordinate) {
            headerNameMap.try_emplace(hdr->getHeaderId(), hdr->getExternalHeaderId());
            receiverxFound = true;
        }
        if (hdr->getHeaderId() == segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ReceiverYCoordinate) {
            headerNameMap.try_emplace(hdr->getHeaderId(), hdr->getExternalHeaderId());
            receiveryFound = true;
        }
        if (hdr->getHeaderId() == segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CoordinateX) {
            headerNameMap.try_emplace(hdr->getHeaderId(), hdr->getExternalHeaderId());
            cmpxFound = true;
        }
        if (hdr->getHeaderId() == segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CoordinateY) {
            headerNameMap.try_emplace(hdr->getHeaderId(), hdr->getExternalHeaderId());
            cmpyFound = true;
        }
        if (hdr->getHeaderId() == segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Inline) {
            headerNameMap.try_emplace(hdr->getHeaderId(), hdr->getExternalHeaderId());
            ilFound = true;
        }
        if (hdr->getHeaderId() == segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Crossline) {
            headerNameMap.try_emplace(hdr->getHeaderId(), hdr->getExternalHeaderId());
            xlFound = true;
        }

        if (hdr->getHeaderId() == segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ShotToRecordingLagTime) {
            headerNameMap.try_emplace(hdr->getHeaderId(), hdr->getExternalHeaderId());
            firstSampleDepthFound = true;
        }
        if (hdr->getHeaderId() == segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SampleRate) {
            headerNameMap.try_emplace(hdr->getHeaderId(), hdr->getExternalHeaderId());
            sampleRateFound = true;
        }
        if (hdr->getHeaderId() == segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::TraceLength) {
            headerNameMap.try_emplace(hdr->getHeaderId(), hdr->getExternalHeaderId());
            sampleCountFound = true;
        }
    }

    // We cannot determine 2D or 3D reliably from trace header mapping, as standard template might be populating both 2D and 3d fields
    const auto is3D = segyIndex_->isSegy3D();

    if (!is3D) {
        if (energySourcePointFound or cmpFound) {
            if (energySourcePointFound) {
                extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::EnergySourcePointNumber);
            }
            if (cmpFound) {
                extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CMP);
            }
        }
        else if (scaledSPFound and spScalarFound and cmpFound) {
            extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ShotNumber);
            extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CMP);
        }
        else if (ilFound and xlFound) {
            // Fall back in case indexing is not reliable yet
            extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Inline);
            extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Crossline);
        }
    }
    else {
        if (ilFound and xlFound) {
            extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Inline);
            extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Crossline);
        }
        else if (energySourcePointFound or cmpFound) {
            // Fall back in case indexing is not reliable yet
            if (energySourcePointFound) {
                extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::EnergySourcePointNumber);
            }
            if (cmpFound) {
                extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CMP);
            }
        }
    }
    // We expect coordinates to mapped to be in sourceX and sourceY fields
    // only check the other coordinate fields if sourceXY are not properly mapped
    if (sourcexFound and sourceyFound) {
        extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SourceXCoordinate);
        extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SourceYCoordinate);
    }
    else if (cmpxFound and cmpyFound) {
        extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CoordinateX);
        extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CoordinateY);
    }
    else if (receiverxFound and receiveryFound) {
        extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CoordinateX);
        extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CoordinateY);
    }

    if (firstSampleDepthFound) {
        extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ShotToRecordingLagTime);
    }
    if (sampleCountFound) {
        extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::TraceLength);
    }
    if (sampleRateFound) {
        extractedHeaders.emplace_back(segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SampleRate);
    }
    segyio::HeaderDefinitionProvider hdp(traceHeaders_, !params.scaledHeaders_);
    segyio::TraceScanner scanner(segyFilesVector_[selectedSegyFile_]->getFile(),
                                 segyFilesVector_[selectedSegyFile_]->getDerivedHeader(), hdp, extractedHeaders);

    // Initialize scanner
    scanner.reset(params);

    size_t extracted_traces = 0;
    headerValues.clear();

    while (scanner.moveNext(params) && extracted_traces < params.tracesToDump_) {

        const auto values = scanner.getHeaderValues();
        std::vector<double> thisTraceHeaderValues;
        for (size_t i = 0; i < extractedHeaders.size(); i++) {
            thisTraceHeaderValues.emplace_back(values[i]);
        }
        headerValues.emplace_back(thisTraceHeaderValues);
        extracted_traces++;
    }

    headerKeys = remapToExternalLiterals(extractedHeaders, headerNameMap);
    return true;
}

std::string
SegySessionImpl::TraceHeadersAsJson(long startTrace,
                                    const std::vector<std::vector<HeaderDataStruct>>& result,
                                    const std::vector<std::shared_ptr<segyio::HeaderDefinition>>& hdrLiterals) const
{

    Json::Value root = getJsonRoot();

    Json::Value metadata;
    metadata["ColumnHeaders"] = getTraceHeadersLiteralsAsJson(hdrLiterals);
    metadata["StartTrace"] = static_cast<int>(startTrace);
    metadata["TraceCount"] = static_cast<Json::UInt64>(result.size());
    root["Metadata"] = metadata;

    Json::Value jsonTraceArray;
    long traceNo = startTrace;

    for (const auto& traceData : result) { // all requested traces
        Json::Value jsonTraceData;

        Json::Value jsonTrace;
        jsonTrace["TraceNo"] = static_cast<int>(traceNo);
        for (const auto& td : traceData) { // single trace fields
            const auto value = td.value;
            jsonTraceData.append(value);
        }
        jsonTrace["Traces"] = jsonTraceData;
        jsonTraceArray.append(jsonTrace);
        traceNo++;
    }

    root["TraceData"] = jsonTraceArray;

    return root.toStyledString();
}

std::string
SegySessionImpl::getRawTraceHeadersAsJson(long startTrace, int tracesToDump) const
{
    return TraceHeadersAsJson(startTrace, getRawTraceHeaders(static_cast<size_t>(startTrace), tracesToDump), traceHeaders_);
}

std::string
SegySessionImpl::getTracesAsJson(long startTrace, int tracesToDump) const
{
    const auto& traces = getTraces(static_cast<size_t>(startTrace), static_cast<size_t>(tracesToDump));
    Json::Value root = getJsonRoot();
    Json::Value metadata;
    metadata["ColumnHeaders"] = getTraceHeadersLiteralsAsJson(traceHeaders_);
    metadata["StartTrace"] = static_cast<int>(startTrace);
    metadata["TraceCount"] = static_cast<Json::UInt64>(traces.size());
    root["Metadata"] = metadata;

    Json::Value jsonTraceArray;
    long traceNo = startTrace;

    for (const auto& trace : traces) { // all requested traces
        Json::Value jsonHeader;
        for (const auto& td : trace.first) { // Header
            const auto value = td.value;
            jsonHeader.append(value);
        }

        Json::Value jsonSamples;
        for (const auto& sample : trace.second) { // Samples
            jsonSamples.append(sample);
        }

        Json::Value jsonTrace;
        jsonTrace["TraceNo"] = static_cast<int>(traceNo);
        jsonTrace["Header"] = jsonHeader;
        jsonTrace["Samples"] = jsonSamples;
        jsonTraceArray.append(jsonTrace);
        traceNo++;
    }

    root["TraceData"] = jsonTraceArray;

    return root.toStyledString();
}

std::vector<std::shared_ptr<segyio::HeaderDefinition>>
SegySessionImpl::getHeadersToExtractForScaledTraceHeaderDump(bool is3D) const
{
    std::vector<std::shared_ptr<segyio::HeaderDefinition>> interpretedHeaders;
    
    for (const auto& hdr : traceHeaders_) {
        if (is3D) {
            if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ScalarOnCoordinates
                || hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Inline
                || hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Crossline
                || hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SourceXCoordinate
                || hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SourceYCoordinate
                || hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SampleRate
                || hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::TraceLength
                || hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ShotToRecordingLagTime) {
                interpretedHeaders.push_back(hdr);
            }
        }
        else {
            if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ScalarOnCoordinates
                || hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SourceXCoordinate
                || hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SourceYCoordinate
                || hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SampleRate
                || hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::TraceLength
                || hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ShotToRecordingLagTime
                || hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ShotNumber
                || hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CMP
                || segysdk::utilities::StringUtils::keyEndsWith(hdr->getExternalHeaderId(), ":CMP")
                || segysdk::utilities::StringUtils::keyEndsWith(hdr->getExternalHeaderId(), ":SP")
                || segysdk::utilities::StringUtils::keyEndsWith(hdr->getExternalHeaderId(), ":SPScaled")
                ) {
                interpretedHeaders.push_back(hdr);
            }
        }
    }

    
    return interpretedHeaders;
}

std::vector<std::vector<HeaderDataStruct>>
SegySessionImpl::getScaledTraceHeaders(size_t startTrace, size_t tracesToDump) const
{
    return getScaledTraceHeaders(request::TraceParams{startTrace}.withTracesToDump(tracesToDump));
}

std::vector<std::vector<HeaderDataStruct>>
SegySessionImpl::getScaledTraceHeaders(const request::TraceParams& params) const
{
    const auto& scope = START_TRACE_SPAN(SpanContext{});
    std::vector<std::vector<HeaderDataStruct>> result;

    const std::vector<std::shared_ptr<segyio::HeaderDefinition>> interpretedHeaders = getHeadersToExtractForScaledTraceHeaderDump(segyIndex_->isSegy3D());
    if (interpretedHeaders.empty())
        return result;

    const bool raw = false;
    segyio::HeaderDefinitionProvider hdp(interpretedHeaders, raw);
    segyio::TraceScanner scanner(segyFilesVector_[selectedSegyFile_]->getFile(),
                                 segyFilesVector_[selectedSegyFile_]->getDerivedHeader(), hdp, hdp.getHeaderNames());

    // Initialize scanner
    scanner.reset(params);

    size_t extracted_traces = 0;

    while (scanner.moveNext(params) && extracted_traces < params.tracesToDump_) {

        const auto values = scanner.getHeaderValues();
        std::vector<HeaderDataStruct> traceData;
        int ix = 0;
        for (const auto& hdr : interpretedHeaders) {
            const std::string id = hdr->getExternalHeaderId();
            const auto offset = static_cast<int>(hdr->getOffset());
            const int byteCount = hdr->getByteCount();

            traceData.emplace_back(id, offset, byteCount, values[ix++]);
        }
        result.push_back(traceData);
        extracted_traces++;
    }

    return result;
}

std::vector<std::vector<double>>
SegySessionImpl::getSelectedScaledTraceHeaderValues(size_t startTrace, size_t tracesToDump, std::vector<std::string> header_ids)
{

    // TODO lots of duplication below (with GetScaledTraceHeaders)
    std::vector<std::vector<double>> result;
    result.reserve(tracesToDump);

    constexpr bool raw = false;

    segyio::HeaderDefinitionProvider provider(traceHeaders_, raw);
    std::vector<std::string> existingHeaders;
    std::vector<size_t> outIndex;
    for (size_t i = 0; i < header_ids.size(); ++i) {
        if (provider.getTraceHeaderDefinition(header_ids[i]) != nullptr) {
            existingHeaders.push_back(header_ids[i]);
            outIndex.push_back(i);
        }
    }

    segyio::TraceScanner scanner(segyFilesVector_[selectedSegyFile_]->getFile(), segyFilesVector_[selectedSegyFile_]->getDerivedHeader(), provider, existingHeaders);

    // Initialize scanner
    scanner.reset(request::TraceParams{startTrace});

    size_t extracted_traces = 0;

    while (scanner.moveNext({}) && extracted_traces < tracesToDump) {

        auto values = scanner.getHeaderValues();

        std::vector<double> traceData(header_ids.size());
        for (size_t ix = 0; ix < outIndex.size(); ++ix) {
            traceData[outIndex[ix]] = values[ix];
        }
        result.push_back(traceData);
        extracted_traces++;
    }

    return result;
}

std::string
SegySessionImpl::getScaledTraceHeadersAsJson(long startTrace, int tracesToDump, bool is3D) const
{

    return TraceHeadersAsJson(startTrace,
                              getScaledTraceHeaders(static_cast<size_t>(startTrace), static_cast<size_t>(tracesToDump)),
                              getHeadersToExtractForScaledTraceHeaderDump(is3D));
}

std::string
SegySessionImpl::getAsciiHeadersAsJson() const
{
    Json::Value root = getJsonRoot();

    root["Textualheader"] = getAsciiHeaders();

    return root.toStyledString();
}

std::string
SegySessionImpl::getAsciiHeadersByLineAsJson() const
{
    Json::Value root = getJsonRoot();

    Json::Value textualLines;

    const auto headers = getAsciiHeadersByLine();
    for (const auto& hdr : headers) {
        textualLines.append(hdr);
    }

    root["Textualheader"] = textualLines;
    return root.toStyledString();
}

std::string
SegySessionImpl::getExtendedAsciiHeadersAsJson() const
{

    Json::Value root = getJsonRoot();

    if (isSegyRev1()) {

        const std::vector<std::string> extHeaders = getExtendedAsciiHeaders();

        Json::Value headerArray;
        for (const auto& hdr : extHeaders) {
            headerArray.append(hdr);
        }

        root["ExtendedTextualHeaders"] = headerArray;

        return root.toStyledString();
    }

    return "{}";
}

std::string
SegySessionImpl::getExtendedAsciiHeadersByLineAsJson() const
{

    Json::Value root = getJsonRoot();

    if (isSegyRev1()) {

        const std::vector<std::vector<std::string>> extHeaders = getExtendedAsciiHeadersByLine();

        Json::Value headerArray;
        for (const auto& extendedHdr : extHeaders) {
            Json::Value lineHdr;
            for (const auto& hdrline : extendedHdr) {
                lineHdr.append(hdrline);
            }
            headerArray.append(lineHdr);
        }

        root["ExtendedTextualHeaders"] = headerArray;

        return root.toStyledString();
    }

    return "{}";
}

std::string
SegySessionImpl::getBinaryHeadersAsJson() const
{

    Json::Value root = getJsonRoot();

    Json::Value binaryHeaderArray;
    const auto binaryHeaderStructs = getBinaryHeaders();
    for (const auto& hdr : binaryHeaderStructs) {
        Json::Value bhdr;
        bhdr["Id"] = hdr.id;
        bhdr["Start"] = static_cast<Json::UInt64>(hdr.offset);
        bhdr["End"] = static_cast<Json::UInt64>(hdr.end);
        bhdr["Value"] = hdr.value;

        binaryHeaderArray.append(bhdr);
    }
    root["BinaryHeaders"] = binaryHeaderArray;

    return root.toStyledString();
}

void
SegySessionImpl::acceptAndRegisterSettings() const
{
    const std::string segyFileName = segyFilesVector_[0]->getFileName();
    const auto jsonString = fileHeaderDescr_->toJsonString();

    const fileio::FileReaderOptions readerOptions{sdCredentials_, CacheSizeOption::none()};
    const fileio::FileReference segyFileRef(segyFileName, readerOptions);

    const auto paramFile = getParameterFileReference(segyFileName, readerOptions);

    fileio::GenericFileManager::writeTextFile(paramFile, jsonString, true);
}

fileio::FileReference
SegySessionImpl::getParameterFileReference(const std::string& segyFileName, const fileio::FileReaderOptions& options)
{
    const std::string fileName = segyio::FileHeaderDescription::getParameterFileName(segyFileName);
    return fileio::FileReference(fileName, options);
}

void
SegySessionImpl::loadStoredSettings()
{
    const std::string segyFileName = segyFilesVector_[0]->getFileName();
    const fileio::FileReaderOptions readerOptions{sdCredentials_, CacheSizeOption::none()};
    const fileio::FileReference segyFileRef(segyFileName, readerOptions);
    if (segyio::FileHeaderDescription::parameterFileExists(segyFileRef)) {
        const std::shared_ptr<segyio::FileHeaderDescription> fhd = segyio::FileHeaderDescription::loadFromParameterFile(segyFileName, readerOptions);
        if (fhd->getBinaryHeaders()->size() > 10 || fhd->getTraceHeaders()->size() > 30) {
            fileHeaderDescr_ = fhd;
            segyFilesVector_[0] = std::make_shared<segyio::SegyFile>(fileHeaderDescr_, segyFileRef);

            // Update local cache
            const auto bhdrs = fhd->getBinaryHeaders();
            binaryHeaders_.clear();
            for (const auto& hdr : *bhdrs) {
                binaryHeaders_.push_back(hdr);
            }
            const auto thdrs = fhd->getTraceHeaders();
            traceHeaders_.clear();
            for (const auto& hdr : *thdrs) {
                traceHeaders_.push_back(hdr);
            }
            const auto bho = fhd->getBinaryHeaderOverrides();
            bhOverrides_.clear();
            for (const auto& [id, headerOverrides] : *bho) {
                bhOverrides_.push_back(headerOverrides);
            }
            const auto tho = fhd->getTraceHeaderOverrides();
            thOverrides_.clear();
            for (const auto& [id, headerOverrides] : *tho) {
                thOverrides_.push_back(headerOverrides);
            }
        }
    }
}

void
SegySessionImpl::loadIndexFile() const
{
    if (segyFilesVector_.empty()) {
        const std::string msg = "Must have at least one indexed file to be able to load index!.";
        throw segysdk::SegyInvalidArgumentException(msg);
    }

    if (!segyIndex_->hasIndexFile()) {
        const std::string msg = "Unable to load index file as no saved index file exists.";
        throw segysdk::SegyInvalidArgumentException(msg);
    }

    try {
        segyIndex_->Load();
    }
    catch (const std::invalid_argument& ex) {
        const std::string exceptionString = ex.what();
        const std::string msg = "SegySessionImpl::loadIndexFile: Invalid argument error loading index file Are input SEG-Y files defined?\n" + exceptionString;
        throw segysdk::SegyInvalidArgumentException(msg);
    }
    catch (const std::runtime_error& ex) {
        const std::string exceptionString = ex.what();
        const std::string msg = "SegySessionImpl::loadIndexFile: Runtime error loading index file - Does index file exist?. \n" + exceptionString;
        throw std::runtime_error(msg);
    }
}

std::string
SegySessionImpl::getSurveyBinGrid() const
{
    const std::string emptyJson = Json::Value().toStyledString();

    const segysdk::library_loader::PluginConfig pluginConfig(segysdk::utilities::Environment::instance().segysdkConfigurationFile());
    const auto computeBinGridPluginPath = pluginConfig.getCalculateBinGridPluginPath().string();

    if (computeBinGridPluginPath.empty()) {
        return emptyJson;
    }
    const std::vector<std::shared_ptr<segyio::HeaderDefinition>> interpretedHeaders = getHeadersToExtractForScaledTraceHeaderDump();
    if (interpretedHeaders.empty()) {
        return emptyJson;
    }

    constexpr bool raw = false;

    segyio::HeaderDefinitionProvider hdp(interpretedHeaders, raw);

    auto scanner = std::make_shared<segyio::TraceConstScanner>(segyFilesVector_[selectedSegyFile_]->getFile(),
                                                               segyFilesVector_[selectedSegyFile_]->getDerivedHeader(), hdp, hdp.getHeaderNames());

    try {
        auto libraryLoader = std::make_unique<segysdk::library_loader::DynamicLibraryLoader>(computeBinGridPluginPath);
        auto plugin = std::make_shared<segysdk::bin_grid_calculation::BinGridCalculationPlugin>(std::move(libraryLoader));

        auto params = std::make_unique<bin_grid_calculation::BinGridPluginParams>(scanner, segyFilesVector_[selectedSegyFile_]->getDerivedHeader(), interpretedHeaders);

        return plugin->getBinGridAsFormattedJson(std::move(params));
    }
    catch (std::exception& e) {
        std::string error = "SegySessionImpl::getSurveyBinGrid: " + std::string(e.what());
        SEGYLOG_REPORT_ERROR(error);
    }
    return emptyJson;
}

void
SegySessionImpl::generateIndexFile() const
{
    generateIndexFile(false, false);
}

void
SegySessionImpl::generateIndexFile(bool forceGenerateIndex, bool forceOverwrite) const
{

    if (segyFilesVector_.empty()) {
        const std::string msg = "Must have at least one indexed file to be able to load index!.";
        throw segysdk::SegyInvalidArgumentException(msg);
    }

    if (segyIndex_->hasIndexFile(true) && !forceOverwrite) {
        const std::string msg = "Unable to generate new index file. Current SEG-Y file already has a saved index file and forceOverwrite is not set";
        throw segysdk::SegyInvalidArgumentException(msg);
    }

    try {
        segyIndex_->GenerateIndex(forceGenerateIndex);
    }
    catch (const std::exception& ex) {
        const std::string exceptionString = ex.what();
        const std::string msg = "Error generating SEG-Y index.\n" + exceptionString;
        throw segysdk::SegyInvalidArgumentException(msg);
    }
}

void
SegySessionImpl::setSelectedSegyFile(const size_t fileNumber)
{
    if (fileNumber < segyFilesVector_.size()) {
        selectedSegyFile_ = fileNumber;
    }
}

Json::Value
SegySessionImpl::getHeaderDefsAsJson(const std::vector<std::shared_ptr<segyio::HeaderDefinition>>& headers)
{
    Json::Value root;

    for (const auto& hdr : headers) {
        Json::Value node;
        node[details::JSONTraceHeaderDefStrings::Id] = hdr->getHeaderId();
        node[details::JSONTraceHeaderDefStrings::ExternalId] = hdr->getExternalHeaderId();
        node[details::JSONTraceHeaderDefStrings::ScalarHeaderId] = hdr->getScalarHeaderId();
        node[details::JSONTraceHeaderDefStrings::DisplayName] = hdr->getDisplayName();
        node[details::JSONTraceHeaderDefStrings::Offset] = static_cast<int>(hdr->getOffset());
        node[details::JSONTraceHeaderDefStrings::ByteCount] = hdr->getByteCount();
        node[details::JSONTraceHeaderDefStrings::DataType] = getDataTypeStringFromEnum(hdr->getHeaderDataType());
        node[details::JSONTraceHeaderDefStrings::ScalarIndicator] = getScalarUsageStrategyStringFromEnum(hdr->getScalarUsageStrategy());
        node[details::JSONTraceHeaderDefStrings::ScalarOverride] = hdr->getScalarOverride();
        node[details::JSONTraceHeaderDefStrings::IsWellKnown] = hdr->getIsWellKnown();
        node[details::JSONTraceHeaderDefStrings::UoM] = hdr->getUoM();
        node[details::JSONTraceHeaderDefStrings::ExtraUnitScalar] = static_cast<double>(hdr->getExtraUnitScalar());

        root.append(node);
    }

    return root;
}

segyio::SampleFormat
SegySessionImpl::getSampleFormat() const
{

    return segyFilesVector_[selectedSegyFile_]->getSampleFormat();
}

bool
SegySessionImpl::isBigEndianSegy() const
{
    return isByteSwapped_;
}

void
SegySessionImpl::setSegy2DFlag(bool isSegy2D)
{
    segyIndex_->setSegy2DFlag(isSegy2D);

    auto traceHeaders = getTraceHeaderDefinitions();
    bool cmpFound = false;
    size_t cmpOffset = 300;

    bool spFound = false;
    size_t spOffset = 300;
    if (isSegy2D) {
        // Also ensure that the key 2D fields are present in trace header mapping

        for (const auto& hdr : *traceHeaders) {
            if (hdr->getHeaderId() == segysdk ::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CMP or hdr->getExternalHeaderId() == segysdk ::segyformat::external::SegyTraceHeaderStringConsts::EnsembleNumber) {
                cmpFound = true;
                cmpOffset = hdr->getOffset();
            }
            if (hdr->getHeaderId() == segysdk ::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::EnergySourcePointNumber or hdr->getExternalHeaderId() == segysdk ::segyformat::external::SegyTraceHeaderStringConsts::EnergySourcePointNumber) {
                spFound = true;
                spOffset = hdr->getOffset();
            }
        }
        for (auto& hdr : *traceHeaders) {
            if (cmpFound and hdr->getOffset() == cmpOffset) {
                hdr->setDisplayName(segysdk ::segyformat::external::SegyTraceHeaderStringConsts::CMP);
                std::string externalToken = segysdk ::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CMP;
                hdr->setExternalHeaderId(externalToken);
            }
            if (!cmpFound and hdr->getOffset() == 20) {
                hdr->setHeaderId(segysdk ::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CMP);
                hdr->setDisplayName(segysdk ::segyformat::external::SegyTraceHeaderStringConsts::CMP);
                std::string externalToken = segysdk ::segyformat::external::SegyTraceHeaderStringConsts::CMP;
                hdr->setExternalHeaderId(externalToken);
            }
            if (spFound and hdr->getOffset() == spOffset) {
                hdr->setDisplayName(segysdk ::segyformat::external::SegyTraceHeaderStringConsts::SP);
                std::string externalToken = segysdk ::segyformat::external::SegyTraceHeaderStringConsts::SP;
                hdr->setExternalHeaderId(externalToken);
            }
            if (!spFound and hdr->getOffset() == 17) {
                hdr->setHeaderId(segysdk ::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::EnergySourcePointNumber);
                hdr->setDisplayName(segysdk ::segyformat::external::SegyTraceHeaderStringConsts::SP);
                std::string externalToken = segysdk ::segyformat::external::SegyTraceHeaderStringConsts::SP;
                hdr->setExternalHeaderId(externalToken);
            }
        }
    }
    else {
        for (const auto& hdr : *traceHeaders) {
            if (hdr->getHeaderId() == segysdk ::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CMP or hdr->getExternalHeaderId() == segysdk ::segyformat::external::SegyTraceHeaderStringConsts::EnsembleNumber) {
                cmpFound = true;
                cmpOffset = hdr->getOffset();
            }
            if (hdr->getHeaderId() == segysdk ::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::EnergySourcePointNumber or hdr->getExternalHeaderId() == segysdk ::segyformat::external::SegyTraceHeaderStringConsts::EnergySourcePointNumber) {
                spFound = true;
                spOffset = hdr->getOffset();
            }
        }
        for (const auto& hdr : *traceHeaders) {
            if (cmpFound and hdr->getOffset() == cmpOffset) {
                hdr->setDisplayName(segysdk ::segyformat::external::SegyTraceHeaderStringConsts::EnsembleNumber);
                std::string externalToken = segysdk ::segyformat::external::SegyTraceHeaderStringConsts::EnsembleNumber;
                hdr->setExternalHeaderId(externalToken);
            }
            if (spFound and hdr->getOffset() == spOffset) {
                hdr->setDisplayName(segysdk ::segyformat::external::SegyTraceHeaderStringConsts::EnergySourcePointNumber);
                std::string externalToken = segysdk ::segyformat::external::SegyTraceHeaderStringConsts::EnergySourcePointNumber;
                hdr->setExternalHeaderId(externalToken);
            }
        }
    }
}

std::string
SegySessionImpl::getDataTypeStringFromEnum(segyio::HeaderDataType dataType)
{
    switch (dataType) {
    case segyio::HeaderDataType::Text:
        return details::JSONNumericalFormatStrings::Text;
    case segyio::HeaderDataType::Integer:
        return details::JSONNumericalFormatStrings::Integer;
    case segyio::HeaderDataType::UnsignedInteger:
        return details::JSONNumericalFormatStrings::UnsignedInteger;
    case segyio::HeaderDataType::Float:
        return details::JSONNumericalFormatStrings::Float;
    case segyio::HeaderDataType::Other:
        return details::JSONNumericalFormatStrings::Other;
    case segyio::HeaderDataType::IBMFloat:
        return details::JSONNumericalFormatStrings::IBMFloat;
    case segyio::HeaderDataType::IEEEFloat:
        return details::JSONNumericalFormatStrings::IEEEFloat;
    }

    return details::JSONNumericalFormatStrings::Undefined;
}

std::string
SegySessionImpl::getScalarUsageStrategyStringFromEnum(segyio::ScalarUsageStrategy scalarUsageStrategy)
{
    switch (scalarUsageStrategy) {
    case segyio::ScalarUsageStrategy::NoScaling:
        return details::JSONScalingEnums::NoScaling;
    case segyio::ScalarUsageStrategy::Default:
        return details::JSONScalingEnums::Default;
    case segyio::ScalarUsageStrategy::Override:
        return details::JSONScalingEnums::Override;
    }

    return details::JSONScalingEnums::Default;
}

Json::Value
SegySessionImpl::getHeaderOverrideDefsAsJson(const std::vector<std::shared_ptr<segyio::HeaderOverride>>& overrides)
{

    Json::Value root;

    for (const auto& hdr : overrides) {
        Json::Value node;
        node["Id"] = hdr->getHeaderId();
        node["Value"] = static_cast<double>(hdr->getValue());

        root.append(node);
    }

    return root;
}

Json::Value
SegySessionImpl::getJsonRoot() const
{

    Json::Value root;

    root["metadata"] = getSegyMetaData();

    return root;
}

Json::Value
SegySessionImpl::getSegyMetaData() const
{

    Json::Value metadata;
    metadata["Filenames"] = getFileNamesAsJson(context_->getSegyFileNames());
    metadata["SegyRevision"] = segyRevision_;

    return metadata;
}

bool
SegySessionImpl::hasSegy3DFieldsDefined() const
{

    bool hasInlineDef = false;
    bool hasCrosslineDef = false;
    bool hasCMPDef = false;
    bool hasSPDef = false;

    for (const auto& hdr : traceHeaders_) {
        if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Inline) {
            hasInlineDef = true;
        }

        if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Crossline) {
            hasCrosslineDef = true;
            break;
        }

        if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ShotNumber) {
            hasSPDef = true;
        }
        if (hdr->getExternalHeaderId() == segyformat::external::SegyTraceHeaderStringConsts::SP) {
            hasSPDef = true;
        }
        if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CMP) {
            hasCMPDef = true;
            break;
        }
    }

    // Temporary Hack. If both 3D literals and only one or none of the 2D literals are defined then return true
    return hasInlineDef and hasCrosslineDef and (!hasCMPDef and !hasSPDef);
}

size_t
SegySessionImpl::getSegyFileSize() const
{
    if (selectedSegyFile_ < segyFilesVector_.size()) {
        return segyFilesVector_[selectedSegyFile_]->getFileSize();
    }

    return segyFilesVector_.size() > 0 ? segyFilesVector_[0]->getFileSize() : 0;
}

Json::Value
SegySessionImpl::getFileNamesAsJson(const std::vector<std::string>& segyFilesVector)
{

    Json::Value root;
    for (const std::string& fname : segyFilesVector) {
        root.append(fname);
    }

    return root;
}

Json::Value
SegySessionImpl::getTraceHeadersLiteralsAsJson(const std::vector<std::shared_ptr<segyio::HeaderDefinition>>& hdrLiterals)
{

    Json::Value root;
    for (const auto& hdr : hdrLiterals) {
        Json::Value columnName;
        columnName["Id"] = hdr->getExternalHeaderId();
        const int offset = static_cast<int>(hdr->getOffset()) + 1;
        columnName["Start"] = offset;
        const int endPos = offset + hdr->getByteCount() - 1;
        columnName["End"] = endPos;
        root.append(columnName);
    }

    return root;
}

std::shared_ptr<std::vector<std::shared_ptr<segyio::HeaderDefinition>>>
SegySessionImpl::getBinaryHeaderDefinitions() const
{
    return fileHeaderDescr_->getBinaryHeaders();
}

std::shared_ptr<std::vector<std::shared_ptr<segyio::HeaderDefinition>>>
SegySessionImpl::getTraceHeaderDefinitions() const
{
    return fileHeaderDescr_->getTraceHeaders();
}

std::vector<std::string>
SegySessionImpl::getHeaderIdsFromTraceHeaders() const
{
    std::vector<std::string> v;
    v.reserve(traceHeaders_.size());
    for (const auto& hdr : traceHeaders_)
        v.push_back(hdr->getHeaderId());
    return v;
}

std::unique_ptr<bulkaccess::SegyBulkReader>
SegySessionImpl::createBulkReader()
{
    loadIndexFile();
    const auto index = getIndex();
    const std::shared_ptr<segyio::SegyFile> file = segyFilesVector_[selectedSegyFile_];
    const auto headerDescription = file->getFileHeaderDescription();
    const auto details = file->getDerivedHeader()->getConversionDetails();
    const auto reader = file->getFile();
    const auto indexReader = file->getCloneFile();

    return bulkaccess::SegyReaderFactory::createReader(reader,
                                                       indexReader,
                                                       index,
                                                       headerDescription,
                                                       details,
                                                       this->cacheSize_);
}

std::unique_ptr<bulkaccess::SegyBulkReader>
SegySessionImpl::createMultiBulkReader(std::shared_ptr<segyindex::SegyIndex> segyIndex,
                                       size_t numberOfTraces, size_t bulksCount)
{
    const std::shared_ptr<segyio::SegyFile> file = segyFilesVector_[selectedSegyFile_];
    const auto headerDescription = file->getFileHeaderDescription();
    const auto details = file->getDerivedHeader()->getConversionDetails();

    std::vector<decltype(file->getCloneFile())> readers(bulksCount, nullptr);
    std::generate(
        readers.begin(), readers.end(), [&file]() -> auto {
            return file->getCloneFile();
        });

    return bulkaccess::SegyReaderFactory::createMultiBulkReader(std::move(readers),
                                                                numberOfTraces,
                                                                segyIndex,
                                                                headerDescription,
                                                                details,
                                                                this->cacheSize_);
}

std::unique_ptr<bulkaccess::SegyBulkReader>
SegySessionImpl::createSectionBulkReader(size_t readersCount)
{
    loadIndexFile();
    const auto index = getIndex();
    const std::shared_ptr<segyio::SegyFile> file = segyFilesVector_[selectedSegyFile_];
    const auto headerDescription = file->getFileHeaderDescription();
    const auto details = file->getDerivedHeader()->getConversionDetails();
    const auto indexReader = file->getFile();

    std::vector<decltype(file->getCloneFile())> readers(readersCount, nullptr);
    std::generate(
        readers.begin(), readers.end(), [&file]() -> auto {
            return file->getCloneFile();
        });

    return bulkaccess::SegyReaderFactory::createSectionBulkReader(std::move(readers),
                                                                  indexReader,
                                                                  index,
                                                                  headerDescription,
                                                                  details,
                                                                  this->cacheSize_);
}

std::unique_ptr<bulkaccess::async::Reader>
SegySessionImpl::createAsyncReader(const ReaderCreateParams& params)
{
    loadIndexFile();
    return std::make_unique<bulkaccess::async::ReaderImpl>(
        std::make_shared<bulkaccess::live::ReaderConnector>(
            bulkaccess::live::ReaderConnectorParams{params}
                .withSegyFile(segyFilesVector_[selectedSegyFile_])
                .withSegyIndex(getIndex())
                .withFileLayout(getFileLayout(params))));
}

FileLayout
SegySessionImpl::getFileLayout(const request::Params& params) const
{
    const auto& file = segyFilesVector_[selectedSegyFile_];
    const auto& index = getIndex(params);
    const auto& bulkIndex = bulkaccess::IndexedBulkIndex{
        index, file->getFileHeaderDescription(), file->getDerivedHeader()->getConversionDetails()};
    return index->isSegy3D()
               ? FileLayout{bulkIndex.getSizeI(), bulkIndex.getSizeJ(), bulkIndex.getSizeK()}
               : FileLayout{bulkIndex.getTracesCount(), 1, bulkIndex.getSamplesCount()};
}

fileio::FileReference
SegySessionImpl::getFileReference(const std::string& fileName) const
{
    return {fileName.empty()
                ? segyFilesVector_[selectedSegyFile_]->getFileName()
                : fileName,
            {sdCredentials_, cacheSize_, context_->getLegalTag()}};
}

std::string
SegySessionImpl::readTextFile(const std::string& textFilePath) const
{
    const auto reader = utilities::JsonUtilities::CreateGenericFileReader(textFilePath);
    if (reader == nullptr or !reader->exists()) {
        const std::string msg = "Input text file does not exist. [" + textFilePath + "]";
        throw segysdk::SegyInvalidArgumentException(msg);
    }

    const auto fileSize = reader->getSize();

    if (fileSize > 0) {
        const auto result = reader->readText();
        return result;
    }

    return "";
}

std::string
SegySessionImpl::getOsduMetadata(const std::string& path_or_id) const
{
    std::cout << "Looking for input file " << path_or_id << "\n";
    return utilities::JsonUtilities::getJsonStringFromPathOrOsduId(path_or_id);
}

bool
SegySessionImpl::validateSession() const
{
    return binaryHeaders_.size() > 15 and traceHeaders_.size() > 30 and segyFilesVector_.size() > 0 and context_ != nullptr;
}

std::unique_ptr<bulkaccess::Segy2DReader>
SegySessionImpl::create2DReader()
{
    loadIndexFile();
    const auto index = getIndex();
    const std::shared_ptr<segyio::SegyFile> file = segyFilesVector_[selectedSegyFile_];
    const auto headerDescription = file->getFileHeaderDescription();
    const auto details = file->getDerivedHeader()->getConversionDetails();
    const auto reader = file->getFile();
    const auto indexReader = file->getCloneFile();

    return bulkaccess::SegyReaderFactory::create2DReader(reader,
                                                         indexReader,
                                                         index,
                                                         headerDescription,
                                                         details,
                                                         this->cacheSize_);
}

SessionImplParams&
SessionImplParams::withSegyContext(const SegyContext& value)
{
    segyContext_ = value;
    return *this;
}

SessionImplParams&
SessionImplParams::withBinaryHeaders(const HeaderDefinitions& value)
{
    binaryHeaders_ = value;
    return *this;
}

SessionImplParams&
SessionImplParams::withTraceHeaders(const HeaderDefinitions& value)
{
    traceHeaders_ = value;
    return *this;
}

SessionImplParams&
SessionImplParams::withBinaryHeaderOverrides(const HeaderOverrides& value)
{
    binaryHeaderOverrides_ = value;
    return *this;
}

SessionImplParams&
SessionImplParams::withTraceHeaderOverrides(const HeaderOverrides& value)
{
    traceHeaderOverrides_ = value;
    return *this;
}

} // namespace segysdk::api
