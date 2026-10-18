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

#include "LoadContextFromJson.h"

#include "SegyHeaderField.h"
#include "SegyTraceHeaderStringConsts.h"
#include "TraceHeaderField.h"
#include "OsduOverrideNodeLiterals.h"
#include "../Logger/Logger.h"
#include <algorithm>

namespace segysdk::segyformat::external
{

LoadContextFromJson::LoadContextFromJson(const Json::Value& json)
  : jsonRoot_(json),
    parsingSuccessful_(!json.empty() && json.getMemberNames().size() > 0)
{
}

LoadContextFromJson::~LoadContextFromJson()
{
}

bool
LoadContextFromJson::canGetContext() const
{
    return parsingSuccessful_;
}

std::shared_ptr<SegyContext>
LoadContextFromJson::getContext(const std::vector<std::string>& segyFileList,
                                const int segyVersion)
{
    if (!canGetContext()) {
        return nullptr;
    }

    auto context = std::make_shared<SegyContext>(segyFileList, segyVersion);
    auto binaryHdrs = SegyContext::getSegyStandardBinaryHeaderDefinitions();
    context->setBinaryHeaderDefinitions(binaryHdrs);

    const auto memberNames = getMemberNames();

    if (!memberNames.empty()) {

        for (const std::string& member : memberNames) {
            auto node = getJsonNode(member);

            if (isHeaderDefinitionsNode(member, node)) {
                auto traceHeaders = updateTraceHeadersFromJSON(node, context);
                context->setTraceHeaderDefinitions(traceHeaders);

                // OSDU override checks
                if (node.isMember(osdu::OsduOverrideNodeLiterals::BinaryHeaderOverrides)) {
                    auto bh_node = node[osdu::OsduOverrideNodeLiterals::BinaryHeaderOverrides];
                    auto binaryHeaderOverrides = updateBinaryHeaderOverridesFromJSON(bh_node);
                    context->setBinaryHeaderOverrideDefinitions(binaryHeaderOverrides);
                }
                if (node.isMember(osdu::OsduOverrideNodeLiterals::TraceHeaderOverrides)) {
                    auto th_node = node[osdu::OsduOverrideNodeLiterals::TraceHeaderOverrides];
                    auto traceHeaderOverrides = updateTraceHeaderOverridesFromJSON(th_node);
                    context->setTraceHeaderOverrideDefinitions(traceHeaderOverrides);
                }
            }
            // Delfi style json checks below
            else if (member == "BinaryHeaderOverrides") {
                auto binaryHeaderOverrides = updateBinaryHeaderOverridesFromJSON(node);
                context->setBinaryHeaderOverrideDefinitions(binaryHeaderOverrides);
            }
            else if (member == "TraceHeaderOverrides") {
                auto traceHeaderOverrides = updateTraceHeaderOverridesFromJSON(node);
                context->setTraceHeaderOverrideDefinitions(traceHeaderOverrides);
            }
        }
    }
    return context;
}

std::string
LoadContextFromJson::getStandardKeyFromInterfaceKey(const std::string& key) const
{
    const auto pos = externalToStandardKeyMapper_.find(key);
    if (pos == externalToStandardKeyMapper_.end()) {
        // Not a OSDU defined key;
        return key;
    }
    else {
        return pos->second;
    }
}

std::string
LoadContextFromJson::getDefaultScalarField(const std::string& key,
                                           const ScalarIndicator scalarIndicator) const
{
    if (scalarIndicator == ScalarIndicator::NOSCALE || scalarIndicator == ScalarIndicator::OVERRIDE)
        return "";

    const auto pos = defaultScalarMapper_.find(key);
    if (pos == defaultScalarMapper_.end()) {
        // Not a scaled field by default;
        return "";
    }
    else {
        return pos->second;
    }
}

std::vector<std::shared_ptr<SegyHeaderField>>
LoadContextFromJson::mergeInputHeadersWithStandard(const std::vector<std::shared_ptr<SegyHeaderField>>& inputHeaders, const int segyVersion)
{

    // Integrate user defined field only if it falls into the acceptable locations, i.e. below location 29, between 69 and 88 or above 180
    std::vector<std::shared_ptr<SegyHeaderField>> stdHeaders = segyformat::external::SegyContext::getDefaultTraceHdrLocations(segyVersion);
    std::sort(stdHeaders.begin(), stdHeaders.end(),
              [](const std::shared_ptr<SegyHeaderField>& lhs, const std::shared_ptr<SegyHeaderField>& rhs) {
                  return lhs->getOffset() < rhs->getOffset();
              });

    std::vector<std::shared_ptr<SegyHeaderField>> mergedHeaders;

    // This vector array will hold the input literals we will actually use for the merge
    // i.e. NOT the ones in the protected offset range.
    std::vector<std::shared_ptr<SegyHeaderField>> inputHeadersToUse;
    for (const auto& ih : inputHeaders) {
        const int offset = ih->getOffset();
        if ((offset > 0 && offset <= 29) || (offset >= 69 && offset <= 89) || (offset >= 100 && offset <= 110) || (offset >= 180 && offset < 239)) {
            mergedHeaders.push_back(ih);
            inputHeadersToUse.push_back(ih);
        }      
    }

    // 1. If inputHdrs[ix]->getKey() == stdHdr[ix]->getKey() use inputHdrs[ix]
    // 2. If inputHdrs[ix] overlaps either partially or wholly, then do not use stdHdr

    for (const auto& stdHdr : stdHeaders) {
        std::string inputKey = stdHdr->getKey();
        if (isMemberOf(inputHeadersToUse, inputKey))
            continue;

        if (isOverlapping(inputHeadersToUse, stdHdr))
            continue;

        mergedHeaders.push_back(stdHdr);
    }

    std::sort(mergedHeaders.begin(), mergedHeaders.end(),
              [](const std::shared_ptr<SegyHeaderField>& lhs, const std::shared_ptr<SegyHeaderField>& rhs) {
                  return lhs->getOffset() < rhs->getOffset();
              });

    checkForOverlappAndEmptyDefinitions(mergedHeaders);
    return mergedHeaders;
}

bool
LoadContextFromJson::isMemberOf(const std::vector<std::shared_ptr<SegyHeaderField>>& headers, const std::string& keyName)
{
    return std::any_of(headers.begin(), headers.end(), [&keyName](const auto hdr) { return hdr->getKey() == keyName; });
}

bool
LoadContextFromJson::isOverlapping(const std::vector<std::shared_ptr<SegyHeaderField>>& headers, const std::shared_ptr<SegyHeaderField>& stdHdr)
{

    for (const auto& hdr : headers) {
        if (stdHdr->getOffset() >= hdr->getOffset() + hdr->getLength())
            continue; // use >= to avoid subtracting one, e.g. offset = 17 and length = 4 does not overlap field with offset = 21
        if (hdr->getOffset() >= stdHdr->getOffset() + stdHdr->getLength())
            return false; // use >= to avoid subtracting one, e.g. offset = 17 and length = 4 does not overlap field with offset = 21

        return true;
    }

    return false;
}

ScalarIndicator
LoadContextFromJson::getScalarIndicatorFromNode(const std::string& value) const
{
    if (value == "STANDARD") {
        return ScalarIndicator::STANDARD;
    }
    else if (value == "NOSCALE") {
        return ScalarIndicator::NOSCALE;
    }
    else if (value == "OVERRIDE") {
        return ScalarIndicator::OVERRIDE;
    }

    return ScalarIndicator::NOSCALE;
}

bool
LoadContextFromJson::checkForOverlappAndEmptyDefinitions(std::vector<std::shared_ptr<SegyHeaderField>>& mergedHdrs)
{
    unsigned char hdrMapping[240]{0};
    for (const auto& hdr : mergedHdrs) {
        const int offset = hdr->getOffset() - 1;
        const int length = hdr->getLength();
        for (auto ix = 0; ix < length; ix++) {
            hdrMapping[offset + ix] += 1;
        }
    }

    bool hasMissingFields = false;
    for (const unsigned char c : hdrMapping) {
        if (c > 1)
            return true;
        if (c == 0)
            hasMissingFields = true;
    }
    if (hasMissingFields) {
        padUpMissingFields(hdrMapping, mergedHdrs);
    }
    return false;
}

void
LoadContextFromJson::padUpMissingFields(const unsigned char hdrMapping[], std::vector<std::shared_ptr<SegyHeaderField>>& mergedhdrs)
{

    // hdrMapping is a 240 byte char array. If a trace header field has been mapped, it will block the extent of that field
    // A value of 0 means the byte is not included in any mapping
    // A value of 1 means the byte has been mapped by a single trace header literal
    // A value > 1 means overlapping definition of one or more fields.

    unsigned short unmappedCount = 0;
    unsigned short byteCount = 0;
    int offset = 0;

    int ix = 0;
    while (ix < TraceHeaderSize) {

        if (hdrMapping[ix] != 0) {
            ix++;
            if (byteCount > 0) {
                if (byteCount == 2 or byteCount == 4) {
                    mergedhdrs.push_back(makePaddedField(unmappedCount, byteCount, offset));
                }
                byteCount = 0;
            }
            continue;
        }

        if (byteCount == 0) {
            offset = ix + 1;
            unmappedCount++;
        }

        ix++;
        byteCount++;
        if (ix >= TraceHeaderSize) {
            break;
        }

        if (ix < TraceHeaderSize && hdrMapping[ix] == 0 && byteCount < 4) {
            continue;
        }

        mergedhdrs.push_back(makePaddedField(unmappedCount, byteCount, offset));
        byteCount = 0;
    }

    if (byteCount > 0) {
        mergedhdrs.push_back(makePaddedField(unmappedCount, byteCount, offset));
    }

    if (unmappedCount > 0) {
        std::sort(mergedhdrs.begin(), mergedhdrs.end(),
                  [](const std::shared_ptr<SegyHeaderField>& lhs, const std::shared_ptr<SegyHeaderField>& rhs) {
                      return lhs->getOffset() < rhs->getOffset();
                  });
    }
}

std::shared_ptr<SegyHeaderField>
LoadContextFromJson::makePaddedField(unsigned short unmappedCount, unsigned short byteCount, int offset)
{
    std::string keyName = (unmappedCount < 10) ? "Unmapped0" : "Unmapped";
    keyName += std::to_string(unmappedCount);
    std::shared_ptr<SegyHeaderField> pad = std::make_shared<TraceHeaderField>(keyName, keyName, DataFormat::Integer, byteCount, offset);

    return pad;
}

std::map<std::string, std::string>
LoadContextFromJson::getScalarMapper() const
{
    std::map<std::string, std::string> mapper;

    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::ReceiverGroupElevation, segyformat::external::SegyTraceHeaderStringConsts::ScalarForElevations);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::SurfaceElevationAtSource, segyformat::external::SegyTraceHeaderStringConsts::ScalarForElevations);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::SourceDepthBelowSurface, segyformat::external::SegyTraceHeaderStringConsts::ScalarForElevations);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::DatumElevationAtReceiverGroup, segyformat::external::SegyTraceHeaderStringConsts::ScalarForElevations);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::DatumElevationAtSource, segyformat::external::SegyTraceHeaderStringConsts::ScalarForElevations);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::WaterDepthAtReceiverGroup, segyformat::external::SegyTraceHeaderStringConsts::ScalarForElevations);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::WaterDepthAtSource, segyformat::external::SegyTraceHeaderStringConsts::ScalarForElevations);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::DatumElevationAtReceiverGroup, segyformat::external::SegyTraceHeaderStringConsts::ScalarForElevations);

    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::SourceCoordinateX, segyformat::external::SegyTraceHeaderStringConsts::ScalarForCoordinates);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::SourceCoordinateY, segyformat::external::SegyTraceHeaderStringConsts::ScalarForCoordinates);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::ReceiverGroupCoordinateX, segyformat::external::SegyTraceHeaderStringConsts::ScalarForCoordinates);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::ReceiverGroupCoordinateY, segyformat::external::SegyTraceHeaderStringConsts::ScalarForCoordinates);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::XCoordinateEnsemble, segyformat::external::SegyTraceHeaderStringConsts::ScalarForCoordinates);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::YCoordinateEnsemble, segyformat::external::SegyTraceHeaderStringConsts::ScalarForCoordinates);

    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::UpholeTimeAtSource, segyformat::external::SegyTraceHeaderStringConsts::ScalarForTimeValues);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::UpholeTimeAtReceiverGroup, segyformat::external::SegyTraceHeaderStringConsts::ScalarForTimeValues);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::SourceStaticCorrection, segyformat::external::SegyTraceHeaderStringConsts::ScalarForTimeValues);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::ReceiverGroupStaticCorrection, segyformat::external::SegyTraceHeaderStringConsts::ScalarForTimeValues);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::TotalStaticApplied, segyformat::external::SegyTraceHeaderStringConsts::ScalarForTimeValues);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::LagTimeA, segyformat::external::SegyTraceHeaderStringConsts::ScalarForTimeValues);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::LagTimeB, segyformat::external::SegyTraceHeaderStringConsts::ScalarForTimeValues);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::DelayRecordingTime, segyformat::external::SegyTraceHeaderStringConsts::ScalarForTimeValues);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::MuteTimeStart, segyformat::external::SegyTraceHeaderStringConsts::ScalarForTimeValues);
    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::MuteTimeEnd, segyformat::external::SegyTraceHeaderStringConsts::ScalarForTimeValues);

    mapper.try_emplace(segyformat::external::SegyTraceHeaderStringConsts::ShotpointNumber, segyformat::external::SegyTraceHeaderStringConsts::ScalarForShotpoint);

    return mapper;
}

} // namespace segysdk::segyformat::external
