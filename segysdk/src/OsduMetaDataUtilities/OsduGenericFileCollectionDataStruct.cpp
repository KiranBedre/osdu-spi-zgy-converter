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

#include "../OsduMetaDataUtilities/OsduGenericFileCollectionDataStruct.h"


namespace segysdk::metadata::osdu
{
OsduGenericFileCollectionDataStruct::OsduGenericFileCollectionDataStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduGenericFileCollectionDataStruct::OsduGenericFileCollectionDataStruct(const OsduGenericFileCollectionDataStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    abstractCommonResourcesStruct(other.abstractCommonResourcesStruct),
    abstractDatasetStruct(other.abstractDatasetStruct),
    abstractFileCollectionStruct(other.abstractFileCollectionStruct),
    extensionPropertiesArrayStruct(other.extensionPropertiesArrayStruct)
{
    /* Empty */
}

OsduGenericFileCollectionDataStruct::OsduGenericFileCollectionDataStruct(OsduGenericFileCollectionDataStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    abstractCommonResourcesStruct(std::move(other.abstractCommonResourcesStruct)),
    abstractDatasetStruct(std::move(other.abstractDatasetStruct)),
    abstractFileCollectionStruct(std::move(other.abstractFileCollectionStruct)),
    extensionPropertiesArrayStruct(std::move(other.extensionPropertiesArrayStruct))
{
    /* Empty */
}

OsduGenericFileCollectionDataStruct&
OsduGenericFileCollectionDataStruct::operator=(const OsduGenericFileCollectionDataStruct& rhs)
{
    return *this = OsduGenericFileCollectionDataStruct(rhs);
}

OsduGenericFileCollectionDataStruct&
OsduGenericFileCollectionDataStruct::operator=(OsduGenericFileCollectionDataStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    abstractCommonResourcesStruct = std::move(rhs.abstractCommonResourcesStruct);
    abstractDatasetStruct = std::move(rhs.abstractDatasetStruct);
    abstractFileCollectionStruct = std::move(rhs.abstractFileCollectionStruct);
    extensionPropertiesArrayStruct = std::move(rhs.extensionPropertiesArrayStruct);

    return *this;
}

bool
OsduGenericFileCollectionDataStruct::isValid() const
{
    return  allNodesValid();
}

bool
OsduGenericFileCollectionDataStruct::abstractDatasetValid() const
{
    return !abstractDatasetStruct.has_value() or abstractDatasetStruct.value().isValid();
}

bool
OsduGenericFileCollectionDataStruct::abstractFileCollectionValid() const
{
    return !abstractFileCollectionStruct.has_value() or abstractFileCollectionStruct.value().isValid();
}

bool
OsduGenericFileCollectionDataStruct::extensionPropertiesArrayValid() const
{
    return !extensionPropertiesArrayStruct.has_value() or extensionPropertiesArrayStruct.value().isValid();
}

bool
OsduGenericFileCollectionDataStruct::abstractCommonResourcesValid() const
{
    return !abstractCommonResourcesStruct.has_value() or abstractCommonResourcesStruct.value().isValid();
}

bool
OsduGenericFileCollectionDataStruct::allNodesValid() const
{
    return abstractDatasetValid() and abstractFileCollectionValid() and extensionPropertiesArrayValid() and abstractCommonResourcesValid();
}

std::vector<std::string>
OsduGenericFileCollectionDataStruct::performAttributeValidityScan() const
{
    if (isValid()) {
        return {};
    }

    std::vector<std::string> results{"OsduGenericFileCollectionDataStruct::isValid() failed."};
    if (!allNodesValid()) {
        if (!abstractDatasetValid() and abstractDatasetStruct.has_value()) {
            appendStringVector(results, abstractDatasetStruct.value().performAttributeValidityScan());
        }

        if (!abstractFileCollectionValid() and abstractFileCollectionStruct.has_value()) {
            appendStringVector(results, abstractFileCollectionStruct.value().performAttributeValidityScan());
        }
        if (!extensionPropertiesArrayValid() and extensionPropertiesArrayStruct.has_value()) {
            appendStringVector(results, extensionPropertiesArrayStruct.value().performAttributeValidityScan());
        }
        if (!abstractCommonResourcesValid() and abstractCommonResourcesStruct.has_value()) {
            appendStringVector(results, abstractCommonResourcesStruct.value().performAttributeValidityScan());
        }
    }

    return results;
}

void
OsduGenericFileCollectionDataStruct::propagatePartition(const std::string& partition)
{
    if (abstractCommonResourcesStruct.has_value()) {
        abstractCommonResourcesStruct.value().setPartition(partition);
    }
    if (abstractDatasetStruct.has_value()) {
        abstractDatasetStruct.value().setPartition(partition);
    }
    if (abstractFileCollectionStruct.has_value()) {
        abstractFileCollectionStruct.value().setPartition(partition);
    }
    if (extensionPropertiesArrayStruct.has_value()) {
        extensionPropertiesArrayStruct.value().setPartition(partition);
    }
}

} // namespace segysdk::metadata::osdu
