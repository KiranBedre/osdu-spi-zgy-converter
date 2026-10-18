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

#include "../OsduMetaDataUtilities/OsduAbstractDatasetStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataBase.h"

namespace segysdk::metadata::osdu
{
class SEGYSDK_DLL_PUBLIC OsduAbstractDataset final : public OsduMetadataBase
{
public:
    OsduAbstractDataset(const std::string& osduPartition);
    OsduAbstractDataset(const OsduAbstractDataset& other) = default;
    OsduAbstractDataset(OsduAbstractDataset&& other) noexcept = default;
    OsduAbstractDataset& operator=(const OsduAbstractDataset& rhs) = default;
    OsduAbstractDataset& operator=(OsduAbstractDataset&& rhs) noexcept = default;
    ~OsduAbstractDataset() override = default;

    explicit OsduAbstractDataset(OsduAbstractDatasetStruct abstractDatasetStruct);
    explicit OsduAbstractDataset(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduAbstractDatasetStruct abstractDatasetStruct_;
};
}
