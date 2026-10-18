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

#include "../OsduMetaDataUtilities/OsduOverridesBinaryHeaderStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>
#include <string>

namespace segysdk::metadata::osdu
{
/*

 "TraceHeaderOverrides": [
      {
        "KeyName": "namespace:reference-data--HeaderKeyName:CMPX:",
        "OverrideByConstant": 12345.6,
        "OverrideByExpressions": [
          {
            "Input": "namespace:reference-data--HeaderKeyName:CMPX:",
            "Operator1": "Plus",
            "Operand1Constant": 12345.6,
            "Operand1Variable": "namespace:reference-data--HeaderKeyName:CMPX:",
            "Operator2": "Plus",
            "Operand2Constant": 12345.6,
            "Operand2Variable": "namespace:reference-data--HeaderKeyName:CMPX:",
            "Remark": "Example Remark"
          }
        ]
      }
    ],
    "BinaryHeaderOverrides": [
      {
        "KeyName": "namespace:reference-data--HeaderKeyName:CMPX:",
        "OverrideByConstant": 2
      }
    ],

  
*/

class SEGYSDK_DLL_PUBLIC OsduOverridesBinaryHeader final : public OsduMetadataBase
{
public:
    OsduOverridesBinaryHeader(std::string osduPartition);
    OsduOverridesBinaryHeader(const OsduOverridesBinaryHeader& other);
    OsduOverridesBinaryHeader(OsduOverridesBinaryHeader&& other) noexcept;
    OsduOverridesBinaryHeader& operator=(const OsduOverridesBinaryHeader& rhs);
    OsduOverridesBinaryHeader& operator=(OsduOverridesBinaryHeader&& rhs) noexcept;
    ~OsduOverridesBinaryHeader() override = default;
    OsduOverridesBinaryHeader(OsduOverridesBinaryHeaderStruct binaryHeaderOverridesStruct);
    explicit OsduOverridesBinaryHeader(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduOverridesBinaryHeaderStruct vhmBhoStruct_;

};

}
