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

#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../OsduMetaDataUtilities/OsduLegalStruct.h"

#include "../segysdk_details/DLL_EXPORT.h"

namespace segysdk::metadata::osdu
{
/*
 
 "legal": {
    "legalTags": [
      "Example legalTags"
    ],
    "otherRelevantDataCountries": [
      "US"
    ],
    "status": "compliant"
  },

*/

class SEGYSDK_DLL_PUBLIC OsduLegal final : public OsduMetadataBase
{
public:
    OsduLegal(const std::string& osduPartition);
    OsduLegal(const OsduLegal& other);
    OsduLegal(OsduLegal&& other) noexcept;
    OsduLegal& operator=(const OsduLegal& rhs);
    OsduLegal& operator=(OsduLegal&& rhs) noexcept;
    ~OsduLegal() override = default;

    OsduLegal(OsduLegalStruct legalStruct);
    OsduLegal(Json::Value json, const std::string& osduPartition);

public:
    void addTag(const std::string& tag);
    void addCountry(const std::string& country);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduLegalStruct legalStruct_;

};

} // namespace segysdk::metadata::osdu
