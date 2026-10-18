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

#include "../OsduMetaDataUtilities/OsduAbstractProjectActivityStruct.h"

#include "../OsduMetaDataUtilities/OsduMetadataBase.h"

namespace segysdk::metadata::osdu
{
class SEGYSDK_DLL_PUBLIC OsduAbstractProjectActivity final : public OsduMetadataBase
{
public:
    OsduAbstractProjectActivity(const std::string& osduPartition);
    OsduAbstractProjectActivity(const OsduAbstractProjectActivity& other) = default;
    OsduAbstractProjectActivity(OsduAbstractProjectActivity&& other) noexcept = default;
    OsduAbstractProjectActivity& operator=(const OsduAbstractProjectActivity& rhs) = default;
    OsduAbstractProjectActivity& operator=(OsduAbstractProjectActivity&& rhs) noexcept = default;
    ~OsduAbstractProjectActivity() override = default;

    explicit OsduAbstractProjectActivity(OsduAbstractProjectActivityStruct activityStruct);
    explicit OsduAbstractProjectActivity(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;
    void initializeJson() override;

private:
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduAbstractProjectActivityStruct projectActivityStruct_;
};
} // namespace segysdk::metadata::osdu
