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

#include "../OsduMetaDataUtilities/OsduMetaElementStruct.h"
#include "../OsduMetaDataUtilities/OsduMetaArrayStruct.h"

#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>


namespace segysdk::metadata::osdu
{
/*

  "meta": [
      {
        "kind": "Unit",
        "name": "ft",
        "persistableReference": "{\"scaleOffset\":{\"scale\":0.3048,\"offset\":0.0},\"symbol\":\"ft\",\"baseMeasurement\":{\"ancestry\":\"Length\",\"type\":\"UM\"},\"type\":\"USO\"}",
        "propertyNames": [
          "elevationReferenceTime.elevationFromMsl.value"
        ]
      },
      {
        "kind": "Unit",
        "name": "ft/s",
        "persistableReference": "{\"scaleOffset\":{\"scale\":0.3048,\"offset\":0.0},\"symbol\":\"ft/s\",\"baseMeasurement\":{\"ancestry\":\"Velocity\",\"type\":\"UM\"},\"type\":\"USO\"}",
        "propertyNames": [
          "elevationReferenceTime.replacementVelocity.value"
        ]
      }
    ]
  
*/

class SEGYSDK_DLL_PUBLIC OsduMetaElement final : public OsduMetadataBase
{
public:
    OsduMetaElement(std::string osduPartition);
    OsduMetaElement(const OsduMetaElement& other);
    OsduMetaElement(OsduMetaElement&& other) noexcept;
    OsduMetaElement& operator=(const OsduMetaElement& rhs);
    OsduMetaElement& operator=(OsduMetaElement&& rhs) noexcept;
    ~OsduMetaElement() override = default;

    OsduMetaElement(OsduMetaElementStruct metaElementStruct);
    OsduMetaElement(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduMetaElementStruct metaStruct_;

};

}
