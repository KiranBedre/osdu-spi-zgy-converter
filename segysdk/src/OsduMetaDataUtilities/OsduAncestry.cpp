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

#include "../OsduMetaDataUtilities/OsduAncestry.h"
#include "../OsduMetaDataUtilities/OsduAncestryLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAncestry::OsduAncestry(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), ancestryStruct_(OsduAncestryStruct(osduPartition))
{
    OsduMetadataBase::InitializeJsonArray(OsduAncestryLiterals::Parents);
}

OsduAncestry::OsduAncestry(OsduAncestryStruct ancestryStruct)
  : OsduMetadataBase(ancestryStruct.getPartition()), ancestryStruct_(std::move(ancestryStruct))
{
    OsduMetadataBase::InitializeJsonArray(OsduAncestryLiterals::Parents);
    populateJsonObject();
}

OsduAncestry::OsduAncestry(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), ancestryStruct_(OsduAncestryStruct(osduPartition))
{
    populateMetadataStruct();
}

OsduAncestry::OsduAncestry(const OsduAncestry& other)
  : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
    ancestryStruct_(other.ancestryStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAncestry::OsduAncestry(OsduAncestry&& other) noexcept
  : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
    ancestryStruct_(std::move(other.ancestryStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAncestry&
OsduAncestry::operator=(const OsduAncestry& rhs)
{
    return *this = OsduAncestry(rhs);
}

OsduAncestry&
OsduAncestry::operator=(OsduAncestry&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    ancestryStruct_ = std::move(rhs.ancestryStruct_);

    return *this;
}

void
OsduAncestry::addParent(const std::string& parent)
{
    if (ancestryStruct_.parents.has_value()) {
        ancestryStruct_.parents.value().emplace_back(parent);
        return;
    }

    ancestryStruct_.parents = std::vector{parent};
}

void
OsduAncestry::initializeJson()
{
    removeMember(OsduAncestryLiterals::Parents);
    InitializeJsonArray(OsduAncestryLiterals::Parents);
}

void
OsduAncestry::populateJsonObject()
{
    clearJsonObject();
    if (ancestryStruct_.parents.has_value()) {
        InitializeJsonArray(OsduAncestryLiterals::Parents);
        for (const auto& parent : ancestryStruct_.parents.value()) {
            appendArrayElement(OsduAncestryLiterals::Parents, parent);
        }
    }
}

void
OsduAncestry::initializeMetadataStruct()
{
    ancestryStruct_ = OsduAncestryStruct(getOsduPartition());
}

void
OsduAncestry::populateMetadataStruct()
{

    initializeMetadataStruct();

    if (isMember(OsduAncestryLiterals::Parents)) {
        std::vector<std::string> parents;
        for (const auto& p : getAllArrayElements<std::string>(OsduAncestryLiterals::Parents)) {
            parents.emplace_back(p);
        }
        ancestryStruct_.parents = parents;
    }
}

bool
OsduAncestry::isValidInput() const
{
    return ancestryStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduAncestry::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAncestryStruct>(ancestryStruct_);
}

} // namespace segysdk::metadata::osdu
