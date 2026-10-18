/*
Copyright 2024 Schlumberger

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

#include "../OsduMetaDataUtilities/OsduCommonMetadataDataStruct.h"

#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../segysdk_details/DLL_EXPORT.h"

namespace segysdk::metadata::osdu
{
class SEGYSDK_DLL_PUBLIC OsduCommonMetadataData final : public OsduMetadataBase
{
public:
    OsduCommonMetadataData(const std::string& osduPartition);
    OsduCommonMetadataData(const OsduCommonMetadataData& other) = default;
    OsduCommonMetadataData(OsduCommonMetadataData&& other) noexcept = default;
    OsduCommonMetadataData& operator=(const OsduCommonMetadataData& rhs) = default;
    OsduCommonMetadataData& operator=(OsduCommonMetadataData&& rhs) noexcept = default;
    ~OsduCommonMetadataData() override = default;

    explicit OsduCommonMetadataData(OsduCommonMetadataDataStruct commonMetadataDataStruct);
    explicit OsduCommonMetadataData(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;
    void initializeJson() override;

private:
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduCommonMetadataDataStruct commonMetadataDataStruct_;
};
} // namespace segysdk::metadata::osdu
