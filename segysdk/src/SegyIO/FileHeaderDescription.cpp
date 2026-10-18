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

#include "FileHeaderDescription.h"

#include "HeaderDefinition.h"
#include "JsonFileUtilities.h"

#include "../FileIO/FileReference.h"
#include "../FileIO/GenericFileManager.h"
#include "../SegyFormat/External/UoMStringConsts.h"
#include "../SegyFormat/Internal/FileHeaderDefinitionStringConstsTraceHeader.h"
#include "../SegyIO/HeaderOverride.h"
#include "../SegyIO/SegyIOException.h"
#include "Logger/Logger.h"

#include <iostream>
#include <json/json.h>
#include <utility>
#include <vector>

namespace segysdk::segyio
{

// constructor
FileHeaderDescription::FileHeaderDescription(const std::vector<std::shared_ptr<HeaderDefinition>>& binaryHeaders, const std::vector<std::shared_ptr<HeaderDefinition>>& traceHeaders)
{
    this->binaryHeaders_ = std::make_shared<std::vector<std::shared_ptr<HeaderDefinition>>>();
    for (const auto& hdr : binaryHeaders) {
        this->binaryHeaders_->push_back(hdr);
    }

    this->traceHeaders_ = std::make_shared<std::vector<std::shared_ptr<HeaderDefinition>>>();
    for (const auto& hdr : traceHeaders) {
        this->traceHeaders_->push_back(hdr);
    }

    binaryHeaderOverrides_ = std::make_shared<std::map<std::string, std::shared_ptr<HeaderOverride>>>();
    traceHeaderOverrides_ = std::make_shared<std::map<std::string, std::shared_ptr<HeaderOverride>>>();
}

FileHeaderDescription::~FileHeaderDescription()
{}

// Copy constructor
FileHeaderDescription::FileHeaderDescription(const FileHeaderDescription& other)
{

    this->binaryHeaders_ = std::make_shared<std::vector<std::shared_ptr<HeaderDefinition>>>();
    for (const auto& hdr : *other.binaryHeaders_) {
        this->binaryHeaders_->push_back(hdr);
    }

    this->traceHeaders_ = std::make_shared<std::vector<std::shared_ptr<HeaderDefinition>>>();
    for (const auto& hdr : *other.traceHeaders_) {
        this->binaryHeaders_->push_back(hdr);
    }

    binaryHeaderOverrides_ = std::make_shared<std::map<std::string, std::shared_ptr<HeaderOverride>>>();
    for (const auto& bho : *other.binaryHeaderOverrides_) {
        binaryHeaderOverrides_->emplace(bho);
    }

    traceHeaderOverrides_ = std::make_shared<std::map<std::string, std::shared_ptr<HeaderOverride>>>();
    for (const auto& tho : *other.traceHeaderOverrides_) {
        traceHeaderOverrides_->emplace(tho);
    }
}

// Move constructor
FileHeaderDescription::FileHeaderDescription(FileHeaderDescription&& other) noexcept
{
    *this = std::move(other);
}

// Copy assignment
FileHeaderDescription&
FileHeaderDescription::operator=(const FileHeaderDescription& other)
{

    if (this == &other)
        return *this;

    this->binaryHeaders_ = std::make_shared<std::vector<std::shared_ptr<HeaderDefinition>>>();
    for (const auto& hdr : *other.binaryHeaders_) {
        this->binaryHeaders_->push_back(hdr);
    }

    this->traceHeaders_ = std::make_shared<std::vector<std::shared_ptr<HeaderDefinition>>>();
    for (const auto& hdr : *other.traceHeaders_) {
        this->binaryHeaders_->push_back(hdr);
    }

    binaryHeaderOverrides_ = std::make_shared<std::map<std::string, std::shared_ptr<HeaderOverride>>>();
    for (const auto& bho : *other.binaryHeaderOverrides_) {
        binaryHeaderOverrides_->emplace(bho);
    }

    traceHeaderOverrides_ = std::make_shared<std::map<std::string, std::shared_ptr<HeaderOverride>>>();
    for (const auto& tho : *other.traceHeaderOverrides_) {
        traceHeaderOverrides_->emplace(tho);
    }

    return *this;
}

// Move assignment
FileHeaderDescription&
FileHeaderDescription::operator=(FileHeaderDescription&& other) noexcept
{

    if (this == &other)
        return *this;

    std::swap(other.binaryHeaders_, binaryHeaders_);
    std::swap(other.binaryHeaderOverrides_, binaryHeaderOverrides_);
    std::swap(other.traceHeaders_, traceHeaders_);
    std::swap(other.traceHeaderOverrides_, traceHeaderOverrides_);

    return *this;
}

std::shared_ptr<std::vector<std::shared_ptr<HeaderDefinition>>>
FileHeaderDescription::getBinaryHeaders() const
{
    return this->binaryHeaders_;
}

std::shared_ptr<std::vector<std::shared_ptr<HeaderDefinition>>>
FileHeaderDescription::getTraceHeaders() const
{
    return this->traceHeaders_;
}

std::shared_ptr<std::map<std::string, std::shared_ptr<HeaderOverride>>>
FileHeaderDescription::getBinaryHeaderOverrides() const
{
    return this->binaryHeaderOverrides_;
}

std::shared_ptr<std::map<std::string, std::shared_ptr<HeaderOverride>>>
FileHeaderDescription::getTraceHeaderOverrides() const
{
    return this->traceHeaderOverrides_;
}

void
FileHeaderDescription::setBinaryHeaderOverrides(const std::vector<std::shared_ptr<HeaderOverride>>& binaryHeaders) const
{
    this->binaryHeaderOverrides_->clear();
    for (const auto& binaryHeader : binaryHeaders) {
        this->binaryHeaderOverrides_->emplace(binaryHeader->getHeaderId(), binaryHeader);
    }
}

void
FileHeaderDescription::setTraceHeaderOverrides(const std::vector<std::shared_ptr<HeaderOverride>>& traceHeaders) const
{
    this->traceHeaderOverrides_->clear();
    for (const auto& traceHeader : traceHeaders) {
        this->traceHeaderOverrides_->emplace(traceHeader->getHeaderId(), traceHeader);
    }
}

void
FileHeaderDescription::setBinaryHeaderOverrides(const std::vector<const HeaderOverride*>& binaryHeaders) const
{
    this->binaryHeaderOverrides_->clear();
    for (const auto* header_override : binaryHeaders) {
        this->binaryHeaderOverrides_->emplace(header_override->getHeaderId(), std::make_shared<HeaderOverride>(*header_override));
    }
}

void
FileHeaderDescription::setTraceHeaderOverrides(const std::vector<const HeaderOverride*>& traceHeaders) const
{
    this->traceHeaderOverrides_->clear();
    for (const auto* header_override : traceHeaders) {
        this->traceHeaderOverrides_->emplace(header_override->getHeaderId(), std::make_shared<HeaderOverride>(*header_override));
    }
}

VerticalDomain
FileHeaderDescription::getVerticalDomain() const
{
    const VerticalUOM uom = getVerticalUOM();

    if (uom == VerticalUOM::meter || uom == VerticalUOM::feet)
        return VerticalDomain::Depth;

    // This is now safe as getVerticalUOM() will throw if UoM is not recognized
    return VerticalDomain::Time;
}

VerticalUOM
FileHeaderDescription::getVerticalUOM() const
{
    const std::shared_ptr<HeaderDefinition> hdr = getTraceHeaderDefinition(segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ShotToRecordingLagTime);

    if (hdr == nullptr) {
        return VerticalUOM::seconds;
    }

    if (hdr->getUoM() == segyformat::external::UoMStringConsts::m) {
        return VerticalUOM::meter;
    }
    if (hdr->getUoM() == segyformat::external::UoMStringConsts::ft || hdr->getUoM() == segyformat::external::UoMStringConsts::ftUS) {
        return VerticalUOM::feet;
    }

    if (hdr->getUoM() == segyformat::external::UoMStringConsts::ms || hdr->getUoM() == segyformat::external::UoMStringConsts::us) {
        return VerticalUOM::seconds;
    }

    const std::string msg = "Missing or invalid header definition: " + hdr->getExternalHeaderId() + " must contain valid vertical Unit of measurement";
    throw SegyFormatException(msg);
}

std::shared_ptr<HeaderDefinition>
FileHeaderDescription::getTraceHeaderDefinition(const std::string& headerId) const
{
    for (auto hdr : *getTraceHeaders()) {
        if (hdr->getHeaderId() == headerId) {
            return hdr;
        }
    }
    return nullptr;
}

bool
FileHeaderDescription::parameterFileExists(const fileio::FileReference& segyFileReference)
{
    try {
        const std::string parameterFileName = getParameterFileName(segyFileReference.fileName());
        const fileio::FileReference paramFile(parameterFileName, segyFileReference.readerOptions());

        return fileio::GenericFileManager::exists(paramFile);
    }
    catch (...) {
        return false;
    }
}

std::vector<std::shared_ptr<HeaderOverride>>
FileHeaderDescription::getHeaderOverridesDefinitionFromJsonParameter(Json::Value node)
{

    std::vector<std::shared_ptr<HeaderOverride>> overrides;

    for (Json::Value::ArrayIndex i = 0; i != node.size(); i++) {
        std::string inputKey;
        double value = -999;

        if (node[i].isMember("KeyName")) {
            inputKey = node[i]["KeyName"].asString();
        }

        if (node[i].isMember("Value")) {
            value = node[i]["Value"].asDouble();
        }

        if (!inputKey.empty() && value != -999) {
            overrides.push_back(std::make_shared<HeaderOverride>(inputKey, static_cast<float>(value)));
        }
    }

    return overrides;
}

HeaderDataType
FileHeaderDescription::getHeaderDataTypeFromString(const std::string& dataType)
{
    if (dataType == "Integer")
        return HeaderDataType::Integer;
    if (dataType == "Float")
        return HeaderDataType::Float;
    if (dataType == "IBMFloat")
        return HeaderDataType::IBMFloat;
    if (dataType == "IEEEFloat")
        return HeaderDataType::IEEEFloat;
    if (dataType == "IEEEFloat")
        return HeaderDataType::IEEEFloat;
    if (dataType == "Other")
        return HeaderDataType::Other;
    if (dataType == "Text")
        return HeaderDataType::Text;
    if (dataType == "UnsignedInteger")
        return HeaderDataType::UnsignedInteger;

    return HeaderDataType::Integer;
}

ScalarUsageStrategy
FileHeaderDescription::getScalarUsageStrategyFromString(const std::string& strategy)
{
    if (strategy == "NoScaling")
        return ScalarUsageStrategy::NoScaling;
    if (strategy == "Default")
        return ScalarUsageStrategy::Default;
    if (strategy == "Override")
        return ScalarUsageStrategy::Override;

    return ScalarUsageStrategy::NoScaling;
}

std::vector<std::shared_ptr<HeaderDefinition>>
FileHeaderDescription::getHeaderDefinitionsFromJsonParameter(Json::Value node)
{
    std::vector<std::shared_ptr<HeaderDefinition>> headerDef;

    for (Json::Value::ArrayIndex i = 0; i != node.size(); i++) {
        auto memberNames = node[i].getMemberNames();

        std::string id;
        std::string externalId;
        std::string displayName;
        HeaderDataType dataType = HeaderDataType::Other;
        int offset = 0;
        int byteCount = 0;
        std::string fieldFormat;
        std::string scalarHeaderId;
        int scalarOverride = 0;
        bool isWellKnown = false;
        std::string uom;
        double extraUnitScalar = 1;
        ScalarUsageStrategy scalarUsageStrategy = ScalarUsageStrategy::Default;

        for (const auto& member : memberNames) {

            if (member == "Id") {
                id = node[i][member].asString();
            }
            else if (member == "ByteCount") {
                byteCount = node[i][member].asInt();
            }
            else if (member == "HeaderDataType") {
                dataType = getHeaderDataTypeFromString(node[i][member].asString());
            }
            else if (member == "DisplayName") {
                displayName = node[i][member].asString();
            }
            else if (member == "ExternalId") {
                externalId = node[i][member].asString();
            }
            else if (member == "ExtraUnitScalar") {
                extraUnitScalar = node[i][member].asDouble();
            }
            else if (member == "Offset") {
                offset = node[i][member].asInt();
            }
            else if (member == "IsWellKnown") {
                isWellKnown = node[i][member].asBool();
            }
            else if (member == "ScalarHeaderId") {
                scalarHeaderId = node[i][member].asString();
            }
            else if (member == "ScalarOverride") {
                scalarOverride = node[i][member].asInt();
            }
            else if (member == "ScalarUsageStrategy") {
                scalarUsageStrategy = getScalarUsageStrategyFromString(node[i][member].asString());
            }
            else if (member == "UoM") {
                uom = node[i][member].asString();
            }
            else {
                SEGYLOG_REPORT_WARNING("FileHeaderDescription::getHeaderDefinitionsFromJsonParameter Member not known: " + member);
            }
        }

        auto hdr = std::make_shared<HeaderDefinition>(id, displayName, offset, static_cast<unsigned short>(byteCount));
        hdr->setExternalHeaderId(externalId);
        hdr->setExtraUnitScalar(static_cast<float>(extraUnitScalar));
        hdr->setHeaderDataType(dataType);
        hdr->setIsWellKnown(isWellKnown);
        hdr->setScalarHeaderId(scalarHeaderId);
        hdr->setScalarOverride(scalarOverride);
        hdr->setScalarUsageStrategy(scalarUsageStrategy);
        hdr->setUoM(uom);

        headerDef.push_back(hdr);
    }

    return headerDef;
}

std::shared_ptr<FileHeaderDescription>
FileHeaderDescription::loadFromParameterFile(const std::string& segyFilename, fileio::FileReaderOptions options)
{
    const fileio::FileReference segyFileReference(segyFilename, std::move(options));
    if (!parameterFileExists(segyFileReference))
        return nullptr;

    return loadFromParameterFile(segyFileReference);
}

std::shared_ptr<FileHeaderDescription>
FileHeaderDescription::loadFromParameterFile(const fileio::FileReference& fileRef)
{
    const std::string parameterFileName = getParameterFileName(fileRef.fileName());
    const fileio::FileReference paramFileReference(parameterFileName, fileRef.readerOptions());

    Json::Value root = fileio::utilities::JsonFileUtilities::getJsonFromFile(paramFileReference);

    const auto membersNames = root.getMemberNames();

    std::vector<std::shared_ptr<HeaderDefinition>> bh;
    std::vector<std::shared_ptr<HeaderDefinition>> th;
    if (membersNames.size() < 2) {
        return std::make_shared<FileHeaderDescription>(bh, th);
    }

    std::vector<std::shared_ptr<HeaderOverride>> bhOverrides;
    std::vector<std::shared_ptr<HeaderOverride>> thOverrides;

    for (const std::string& member : membersNames) {
        const auto& node = root[member];

        if (member == "BinaryHeaders") {
            bh = getHeaderDefinitionsFromJsonParameter(node);
        }
        else if (member == "TraceHeaders") {
            th = getHeaderDefinitionsFromJsonParameter(node);
        }
        else if (member == "BinaryHeaderOverrides") {
            bhOverrides = getHeaderOverridesDefinitionFromJsonParameter(node);
        }
        else if (member == "TraceHeaderOverrides") {
            thOverrides = getHeaderOverridesDefinitionFromJsonParameter(node);
        }
    }

    auto fhd = std::make_shared<FileHeaderDescription>(bh, th);
    if (!bhOverrides.empty()) {
        fhd->setBinaryHeaderOverrides(bhOverrides);
    }
    if (!thOverrides.empty()) {
        fhd->setTraceHeaderOverrides(thOverrides);
    }
    return fhd;
}
std::string
FileHeaderDescription::getParameterFileName(const std::string& segyFileName)
{
    return segyFileName + ".params.json";
}

std::string
FileHeaderDescription::getHeaderDataTypeJson(const HeaderDataType dataType)
{
    switch (dataType) {
    case HeaderDataType::Text:
        return "Text";
    case HeaderDataType::Integer:
        return "Integer";
    case HeaderDataType::UnsignedInteger:
        return "UnsignedInteger";
    case HeaderDataType::Float:
        return "Float";
    case HeaderDataType::Other:
        return "Other";
    case HeaderDataType::IBMFloat:
        return "IBMFloat";
    case HeaderDataType::IEEEFloat:
        return "IEEEFloat";
    default: return "Integer";
    }
}

std::string
FileHeaderDescription::getHeaderScalarUsageStrategy(const ScalarUsageStrategy strategy)
{
    switch (strategy) {
    case ScalarUsageStrategy::Default:
        return "Default";
    case ScalarUsageStrategy::NoScaling:
        return "NoScaling";
    case ScalarUsageStrategy::Override:
        return "Override";
    default: return "Default";
    }
}

Json::Value
FileHeaderDescription::getSEGYHeaderJson(const std::vector<std::shared_ptr<HeaderDefinition>>& headers)
{
    Json::Value headerArrayJson;

    for (const auto& hdr : headers) {
        Json::Value hdrJson;

        hdrJson["Id"] = hdr->getHeaderId();
        hdrJson["ExternalId"] = hdr->getExternalHeaderId();
        hdrJson["DisplayName"] = hdr->getDisplayName();
        hdrJson["Offset"] = static_cast<Json::UInt64>(hdr->getOffset());
        hdrJson["ByteCount"] = hdr->getByteCount();
        hdrJson["ExtraUnitScalar"] = hdr->getExtraUnitScalar();
        hdrJson["HeaderDataType"] = getHeaderDataTypeJson(hdr->getHeaderDataType());
        hdrJson["IsWellKnown"] = hdr->getIsWellKnown();
        hdrJson["ScalarHeaderId"] = hdr->getScalarHeaderId();
        hdrJson["ScalarOverride"] = hdr->getScalarOverride();
        hdrJson["ScalarUsageStrategy"] = getHeaderScalarUsageStrategy(hdr->getScalarUsageStrategy());
        hdrJson["UoM"] = hdr->getUoM();

        headerArrayJson.append(hdrJson);
    }

    return headerArrayJson;
}

Json::Value
FileHeaderDescription::getSEGYHeaderOverridesJson(const std::map<std::string, std::shared_ptr<HeaderOverride>>& overrides)
{

    Json::Value overridesArrayJson;

    for (const auto& [id, header] : overrides) {
        Json::Value overrideJson;
        overrideJson["Id"] = header->getHeaderId();
        overrideJson["Value"] = header->getValue();

        overridesArrayJson.append(overrideJson);
    }

    return overridesArrayJson;
}

std::string
FileHeaderDescription::toJsonString() const
{
    Json::Value root;

    root["BinaryHeaders"] = getSEGYHeaderJson(*getBinaryHeaders());
    root["TraceHeaders"] = getSEGYHeaderJson(*getTraceHeaders());
    root["BinaryHeaderOverrides"] = getSEGYHeaderOverridesJson(*getBinaryHeaderOverrides());
    root["TraceHeaderOverrides"] = getSEGYHeaderOverridesJson(*getTraceHeaderOverrides());

    return root.toStyledString();
}

} // namespace segysdk::segyio
