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

#include "../OsduMetaDataUtilities/OsduAbstractTechnicalAssurance.h"

#include "../OsduMetaDataUtilities/OsduAbstractAcceptableUsage.h"
#include "../OsduMetaDataUtilities/OsduAbstractTechnicalAssuranceLiterals.h"
#include "../OsduMetaDataUtilities/OsduAbstractReviewer.h"

namespace segysdk::metadata::osdu
{

OsduAbstractTechnicalAssurance::OsduAbstractTechnicalAssurance(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), technicalAssuranceStruct_(OsduAbstractTechnicalAssuranceStruct(osduPartition))
{
    /* Empty */
}

OsduAbstractTechnicalAssurance::OsduAbstractTechnicalAssurance(const OsduAbstractTechnicalAssurance& other)
  : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
    technicalAssuranceStruct_(other.technicalAssuranceStruct_)
{
    setOsduPartition(other.getOsduPartition());
    setJson(other.getJson());
}

OsduAbstractTechnicalAssurance::OsduAbstractTechnicalAssurance(OsduAbstractTechnicalAssurance&& other) noexcept
  : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
    technicalAssuranceStruct_(std::move(other.technicalAssuranceStruct_))
{
    setOsduPartition(other.getOsduPartition());
    setJson(other.getJson());
}

OsduAbstractTechnicalAssurance&
OsduAbstractTechnicalAssurance::operator=(const OsduAbstractTechnicalAssurance& rhs)
{
    return *this = OsduAbstractTechnicalAssurance(rhs);
}

OsduAbstractTechnicalAssurance&
OsduAbstractTechnicalAssurance::operator=(OsduAbstractTechnicalAssurance&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setIsArrayElement(rhs.getIsArrayElement());
    setJson(rhs.getAsJson());

    technicalAssuranceStruct_ = std::move(rhs.technicalAssuranceStruct_);

    return *this;
}

OsduAbstractTechnicalAssurance::OsduAbstractTechnicalAssurance(OsduAbstractTechnicalAssuranceStruct technicalAssuranceStruct)
  : OsduMetadataBase(technicalAssuranceStruct.getPartition()), technicalAssuranceStruct_(std::move(technicalAssuranceStruct))
{
    populateJsonObject();
}

OsduAbstractTechnicalAssurance::OsduAbstractTechnicalAssurance(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), technicalAssuranceStruct_(OsduAbstractTechnicalAssuranceStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractTechnicalAssurance::isValidInput() const
{
    return technicalAssuranceStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractTechnicalAssurance::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractTechnicalAssuranceStruct>(technicalAssuranceStruct_);
}

void
OsduAbstractTechnicalAssurance::initializeJson()
{
    clearJsonObject();
}

void
OsduAbstractTechnicalAssurance::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduAbstractTechnicalAssuranceLiterals::TechnicalAssuranceTypeID, technicalAssuranceStruct_.technicalAssuranceTypeID);
    if (technicalAssuranceStruct_.reviewers.has_value()) {
        InitializeJsonArray(OsduAbstractTechnicalAssuranceLiterals::Reviewers);
        for (const OsduAbstractReviewerStruct& reviewerStruct : technicalAssuranceStruct_.reviewers.value()) {
            OsduAbstractReviewer reviewer(reviewerStruct);
            appendArrayElement(OsduAbstractTechnicalAssuranceLiterals::Reviewers, reviewer.getAsJson());
        }
    }
    if (technicalAssuranceStruct_.acceptableUsage.has_value()) {
        InitializeJsonArray(OsduAbstractTechnicalAssuranceLiterals::AcceptableUsage);
        for (const OsduAbstractAcceptableUsageStruct& usageStruct : technicalAssuranceStruct_.acceptableUsage.value()) {
            OsduAbstractAcceptableUsage usage(usageStruct);
            appendArrayElement(OsduAbstractTechnicalAssuranceLiterals::AcceptableUsage, usage.getAsJson());
        }
    }
    if (technicalAssuranceStruct_.unacceptableUsage.has_value()) {
        InitializeJsonArray(OsduAbstractTechnicalAssuranceLiterals::UnacceptableUsage);
        for (const OsduAbstractAcceptableUsageStruct& usageStruct : technicalAssuranceStruct_.unacceptableUsage.value()) {
            OsduAbstractAcceptableUsage usage(usageStruct);
            appendArrayElement(OsduAbstractTechnicalAssuranceLiterals::UnacceptableUsage, usage.getAsJson());
        }
    }
    setOptionalParameter(OsduAbstractTechnicalAssuranceLiterals::Comment, technicalAssuranceStruct_.comment);
    setOptionalParameter(OsduAbstractTechnicalAssuranceLiterals::EffectiveDate, technicalAssuranceStruct_.effectiveDate);
}

