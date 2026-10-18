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

#include "../OsduMetaDataUtilities/OsduLegal.h"
#include "../OsduMetaDataUtilities/OsduLegalLiterals.h"

namespace segysdk::metadata::osdu
{
OsduLegal::OsduLegal(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), legalStruct_(OsduLegalStruct(osduPartition)) {}

OsduLegal::OsduLegal(const OsduLegal& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      legalStruct_(other.legalStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduLegal::OsduLegal(OsduLegal&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      legalStruct_(std::move(other.legalStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduLegal&
OsduLegal::operator=(const OsduLegal& rhs)
{
    return *this = OsduLegal(rhs);
}

OsduLegal&
OsduLegal::operator=(OsduLegal&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    legalStruct_ = std::move(rhs.legalStruct_);

    return *this;
}

OsduLegal::OsduLegal(OsduLegalStruct legalStruct)
    : OsduMetadataBase(legalStruct.getPartition()), legalStruct_(std::move(legalStruct))
{
    populateJsonObject();
}

OsduLegal::OsduLegal(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), legalStruct_(OsduLegalStruct(osduPartition))
{
    populateMetadataStruct();
}

void
OsduLegal::addTag(const std::string& tag)
{
    appendArrayElement(OsduLegalLiterals::Legaltags, tag);
}

void
OsduLegal::addCountry(const std::string& country)
{
    appendArrayElement(OsduLegalLiterals::OtherRelevantDataCountries, country);
}

bool
OsduLegal::isValidInput() const
{
    return legalStruct_.isValid();
}

void
OsduLegal::initializeJson()
{
    removeMember(OsduLegalLiterals::Legaltags);
    removeMember(OsduLegalLiterals::OtherRelevantDataCountries);
    removeMember(OsduLegalLiterals::Status);
}

void
OsduLegal::populateJsonObject()
{
    clearJsonObject();
    for (const auto& tag : legalStruct_.legalTags) {
        appendArrayElement(OsduLegalLiterals::Legaltags, tag);
    }

    for (const auto& country : legalStruct_.otherRelevantDataCountries) {
        appendArrayElement(OsduLegalLiterals::OtherRelevantDataCountries, country);
    }

    if (legalStruct_.status.has_value()) {
        setParameter(OsduLegalLiterals::Status, legalStruct_.status.value());
    }
}

void
OsduLegal::initializeMetadataStruct()
{
    legalStruct_ = OsduLegalStruct(getOsduPartition());
}

void
OsduLegal::populateMetadataStruct()
{
    initializeMetadataStruct();

    if (isMember(OsduLegalLiterals::Legaltags)) {
        for (const auto& tag : getAllArrayElements<std::string>(OsduLegalLiterals::Legaltags)) {
            legalStruct_.legalTags.emplace_back(tag);
        }
    }
    if (isMember(OsduLegalLiterals::OtherRelevantDataCountries)) {
        for (const auto& country : getAllArrayElements<std::string>(OsduLegalLiterals::OtherRelevantDataCountries)) {
            legalStruct_.otherRelevantDataCountries.emplace_back(country);
        }
    }

    legalStruct_.status = getOptionalParameter<std::string>(OsduLegalLiterals::Status);
}

std::shared_ptr<OsduMetadataStructBase>
OsduLegal::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduLegalStruct>(legalStruct_);
}

} // namespace segysdk::metadata::osdu
