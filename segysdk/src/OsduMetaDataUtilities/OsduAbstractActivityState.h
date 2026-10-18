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

#include "../OsduMetaDataUtilities/OsduAbstractActivityStateStruct.h"

#include "../OsduMetaDataUtilities/OsduMetadataBase.h"

namespace segysdk::metadata::osdu
{
class SEGYSDK_DLL_PUBLIC OsduAbstractActivityState final : public OsduMetadataBase
{
public:
    OsduAbstractActivityState(const std::string& osduPartition);
    OsduAbstractActivityState(const OsduAbstractActivityState& other) = default;
    OsduAbstractActivityState(OsduAbstractActivityState&& other) noexcept = default;
    OsduAbstractActivityState& operator=(const OsduAbstractActivityState& rhs) = default;
    OsduAbstractActivityState& operator=(OsduAbstractActivityState&& rhs) noexcept = default;
    ~OsduAbstractActivityState() override = default;

    explicit OsduAbstractActivityState(OsduAbstractActivityStateStruct activityStateStruct);
    explicit OsduAbstractActivityState(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;
    void initializeJson() override;

private:
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduAbstractActivityStateStruct activityStateStruct_;
};
} // namespace segysdk::metadata::osdu
