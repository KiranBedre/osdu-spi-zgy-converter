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

#include "../OsduMetaDataUtilities/OsduLegalStruct.h"

namespace segysdk::metadata::osdu
{
OsduLegalStruct::OsduLegalStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduLegalStruct::OsduLegalStruct(const OsduLegalStruct& other)
    : OsduMetadataStructBase(other.getPartition()),
      legalTags(other.legalTags),
      otherRelevantDataCountries(other.otherRelevantDataCountries),
      status(other.status)
{
    /* Empty */
}

OsduLegalStruct::OsduLegalStruct(OsduLegalStruct&& other) noexcept
    : OsduMetadataStructBase(other.getPartition()),
      legalTags(std::move(other.legalTags)),
      otherRelevantDataCountries(std::move(other.otherRelevantDataCountries)),
      status(std::move(other.status))
{
    /* Empty */
}

OsduLegalStruct&
OsduLegalStruct::operator=(const OsduLegalStruct& rhs)
{
    return *this = OsduLegalStruct(rhs);
}

OsduLegalStruct&
OsduLegalStruct::operator=(OsduLegalStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    legalTags = std::move(rhs.legalTags);
    otherRelevantDataCountries = std::move(rhs.otherRelevantDataCountries);
    status = std::move(rhs.status);

    return *this;
}

bool
OsduLegalStruct::isValid() const
{
    return !otherRelevantDataCountries.empty() and isValidStringArrayRegex(otherRelevantDataCountries, countryCodeStringRegex_)
           and isValidStringRegex(status, complianceStringRegex_);
}

std::vector<std::string>
OsduLegalStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }
    std::vector<std::string> results;

    results.emplace_back("OsduLegalStruct::isValid() failed.");

    if (otherRelevantDataCountries.empty()) {
        results.emplace_back("OsduLegalStruct::otherRelevantDataCountries is empty.");
    }

    performStringArrayRegexValidityScan(results, otherRelevantDataCountries, "OsduLegalStruct::otherRelevantDataCountries", countryCodeStringRegex_);
    performStringRegexValidityScan(results, status, "OsduLegalStruct::status", complianceStringRegex_);

    return results;
}

} // namespace segysdk::metadata::osdu
