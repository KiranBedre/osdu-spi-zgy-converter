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

#include "../OsduMetaDataUtilities/OsduTagArrayStruct.h"

#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>


namespace segysdk::metadata::osdu
{
/*

 "tags": {
	"NameOfKey1" : "String value",
	"NameOfKey2" : "String value"
	...
}

  
*/

class SEGYSDK_DLL_PUBLIC OsduTagArray final : public OsduMetadataBase
{
public:
    OsduTagArray(const std::string& osduPartition);
    OsduTagArray(const OsduTagArray& other);
    OsduTagArray(OsduTagArray&& other) noexcept;
    OsduTagArray& operator=(const OsduTagArray& rhs);
    OsduTagArray& operator=(OsduTagArray&& rhs) noexcept;
    ~OsduTagArray() override = default;


    OsduTagArray(OsduTagArrayStruct tagArray);
    OsduTagArray(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduTagArrayStruct tagArrayStruct_;

};

}
