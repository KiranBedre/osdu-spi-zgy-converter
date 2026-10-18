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

#include "LoadContextFromJson.h"

#include "SegyHeaderField.h"

#include <json/json.h>

#include <map>
#include <optional>

namespace segysdk::segyformat::external
{

class SEGYSDK_DLL_PUBLIC LoadContextFromOSDUJson final : public LoadContextFromJson
{
protected:
    bool isHeaderDefinitionsNode(const std::string& key, const Json::Value& node) const override;

    std::map<std::string, std::string> getInterfaceToStandardKeyMapper() const override;
    std::string getInputKeyFromKey(const std::string& key) const override;

    virtual DataFormat getDataFormatFromNode(const std::string& formatString) const override;

    virtual std::string getUoMFromNode(const std::string& unitString) const override;

public:
    explicit LoadContextFromOSDUJson(const Json::Value& json);

    std::shared_ptr<SegyContext> getContext(const std::vector<std::string>& segyFileList, const int segyVersion) override;
    
    std::vector<std::shared_ptr<SegyOverrideField>> updateBinaryHeaderOverridesFromJSON(const Json::Value& node) const override;
    virtual std::vector<std::shared_ptr<SegyOverrideField>> updateTraceHeaderOverridesFromJSON(const Json::Value& node) const override;
    virtual std::vector<std::shared_ptr<SegyHeaderField>> updateTraceHeadersFromJSON(const Json::Value& node, const std::shared_ptr<SegyContext>& context) override;
};

} // namespace segysdk::segyformat::external
