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

#include "../OsduMetaDataUtilities/OsduSeismic2DInterpretationSetDataStruct.h"

namespace segysdk::metadata::osdu
{
OsduSeismic2DInterpretationSetDataStruct::OsduSeismic2DInterpretationSetDataStruct(std::string osduPartition)
  : OsduMetadataStructBase(osduPartition)
{
    /* Empty */
}

bool
OsduSeismic2DInterpretationSetDataStruct::isValid() const
{
    return structValid(commonResourcesStruct) and structValid(masterStruct) and structValid(projectStruct)
           and structValid(activityStruct) and arrayStructValid(seismicLineGeometriesStruct);
}

std::vector<std::string>
OsduSeismic2DInterpretationSetDataStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduSeismic2DInterpretationSetDataStruct::isValid() failed.");
    }

    performStructValidityScan(results, commonResourcesStruct);
    performStructValidityScan(results, masterStruct);
    performStructValidityScan(results, projectStruct);
    performStructValidityScan(results, activityStruct);
    performArrayStructValidityScan(results, seismicLineGeometriesStruct);

    return results;
}

void
OsduSeismic2DInterpretationSetDataStruct::propagatePartition(const std::string& partition)
{
    if (commonResourcesStruct.has_value()) {
        commonResourcesStruct.value().setPartition(partition);
    }
    if (masterStruct.has_value()) {
        masterStruct.value().setPartition(partition);
    }
    if (projectStruct.has_value()) {
        projectStruct.value().setPartition(partition);
    }
    if (activityStruct.has_value()) {
        activityStruct.value().setPartition(partition);
    }
    if (seismicLineGeometriesStruct.has_value()) {
        for (auto& seismicLineGeometryRef : seismicLineGeometriesStruct.value()) {
            seismicLineGeometryRef.setPartition(partition);
        }
    }
}
} // namespace segysdk::metadata::osdu
