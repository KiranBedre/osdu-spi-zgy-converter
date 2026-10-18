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

#include "../OsduMetaDataUtilities/OsduAbstractWPCGroupType.h"

#include "../OsduMetaDataUtilities/OsduAbstractTechnicalAssurance.h"
#include "../OsduMetaDataUtilities/OsduAbstractWPCGroupTypeLiterals.h"
#include "../OsduMetaDataUtilities/OsduAbstractArtefact.h"
#include "../OsduMetaDataUtilities/OsduAbstractNameAlias.h"

namespace segysdk::metadata::osdu
{
OsduAbstractWPCGroupType::OsduAbstractWPCGroupType(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    abstractWPCGroupStruct_(OsduAbstractWPCGroupTypeStruct(osduPartition))
{
    /* Empty */
}

OsduAbstractWPCGroupType::OsduAbstractWPCGroupType(OsduAbstractWPCGroupTypeStruct commonResourceStruct)
    : OsduMetadataBase(commonResourceStruct.getPartition()), abstractWPCGroupStruct_(std::move(commonResourceStruct))
{
    populateJsonObject();
}


OsduAbstractWPCGroupType::OsduAbstractWPCGroupType(Json::Value json, const std::string& osduPartition)
    : OsduMetadataBase(std::move(json), osduPartition),
      abstractWPCGroupStruct_(OsduAbstractWPCGroupTypeStruct(osduPartition))
{
    populateMetadataStruct();
}

OsduAbstractWPCGroupType::OsduAbstractWPCGroupType(const OsduAbstractWPCGroupType& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      abstractWPCGroupStruct_(other.abstractWPCGroupStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAbstractWPCGroupType::OsduAbstractWPCGroupType(OsduAbstractWPCGroupType&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      abstractWPCGroupStruct_(std::move(other.abstractWPCGroupStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAbstractWPCGroupType&
OsduAbstractWPCGroupType::operator=(const OsduAbstractWPCGroupType& rhs)
{
    return *this = OsduAbstractWPCGroupType(rhs);
}

OsduAbstractWPCGroupType&
OsduAbstractWPCGroupType::operator=(OsduAbstractWPCGroupType&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    abstractWPCGroupStruct_ = std::move(rhs.abstractWPCGroupStruct_);

    return *this;
}

bool
OsduAbstractWPCGroupType::isValidInput() const
{
    return abstractWPCGroupStruct_.isValid();
}


void
OsduAbstractWPCGroupType::initializeJson()
{
    removeMember(OsduAbstractWPCGroupTypeLiterals::Datasets);
    removeMember(OsduAbstractWPCGroupTypeLiterals::DDMSDatasets);
    removeMember(OsduAbstractWPCGroupTypeLiterals::Artefacts);
    removeMember(OsduAbstractWPCGroupTypeLiterals::IsExtendedLoad);
    removeMember(OsduAbstractWPCGroupTypeLiterals::IsDiscoverable);
    removeMember(OsduAbstractWPCGroupTypeLiterals::TechnicalAssurances);
    removeMember(OsduAbstractWPCGroupTypeLiterals::NameAliases);
}

void
OsduAbstractWPCGroupType::populateJsonObject()
{
    // Do NOT clean Json object - it is in OsduDataElement space ...
    initializeJson();

    if (abstractWPCGroupStruct_.datasets.has_value()) {
        InitializeJsonArray(OsduAbstractWPCGroupTypeLiterals::Datasets);
        for (const auto& dataset : abstractWPCGroupStruct_.datasets.value()) {
            appendArrayElement(OsduAbstractWPCGroupTypeLiterals::Datasets, dataset);
        }
    }
    if (abstractWPCGroupStruct_.ddmsDatasets.has_value()) {
        InitializeJsonArray(OsduAbstractWPCGroupTypeLiterals::DDMSDatasets);
        for (const auto& dataset : abstractWPCGroupStruct_.ddmsDatasets.value()) {
            appendArrayElement(OsduAbstractWPCGroupTypeLiterals::DDMSDatasets, dataset);
        }
    }
    if (abstractWPCGroupStruct_.artefacts.has_value()) {
        InitializeJsonArray(OsduAbstractWPCGroupTypeLiterals::Artefacts);
        for (const auto& artefactStruct : abstractWPCGroupStruct_.artefacts.value()) {
            OsduAbstractArtefact artefact(artefactStruct);
            appendArrayElement(OsduAbstractWPCGroupTypeLiterals::Artefacts, artefact.getAsJson());
        }
    }

    setOptionalParameter(OsduAbstractWPCGroupTypeLiterals::IsDiscoverable, abstractWPCGroupStruct_.isDiscoverable);
    setOptionalParameter(OsduAbstractWPCGroupTypeLiterals::IsExtendedLoad, abstractWPCGroupStruct_.isExtendedLoad);

     if (abstractWPCGroupStruct_.technicalAssurances.has_value()) {
        InitializeJsonArray(OsduAbstractWPCGroupTypeLiterals::TechnicalAssurances);
        for (const auto& taStruct : abstractWPCGroupStruct_.technicalAssurances.value()) {
            OsduAbstractTechnicalAssurance ta(taStruct);
            appendArrayElement(OsduAbstractWPCGroupTypeLiterals::TechnicalAssurances, ta.getAsJson());
        }
    }

    if (abstractWPCGroupStruct_.nameAliases.has_value()) {
        InitializeJsonArray(OsduAbstractWPCGroupTypeLiterals::NameAliases);
        for (const auto& nameAliasStruct : abstractWPCGroupStruct_.nameAliases.value()) {
            OsduAbstractNameAlias nameAlias(nameAliasStruct);
            appendArrayElement(OsduAbstractWPCGroupTypeLiterals::NameAliases, nameAlias.getAsJson());
        }
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractWPCGroupType::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractWPCGroupTypeStruct>(abstractWPCGroupStruct_);
}

void
OsduAbstractWPCGroupType::initializeMetadataStruct()
{
    abstractWPCGroupStruct_ = OsduAbstractWPCGroupTypeStruct(getOsduPartition());
}

void
OsduAbstractWPCGroupType::populateMetadataStruct()
{
    initializeMetadataStruct();

    if (isMember(OsduAbstractWPCGroupTypeLiterals::Datasets)) {
        abstractWPCGroupStruct_.datasets = getAllArrayElements<std::string>(OsduAbstractWPCGroupTypeLiterals::Datasets);
    }
    if (isMember(OsduAbstractWPCGroupTypeLiterals::DDMSDatasets)) {
        abstractWPCGroupStruct_.ddmsDatasets = getAllArrayElements<std::string>(OsduAbstractWPCGroupTypeLiterals::DDMSDatasets);
    }
    if (isMember(OsduAbstractWPCGroupTypeLiterals::Artefacts)) {
        std::vector<OsduAbstractArtefactStruct> result;
        const auto jsonArray = getAllArrayElements<Json::Value>(OsduAbstractWPCGroupTypeLiterals::Artefacts);
        for (auto& json : jsonArray) {
            OsduAbstractArtefact artefact(json, getOsduPartition());
            const auto ptr = std::reinterpret_pointer_cast<OsduAbstractArtefactStruct>(artefact.getMetaDataStruct());
            if (ptr != nullptr) {
                result.emplace_back(*ptr);
            }
        }
        abstractWPCGroupStruct_.artefacts = result;
    }

    abstractWPCGroupStruct_.isDiscoverable = getOptionalParameter<bool>(OsduAbstractWPCGroupTypeLiterals::IsDiscoverable);
    abstractWPCGroupStruct_.isExtendedLoad = getOptionalParameter<bool>(OsduAbstractWPCGroupTypeLiterals::IsExtendedLoad);

    if (isMember(OsduAbstractWPCGroupTypeLiterals::TechnicalAssurances)) {
        std::vector<OsduAbstractTechnicalAssuranceStruct> result;
        const auto jsonArray = getAllArrayElements<Json::Value>(OsduAbstractWPCGroupTypeLiterals::TechnicalAssurances);
        for (auto& json : jsonArray) {
            OsduAbstractTechnicalAssurance ta(json, getOsduPartition());
            const auto ptr = std::reinterpret_pointer_cast<OsduAbstractTechnicalAssuranceStruct>(ta.getMetaDataStruct());
            if (ptr != nullptr) {
                result.emplace_back(*ptr);
            }
        }
        abstractWPCGroupStruct_.technicalAssurances = result;
    }

    if (isMember(OsduAbstractWPCGroupTypeLiterals::NameAliases)) {
        std::vector<OsduAbstractNameAliasStruct> result;
        const auto jsonArray = getAllArrayElements<Json::Value>(OsduAbstractWPCGroupTypeLiterals::NameAliases);
        for (auto& json : jsonArray) {
            OsduAbstractNameAlias nameAlias(json, getOsduPartition());
            const auto ptr = std::reinterpret_pointer_cast<OsduAbstractNameAliasStruct>(nameAlias.getMetaDataStruct());
            if (ptr != nullptr) {
                result.emplace_back(*ptr);
            }
        }
        abstractWPCGroupStruct_.nameAliases = result;
    }
}

} // namespace segysdk::metadata::osdu
