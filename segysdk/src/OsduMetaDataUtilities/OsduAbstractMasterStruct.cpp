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

#include "../OsduMetaDataUtilities/OsduAbstractMasterStruct.h"

namespace segysdk::metadata::osdu
{
OsduAbstractMasterStruct::OsduAbstractMasterStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

bool
OsduAbstractMasterStruct::isValid() const
{
    return arrayStructValid(nameAliasesStruct) and arrayStructValid(geoContextsStruct)
           and structValid(spatialLocationStruct) and arrayStructValid(technicalAssurancesStruct);
}

std::vector<std::string>
OsduAbstractMasterStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduAbstractMasterStruct::isValid() failed.");
    }

    performArrayStructValidityScan(results, nameAliasesStruct);
    performArrayStructValidityScan(results, geoContextsStruct);
    performStructValidityScan(results, spatialLocationStruct);
    performArrayStructValidityScan(results, technicalAssurancesStruct);

    return results;
}

void
OsduAbstractMasterStruct::propagatePartition(const std::string& partition)
{
    if (nameAliasesStruct.has_value()) {
        for (auto& alias : nameAliasesStruct.value()) {
            alias.propagatePartition(partition);
        }
    }
    if (geoContextsStruct.has_value()) {
        for (auto& geoContext : geoContextsStruct.value()) {
            geoContext.propagatePartition(partition);
        }
    }
    if (spatialLocationStruct.has_value()) {
        spatialLocationStruct.value().propagatePartition(partition);
    }
    if (technicalAssurancesStruct.has_value()) {
        for (auto& technicalAssurance : technicalAssurancesStruct.value()) {
            technicalAssurance.propagatePartition(partition);
        }
    }
}
} // namespace segysdk::metadata::osdu
