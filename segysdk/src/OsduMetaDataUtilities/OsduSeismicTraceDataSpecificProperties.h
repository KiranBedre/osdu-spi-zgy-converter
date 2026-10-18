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

#include "../OsduMetaDataUtilities/OsduSeismicTraceDataSpecificPropertiesStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataBase.h"

namespace segysdk::metadata::osdu
{
class SEGYSDK_DLL_PUBLIC OsduSeismicTraceDataSpecificProperties final : public OsduMetadataBase
{
public:
    OsduSeismicTraceDataSpecificProperties(const std::string& osduPartition);
    OsduSeismicTraceDataSpecificProperties(const OsduSeismicTraceDataSpecificProperties& other) = default;
    OsduSeismicTraceDataSpecificProperties(OsduSeismicTraceDataSpecificProperties&& other) noexcept = default;
    OsduSeismicTraceDataSpecificProperties& operator=(const OsduSeismicTraceDataSpecificProperties& rhs) = default;
    OsduSeismicTraceDataSpecificProperties& operator=(OsduSeismicTraceDataSpecificProperties&& rhs) noexcept = default;
    ~OsduSeismicTraceDataSpecificProperties() override = default;

    explicit OsduSeismicTraceDataSpecificProperties(OsduSeismicTraceDataSpecificPropertiesStruct dataStruct);
    explicit OsduSeismicTraceDataSpecificProperties(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduSeismicTraceDataSpecificPropertiesStruct seismicTraceDataPropertiesStruct_;
};
}
