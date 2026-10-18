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

#include "../OsduMetaDataUtilities/OsduFileCollectionSegyDataStruct.h"

namespace segysdk::metadata::osdu
{

OsduFileCollectionSegyDataStruct::OsduFileCollectionSegyDataStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

bool
OsduFileCollectionSegyDataStruct::isValid() const
{
    return structValid(commonResourcesStruct) and structValid(datasetStruct) and structValid(fileCollectionStruct)
           and structValid(vectorHeaderMappingStruct) and structValid(binaryHeaderOverridesStruct) and structValid(traceHeaderOverridesStruct)
           and structValid(precisionStruct) and isValidIdString(storageFacilityID, IdStringCheckType::Reference);
}

std::vector<std::string>
OsduFileCollectionSegyDataStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }
    
    std::vector<std::string> results;
    results.emplace_back("OsduFileCollectionSegyDataStruct::isValid() failed.");
    
    performStructValidityScan(results, commonResourcesStruct);
    performStructValidityScan(results, datasetStruct);
    performStructValidityScan(results, fileCollectionStruct);
    performStructValidityScan(results, vectorHeaderMappingStruct);
    performStructValidityScan(results, binaryHeaderOverridesStruct);
    performStructValidityScan(results, traceHeaderOverridesStruct);
    performStructValidityScan(results, precisionStruct);
    performIdStringValidityScan(results, storageFacilityID, "OsduFileCollectionSegyDataStruct::storageFacilityID", IdStringCheckType::Reference);

    return results;
}

void
OsduFileCollectionSegyDataStruct::propagatePartition(const std::string& partition)
{
    if (commonResourcesStruct.has_value()) {
        commonResourcesStruct->setPartition(partition);
    }
    if (datasetStruct.has_value()) {
        datasetStruct->setPartition(partition);
    }
    if (fileCollectionStruct.has_value()) {
        fileCollectionStruct->setPartition(partition);
    }

    if (vectorHeaderMappingStruct.has_value()) {
        vectorHeaderMappingStruct->setPartition(partition);
    }
    if (binaryHeaderOverridesStruct.has_value()) {
        binaryHeaderOverridesStruct->setPartition(partition);
    }
    if (traceHeaderOverridesStruct.has_value()) {
        traceHeaderOverridesStruct->setPartition(partition);
    }
    if (precisionStruct.has_value()) {
        precisionStruct->setPartition(partition);
    }
}
} // namespace segysdk::metadata::osdu
