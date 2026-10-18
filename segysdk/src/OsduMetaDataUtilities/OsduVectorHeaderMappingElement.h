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

#include "../OsduMetaDataUtilities/OsduVectorHeaderMappingElementStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataBase.h"

namespace segysdk::metadata::osdu
{
/*

 "VectorHeaderMapping": [
            {
                "KeyName": "opendes:reference-data--HeaderKeyName:SP",
                "Position": 9,
                "ScalarIndicator": "NOSCALE",
                "ScalarOverride": 0,
                "UoM": "opendes:reference-data--UnitOfMeasure:Unitless",
                "WordFormat": "opendes:reference-data--WordFormatType:IEEEFLOAT",
                "WordWidth": 4
            },
            {
                "KeyName": "opendes:reference-data--HeaderKeyName:CMP",
                "Position": 21,
                "ScalarIndicator": "NOSCALE",
                "ScalarOverride": 0,
                "UoM": "opendes:reference-data--UnitOfMeasure:Unitless",
                "WordFormat": "opendes:reference-data--WordFormatType:IEEEFLOAT",
                "WordWidth": 4
            }
        ]
        
*/

class SEGYSDK_DLL_PUBLIC OsduVectorHeaderMappingElement final : public OsduMetadataBase
{
public:
    OsduVectorHeaderMappingElement(const std::string& osduPartition);
    OsduVectorHeaderMappingElement(const OsduVectorHeaderMappingElement& other) = default;
    OsduVectorHeaderMappingElement(OsduVectorHeaderMappingElement&& other) noexcept = default;
    OsduVectorHeaderMappingElement& operator=(const OsduVectorHeaderMappingElement& rhs) = default;
    OsduVectorHeaderMappingElement& operator=(OsduVectorHeaderMappingElement&& rhs) noexcept = default;
    ~OsduVectorHeaderMappingElement() override = default;

    OsduVectorHeaderMappingElement(OsduVectorHeaderMappingElementStruct vhmStruct);
    explicit OsduVectorHeaderMappingElement(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduVectorHeaderMappingElementStruct vhmElementStruct_;
};
}
