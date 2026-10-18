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

#include "../OsduMetaDataUtilities/OsduTagArrayStruct.h"

#include <algorithm>


namespace segysdk::metadata::osdu
{

OsduTagArrayStruct::OsduTagArrayStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduTagArrayStruct::OsduTagArrayStruct(const OsduTagArrayStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    tagStructs_(other.tagStructs_)
{
    /* Empty */
}

OsduTagArrayStruct::OsduTagArrayStruct(OsduTagArrayStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    tagStructs_(std::move(other.tagStructs_))
{
    /* Empty */
}

OsduTagArrayStruct&
OsduTagArrayStruct::operator=(const OsduTagArrayStruct& rhs)
{
    return *this = OsduTagArrayStruct(rhs);
}

OsduTagArrayStruct&
OsduTagArrayStruct::operator=(OsduTagArrayStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    tagStructs_ = std::move(rhs.tagStructs_);

    return *this;
}

OsduTagArrayStruct::OsduTagArrayStruct(const std::vector<OsduTagElementStruct>& tagStructs)
    : OsduMetadataStructBase(""), tagStructs_(tagStructs)
{
    if (tagStructs_.has_value() and !tagStructs_.value().empty()) {
        setPartition(tagStructs_.value()[0].getPartition());
    }
}


bool
OsduTagArrayStruct::allStructsOK() const
{
    if (tagStructs_.has_value()) {
        return std::all_of(tagStructs_.value().cbegin(), tagStructs_.value().cend(), [](const OsduTagElementStruct& x) { return x.isValid(); });
    }
    return true;
}

bool
OsduTagArrayStruct::isValid() const
{
    return !tagStructs_.has_value() or allStructsOK();
}

std::vector<std::string>
OsduTagArrayStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduTagArrayStruct::isValid() failed.");
    }
    if (tagStructs_.has_value()) {
        for (const auto& t : tagStructs_.value()) {
            appendStringVector(results, t.performAttributeValidityScan());
        }
    }
    return results;
}

void
OsduTagArrayStruct::propagatePartition(const std::string& partition)
{
    if (tagStructs_.has_value()) {
        for (auto& tag : tagStructs_.value()) {
            tag.setPartition(partition);
        }
    }
}

} // namespace segysdk::metadata::osdu
