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

#include "../OsduMetaDataUtilities/OsduOverrideByConstantValueStruct.h"


namespace segysdk::metadata::osdu
{
OsduOverrideByConstantValueStruct::OsduOverrideByConstantValueStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition)), 
    overrideByConstant(0)
{
    /* Empty */
}

bool
OsduOverrideByConstantValueStruct::isValid() const
{
    return !keyName.empty();
}

std::vector<std::string>
OsduOverrideByConstantValueStruct::performAttributeValidityScan() const
{
    if (!isValid()) {
        return {};
    }

    std::vector<std::string> result;
    result.emplace_back("OsduVectorHeaderMappingOverridesStruct::isValid() failed.");

    if (keyName.empty()) {
        result.emplace_back("OsduVectorHeaderMappingOverridesStruct: KeyName must have value");
    }
    performIdStringValidityScan(result, keyName, "OsduVectorHeaderMappingOverridesStruct::keyName", IdStringCheckType::Reference);

    return result;
}

} // namespace segysdk::metadata::osdu
