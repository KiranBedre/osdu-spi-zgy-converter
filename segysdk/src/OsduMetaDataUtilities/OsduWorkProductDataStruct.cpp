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

#include "../OsduMetaDataUtilities/OsduWorkProductDataStruct.h"

namespace segysdk::metadata::osdu
{
OsduWorkProductDataStruct::OsduWorkProductDataStruct(std::string osduPartition)
  : OsduMetadataStructBase(osduPartition)
{
    /* Empty */
}

bool
OsduWorkProductDataStruct::isValid() const
{
    return isValidIdStringArray(components, IdStringCheckType::Reference) and structValid(spatialPointStruct)
           and structValid(spatialAreaStruct) and arrayStructValid(lineageAssertionsStruct);
}

std::vector<std::string>
OsduWorkProductDataStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduWorkProductDataStruct::isValid() failed.");
    }

    performIdStringArrayValidityScan(results, components, "OsduWorkProductDataStruct:components: ", IdStringCheckType::Reference);
    performStructValidityScan(results, spatialPointStruct);
    performStructValidityScan(results, spatialAreaStruct);
    performArrayStructValidityScan(results, lineageAssertionsStruct);

    return results;
}

void
OsduWorkProductDataStruct::propagatePartition(const std::string& partition)
{
    if (commonResources.has_value()) {
        commonResources.value().setPartition(partition);
    }
    if (spatialPointStruct.has_value()) {
        spatialPointStruct.value().setPartition(partition);
    }
    if (spatialAreaStruct.has_value()) {
        spatialAreaStruct.value().setPartition(partition);
    }
    if (lineageAssertionsStruct.has_value()) {
        for (auto& lineageAssertion : lineageAssertionsStruct.value()) {
            lineageAssertion.setPartition(partition);
        }
    }
}
} // namespace segysdk::metadata::osdu
