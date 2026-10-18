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

#include "../OsduMetaDataUtilities/OsduAbstractDatasetStruct.h"

#include "../OsduMetaDataUtilities/OsduAbstractDatasetLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAbstractDatasetStruct::OsduAbstractDatasetStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

bool
OsduAbstractDatasetStruct::endianValid() const
{
    return !endian.has_value()
           or endian.value() == OsduAbstractDatasetLiterals::BigEndian
           or endian.value() == OsduAbstractDatasetLiterals::LittleEndian;
}

bool
OsduAbstractDatasetStruct::isValid() const
{
    return isValidStringRegex(totalSize, numericStringRegex_) and isValidIdString(encodingFormatTypeID, IdStringCheckType::Reference)
           and isValidIdString(schemaFormatTypeID, IdStringCheckType::Reference) and endianValid();
}

std::vector<std::string>
OsduAbstractDatasetStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduAbstractDatasetStruct::isValid() failed.");
    }

    performStringRegexValidityScan(results, totalSize, "OsduAbstractDatasetStruct::totalSize", numericStringRegex_);
    performIdStringValidityScan(results, encodingFormatTypeID, "OsduAbstractDatasetStruct::encodingFormatTypeID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, schemaFormatTypeID, "OsduAbstractDatasetStruct::schemaFormatTypeID", IdStringCheckType::Reference);

    if (!endianValid()) {
        results.emplace_back("OsduAbstractDatasetStruct::Endian is not valid.");
    }

    return results;
}

} // namespace segysdk::metadata::osdu
