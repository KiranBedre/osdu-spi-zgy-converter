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

#include "../OsduMetaDataUtilities/OsduAbstractFileSourceInfoStruct.h"

namespace segysdk::metadata::osdu
{
OsduAbstractFileSourceInfoStruct::OsduAbstractFileSourceInfoStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

bool
OsduAbstractFileSourceInfoStruct::isValid() const
{
    return !fileSource.empty() and isValidStringRegex(fileSize, numericStringRegex_)
           and isValidIdString(encodingFormatTypeID, IdStringCheckType::Reference) and isValidStringRegex(checksum, hexEvenNumCharStringRegex_);
}

std::vector<std::string>
OsduAbstractFileSourceInfoStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }

    std::vector<std::string> results;
    results.emplace_back("OsduAbstractFileSourceInfoStruct::isValid() failed.");

    if (fileSource.empty()) {
        results.emplace_back("OsduAbstractFileSourceInfoStruct::fileSource cannot be empty.");
    }

    performStringRegexValidityScan(results, fileSize, "OsduAbstractFileSourceInfoStruct::fileSize", numericStringRegex_);
    performIdStringValidityScan(results, encodingFormatTypeID, "OsduAbstractFileSourceInfoStruct::encodingFormatTypeID", IdStringCheckType::Reference);
    performStringRegexValidityScan(results, checksum, "OsduAbstractFileSourceInfoStruct::checksum", hexEvenNumCharStringRegex_);

    return results;
}
} // namespace segysdk::metadata::osdu
