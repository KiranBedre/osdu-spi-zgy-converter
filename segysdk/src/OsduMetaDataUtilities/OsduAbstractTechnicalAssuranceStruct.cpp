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

#include "../OsduMetaDataUtilities/OsduAbstractTechnicalAssuranceStruct.h"

#include <algorithm>

namespace segysdk::metadata::osdu
{

OsduAbstractTechnicalAssuranceStruct::OsduAbstractTechnicalAssuranceStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduAbstractTechnicalAssuranceStruct::OsduAbstractTechnicalAssuranceStruct(const OsduAbstractTechnicalAssuranceStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    technicalAssuranceTypeID(other.technicalAssuranceTypeID),
    reviewers(other.reviewers),
    acceptableUsage(other.acceptableUsage),
    unacceptableUsage(other.unacceptableUsage),
    effectiveDate(other.effectiveDate),
    comment(other.comment)
{
    /* Empty */
}

OsduAbstractTechnicalAssuranceStruct::OsduAbstractTechnicalAssuranceStruct(OsduAbstractTechnicalAssuranceStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    technicalAssuranceTypeID(std::move(other.technicalAssuranceTypeID)),
    reviewers(std::move(other.reviewers)),
    acceptableUsage(std::move(other.acceptableUsage)),
    unacceptableUsage(std::move(other.unacceptableUsage)),
    effectiveDate(std::move(other.effectiveDate)),
    comment(std::move(other.comment))
{
    /* Empty */
}

OsduAbstractTechnicalAssuranceStruct&
OsduAbstractTechnicalAssuranceStruct::operator=(const OsduAbstractTechnicalAssuranceStruct& rhs)
{
    return *this = OsduAbstractTechnicalAssuranceStruct(rhs);
}

OsduAbstractTechnicalAssuranceStruct&
OsduAbstractTechnicalAssuranceStruct::operator=(OsduAbstractTechnicalAssuranceStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    technicalAssuranceTypeID = std::move(rhs.technicalAssuranceTypeID);
    reviewers = std::move(rhs.reviewers);
    acceptableUsage = std::move(rhs.acceptableUsage);
    unacceptableUsage = std::move(rhs.unacceptableUsage);
    effectiveDate = std::move(rhs.effectiveDate);
    comment = std::move(rhs.comment);


    return *this;
}

bool
OsduAbstractTechnicalAssuranceStruct::isValid() const
{
    return technicalAssuranceTypeIDValid() and ReviewersValid() and AcceptableUsageValid() and UnacceptableUsageValid();
}

bool
OsduAbstractTechnicalAssuranceStruct::technicalAssuranceTypeIDValid() const
{
    return !technicalAssuranceTypeID.has_value() or isValidOSDUReferenceIdString(technicalAssuranceTypeID.value());
}

bool
OsduAbstractTechnicalAssuranceStruct::ReviewersValid() const
{
    if (!reviewers.has_value()) {
        return true;
    }
    return std::all_of(reviewers.value().cbegin(), reviewers.value().cend(), [](OsduAbstractReviewerStruct x) { return x.isValid(); });
}

bool
OsduAbstractTechnicalAssuranceStruct::AcceptableUsageValid() const
{
    if (!acceptableUsage.has_value()) {
        return true;
    }
    return std::all_of(acceptableUsage.value().cbegin(), acceptableUsage.value().cend(), [](OsduAbstractAcceptableUsageStruct x) { return x.isValid(); });
}

bool
OsduAbstractTechnicalAssuranceStruct::UnacceptableUsageValid() const
{
    if (!unacceptableUsage.has_value()) {
        return true;
    }
    return std::all_of(unacceptableUsage.value().cbegin(), unacceptableUsage.value().cend(), [](OsduAbstractAcceptableUsageStruct x) { return x.isValid(); });
}

std::vector<std::string>
OsduAbstractTechnicalAssuranceStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }
    std::vector<std::string> result{"OsduAbstractTechnicalAssuranceStruct:isValid() failed"};

    if (!technicalAssuranceTypeIDValid()) {
        result.emplace_back("OsduAbstractTechnicalAssuranceStruct:technicalAssuranceTypeIDValid() failed");
    }
    if (!ReviewersValid() and reviewers.has_value()) {
        result.emplace_back("OsduAbstractTechnicalAssuranceStruct:ReviewersValid() failed");
        for (const OsduAbstractReviewerStruct& reviewer : reviewers.value()) {
            if (!reviewer.isValid()) {
                appendStringVector(result, reviewer.performAttributeValidityScan());
            }
        }
    }
    if (!AcceptableUsageValid() and acceptableUsage.has_value()) {
        result.emplace_back("OsduAbstractTechnicalAssuranceStruct:AcceptableUsageValid() failed");
        for (const OsduAbstractAcceptableUsageStruct& au : acceptableUsage.value()) {
            if (!au.isValid()) {
                appendStringVector(result, au.performAttributeValidityScan());
            }
        }
    }
    if (!UnacceptableUsageValid() and unacceptableUsage.has_value()) {
        result.emplace_back("OsduAbstractTechnicalAssuranceStruct:UnacceptableUsageValid() failed");

        for (const OsduAbstractAcceptableUsageStruct& uu : unacceptableUsage.value()) {
            if (!uu.isValid()) {
                appendStringVector(result, uu.performAttributeValidityScan());
            }
        }
    }

    return result;
}

} // namespace segysdk::metadata::osdu
