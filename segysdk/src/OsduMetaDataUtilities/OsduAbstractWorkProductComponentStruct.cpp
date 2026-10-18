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

#include "../OsduMetaDataUtilities/OsduAbstractWorkProductComponentStruct.h"


namespace segysdk::metadata::osdu
{
OsduAbstractWorkProductComponentStruct::OsduAbstractWorkProductComponentStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduAbstractWorkProductComponentStruct::OsduAbstractWorkProductComponentStruct(const OsduAbstractWorkProductComponentStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    name(other.name),
    description(other.description),
    creationDateTime(other.creationDateTime),
    tags(other.tags),
    spatialPointStruct(other.spatialPointStruct),
    spatialAreaStruct(other.spatialAreaStruct),
    geoContextsArray(other.geoContextsArray),
    submitterName(other.submitterName),
    authorIDs(other.authorIDs),
    businessActivities(other.businessActivities),
    lineageAssertions(other.lineageAssertions)
{
    /* Empty */
}

OsduAbstractWorkProductComponentStruct::OsduAbstractWorkProductComponentStruct(OsduAbstractWorkProductComponentStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    name(std::move(other.name)),
    description(std::move(other.description)),
    creationDateTime(std::move(other.creationDateTime)),
    tags(std::move(other.tags)),
    spatialPointStruct(std::move(other.spatialPointStruct)),
    spatialAreaStruct(std::move(other.spatialAreaStruct)),
    geoContextsArray(std::move(other.geoContextsArray)),
    submitterName(std::move(other.submitterName)),
    authorIDs(std::move(other.authorIDs)),
    businessActivities(std::move(other.businessActivities)),
    lineageAssertions(std::move(other.lineageAssertions))
{
    /* Empty */
}

OsduAbstractWorkProductComponentStruct&
OsduAbstractWorkProductComponentStruct::operator=(const OsduAbstractWorkProductComponentStruct& rhs)
{
    return *this = OsduAbstractWorkProductComponentStruct(rhs);
}

OsduAbstractWorkProductComponentStruct&
OsduAbstractWorkProductComponentStruct::operator=(OsduAbstractWorkProductComponentStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    name = std::move(rhs.name);
    description = std::move(rhs.description);
    creationDateTime = std::move(rhs.creationDateTime);
    tags = std::move(rhs.tags);
    spatialPointStruct = std::move(rhs.spatialPointStruct);
    spatialAreaStruct = std::move(rhs.spatialAreaStruct);
    geoContextsArray = std::move(rhs.geoContextsArray);
    submitterName = std::move(rhs.submitterName);
    authorIDs = std::move(rhs.authorIDs);
    businessActivities = std::move(rhs.businessActivities);
    lineageAssertions = std::move(rhs.lineageAssertions);
    return *this;
}

bool
OsduAbstractWorkProductComponentStruct::isValid() const
{
    return spatialPointStructValid() && spatialAreaStructValid() && geoContextsArrayValid() && lineageAssertionsValid();
}

bool
OsduAbstractWorkProductComponentStruct::spatialPointStructValid() const
{
    return !spatialPointStruct.has_value() or spatialPointStruct.value().isValid();
}

bool
OsduAbstractWorkProductComponentStruct::spatialAreaStructValid() const
{
    return !spatialAreaStruct.has_value() or spatialAreaStruct.value().isValid();
}

bool
OsduAbstractWorkProductComponentStruct::geoContextsArrayValid() const
{
    return !geoContextsArray.has_value() or std::all_of(geoContextsArray.value().cbegin(), geoContextsArray.value().cend(), [](const OsduAbstractGeoContextStruct& x) { return x.isValid(); });
}

bool
OsduAbstractWorkProductComponentStruct::lineageAssertionsValid() const
{
    return !lineageAssertions.has_value() or std::all_of(lineageAssertions.value().cbegin(), lineageAssertions.value().cend(), [](const OsduLineageAssertionStruct& x) { return x.isValid(); });
}

std::vector<std::string>
OsduAbstractWorkProductComponentStruct::performAttributeValidityScan() const
{
   
    if (isValid()) {
        return {};
    }

    std::vector<std::string> results;
    results.emplace_back("OsduAbstractWorkProductComponentStruct::isValid() failed.");

    if (spatialPointStruct.has_value()  and !spatialPointStructValid()) {
        results.emplace_back("OsduAbstractWorkProductComponentStruct::spatialPointStructValid() failed.");
        appendStringVector(results, spatialPointStruct.value().performAttributeValidityScan());
    }
    if (!spatialAreaStructValid()) {
        results.emplace_back("OsduAbstractWorkProductComponentStruct::spatialAreaStructValid() failed.");
        if (spatialAreaStruct.has_value()) {
            appendStringVector(results, spatialAreaStruct.value().performAttributeValidityScan());
        }
    }
    if (!geoContextsArrayValid()) {
        results.emplace_back("OsduAbstractWorkProductComponentStruct::geoContextsArrayValid() failed.");
        if (geoContextsArray.has_value()) {
            for (const auto& gc : geoContextsArray.value()) {
                if (!gc.isValid()) {
                    appendStringVector(results, gc.performAttributeValidityScan());
                }
            }
        }
    }
    if (!lineageAssertionsValid()) {
        results.emplace_back("OsduAbstractWorkProductComponentStruct::lineageAssertionsValid() failed.");
        if (lineageAssertions.has_value()) {
            for (const auto& la : lineageAssertions.value()) {
                if (!la.isValid()) {
                    appendStringVector(results, la.performAttributeValidityScan());
                }
            }
        }
    }
    return results;
}

void
OsduAbstractWorkProductComponentStruct::propagatePartition(const std::string& partition)
{
    if (spatialPointStruct.has_value()) {
        spatialPointStruct.value().setPartition(partition);
    }
    if (spatialAreaStruct.has_value()) {
        spatialAreaStruct.value().setPartition(partition);
    }
    if (geoContextsArray.has_value()) {
        for (auto& geoctxt : geoContextsArray.value()) {
            geoctxt.setPartition(partition);
        }
    }
    if (lineageAssertions.has_value()) {
        for (auto& lineage : lineageAssertions.value()) {
            lineage.setPartition(partition);
        }
    }
}

} // namespace segysdk::metadata::osdu
