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

#include "../OsduMetaDataUtilities/OsduPointRangeStruct.h"

#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>
#include <string>

namespace segysdk::metadata::osdu
{
class SEGYSDK_DLL_PUBLIC OsduPointRange final : public OsduMetadataBase
{
public:
    OsduPointRange(const std::string& osduPartition);
    OsduPointRange(const OsduPointRange& other) = default;
    OsduPointRange(OsduPointRange&& other) noexcept = default;
    OsduPointRange& operator=(const OsduPointRange& rhs) = default;
    OsduPointRange& operator=(OsduPointRange&& rhs) noexcept = default;
    ~OsduPointRange() override = default;

    explicit OsduPointRange(OsduPointRangeStruct pointRangeStruct);
    explicit OsduPointRange(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;
    void initializeJson() override;

private:
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduPointRangeStruct pointRangeStruct_;
};
} // namespace segysdk::metadata::osdu
