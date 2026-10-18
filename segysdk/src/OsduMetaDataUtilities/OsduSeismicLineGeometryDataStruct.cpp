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

#include "../OsduMetaDataUtilities/OsduSeismicLineGeometryDataStruct.h"

namespace segysdk::metadata::osdu
{
OsduSeismicLineGeometryDataStruct::OsduSeismicLineGeometryDataStruct(std::string osduPartition)
  : OsduMetadataStructBase(osduPartition)
{
    /* Empty */
}

bool
OsduSeismicLineGeometryDataStruct::isValid() const
{
    return structValid(commonResourcesStruct) and structValid(wpcGroupTypeStruct) and structValid(workProductComponentStruct)
           and isValidIdString(notionalSeismicLineID, IdStringCheckType::Reference)
           and isValidIdString(preferred2DInterpretationSetID, IdStringCheckType::Reference)
           and isValidIdString(horizontalCRSID, IdStringCheckType::CRS)
           and structValid(firstLocation) and structValid(lastLocation);
}

std::vector<std::string>
OsduSeismicLineGeometryDataStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduSeismicLineGeometryDataStruct::isValid() failed.");
    }

    performStructValidityScan(results, commonResourcesStruct);
    performStructValidityScan(results, wpcGroupTypeStruct);
    performStructValidityScan(results, workProductComponentStruct);

    performIdStringValidityScan(results, notionalSeismicLineID, "OsduSeismicLineGeometryDataStruct:NotionalSeismicLineID: ", IdStringCheckType::Reference);
    performIdStringValidityScan(results, preferred2DInterpretationSetID, "OsduSeismicLineGeometryDataStruct:Preferred2DInterpretationSetID: ", IdStringCheckType::Reference);
    performIdStringValidityScan(results, horizontalCRSID, "OsduSeismicLineGeometryDataStruct:HorizontalCRSID: ", IdStringCheckType::CRS);

    performStructValidityScan(results, firstLocation);
    performStructValidityScan(results, lastLocation);

    return results;
}

void
OsduSeismicLineGeometryDataStruct::propagatePartition(const std::string& partition)
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
    if (firstLocation.has_value()) {
        firstLocation.value().setPartition(partition);
    }
    if (lastLocation.has_value()) {
        lastLocation.value().setPartition(partition);
    }
}
} // namespace segysdk::metadata::osdu
