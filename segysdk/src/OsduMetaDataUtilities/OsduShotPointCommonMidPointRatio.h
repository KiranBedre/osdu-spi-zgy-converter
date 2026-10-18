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

#include "../OsduMetaDataUtilities/OsduShotPointCommonMidPointRatioStruct.h"

#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>
#include <string>

namespace segysdk::metadata::osdu
{
class SEGYSDK_DLL_PUBLIC OsduShotPointCommonMidPointRatio final : public OsduMetadataBase
{
public:
    OsduShotPointCommonMidPointRatio(const std::string& osduPartition);
    OsduShotPointCommonMidPointRatio(const OsduShotPointCommonMidPointRatio& other) = default;
    OsduShotPointCommonMidPointRatio(OsduShotPointCommonMidPointRatio&& other) noexcept = default;
    OsduShotPointCommonMidPointRatio& operator=(const OsduShotPointCommonMidPointRatio& rhs) = default;
    OsduShotPointCommonMidPointRatio& operator=(OsduShotPointCommonMidPointRatio&& rhs) noexcept = default;
    ~OsduShotPointCommonMidPointRatio() override = default;

    explicit OsduShotPointCommonMidPointRatio(OsduShotPointCommonMidPointRatioStruct shotPointCommonMidPointRatioStruct);
    explicit OsduShotPointCommonMidPointRatio(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;
    void initializeJson() override;

private:
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduShotPointCommonMidPointRatioStruct shotPointCommonMidPointRatioStruct_;
};
} // namespace segysdk::metadata::osdu
