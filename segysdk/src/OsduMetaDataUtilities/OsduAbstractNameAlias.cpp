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

#include <utility>

#include "../OsduMetaDataUtilities/OsduAbstractNameAlias.h"

#include "../OsduMetaDataUtilities/OsduAbstractNameAliasLiterals.h"

namespace segysdk::metadata::osdu
{

OsduAbstractNameAlias::OsduAbstractNameAlias(const std::string& osduPartition)
    : OsduMetadataBase(osduPartition), nameAliasesStruct_(OsduAbstractNameAliasStruct(osduPartition))
{
    /* Empty */
}

OsduAbstractNameAlias::OsduAbstractNameAlias(const OsduAbstractNameAlias& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      nameAliasesStruct_(other.nameAliasesStruct_)
{
    setOsduPartition(other.getOsduPartition());
    setJson(other.getJson());
}

OsduAbstractNameAlias::OsduAbstractNameAlias(OsduAbstractNameAlias&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      nameAliasesStruct_(std::move(other.nameAliasesStruct_))
{
    setOsduPartition(other.getOsduPartition());
    setJson(other.getJson());
}

OsduAbstractNameAlias&
OsduAbstractNameAlias::operator=(const OsduAbstractNameAlias& rhs)
{
    return *this = OsduAbstractNameAlias(rhs);
}

OsduAbstractNameAlias&
OsduAbstractNameAlias::operator=(OsduAbstractNameAlias&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setIsArrayElement(rhs.getIsArrayElement());
    setJson(rhs.getAsJson());

    nameAliasesStruct_ = std::move(rhs.nameAliasesStruct_);

    return *this;
}

OsduAbstractNameAlias::OsduAbstractNameAlias(OsduAbstractNameAliasStruct nameAliasStruct)
  : OsduMetadataBase(nameAliasStruct.getPartition()), nameAliasesStruct_(std::move(nameAliasStruct))
{
    populateJsonObject();
}

OsduAbstractNameAlias::OsduAbstractNameAlias(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), nameAliasesStruct_(OsduAbstractNameAliasStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractNameAlias::isValidInput() const
{
    return nameAliasesStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractNameAlias::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractNameAliasStruct>(nameAliasesStruct_);
}

void
OsduAbstractNameAlias::initializeJson()
{
    clearJsonObject();
}

void
OsduAbstractNameAlias::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduAbstractNameAliasesLiterals::AliasName, nameAliasesStruct_.aliasName);
    setOptionalParameter(OsduAbstractNameAliasesLiterals::AliasNameTypeID, nameAliasesStruct_.aliasNameTypeID);
    setOptionalParameter(OsduAbstractNameAliasesLiterals::DefinitionOrganisationID, nameAliasesStruct_.definitionOrganisationID);
    setOptionalParameter(OsduAbstractNameAliasesLiterals::EffectiveDateTime, nameAliasesStruct_.effectiveDateTime);
    setOptionalParameter(OsduAbstractNameAliasesLiterals::TerminationDateTime, nameAliasesStruct_.terminationDateTime);
}

void
OsduAbstractNameAlias::initializeMetadataStruct() {
    nameAliasesStruct_ = OsduAbstractNameAliasStruct(getOsduPartition());
}

void
OsduAbstractNameAlias::populateMetadataStruct()
{
    nameAliasesStruct_.aliasName = getOptionalParameter<std::string> (OsduAbstractNameAliasesLiterals::AliasName);
    nameAliasesStruct_.aliasNameTypeID = getOptionalParameter<std::string>(OsduAbstractNameAliasesLiterals::AliasNameTypeID);
    nameAliasesStruct_.definitionOrganisationID = getOptionalParameter<std::string>(OsduAbstractNameAliasesLiterals::DefinitionOrganisationID);
    nameAliasesStruct_.effectiveDateTime = getOptionalParameter<std::string>(OsduAbstractNameAliasesLiterals::EffectiveDateTime);
    nameAliasesStruct_.terminationDateTime = getOptionalParameter<std::string>(OsduAbstractNameAliasesLiterals::TerminationDateTime);
}

} // namespace segysdk::metadata::osdu
