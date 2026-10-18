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
OsduProjectStateStruct::OsduProjectStateStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

bool
OsduProjectStateStruct::isValid() const
{
    return isValidIdString(projectStateTypeID, IdStringCheckType::Reference);
}

std::vector<std::string>
OsduProjectStateStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduProjectStateStruct::isValid() failed.");
    }

    performIdStringValidityScan(results, projectStateTypeID, "OsduProjectStateStruct:projectStateTypeID: ", IdStringCheckType::Reference);

    return results;
}

void
OsduProjectStateStruct::propagatePartition(const std::string& /*partition*/)
{
    /* Empty */
}
} // namespace segysdk::metadata::osdu
