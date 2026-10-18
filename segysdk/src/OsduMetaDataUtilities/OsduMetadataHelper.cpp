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

#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"

#include <filesystem>

namespace segysdk::metadata::osdu
{
OsduMetadataHelper::OsduMetadataHelper()
    : OsduMetadataHelper(dummyNamespace) {}

OsduMetadataHelper::OsduMetadataHelper(std::string osduPartition)
    : osdu_partition_(std::move(osduPartition)) {}

void
OsduMetadataHelper::setOsduPartition(const std::string& osduPartition)
{
    osdu_partition_ = osduPartition;
}

std::string
OsduMetadataHelper::osduReferenceLookupString(const std::string& referencePath, const std::string& referenceValue, const std::string& version) const
{
    // "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0",
    // json["kind"] = osduReferenceLookupString("wks","dataset--FileCollection.SEGY", "1.0.0");
    return osdu_partition_ + separator_ + referencePath + separator_ + referenceValue + separator_ + version;
}

std::string 
OsduMetadataHelper::osduReferenceLookupString(const std::string& storageId, int64_t version)
{
    return storageId + separator_ + std::to_string(version);
}


std::string
OsduMetadataHelper::osduReferenceLookupString(const std::string& storageId) const
{
    // e.g. storageId = "opendes:master-data--Seismic3DInterpretationSet:65db1872b1d94957a5743ea92862b060"
    const auto parts = parseReferenceAttribute(storageId);
    if (parts.size() > 2) {
        const auto& referencePath = parts[1];
        const auto& referenceValue = parts[2];
        const std::string version = osdu_version();
        return osduReferenceLookupString(referencePath, referenceValue, version);
    }

    // File path?
    return storageId;
}

std::string
OsduMetadataHelper::osduStorageSystemIdString(const std::string& id_class, const std::string& id_value) const
{
    // "id": "namespace:dataset--File.SeismicHistogram:11eb0d5c-a6bd-5d74-8e32-ac8c429ee9f9",
    // json["id"] = osduStorageSystemIdString("dataset--File.SeismicHistogram", "11eb0d5c-a6bd-5d74-8e32-ac8c429ee9f9");
    return osdu_partition_ + separator_ + id_class + separator_ + id_value;
}

std::string
OsduMetadataHelper::osduWksReferenceLookupString(const std::string& referenceValue, const std::string& version)
{
    // "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0",
    return osdu_static_ + separator_ + wks_static_ + separator_ + referenceValue + separator_ + version;
}

std::string
OsduMetadataHelper::osduUnitReferenceString(const std::string& unit, const std::string& referenceType) const
{
    // "kind": "osdu:wks:dataset--FileCollection.SEGY:1.0.0",
    return osdu_partition_ + separator_ + referenceType + separator_ + unit + separator_;
}

std::string
OsduMetadataHelper::osdu_version(int major, int minor, int incremental)
{
    // e.g. ":1.0.0"
    return std::to_string(major) + dot_ + std::to_string(minor) + dot_ + std::to_string(incremental);
}

std::string
OsduMetadataHelper::osdu_version()
{
    // e.g. ":1.0.0"
    return "";
}

bool
OsduMetadataHelper::isValidOSDUReferenceIdString(const std::string& attributeValue) const
{
    // Skip validity test for empty value
    if (attributeValue.empty()) {
        return true;
    }
    const auto referenceIdVector = parseReferenceAttribute(attributeValue);

    // Valid number of components, valid OSDU partition and valid path (non-empty) and id value (non-empty)
    return referenceIdVector.size() == 4 and !referenceIdVector[1].empty() and !referenceIdVector[2].empty() and validPartitionCheck(referenceIdVector[0]);
}

bool
OsduMetadataHelper::isValidOSDUStorageSystemIdString(const std::string& idValue) const
{
    // Skip validity test for empty value
    if (idValue.empty()) {
        return true;
    }

    const std::vector<std::string> idVector = parseReferenceAttribute(idValue);

    // Valid number of components, valid OSDU partition and valid path (non-empty) and id value (non-empty)
    if (idVector.size() == 2) {
        if (idVector[0] == "sd://" or (idVector[0].size() == 1)) {
            return true;
        }
    }
    return idVector.size() == 3 and !idVector[1].empty() and !idVector[2].empty() and validPartitionCheck(idVector[0]);
}

bool
OsduMetadataHelper::validPartitionCheck(const std::string& partition) const
{

    return partition == osdu_partition_ or partition == osdu_static_;
}

std::vector<std::string>
OsduMetadataHelper::parseReferenceAttribute(const std::string& osduReference)
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

} // namespace segysdk::metadata::osdu
