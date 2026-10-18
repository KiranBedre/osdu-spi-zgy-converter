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

#include "../OsduMetaDataUtilities/OsduFundsAuthorization.h"

#include "../OsduMetaDataUtilities/OsduFundsAuthorizationLiterals.h"

namespace segysdk::metadata::osdu
{
OsduFundsAuthorization::OsduFundsAuthorization(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    fundsAuthorizationStruct_(osduPartition)
{
    /* Empty */
}
OsduFundsAuthorization::OsduFundsAuthorization(OsduFundsAuthorizationStruct fundsAuthorizationStruct)
  : OsduMetadataBase(fundsAuthorizationStruct.getPartition()), fundsAuthorizationStruct_(std::move(fundsAuthorizationStruct))
{
    populateJsonObject();
}

OsduFundsAuthorization::OsduFundsAuthorization(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), fundsAuthorizationStruct_(OsduFundsAuthorizationStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduFundsAuthorization::isValidInput() const
{
    return fundsAuthorizationStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduFundsAuthorization::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduFundsAuthorizationStruct>(fundsAuthorizationStruct_);
}

void
OsduFundsAuthorization::initializeJson()
{
    clearJsonObject();
}

void
OsduFundsAuthorization::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduFundsAuthorizationLiterals::AuthorizationID, fundsAuthorizationStruct_.authorizationId);
    setOptionalParameter(OsduFundsAuthorizationLiterals::EffectiveDateTime, fundsAuthorizationStruct_.effectiveDateTime);
    setOptionalParameter(OsduFundsAuthorizationLiterals::FundsAmount, fundsAuthorizationStruct_.fundsAmount);
    setOptionalParameter(OsduFundsAuthorizationLiterals::CurrencyID, fundsAuthorizationStruct_.currencyID);
}

void
OsduFundsAuthorization::initializeMetadataStruct()
{
    fundsAuthorizationStruct_ = OsduFundsAuthorizationStruct(getOsduPartition());
}

void
OsduFundsAuthorization::populateMetadataStruct()
{
    initializeMetadataStruct();

    fundsAuthorizationStruct_.authorizationId = getOptionalParameter<std::string>(OsduFundsAuthorizationLiterals::AuthorizationID);
    fundsAuthorizationStruct_.effectiveDateTime = getOptionalParameter<std::string>(OsduFundsAuthorizationLiterals::EffectiveDateTime);
    fundsAuthorizationStruct_.fundsAmount = getOptionalParameter<double>(OsduFundsAuthorizationLiterals::FundsAmount);
    fundsAuthorizationStruct_.currencyID = getOptionalParameter<std::string>(OsduFundsAuthorizationLiterals::CurrencyID);
}
} // namespace segysdk::metadata::osdu
