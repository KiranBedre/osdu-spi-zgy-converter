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

#include "../OsduMetaDataUtilities/OsduFileCollectionOpenZgyStruct.h"
#include "../OsduMetaDataUtilities/OsduFileCollectionSegyStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataBase.h"

namespace segysdk::metadata::osdu
{
class SEGYSDK_DLL_PUBLIC OsduFileCollectionOpenZgy final : public OsduMetadataBase
{
public:
    OsduFileCollectionOpenZgy(const std::string& osduPartition);
    OsduFileCollectionOpenZgy(const OsduFileCollectionOpenZgy& other) = default;
    OsduFileCollectionOpenZgy(OsduFileCollectionOpenZgy&& other) noexcept = default;
    OsduFileCollectionOpenZgy& operator=(const OsduFileCollectionOpenZgy& rhs) = default;
    OsduFileCollectionOpenZgy& operator=(OsduFileCollectionOpenZgy&& rhs) noexcept = default;
    ~OsduFileCollectionOpenZgy() override = default;

    explicit OsduFileCollectionOpenZgy(OsduFileCollectionOpenZgyStruct FileCollectionOpenZgyStruct);
   
    explicit OsduFileCollectionOpenZgy(const OsduFileCollectionSegyStruct& FileCollectionSegyStruct);

    explicit OsduFileCollectionOpenZgy(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void populateStructFromFileCollectionSegy(const OsduFileCollectionSegyStruct& osduFileCollectionSegyStruct);

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduFileCollectionOpenZgyStruct fileCollectionOpenZgyStruct_;
    
};
}
