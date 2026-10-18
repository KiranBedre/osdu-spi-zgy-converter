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

#include "../OsduMetaDataUtilities/OsduAbstractActivityParameterStruct.h"

namespace segysdk::metadata::osdu
{
OsduAbstractActivityParameterStruct::OsduAbstractActivityParameterStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

bool
OsduAbstractActivityParameterStruct::isValid() const
{
    return arrayStructValid(keysStruct) and isValidIdString(dataObject, IdStringCheckType::Reference)
           and isValidIdString(kindId, IdStringCheckType::Reference) and isValidIdString(roleId, IdStringCheckType::Reference)
           and isValidIdString(dataQuantityUomId, IdStringCheckType::Reference);
}

std::vector<std::string>
OsduAbstractActivityParameterStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduAbstractActivityParameterStruct::isValid() failed.");
    }

    performArrayStructValidityScan(results, keysStruct);
    performIdStringValidityScan(results, dataObject, "OsduAbstractActivityParameterStruct:dataObject: ", IdStringCheckType::Reference);
    performIdStringValidityScan(results, kindId, "OsduAbstractActivityParameterStruct:kindId: ", IdStringCheckType::Reference);
    performIdStringValidityScan(results, roleId, "OsduAbstractActivityParameterStruct:roleId: ", IdStringCheckType::Reference);
    performIdStringValidityScan(results, dataQuantityUomId, "OsduAbstractActivityParameterStruct:dataQuantityUomId: ", IdStringCheckType::Reference);

    return results;
}

void
OsduAbstractActivityParameterStruct::propagatePartition(const std::string& partition)
{
    if (keysStruct.has_value()) {
        for (auto& keyStruct : keysStruct.value()) {
            keyStruct.propagatePartition(partition);
        }
    }
}
} // namespace segysdk::metadata::osdu
