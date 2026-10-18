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

#include "../OsduMetaDataUtilities/OsduAcl.h"

#include "../OsduMetaDataUtilities/OsduAclStruct.h"
#include "../OsduMetaDataUtilities/OsduAclLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAcl::OsduAcl(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), aclStruct_(OsduAclStruct(osduPartition)) {}

OsduAcl::OsduAcl(const OsduAcl& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      aclStruct_(other.aclStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAcl::OsduAcl(OsduAcl&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      aclStruct_(std::move(other.aclStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAcl&
OsduAcl::operator=(const OsduAcl& rhs)
{
    return *this = OsduAcl(rhs);
}

OsduAcl&
OsduAcl::operator=(OsduAcl&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    aclStruct_ = std::move(rhs.aclStruct_);

    return *this;
}

OsduAcl::OsduAcl(OsduAclStruct aclStruct)
    : OsduMetadataBase(aclStruct.getPartition()), aclStruct_(std::move(aclStruct))
{
    populateJsonObject();
}

OsduAcl::OsduAcl(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), aclStruct_(OsduAclStruct(osduPartition))
{
    populateMetadataStruct();
}

void
OsduAcl::addOwner(const std::string& owner)
{
    aclStruct_.owners.emplace_back(owner);
}

void
OsduAcl::addViewer(const std::string& viewer)
{
    aclStruct_.viewers.emplace_back(viewer);
}

bool
OsduAcl::isValidInput() const
{
    return aclStruct_.isValid();
}

void
OsduAcl::initializeJson()
{
    removeMember(OsduAclLiterals::Owners);
    removeMember(OsduAclLiterals::Viewers);
}

void
OsduAcl::populateJsonObject()
{
    clearJsonObject();
    InitializeJsonArray(OsduAclLiterals::Owners);
    for (const auto& owner : aclStruct_.owners) {
        appendArrayElement(OsduAclLiterals::Owners, owner);
    }

    InitializeJsonArray(OsduAclLiterals::Viewers);
    for (const auto& viewer : aclStruct_.viewers) {
        appendArrayElement(OsduAclLiterals::Viewers, viewer);
    }
}

void
OsduAcl::initializeMetadataStruct()
{
    aclStruct_ = OsduAclStruct(getOsduPartition());
}

void
OsduAcl::populateMetadataStruct()
{
    initializeMetadataStruct();

    if (isMember(OsduAclLiterals::Owners)) {
        for (const auto& owner : getAllArrayElements<std::string>(OsduAclLiterals::Owners)) {
            aclStruct_.owners.emplace_back(owner);
        }
    }
    if (isMember(OsduAclLiterals::Viewers)) {
        for (const auto& viewer : getAllArrayElements<std::string>(OsduAclLiterals::Viewers)) {
            aclStruct_.viewers.emplace_back(viewer);
        }
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduAcl::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAclStruct>(aclStruct_);
}

} // namespace segysdk::metadata::osdu
