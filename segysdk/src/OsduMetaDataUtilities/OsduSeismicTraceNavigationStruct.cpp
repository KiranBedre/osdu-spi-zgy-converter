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

#include "../OsduMetaDataUtilities/OsduSeismicTraceNavigationStruct.h"

namespace segysdk::metadata::osdu
{
OsduSeismicTraceNavigationStruct::OsduSeismicTraceNavigationStruct(std::string osduPartition)
  : OsduMetadataStructBase(osduPartition),
    contentStruct(osduPartition),
    helper_(OsduMetadataHelper(getPartition()))
{
    /* Empty */
}

bool
OsduSeismicTraceNavigationStruct::isValid() const
{
    return isValidIdString(originalSeismicLineGeometryId, IdStringCheckType::Reference) and structValid(contentStruct)
           and arrayStructValid(lineSegmentsStruct) and structValid(spatialLocationStruct);
}

std::vector<std::string>
OsduSeismicTraceNavigationStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduSeismicTraceNavigationStruct::isValid() failed.");
    }

    performIdStringValidityScan(results, originalSeismicLineGeometryId, "OsduSeismicTraceNavigationStruct:originalSeismicLineGeometryId: ", IdStringCheckType::Reference);
    performStructValidityScan(results, contentStruct);
    performArrayStructValidityScan(results, lineSegmentsStruct);
    performStructValidityScan(results, spatialLocationStruct);

    return results;
}

void
OsduSeismicTraceNavigationStruct::propagatePartition(const std::string& partition)
{
    contentStruct.setPartition(partition);

    if (lineSegmentsStruct.has_value()) {
        for (auto& lineSegment : lineSegmentsStruct.value()) {
            lineSegment.setPartition(partition);
        }
    }

    if (spatialLocationStruct.has_value()) {
        spatialLocationStruct.value().setPartition(partition);
    }
}
} // namespace segysdk::metadata::osdu
