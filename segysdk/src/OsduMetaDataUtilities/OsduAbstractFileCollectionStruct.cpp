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

#include "../OsduMetaDataUtilities/OsduAbstractFileCollectionStruct.h"

namespace segysdk::metadata::osdu
{
OsduAbstractFileCollectionStruct::OsduAbstractFileCollectionStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

bool
OsduAbstractFileCollectionStruct::isValid() const
{
    return !fileCollectionPath.empty() and arrayStructValid(fileSourceInfoStructs) and isValidStringRegex(checksum, hexEvenNumCharStringRegex_);
}

std::vector<std::string>
OsduAbstractFileCollectionStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }

    std::vector<std::string> results;
    results.emplace_back("OsduAbstractFileCollectionStruct::isValid() failed.");

    if (fileCollectionPath.empty()) {
        results.emplace_back("OsduAbstractFileCollectionStruct::fileCollectionPath does not have a value.");
    }

    performArrayStructValidityScan(results, fileSourceInfoStructs);
    performStringRegexValidityScan(results, checksum, "", hexEvenNumCharStringRegex_);

    return results;
}

void
OsduAbstractFileCollectionStruct::propagatePartition(const std::string& partition)
{
    if (fileSourceInfoStructs.has_value()) {
        for (auto& fileSourceInfo : fileSourceInfoStructs.value()) {
            fileSourceInfo.setPartition(partition);
        }
    }
}
} //namespace segysdk::metadata::osdu
