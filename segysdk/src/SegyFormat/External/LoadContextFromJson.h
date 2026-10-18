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

#include "SegyContext.h"
#include "SegyHeaderField.h"

#include <json/json.h>

#include <map>
#include <memory>
#include <vector>

namespace segysdk::segyformat::external
{

class SEGYSDK_DLL_PUBLIC LoadContextFromJson
{

private:
    Json::Value jsonRoot_;
    bool parsingSuccessful_;
    std::map<std::string, std::string> defaultScalarMapper_;
    std::map<std::string, std::string> externalToStandardKeyMapper_;

protected:
    virtual bool isHeaderDefinitionsNode(const std::string& key, const Json::Value& node) const = 0;
    virtual std::string getInputKeyFromKey(const std::string& key) const = 0;
    virtual std::map<std::string, std::string> getScalarMapper() const final;

public:
    static const int TraceHeaderSize = 240;

protected:
    explicit LoadContextFromJson(const Json::Value& json);
    virtual ~LoadContextFromJson();

    void SetDefaultScalarMapper(const std::map<std::string, std::string>& scalsrMapper) { defaultScalarMapper_ = scalsrMapper; }
    void SetExternalToStandardKeyMapper(const std::map<std::string, std::string>& mapper) { externalToStandardKeyMapper_ = mapper; }

    std::vector<std::string> getMemberNames() const { return jsonRoot_.getMemberNames(); }
    Json::Value getJsonNode(const std::string& member) { return jsonRoot_.isMember(member) ? jsonRoot_[member] : Json::Value(); }
    std::string getMemberStringValue(const std::string& member) { return jsonRoot_.isMember(member) ? jsonRoot_[member].asString() : ""; }
    int getMemberIntValue(const std::string& member) { return jsonRoot_.isMember(member) ? jsonRoot_[member].asInt() : 0; }
    uint32_t getMemberUIntValue(const std::string& member) { return jsonRoot_.isMember(member) ? jsonRoot_[member].asUInt() : 0; }
    uint64_t getMemberUInt64Value(const std::string& member) { return jsonRoot_.isMember(member) ? jsonRoot_[member].asUInt64() : 0; }

public:
    bool canGetContext() const;
    virtual std::shared_ptr<SegyContext> getContext(const std::vector<std::string>& segyFileList, const int segyVersion);

public:
    static std::vector<std::shared_ptr<SegyHeaderField>> mergeInputHeadersWithStandard(const std::vector<std::shared_ptr<SegyHeaderField>>& inputHeaders, const int segyVersion);
    static bool checkForOverlappAndEmptyDefinitions(std::vector<std::shared_ptr<SegyHeaderField>>& mergedHdrs);

protected:
    static void padUpMissingFields(const unsigned char hdrMapping[], std::vector<std::shared_ptr<SegyHeaderField>>& mergedHdrs);
    static std::shared_ptr<SegyHeaderField> makePaddedField(unsigned short unmappedCount, unsigned short byteCount, int offset);

protected:
    virtual std::map<std::string, std::string> getInterfaceToStandardKeyMapper() const = 0;

    virtual std::vector<std::shared_ptr<SegyOverrideField>> updateBinaryHeaderOverridesFromJSON(const Json::Value& node) const = 0;
    virtual std::vector<std::shared_ptr<SegyOverrideField>> updateTraceHeaderOverridesFromJSON(const Json::Value& node) const = 0;
    virtual std::vector<std::shared_ptr<SegyHeaderField>> updateTraceHeadersFromJSON(const Json::Value& node, const std::shared_ptr<SegyContext>& context) = 0;

    virtual DataFormat getDataFormatFromNode(const std::string& value) const = 0;
    virtual std::string getUoMFromNode(const std::string& value) const = 0;
    virtual ScalarIndicator getScalarIndicatorFromNode(const std::string& value) const final;
    virtual std::string getStandardKeyFromInterfaceKey(const std::string& key) const final;
    virtual std::string getDefaultScalarField(const std::string& key, const ScalarIndicator scalarIndicator) const final;

private:
    static bool isMemberOf(const std::vector<std::shared_ptr<SegyHeaderField>>& headers, const std::string& keyName);
    static bool isOverlapping(const std::vector<std::shared_ptr<SegyHeaderField>>& headers, const std::shared_ptr<SegyHeaderField>& stdHdr);
};

} // namespace segysdk::segyformat::external
