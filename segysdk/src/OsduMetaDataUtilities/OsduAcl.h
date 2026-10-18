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

#include "../OsduMetaDataUtilities/OsduAclStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>
#include <string>

namespace segysdk::metadata::osdu
{
/*

 "acl": {
    "owners": [
      "someone@company.com"
    ],
    "viewers": [
      "someone@company.com"
    ]
  },

*/

class SEGYSDK_DLL_PUBLIC OsduAcl final : public OsduMetadataBase
{
public:
    OsduAcl(const std::string& osduPartition);
    OsduAcl(const OsduAcl& other);
    OsduAcl(OsduAcl&& other) noexcept;
    OsduAcl& operator=(const OsduAcl& rhs);
    OsduAcl& operator=(OsduAcl&& rhs) noexcept;
    ~OsduAcl() override = default;

    OsduAcl(OsduAclStruct aclStruct);
    explicit OsduAcl(Json::Value json, const std::string& osduPartition);

public:
    void addOwner(const std::string& owner);
    void addViewer(const std::string& viewer);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduAclStruct aclStruct_;
};

} // namespace segysdk::metadata::osdu
