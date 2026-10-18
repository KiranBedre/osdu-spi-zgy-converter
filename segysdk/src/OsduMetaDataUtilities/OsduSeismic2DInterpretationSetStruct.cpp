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

#include "../OsduMetaDataUtilities/OsduSeismic2DInterpretationSetStruct.h"

namespace segysdk::metadata::osdu
{
OsduSeismic2DInterpretationSetStruct::OsduSeismic2DInterpretationSetStruct(std::string osduPartition)
  : OsduMetadataStructBase(osduPartition),
    commonMetadataStruct(osduPartition),
    helper_(OsduMetadataHelper(getPartition()))
{
    /* Empty */
}

bool
OsduSeismic2DInterpretationSetStruct::isValid() const
{
    return structValid(commonMetadataStruct) and structValid(dataStruct);
}

std::vector<std::string>
OsduSeismic2DInterpretationSetStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduSeismic2DInterpretationSetStruct::isValid() failed.");
    }

    performStructValidityScan(results, commonMetadataStruct);
    performStructValidityScan(results, dataStruct);

    return results;
}

void
OsduSeismic2DInterpretationSetStruct::propagatePartition(const std::string& partition)
{
    commonMetadataStruct.setPartition(partition);
    if (dataStruct.has_value()) {
        dataStruct.value().setPartition(partition);
    }
}
} // namespace segysdk::metadata::osdu
