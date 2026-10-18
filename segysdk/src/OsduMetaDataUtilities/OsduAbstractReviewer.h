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

#include "../OsduMetaDataUtilities/OsduAbstractReviewerStruct.h"

#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>


namespace segysdk::metadata::osdu
{

class SEGYSDK_DLL_PUBLIC OsduAbstractReviewer final : public OsduMetadataBase
{
public:
    OsduAbstractReviewer(const std::string& osduPartition);
    OsduAbstractReviewer(const OsduAbstractReviewer& other);
    OsduAbstractReviewer(OsduAbstractReviewer&& other) noexcept;
    OsduAbstractReviewer& operator=(const OsduAbstractReviewer& rhs);
    OsduAbstractReviewer& operator=(OsduAbstractReviewer&& rhs) noexcept;
    ~OsduAbstractReviewer() override = default;

    OsduAbstractReviewer(OsduAbstractReviewerStruct metaElementStruct);
    OsduAbstractReviewer(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    void populateMetadataStruct() override;

private:
    OsduAbstractReviewerStruct abstractReviewerStruct_;

};

}
