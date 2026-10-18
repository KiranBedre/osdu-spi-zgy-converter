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

#include "../OsduMetaDataUtilities/OsduFileCollectionOpenZgyDataStruct.h"

namespace segysdk::metadata::osdu
{

OsduFileCollectionOpenZgyDataStruct::OsduFileCollectionOpenZgyDataStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

bool
OsduFileCollectionOpenZgyDataStruct::isValid() const
{
    return structValid(commonResourcesStruct) and structValid(datasetStruct) and structValid(fileCollectionStruct)        
           and structValid(compressionInfoStruct)
           and structValid(precisionStruct);
}

std::vector<std::string>
OsduFileCollectionOpenZgyDataStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }
    
    std::vector<std::string> results;
    results.emplace_back("OsduFileCollectionOpenZgyDataStruct::isValid() failed.");
    
    performStructValidityScan(results, commonResourcesStruct);
    performStructValidityScan(results, datasetStruct);
    performStructValidityScan(results, fileCollectionStruct);
    performStructValidityScan(results, compressionInfoStruct);
    performStructValidityScan(results, precisionStruct);

    return results;
}

void
OsduFileCollectionOpenZgyDataStruct::propagatePartition(const std::string& partition)
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

    if (compressionInfoStruct.has_value()) {
        compressionInfoStruct->setPartition(partition);
    }

    if (precisionStruct.has_value()) {
        precisionStruct->setPartition(partition);
    }
}
} // namespace segysdk::metadata::osdu
