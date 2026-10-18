/*
Copyright 2024 Schlumberger

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

#include "../OsduMetaDataUtilities/OsduCommonMetadataStruct.h"

namespace segysdk::metadata::osdu
{
OsduCommonMetadataStruct::OsduCommonMetadataStruct(std::string osduPartition)
  : OsduMetadataStructBase(osduPartition),
    aclStruct(osduPartition),
    legalStruct(osduPartition),
    metaStruct(osduPartition)
{
    /* Empty */
}

bool
OsduCommonMetadataStruct::isValid() const
{
    return !kind.empty() and isValidStringRegex(kind, contentSchemaKindStringRegex_)
           and isValidIdString(id, IdStringCheckType::StorageSystem) and structValid(aclStruct) and structValid(legalStruct)
           and structValid(tagsStruct) and structValid(ancestryStruct) and structValid(metaStruct) and structValid(dataStruct);
}

std::vector<std::string>
OsduCommonMetadataStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduCommonMetadataStruct::isValid() failed.");
    }

    performIdStringValidityScan(results, id, "OsduCommonMetadataStruct:id: ", IdStringCheckType::StorageSystem);
    performStructValidityScan(results, aclStruct);
    performStructValidityScan(results, legalStruct);
    performStructValidityScan(results, tagsStruct);
    performStructValidityScan(results, ancestryStruct);
    performStructValidityScan(results, metaStruct);
    performStructValidityScan(results, dataStruct);

    return results;
}

void
OsduCommonMetadataStruct::propagatePartition(const std::string& partition)
{
    aclStruct.setPartition(partition);
    legalStruct.setPartition(partition);

    if (tagsStruct.has_value()) {
        tagsStruct.value().setPartition(partition);
    }
    if (ancestryStruct.has_value()) {
        ancestryStruct.value().setPartition(partition);
    }

    metaStruct.setPartition(partition);

    if (dataStruct.has_value()) {
        dataStruct.value().setPartition(partition);
    }
}
} // namespace segysdk::metadata::osdu
