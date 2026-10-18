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

#include <utility>

#include "../OsduMetaDataUtilities/OsduWorkProduct.h"

#include "../OsduMetaDataUtilities/OsduCommonMetadata.h"
#include "../OsduMetaDataUtilities/OsduWorkProductData.h"

#include "../OsduMetaDataUtilities/OsduWorkProductLiterals.h"

namespace segysdk::metadata::osdu
{
OsduWorkProduct::OsduWorkProduct(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    workProductStruct_(osduPartition)
{
    /* Empty */
}

OsduWorkProduct::OsduWorkProduct(OsduWorkProductStruct workProductStruct)
  : OsduMetadataBase(workProductStruct.getPartition()), workProductStruct_(std::move(workProductStruct))
{
    populateJsonObject();
}

OsduWorkProduct::OsduWorkProduct(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), workProductStruct_(OsduWorkProductStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduWorkProduct::isValidInput() const
{
    return workProductStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduWorkProduct::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduWorkProductStruct>(workProductStruct_);
}

void
OsduWorkProduct::initializeJson()
{
    clearJsonObject();
}

void
OsduWorkProduct::populateJsonObject()
{
    initializeJson();

    mergeInJson(OsduCommonMetadata(workProductStruct_.commonMetadataStruct).getAsJson());
    if (workProductStruct_.dataStruct.has_value()) {
        Json::Value additionJson{Json::objectValue};
        additionJson[OsduWorkProductLiterals::Data] = OsduWorkProductData(workProductStruct_.dataStruct.value()).getAsJson();
        mergeInJsonRecursively(additionJson);
    }
}

void
OsduWorkProduct::initializeMetadataStruct()
{
    workProductStruct_ = OsduWorkProductStruct(getOsduPartition());
}

void
OsduWorkProduct::populateMetadataStruct()
{
    initializeMetadataStruct();

    workProductStruct_.commonMetadataStruct = getStructParameter<OsduCommonMetadata, OsduCommonMetadataStruct>();
    workProductStruct_.dataStruct = getOptionalStructParameter<OsduWorkProductData, OsduWorkProductDataStruct>(OsduWorkProductLiterals::Data);
}
} // namespace segysdk::metadata::osdu
