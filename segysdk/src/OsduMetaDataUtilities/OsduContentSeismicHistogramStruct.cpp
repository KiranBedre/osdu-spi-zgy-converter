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

#include "../OsduMetaDataUtilities/OsduContentSeismicHistogramStruct.h"

namespace segysdk::metadata::osdu
{
OsduContentSeismicHistogramStruct::OsduContentSeismicHistogramStruct(std::string osduPartition)
    : OsduMetadataStructBase(osduPartition),
    abstractContentStruct(osduPartition)
{
    /* Empty */
}

bool
OsduContentSeismicHistogramStruct::isValid() const
{
    return structValid(abstractContentStruct) and isValidIdString(seismicRecordID, IdStringCheckType::Reference)
           and isValidIdString(amplitudeUnitOfMeasureID, IdStringCheckType::Reference) and arrayStructValid(histograms);
}

std::vector<std::string>
OsduContentSeismicHistogramStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduContentSeismicHistogramStruct::isValid() failed.");
    }

    performStructValidityScan(results, abstractContentStruct);
    performIdStringValidityScan(results, seismicRecordID, "OsduContentSeismicHistogramStruct:seismicRecordID", IdStringCheckType::Reference);
    performIdStringValidityScan(results, amplitudeUnitOfMeasureID, "OsduContentSeismicHistogramStruct:amplitudeUnitOfMeasureID", IdStringCheckType::Reference);
    performArrayStructValidityScan(results, histograms);

    return results;
}

void
OsduContentSeismicHistogramStruct::propagatePartition(const std::string& partition)
{
    abstractContentStruct.setPartition(partition);

    if (histograms.has_value() && !histograms.value().empty()) {
        for (auto& histogram : histograms.value()) {
            histogram.setPartition(partition);
        }
    }
}
} // namespace segysdk::metadata::osdu
