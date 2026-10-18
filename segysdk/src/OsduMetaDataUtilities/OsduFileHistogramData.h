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

#include "../OsduMetaDataUtilities/OsduFileHistogramDataStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataBase.h"

namespace segysdk::metadata::osdu
{
class SEGYSDK_DLL_PUBLIC OsduFileHistogramData final : public OsduMetadataBase
{
public:
    OsduFileHistogramData(const std::string& osduPartition);
    OsduFileHistogramData(const OsduFileHistogramData& other) = default;
    OsduFileHistogramData(OsduFileHistogramData&& other) noexcept = default;
    OsduFileHistogramData& operator=(const OsduFileHistogramData& rhs) = default;
    OsduFileHistogramData& operator=(OsduFileHistogramData&& rhs) noexcept = default;
    ~OsduFileHistogramData() override = default;

    explicit OsduFileHistogramData(OsduFileHistogramDataStruct fileHistogramStruct);
    explicit OsduFileHistogramData(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduFileHistogramDataStruct fileHistogramDataStruct_;
};
} // namespace segysdk::metadata::osdu
