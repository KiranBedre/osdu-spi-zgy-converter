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

#include "../OsduMetaDataUtilities/OsduFileHistogramDataStruct.h"

namespace segysdk::metadata::osdu
{
OsduFileHistogramDataStruct::OsduFileHistogramDataStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

bool
OsduFileHistogramDataStruct::isValid() const
{
    return structValid(commonResourcesStruct) and structValid(datasetStruct) and structValid(fileStruct)
           and isValidStringRegex(contentSchemaKind, contentSchemaKindStringRegex_);
}

std::vector<std::string>
OsduFileHistogramDataStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }

    std::vector<std::string> results;
    results.emplace_back("OsduFileHistogramDataStruct::isValid() failed.");

    performStructValidityScan(results, commonResourcesStruct);
    performStructValidityScan(results, datasetStruct);
    performStructValidityScan(results, fileStruct);

    performStringRegexValidityScan(results, contentSchemaKind, "OsduFileHistogramDataStruct::contentSchemaKind", contentSchemaKindStringRegex_);

    return results;
}

void
OsduFileHistogramDataStruct::propagatePartition(const std::string& partition)
{
    if (commonResourcesStruct.has_value()) {
        commonResourcesStruct->setPartition(partition);
    }
    if (datasetStruct.has_value()) {
        datasetStruct->setPartition(partition);
    }
    if (fileStruct.has_value()) {
        fileStruct->setPartition(partition);
    }
}
} // namespace segysdk::metadata::osdu
