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

#include "../OsduMetaDataUtilities/OsduLineageAssertion.h"
#include "../OsduMetaDataUtilities/OsduLineageAssertionLiterals.h"

namespace segysdk::metadata::osdu
{
OsduLineageAssertion::OsduLineageAssertion(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), lineageAssertionStruct_(OsduLineageAssertionStruct(osduPartition)) {}

OsduLineageAssertion::OsduLineageAssertion(const OsduLineageAssertion& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      lineageAssertionStruct_(other.lineageAssertionStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduLineageAssertion::OsduLineageAssertion(OsduLineageAssertion&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      lineageAssertionStruct_(std::move(other.lineageAssertionStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduLineageAssertion&
OsduLineageAssertion::operator=(const OsduLineageAssertion& rhs)
{
    return *this = OsduLineageAssertion(rhs);
}

OsduLineageAssertion&
OsduLineageAssertion::operator=(OsduLineageAssertion&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setIsArrayElement(rhs.getIsArrayElement());
    setJson(rhs.getAsJson());

    lineageAssertionStruct_ = std::move(rhs.lineageAssertionStruct_);

    return *this;
}

OsduLineageAssertion::OsduLineageAssertion(OsduLineageAssertionStruct legalStruct)
    : OsduMetadataBase(legalStruct.getPartition()), lineageAssertionStruct_(std::move(legalStruct))
{
    populateJsonObject();
}

OsduLineageAssertion::OsduLineageAssertion(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), lineageAssertionStruct_(OsduLineageAssertionStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduLineageAssertion::isValidInput() const
{
    return lineageAssertionStruct_.isValid();
}

void
OsduLineageAssertion::initializeJson()
{
    removeMember(OsduLineageAssertionLiterals::ID);
    removeMember(OsduLineageAssertionLiterals::LineageRelationshipType);
}

void
OsduLineageAssertion::populateJsonObject()
{
    clearJsonObject();
    setOptionalParameter(OsduLineageAssertionLiterals::ID, lineageAssertionStruct_.id);
    setOptionalParameter(OsduLineageAssertionLiterals::LineageRelationshipType, lineageAssertionStruct_.lineageRelationshipType);
}

void
OsduLineageAssertion::initializeMetadataStruct()
{
    lineageAssertionStruct_ = OsduLineageAssertionStruct(getOsduPartition());
}

void
OsduLineageAssertion::populateMetadataStruct()
{
    initializeMetadataStruct();
    lineageAssertionStruct_.id = getOptionalParameter<std::string>(OsduLineageAssertionLiterals::ID);
    lineageAssertionStruct_.lineageRelationshipType = getOptionalParameter<std::string>(OsduLineageAssertionLiterals::LineageRelationshipType);
}

std::shared_ptr<OsduMetadataStructBase>
OsduLineageAssertion::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduLineageAssertionStruct>(lineageAssertionStruct_);
}

} // namespace segysdk::metadata::osdu
