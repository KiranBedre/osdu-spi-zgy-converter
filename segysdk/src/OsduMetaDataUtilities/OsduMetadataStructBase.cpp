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

#include "../OsduMetaDataUtilities/OsduMetadataStructBase.h"
#include "../Logger/Logger.h"

#include <algorithm>
#include <regex>

namespace segysdk::metadata::osdu
{
OsduMetadataStructBase::OsduMetadataStructBase(std::string osduPartition)
  : partition_(std::move(osduPartition))
{
    if (partition_.empty()) {
        // Use default value for no parition cases.
        partition_ = "no-partition-defined";
    }
}

OsduMetadataStructBase::OsduMetadataStructBase(OsduMetadataStructBase&& other) noexcept
  : partition_(std::move(other.partition_))
{
    /* Empty */
}

OsduMetadataStructBase&
OsduMetadataStructBase::operator=(OsduMetadataStructBase&& rhs) noexcept
{
    partition_ = std::move(rhs.partition_);
    return *this;
}

std::string
OsduMetadataStructBase::getPartitionFromReference(const std::string& refAttributeString)
{
    const auto components = parseReferenceAttribute(refAttributeString);
    if (components.size() >= 3) {
        return components.at(0);
    }
    return {};
}

void
OsduMetadataStructBase::setPartition(std::string partition)
{
    propagatePartition(partition);

    partition_ = std::move(partition);
}

std::vector<std::string>
OsduMetadataStructBase::parseReferenceAttribute(const std::string& osduReference)
{
    std::vector<std::string> referenceIdVector;

    std::string attributeId = osduReference;

    // "osduReference":  "opendes:reference-data--SchemaFormatType:SEGY:",
    // "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0",

    auto pos = attributeId.find_first_of(':');
    while (pos != std::string::npos) {
        if (pos > 0) {
            referenceIdVector.emplace_back(attributeId.substr(0, pos));
        }
        else {
            referenceIdVector.emplace_back("");
        }
        attributeId = attributeId.substr(pos + 1);
        pos = attributeId.find_first_of(':');
    }
    // Values might have trailing colon, so accept last field as empty string
    referenceIdVector.emplace_back(attributeId);

    return referenceIdVector;
}

void
OsduMetadataStructBase::appendStringVector(std::vector<std::string>& targetVector, const std::vector<std::string>& fromVector) const
{
    std::copy_if(fromVector.begin(), fromVector.end(),
                 std::back_inserter(targetVector),
                 [](const std::string& s) { return !s.empty(); });
}

bool
OsduMetadataStructBase::isValidOSDUCoordinateReferenceIdString(const std::string& attributeValue) const
{
    // Skip validity test for empty value
    if (attributeValue.empty()) {
        return true;
    }
    const auto referenceIdVector = parseReferenceAttribute(attributeValue);

    // Valid number of components, valid OSDU partition and valid path (non-empty) and id value (non-empty)
    return referenceIdVector.size() > 4 and !referenceIdVector[1].empty() and !referenceIdVector[2].empty() and referenceIdVector[3] == "EPSG" and validPartitionCheck(referenceIdVector[0]);
}

bool
OsduMetadataStructBase::isValidOSDUReferenceIdString(const std::string& attributeValue) const
{
    // Skip validity test for empty value
    if (attributeValue.empty()) {
        return true;
    }

    return isValidStringRegex(attributeValue, referenceStringRegex_);
}

bool
OsduMetadataStructBase::isValidOSDUStorageSystemIdString(const std::string& idValue) const
{
    // Skip validity test for empty value
    if (idValue.empty()) {
        return true;
    }

    const auto idVector = parseReferenceAttribute(idValue);

    // Valid number of components, valid OSDU partition and valid path (non-empty) and id value (non-empty)
    return idVector.size() == 3 and !idVector[1].empty() and !idVector[2].empty() and validPartitionCheck(idVector[0]);
}

bool
OsduMetadataStructBase::validPartitionCheck(const std::string& partition) const
{
    static const std::string osdu_static = "osdu";

    return partition == partition_ or partition == osdu_static;
}

std::string
OsduMetadataStructBase::getValidReferenceString(const std::string& partition, const std::string& areaPath, const std::string& className, const std::string& version) const
{
    return partition + refSeparator_ + areaPath + refSeparator_ + className + refSeparator_ + version;
}

bool
OsduMetadataStructBase::isValidStringRegex(const std::string& str, const std::string& regex)
{
    try {
        const std::regex rx{regex};
        return std::regex_match(str, rx);
    }
    catch (std::exception& ex) {
        const std::string msg = "Error parsing regular expression: " + std::string(ex.what());
        SEGYLOG_REPORT_ERROR(msg);
    }
    return false;
}

bool
OsduMetadataStructBase::isValidStringRegex(const std::optional<std::string>& str, const std::string& regex) const
{
    return !str.has_value() or isValidStringRegex(str.value(), regex);
}

bool
OsduMetadataStructBase::isValidStringArrayRegex(const std::vector<std::string>& strs, const std::string& regex) const
{
    return std::all_of(strs.begin(), strs.end(), [&regex](const auto& str) {
        return isValidStringRegex(str, regex); });
}

bool
OsduMetadataStructBase::isValidStringArrayRegex(const std::optional<std::vector<std::string>>& strs, const std::string& regex) const
{
    return !strs.has_value() or isValidStringArrayRegex(strs.value(), regex);
}

bool
OsduMetadataStructBase::isValidIdString(const std::string& id, IdStringCheckType checkType) const
{
    switch (checkType) {
    case IdStringCheckType::CRS:
        return isValidOSDUCoordinateReferenceIdString(id);
    case IdStringCheckType::Reference:
        return isValidOSDUReferenceIdString(id);
    case IdStringCheckType::StorageSystem:
        return isValidOSDUStorageSystemIdString(id);
    }

    return false;
}

bool
OsduMetadataStructBase::isValidIdString(const std::optional<std::string>& id, IdStringCheckType checkType) const
{
    return !id.has_value() or isValidIdString(id.value(), checkType);
}

bool
OsduMetadataStructBase::isValidIdStringArray(const std::vector<std::string>& ids, IdStringCheckType checkType) const
{
    return std::all_of(ids.begin(), ids.end(), [&](const auto& id) { return isValidIdString(id, checkType); });
}

bool
OsduMetadataStructBase::isValidIdStringArray(const std::optional<std::vector<std::string>>& ids, IdStringCheckType checkType) const
{
    return !ids.has_value() or isValidIdStringArray(ids.value(), checkType);
}

void
OsduMetadataStructBase::performStringRegexValidityScan(std::vector<std::string>& results, const std::string& str, const std::string& tag, const std::string& regex) const
{
    if (!isValidStringRegex(str, regex)) {
        results.emplace_back(tag + ": string doesn't match required pattern: " + regex);
    }
}

void
OsduMetadataStructBase::performStringRegexValidityScan(std::vector<std::string>& results, const std::optional<std::string>& str, const std::string& tag, const std::string& regex) const
{
    if (str.has_value()) {
        performStringRegexValidityScan(results, str.value(), tag, regex);
    }
}

void
OsduMetadataStructBase::performStringArrayRegexValidityScan(std::vector<std::string>& results, const std::vector<std::string>& strs, const std::string& tag, const std::string& regex) const
{
    for (const auto& str : strs) {
        if (!isValidStringRegex(str, regex)) {
            results.emplace_back(tag + ": string doesn't match required pattern: " + regex);
        }
    }
}

void
OsduMetadataStructBase::performStringArrayRegexValidityScan(std::vector<std::string>& results, const std::optional<std::vector<std::string>>& strs, const std::string& tag, const std::string& regex) const
{
    if (strs.has_value()) {
        performStringArrayRegexValidityScan(results, strs.value(), tag, regex);
    }
}

void
OsduMetadataStructBase::performIdStringValidityScan(std::vector<std::string>& results, const std::string& id, const std::string& tag, IdStringCheckType checkType) const
{
    switch (checkType) {
    case IdStringCheckType::CRS:
        if (!isValidOSDUCoordinateReferenceIdString(id)) {
            results.emplace_back(tag + ": Not properly formatted CRS reference id attribute.");
        }
        break;
    case IdStringCheckType::Reference:
        if (!isValidOSDUReferenceIdString(id)) {
            results.emplace_back(tag + ": Not properly formatted reference attribute.");
        }
        break;
    case IdStringCheckType::StorageSystem:
        if (!isValidOSDUStorageSystemIdString(id)) {
            results.emplace_back(tag + ": Not properly formatted storage system attribute.");
        }
        break;
    }
}

void
OsduMetadataStructBase::performIdStringValidityScan(std::vector<std::string>& results, const std::optional<std::string>& id, const std::string& tag, IdStringCheckType checkType) const
{
    if (id.has_value()) {
        performIdStringValidityScan(results, id.value(), tag, checkType);
    }
}

void
OsduMetadataStructBase::performIdStringArrayValidityScan(std::vector<std::string>& results, const std::vector<std::string>& ids, const std::string& tag, IdStringCheckType checkType) const
{
    for (const auto& id : ids) {
        performIdStringValidityScan(results, id, tag, checkType);
    }
}

void
OsduMetadataStructBase::performIdStringArrayValidityScan(std::vector<std::string>& results, const std::optional<std::vector<std::string>>& ids, const std::string& tag, IdStringCheckType checkType) const
{
    if (ids.has_value()) {
        performIdStringArrayValidityScan(results, ids.value(), tag, checkType);
    }
}

void
OsduMetadataStructBase::propagatePartition(const std::string& partition)
{
    /* No op by default */
    (void)partition;
}

} // namespace segysdk::metadata::osdu
