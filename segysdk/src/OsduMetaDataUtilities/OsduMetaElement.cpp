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

#include "../OsduMetaDataUtilities/OsduMetaElement.h"

#include "../OsduMetaDataUtilities/OsduMetaLiterals.h"

namespace segysdk::metadata::osdu
{

OsduMetaElement::OsduMetaElement(std::string osduPartition)
    : OsduMetadataBase(std::move(osduPartition)), metaStruct_(OsduMetaElementStruct(getOsduPartition()))
{
    /* Empty */
}

OsduMetaElement::OsduMetaElement(const OsduMetaElement& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      metaStruct_(other.metaStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduMetaElement::OsduMetaElement(OsduMetaElement&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      metaStruct_(std::move(other.metaStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduMetaElement&
OsduMetaElement::operator=(const OsduMetaElement& rhs)
{
    return *this = OsduMetaElement(rhs);
}

OsduMetaElement&
OsduMetaElement::operator=(OsduMetaElement&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    metaStruct_ = std::move(rhs.metaStruct_);

    return *this;
}

OsduMetaElement::OsduMetaElement(OsduMetaElementStruct metaElementStruct)
    : OsduMetadataBase(metaElementStruct.getPartition()), metaStruct_(std::move(metaElementStruct))
{
    populateJsonObject();
}

OsduMetaElement::OsduMetaElement(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), metaStruct_(OsduMetaElementStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduMetaElement::isValidInput() const
{
    return metaStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduMetaElement::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduMetaElementStruct>(metaStruct_);
}

void
OsduMetaElement::initializeJson()
{
    clearJsonObject();
}

void
OsduMetaElement::populateJsonObject()
{
    initializeJson();

    setParameter(OsduMetaLiterals::Kind, metaStruct_.kind);
    setParameter(OsduMetaLiterals::PersistableReference, metaStruct_.persistableReference);

    setOptionalParameter(OsduMetaLiterals::Name, metaStruct_.name);
    setOptionalParameter(OsduMetaLiterals::UnitOfMeasureID, metaStruct_.unitOfMeasureID);

    if (metaStruct_.propertyNames.has_value()) {
        for (const auto& prop : metaStruct_.propertyNames.value()) {
            appendArrayElement(OsduMetaLiterals::PropertyNames, prop);
        }
    }
}

void
OsduMetaElement::initializeMetadataStruct() {
    metaStruct_ = OsduMetaElementStruct(getOsduPartition());
}

void
OsduMetaElement::populateMetadataStruct()
{
    metaStruct_.name = getOptionalParameter<std::string>(OsduMetaLiterals::Name);

    metaStruct_.kind = getParameter<std::string>(OsduMetaLiterals::Kind);
    metaStruct_.persistableReference = getParameter<std::string>(OsduMetaLiterals::PersistableReference);

    if (isMember(OsduMetaLiterals::PropertyNames)) {
        if (!isEmptyArray(OsduMetaLiterals::PropertyNames)) {
            std::vector<std::string> property_list;

            for (const auto& property_name : getAllArrayElements<std::string>(OsduMetaLiterals::PropertyNames)) {
                property_list.emplace_back(property_name);
            }
            metaStruct_.propertyNames = property_list;
        }
    }
    metaStruct_.unitOfMeasureID = getOptionalParameter<std::string>(OsduMetaLiterals::UnitOfMeasureID);
}

} // namespace segysdk::metadata::osdu
