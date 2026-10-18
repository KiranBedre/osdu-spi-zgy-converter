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

#include "../OsduMetaDataUtilities/OsduGenericFileCollectionStruct.h"


namespace segysdk::metadata::osdu
{
OsduGenericFileCollectionStruct::OsduGenericFileCollectionStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition)), 
    acl(OsduAclStruct(getPartition())), 
    legal(OsduLegalStruct(getPartition()))
{
    /* Empty */
}

OsduGenericFileCollectionStruct::OsduGenericFileCollectionStruct(const OsduGenericFileCollectionStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    acl(other.acl),
    legal(other.legal),
    ancestry(other.ancestry),
    data(other.data),
    meta(other.meta),
    tags((other.tags))
{
    /* Empty */
}

OsduGenericFileCollectionStruct::OsduGenericFileCollectionStruct(OsduGenericFileCollectionStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    acl(std::move(other.acl)),
    legal(std::move(other.legal)),
    ancestry(std::move(other.ancestry)),
    data(std::move(other.data)),
    meta(std::move(other.meta)),
    tags(std::move(other.tags))
{
    /* Empty */
}

OsduGenericFileCollectionStruct&
OsduGenericFileCollectionStruct::operator=(const OsduGenericFileCollectionStruct& rhs)
{
    return *this = OsduGenericFileCollectionStruct(rhs);
}

OsduGenericFileCollectionStruct&
OsduGenericFileCollectionStruct::operator=(OsduGenericFileCollectionStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    acl = std::move(rhs.acl);
    legal = std::move(rhs.legal);
    ancestry = std::move(rhs.ancestry);
    data = std::move(rhs.data);
    meta = std::move(rhs.meta);
    tags = std::move(rhs.tags);

    return *this;
}

bool
OsduGenericFileCollectionStruct::isValid() const
{
    return acl.isValid() and legal.isValid() and allNodesValid();
}

bool
OsduGenericFileCollectionStruct::allNodesValid() const
{
    if (ancestry.has_value()) {
        if (!ancestry.value().isValid()) {
            return false;
        }
    }
    if (data.has_value()) {
        if (!data.value().isValid()) {
            return false;
        }
    }
    if (meta.has_value()) {
        if (!meta.value().isValid()) {
            return false;
        }
    }
    if (tags.has_value()) {
        if (!tags.value().isValid()) {
            return false;
        }
    }
    return true;
}

std::vector<std::string>
OsduGenericFileCollectionStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }

    std::vector<std::string> results{"OsduGenericFileCollectionStruct::isValid() failed."};

    performStructValidityScan(results, acl);
    performStructValidityScan(results, legal);

    performStructValidityScan(results, ancestry);
    performStructValidityScan(results, data);
    performStructValidityScan(results, meta);
    performStructValidityScan(results, tags);

    return results;
}

void
OsduGenericFileCollectionStruct::propagatePartition(const std::string& partition)
{
    acl.setPartition(partition);
    legal.setPartition(partition);
    if (ancestry.has_value()) {
        ancestry.value().setPartition(partition);
    }
    if (data.has_value()) {
        data.value().setPartition(partition);
    }
    if (meta.has_value()) {
        meta.value().setPartition(partition);
    }
    if (tags.has_value())
        {
        tags.value().setPartition(partition);
    }
}



} // namespace segysdk::metadata::osdu
