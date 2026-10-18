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

#include "../OsduMetaDataUtilities/OsduAbstractCommonResourcesStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>
#include <string>

namespace segysdk::metadata::osdu
{

class SEGYSDK_DLL_PUBLIC OsduAbstractCommonResources final : public OsduMetadataBase
{
public:
    OsduAbstractCommonResources(const std::string& osduPartition);
    explicit OsduAbstractCommonResources(OsduAbstractCommonResourcesStruct commonResourceStruct);
    OsduAbstractCommonResources(const OsduAbstractCommonResources& other);
    OsduAbstractCommonResources(OsduAbstractCommonResources&& other) noexcept;
    OsduAbstractCommonResources& operator=(const OsduAbstractCommonResources& rhs);
    OsduAbstractCommonResources& operator=(OsduAbstractCommonResources&& rhs) noexcept;
    ~OsduAbstractCommonResources() override = default;
    explicit OsduAbstractCommonResources(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduAbstractCommonResourcesStruct commonResourcesStruct_;

};

}
