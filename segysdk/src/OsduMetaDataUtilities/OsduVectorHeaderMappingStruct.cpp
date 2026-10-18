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

#include "../OsduMetaDataUtilities/OsduVectorHeaderMappingStruct.h"
#include <algorithm>


namespace segysdk::metadata::osdu
{
OsduVectorHeaderMappingStruct::OsduVectorHeaderMappingStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

bool
OsduVectorHeaderMappingStruct::isValid() const
{
    return arrayStructValid(vectorHeaderMapping_);
}

std::vector<std::string>
OsduVectorHeaderMappingStruct::performAttributeValidityScan() const
{
    std::vector<std::string> elementsResults;
    performArrayStructValidityScan(elementsResults, vectorHeaderMapping_);

    if (!elementsResults.empty()) {
        std::vector<std::string> results;
        results.emplace_back("OsduVectorHeaderMappingStruct::isValid() failed.");
        std::copy(elementsResults.begin(), elementsResults.end(), std::back_inserter(results));
        return results;
    }

    return {};
}

void
OsduVectorHeaderMappingStruct::propagatePartition(const std::string& partition)
{
    for (auto& mapping : vectorHeaderMapping_) {
        mapping.setPartition(partition);
    }
}

} // namespace segysdk::metadata::osdu
