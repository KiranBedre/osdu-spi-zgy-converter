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

#include "../OsduMetaDataUtilities/OsduGenericFileDataStruct.h"


namespace segysdk::metadata::osdu
{
OsduGenericFileDataStruct::OsduGenericFileDataStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduGenericFileDataStruct::OsduGenericFileDataStruct(const OsduGenericFileDataStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    abstractCommonResourcesStruct(other.abstractCommonResourcesStruct),
    abstractDatasetStruct(other.abstractDatasetStruct),
    abstractFileStruct(other.abstractFileStruct),
    extensionPropertiesArrayStruct(other.extensionPropertiesArrayStruct)
{
    /* Empty */
}

OsduGenericFileDataStruct::OsduGenericFileDataStruct(OsduGenericFileDataStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    abstractCommonResourcesStruct(std::move(other.abstractCommonResourcesStruct)),
    abstractDatasetStruct(std::move(other.abstractDatasetStruct)),
    abstractFileStruct(std::move(other.abstractFileStruct)),
    extensionPropertiesArrayStruct(std::move(other.extensionPropertiesArrayStruct))
{
    /* Empty */
}

OsduGenericFileDataStruct&
OsduGenericFileDataStruct::operator=(const OsduGenericFileDataStruct& rhs)
{
    return *this = OsduGenericFileDataStruct(rhs);
}

OsduGenericFileDataStruct&
OsduGenericFileDataStruct::operator=(OsduGenericFileDataStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    abstractCommonResourcesStruct = std::move(rhs.abstractCommonResourcesStruct);
    abstractDatasetStruct = std::move(rhs.abstractDatasetStruct);
    abstractFileStruct = std::move(rhs.abstractFileStruct);
    extensionPropertiesArrayStruct = std::move(rhs.extensionPropertiesArrayStruct);

    return *this;
}

bool
OsduGenericFileDataStruct::isValid() const
{
    return  allNodesValid();
}

bool
OsduGenericFileDataStruct::abstractDatasetValid() const
{
    return (!abstractDatasetStruct.has_value() or abstractDatasetStruct.value().isValid());
}

bool
OsduGenericFileDataStruct::abstractFileValid() const
{
    return (!abstractFileStruct.has_value() or abstractFileStruct.value().isValid());
}

bool
OsduGenericFileDataStruct::abstractCommonResourcesValid() const
{
    return (!abstractCommonResourcesStruct.has_value() or abstractCommonResourcesStruct.value().isValid());
}

bool
OsduGenericFileDataStruct::extensionPropertiesArrayValid() const
{
    return (!extensionPropertiesArrayStruct.has_value() or extensionPropertiesArrayStruct.value().isValid());
}

bool
OsduGenericFileDataStruct::allNodesValid() const
{
    return abstractDatasetValid() and abstractFileValid() and abstractCommonResourcesValid() and extensionPropertiesArrayValid();
}

std::vector<std::string>
OsduGenericFileDataStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduGenericFileDataStruct::isValid() failed.");
    }
    std::string allNodesTest = allNodesValid() ? "" : "OsduGenericFileDataStruct node not valid";

    if (!allNodesTest.empty()) {
        results.emplace_back(allNodesTest);
    }
    return results;
}

void
OsduGenericFileDataStruct::propagatePartition(const std::string& partition)
{
    if (abstractCommonResourcesStruct.has_value()) {
        abstractCommonResourcesStruct.value().setPartition(partition);
    }
    if (abstractDatasetStruct.has_value()) {
        abstractDatasetStruct.value().setPartition(partition);
    }
    if (abstractFileStruct.has_value()) {
        abstractFileStruct.value().setPartition(partition);
    }
    if (extensionPropertiesArrayStruct.has_value()) {
        extensionPropertiesArrayStruct.value().setPartition(partition);
    }
}

} // namespace segysdk::metadata::osdu
