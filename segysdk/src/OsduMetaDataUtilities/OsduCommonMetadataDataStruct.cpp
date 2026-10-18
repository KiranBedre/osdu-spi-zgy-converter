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

#include "../OsduMetaDataUtilities/OsduCommonMetadataDataStruct.h"

namespace segysdk::metadata::osdu
{
OsduCommonMetadataDataStruct::OsduCommonMetadataDataStruct(std::string osduPartition)
  : OsduMetadataStructBase(osduPartition),
    extensionPropertiesStruct(OsduExtensionPropertyArrayStruct(osduPartition))
{
    /* Empty */
}

bool
OsduCommonMetadataDataStruct::isValid() const
{
    return structValid(extensionPropertiesStruct);
}

std::vector<std::string>
OsduCommonMetadataDataStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduCommonMetadataDataStruct::isValid() failed.");
    }

    performStructValidityScan(results, extensionPropertiesStruct);

    return results;
}

void
OsduCommonMetadataDataStruct::propagatePartition(const std::string& partition)
{
        extensionPropertiesStruct.setPartition(partition);
}

} // namespace segysdk::metadata::osdu
