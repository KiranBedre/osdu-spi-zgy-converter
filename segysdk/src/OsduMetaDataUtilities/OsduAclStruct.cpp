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

#include "../OsduMetaDataUtilities/OsduAclStruct.h"


namespace segysdk::metadata::osdu
{
OsduAclStruct::OsduAclStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduAclStruct::OsduAclStruct(const OsduAclStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    owners(other.owners),
    viewers(other.viewers)
{
    /* Empty */
}

OsduAclStruct::OsduAclStruct(OsduAclStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    owners(std::move(other.owners)),
    viewers(std::move(other.viewers))
{
    /* Empty */
}

OsduAclStruct&
OsduAclStruct::operator=(const OsduAclStruct& rhs)
{
    return *this = OsduAclStruct(rhs);
}

OsduAclStruct&
OsduAclStruct::operator=(OsduAclStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    owners = std::move(rhs.owners);
    viewers = std::move(rhs.viewers);

    return *this;
}

bool
OsduAclStruct::isValid() const
{
    return !owners.empty() and isValidStringArrayRegex(owners, emailStringRegex_)
           and !viewers.empty() and isValidStringArrayRegex(viewers, emailStringRegex_);
}

std::vector<std::string>
OsduAclStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }

    std::vector<std::string> results;
    results.emplace_back("OsduAclStruct::isValid() failed.");

    if (owners.empty()) {
        results.emplace_back("OsduAclStruct::owners cannot be empty.");
    }
    if (viewers.empty()) {
        results.emplace_back("OsduAclStruct::viewers cannot be empty.");
    }

    performStringArrayRegexValidityScan(results, owners, "OsduAclStruct::owners", emailStringRegex_);
    performStringArrayRegexValidityScan(results, viewers, "OsduAclStruct::viewers", emailStringRegex_);

    return results;
}

} // namespace segysdk::metadata::osdu
