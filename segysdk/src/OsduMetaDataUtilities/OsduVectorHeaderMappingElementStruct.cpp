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

#include "../OsduMetaDataUtilities/OsduVectorHeaderMappingElementStruct.h"

#include <sstream>

namespace segysdk::metadata::osdu
{
OsduVectorHeaderMappingElementStruct::OsduVectorHeaderMappingElementStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

bool
OsduVectorHeaderMappingElementStruct::isValid() const
{
    return isValidIdString(keyName, IdStringCheckType::Reference)
           and (!wordWidth.has_value() or wordWidth.value() == 2 or wordWidth.value() == 4)
           and isValidIdString(wordFormatType, IdStringCheckType::Reference)
           and isValidIdString(uom, IdStringCheckType::Reference)
           and (position.has_value() and position.value() >= 0 and position.value() <= 238);
}

std::vector<std::string>
OsduVectorHeaderMappingElementStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduVectorHeaderMappingElementStruct::isValid() failed.");
    }

    performIdStringValidityScan(results, keyName, "OsduVectorHeaderMappingElementStruct:keyName: ", IdStringCheckType::Reference);

    if (wordWidth.has_value() and wordWidth.value() != 2 and wordWidth.value() != 4) {
        std::stringstream errMsg;
        errMsg << "OsduVectorHeaderMappingElementStruct:wordWidth: unexpected value " << wordWidth.value() << ". Expected: 2 or 4";
        results.emplace_back(errMsg.str());
    }

    performIdStringValidityScan(results, wordFormatType, "OsduVectorHeaderMappingElementStruct:wordFormatType: ", IdStringCheckType::Reference);
    performIdStringValidityScan(results, uom, "OsduVectorHeaderMappingElementStruct:uom: ", IdStringCheckType::Reference);

    if (position.has_value() and (position.value() < 0 or position.value() > 238)) {
        std::stringstream errMsg;
        errMsg << "OsduVectorHeaderMappingElementStruct:position: unexpected value " << position.value() << ". Expected: [0..238]";
        results.emplace_back(errMsg.str());
    }

    return results;
}

} // namespace segysdk::metadata::osdu
