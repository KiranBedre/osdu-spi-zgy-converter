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

#include "../OsduMetaDataUtilities/OsduLineSegmentStruct.h"

namespace segysdk::metadata::osdu
{
OsduLineSegmentStruct::OsduLineSegmentStruct(std::string osduPartition)
  : OsduMetadataStructBase(osduPartition)
{
    /* Empty */
}

bool
OsduLineSegmentStruct::isValid() const
{
    return structValid(shotPointRangeStruct) and structValid(commonMidPointRangeStruct)
           and structValid(shotPointCommonMidPointRatioStruct) and structValid(genericLabelsStruct);
}

std::vector<std::string>
OsduLineSegmentStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduLineSegmentStruct::isValid() failed.");
    }

    performStructValidityScan(results, shotPointRangeStruct);
    performStructValidityScan(results, commonMidPointRangeStruct);
    performStructValidityScan(results, shotPointCommonMidPointRatioStruct);
    performStructValidityScan(results, genericLabelsStruct);

    return results;
}

void
OsduLineSegmentStruct::propagatePartition(const std::string& partition)
{
    if (shotPointRangeStruct.has_value()) {
        shotPointRangeStruct.value().setPartition(partition);
    }

    if (commonMidPointRangeStruct.has_value()) {
        commonMidPointRangeStruct.value().setPartition(partition);
    }

    if (shotPointCommonMidPointRatioStruct.has_value()) {
        shotPointCommonMidPointRatioStruct.value().setPartition(partition);
    }

    if (genericLabelsStruct.has_value()) {
        genericLabelsStruct.value().setPartition(partition);
    }
}
} // namespace segysdk::metadata::osdu
