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

#include "../OsduMetaDataUtilities/OsduAbstractArtefact.h"

#include "../OsduMetaDataUtilities/OsduAbstractArtefactLiterals.h"

namespace segysdk::metadata::osdu
{

OsduAbstractArtefact::OsduAbstractArtefact(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), artefactsStruct_(OsduAbstractArtefactStruct(osduPartition))
{
    /* Empty */
}

OsduAbstractArtefact::OsduAbstractArtefact(const OsduAbstractArtefact& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      artefactsStruct_(other.artefactsStruct_)
{
    setOsduPartition(other.getOsduPartition());
    setJson(other.getJson());
}

OsduAbstractArtefact::OsduAbstractArtefact(OsduAbstractArtefact&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      artefactsStruct_(std::move(other.artefactsStruct_))
{
    setOsduPartition(other.getOsduPartition());
    setJson(other.getJson());
}

OsduAbstractArtefact&
OsduAbstractArtefact::operator=(const OsduAbstractArtefact& rhs)
{
    return *this = OsduAbstractArtefact(rhs);
}

OsduAbstractArtefact&
OsduAbstractArtefact::operator=(OsduAbstractArtefact&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setIsArrayElement(rhs.getIsArrayElement());
    setJson(rhs.getAsJson());

    artefactsStruct_ = std::move(rhs.artefactsStruct_);

    return *this;
}

OsduAbstractArtefact::OsduAbstractArtefact(OsduAbstractArtefactStruct artefactStruct)
  : OsduMetadataBase(artefactStruct.getPartition()), artefactsStruct_(std::move(artefactStruct))
{
    populateJsonObject();
}

OsduAbstractArtefact::OsduAbstractArtefact(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), artefactsStruct_(OsduAbstractArtefactStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractArtefact::isValidInput() const
{
    return artefactsStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractArtefact::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractArtefactStruct>(artefactsStruct_);
}

void
OsduAbstractArtefact::initializeJson()
{
    clearJsonObject();
}

void
OsduAbstractArtefact::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduAbstractArtefactLiterals::RoleID, artefactsStruct_.roleID);
    setOptionalParameter(OsduAbstractArtefactLiterals::ResourceKind, artefactsStruct_.resourceKind);
    setOptionalParameter(OsduAbstractArtefactLiterals::ResourceID, artefactsStruct_.resourceID);
}

void
OsduAbstractArtefact::initializeMetadataStruct() {
    artefactsStruct_ = OsduAbstractArtefactStruct(getOsduPartition());
}

void
OsduAbstractArtefact::populateMetadataStruct()
{
    artefactsStruct_.roleID = getOptionalParameter<std::string>(OsduAbstractArtefactLiterals::RoleID);
    artefactsStruct_.resourceID = getOptionalParameter<std::string>(OsduAbstractArtefactLiterals::ResourceID);
    artefactsStruct_.resourceKind = getOptionalParameter<std::string>(OsduAbstractArtefactLiterals::ResourceKind);
}

} // namespace segysdk::metadata::osdu
