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

#include "../OsduMetaDataUtilities/OsduAbstractGeoContextStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>
#include <string>

namespace segysdk::metadata::osdu
{

class SEGYSDK_DLL_PUBLIC OsduAbstractGeoContext final : public OsduMetadataBase
{
public:
    OsduAbstractGeoContext(const std::string& osduPartition);
    explicit OsduAbstractGeoContext(OsduAbstractGeoContextStruct commonResourceStruct);
    explicit OsduAbstractGeoContext(Json::Value json, const std::string& osduPartition);
    OsduAbstractGeoContext(const OsduAbstractGeoContext& other);
    OsduAbstractGeoContext(OsduAbstractGeoContext&& other) noexcept;
    OsduAbstractGeoContext& operator=(const OsduAbstractGeoContext& rhs);
    OsduAbstractGeoContext& operator=(OsduAbstractGeoContext&& rhs) noexcept;
    ~OsduAbstractGeoContext() override = default;

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduAbstractGeoContextStruct geoContextStruct_;

};

}
