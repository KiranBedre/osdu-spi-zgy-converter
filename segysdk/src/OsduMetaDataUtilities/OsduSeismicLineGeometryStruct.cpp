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

#include "../OsduMetaDataUtilities/OsduSeismicLineGeometryStruct.h"

namespace segysdk::metadata::osdu
{
OsduSeismicLineGeometryStruct::OsduSeismicLineGeometryStruct(std::string osduPartition)
  : OsduMetadataStructBase(osduPartition),
    commonMetadataStruct(osduPartition),
    helper_(OsduMetadataHelper(getPartition()))
{
    /* Empty */
}

bool
OsduSeismicLineGeometryStruct::isValid() const
{
    return structValid(commonMetadataStruct) and structValid(seismicLineGeometryDataStruct);
}

std::vector<std::string>
OsduSeismicLineGeometryStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduSeismicLineGeometryStruct::isValid() failed.");
    }

    performStructValidityScan(results, commonMetadataStruct);
    performStructValidityScan(results, seismicLineGeometryDataStruct);

    return results;
}

void
OsduSeismicLineGeometryStruct::propagatePartition(const std::string& partition)
{
    commonMetadataStruct.setPartition(partition);
    if (seismicLineGeometryDataStruct.has_value()) {
        seismicLineGeometryDataStruct.value().setPartition(partition);
    }
}
} // namespace segysdk::metadata::osdu