void
OsduAbstractTechnicalAssurance::initializeMetadataStruct()
{
    technicalAssuranceStruct_ = OsduAbstractTechnicalAssuranceStruct(getOsduPartition());
}

void
OsduAbstractTechnicalAssurance::populateMetadataStruct()
{
    technicalAssuranceStruct_.technicalAssuranceTypeID = getOptionalParameter<std::string>(OsduAbstractTechnicalAssuranceLiterals::TechnicalAssuranceTypeID);

    if (isMember(OsduAbstractTechnicalAssuranceLiterals::Reviewers)) {
        const auto jsonArray = getAllArrayElements<Json::Value>(OsduAbstractTechnicalAssuranceLiterals::Reviewers);
        std::vector<OsduAbstractReviewerStruct> result;
        for (const auto& json : jsonArray) {
            OsduAbstractReviewer reviewer(json, getOsduPartition());
            const auto ptr = std::reinterpret_pointer_cast<OsduAbstractReviewerStruct>(reviewer.getMetaDataStruct());
            if (ptr != nullptr) {
                result.emplace_back(*ptr);
            }
        }
        technicalAssuranceStruct_.reviewers = result;
    }

    if (isMember(OsduAbstractTechnicalAssuranceLiterals::AcceptableUsage)) {
        const auto jsonArray = getAllArrayElements<Json::Value>(OsduAbstractTechnicalAssuranceLiterals::AcceptableUsage);
        std::vector<OsduAbstractAcceptableUsageStruct> result;
        for (const auto& json : jsonArray) {
            OsduAbstractAcceptableUsage reviewer(json, getOsduPartition());
            const auto ptr = std::reinterpret_pointer_cast<OsduAbstractAcceptableUsageStruct>(reviewer.getMetaDataStruct());
            if (ptr != nullptr) {
                result.emplace_back(*ptr);
            }
        }
        technicalAssuranceStruct_.acceptableUsage = result;
    }

     if (isMember(OsduAbstractTechnicalAssuranceLiterals::UnacceptableUsage)) {
        const auto jsonArray = getAllArrayElements<Json::Value>(OsduAbstractTechnicalAssuranceLiterals::UnacceptableUsage);
        std::vector<OsduAbstractAcceptableUsageStruct> result;
        for (const auto& json : jsonArray) {
            OsduAbstractAcceptableUsage reviewer(json, getOsduPartition());
            const auto ptr = std::reinterpret_pointer_cast<OsduAbstractAcceptableUsageStruct>(reviewer.getMetaDataStruct());
            if (ptr != nullptr) {
                result.emplace_back(*ptr);
            }
        }
        technicalAssuranceStruct_.unacceptableUsage = result;
    }

     technicalAssuranceStruct_.effectiveDate = getOptionalParameter<std::string>(OsduAbstractTechnicalAssuranceLiterals::EffectiveDate);

     technicalAssuranceStruct_.comment = getOptionalParameter<std::string>(OsduAbstractTechnicalAssuranceLiterals::Comment);
}

} // namespace segysdk::metadata::osdu
