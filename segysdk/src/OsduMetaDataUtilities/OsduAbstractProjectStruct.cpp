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

#include "../OsduMetaDataUtilities/OsduAbstractProjectStruct.h"

namespace segysdk::metadata::osdu
{
OsduAbstractProjectStruct::OsduAbstractProjectStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

bool
OsduAbstractProjectStruct::isValid() const
{
    return arrayStructValid(fundsAuthorizations) and isValidIdStringArray(contractIds, IdStringCheckType::Reference)
           and isValidIdString(operator_, IdStringCheckType::Reference) and arrayStructValid(contractorsStruct)
           and arrayStructValid(personnelStruct) and arrayStructValid(specificationsStruct) and arrayStructValid(statesStruct);
}

std::vector<std::string>
OsduAbstractProjectStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduAbstractProjectStruct::isValid() failed.");
    }

    performIdStringValidityScan(results, id, "OsduAbstractProjectStruct:id: ", IdStringCheckType::StorageSystem);
    performArrayStructValidityScan(results, fundsAuthorizations);
    performIdStringArrayValidityScan(results, contractIds, "OsduAbstractProjectStruct:contractIds: ", IdStringCheckType::Reference);
    performIdStringValidityScan(results, operator_, "OsduAbstractProjectStruct:operator: ", IdStringCheckType::Reference);
    performArrayStructValidityScan(results, contractorsStruct);
    performArrayStructValidityScan(results, personnelStruct);
    performArrayStructValidityScan(results, specificationsStruct);
    performArrayStructValidityScan(results, statesStruct);

    return results;
}

void
OsduAbstractProjectStruct::propagatePartition(const std::string& partition)
{
    if (fundsAuthorizations.has_value()) {
        for (auto& fundAuthorization : fundsAuthorizations.value()) {
            fundAuthorization.propagatePartition(partition);
        }
    }
    if (contractorsStruct.has_value()) {
        for (auto& contractorStruct : contractorsStruct.value()) {
            contractorStruct.propagatePartition(partition);
        }
    }
    if (personnelStruct.has_value()) {
        for (auto& personalStruct : personnelStruct.value()) {
            personalStruct.propagatePartition(partition);
        }
    }
    if (specificationsStruct.has_value()) {
        for (auto& specificationStruct : specificationsStruct.value()) {
            specificationStruct.propagatePartition(partition);
        }
    }
    if (statesStruct.has_value()) {
        for (auto& stateStruct : statesStruct.value()) {
            stateStruct.propagatePartition(partition);
        }
    }
}
} // namespace segysdk::metadata::osdu
