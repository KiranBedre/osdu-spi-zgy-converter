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

#include "../OsduMetaDataUtilities/OsduSeismicBinGridDataStruct.h"

namespace segysdk::metadata::osdu
{
OsduSeismicBinGridDataStruct::OsduSeismicBinGridDataStruct(std::string osduPartition)
  : OsduMetadataStructBase(osduPartition)
{
    /* Empty */
}

bool
OsduSeismicBinGridDataStruct::isValid() const
{
    return structValid(commonResourcesStruct) and structValid(wpcGroupTypeStruct)
           and structValid(workProductComponentStruct) and structValid(binGridStruct);
}

bool 
OsduSeismicBinGridDataStruct::isCompleteP6Definition() const
{
    return binGridStruct.has_value() and binGridStruct.value().isCompleteP6Definition();
}


std::vector<std::string>
OsduSeismicBinGridDataStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduSeismicBinGridDataStruct::isValid() failed.");
    }

    performStructValidityScan(results, commonResourcesStruct);
    performStructValidityScan(results, wpcGroupTypeStruct);
    performStructValidityScan(results, workProductComponentStruct);
    performStructValidityScan(results, binGridStruct);

    return results;
}

void
OsduSeismicBinGridDataStruct::propagatePartition(const std::string& partition)
{
    if (commonResourcesStruct.has_value()) {
        commonResourcesStruct.value().setPartition(partition);
    }
    if (wpcGroupTypeStruct.has_value()) {
        wpcGroupTypeStruct.value().setPartition(partition);
    }
    if (workProductComponentStruct.has_value()) {
        workProductComponentStruct.value().setPartition(partition);
    }
    if (binGridStruct.has_value()) {
        binGridStruct.value().setPartition(partition);
    }
}
} // namespace segysdk::metadata::osdu
