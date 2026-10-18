/*
Copyright 2023 Schlumberger

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

#include "../OsduMetaDataUtilities/OsduAbstractContentStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>
#include <string>

namespace segysdk::metadata::osdu
{
/*
{
 "SchemaKind": "osd:wks:content--SeismicHistogram:1.0.0",
  "Name": "ST0202R08_PS_PSDM_RAW_PP_TIME.MIG_RAW.POST_Stack.3D.JS-017534",
  "Description": "Example histogram for time-migrated trace data.",
  "ExtensionProperties": {
    "Kind": "abc:xyz:CustomKind:0.0.0",
    "Custom": "Value"
  },
  ...

  }
*/

class SEGYSDK_DLL_PUBLIC OsduAbstractContent final : public OsduMetadataBase
{
public:
    OsduAbstractContent(const std::string& osduPartition);
    OsduAbstractContent(const OsduAbstractContent& other);
    OsduAbstractContent(OsduAbstractContent&& other) noexcept;
    OsduAbstractContent& operator=(const OsduAbstractContent& rhs);
    OsduAbstractContent& operator=(OsduAbstractContent&& rhs) noexcept;
    ~OsduAbstractContent() override = default;

    OsduAbstractContent(OsduAbstractContentStruct abstractContentStruct);
    explicit OsduAbstractContent(Json::Value json, const std::string& osduPartition);

public:
    void addOwner(const std::string& owner);
    void addViewer(const std::string& viewer);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    std::optional<Json::Value> getExtensionPropertiesJson() const;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduAbstractContentStruct abstractContentStruct_;
};

} // namespace segysdk::metadata::osdu
