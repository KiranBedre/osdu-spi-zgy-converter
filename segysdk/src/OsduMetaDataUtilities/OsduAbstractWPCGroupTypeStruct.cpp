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

#include "../OsduMetaDataUtilities/OsduAbstractWPCGroupTypeStruct.h"

#include <algorithm>

namespace segysdk::metadata::osdu
{
OsduAbstractWPCGroupTypeStruct::OsduAbstractWPCGroupTypeStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduAbstractWPCGroupTypeStruct::OsduAbstractWPCGroupTypeStruct(const OsduAbstractWPCGroupTypeStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    datasets(other.datasets),
    ddmsDatasets(other.ddmsDatasets),
    artefacts(other.artefacts),
    isExtendedLoad(other.isExtendedLoad),
    isDiscoverable(other.isDiscoverable),
    technicalAssurances(other.technicalAssurances),
    nameAliases(other.nameAliases)
{
    /* Empty */
}

OsduAbstractWPCGroupTypeStruct::OsduAbstractWPCGroupTypeStruct(OsduAbstractWPCGroupTypeStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    datasets(std::move(other.datasets)),
    ddmsDatasets(std::move(other.ddmsDatasets)),
    artefacts(std::move(other.artefacts)),
    isExtendedLoad(other.isExtendedLoad),
    isDiscoverable(other.isDiscoverable),
    technicalAssurances(std::move(other.technicalAssurances)),
    nameAliases(std::move(other.nameAliases))
{
    /* Empty */
}

OsduAbstractWPCGroupTypeStruct&
OsduAbstractWPCGroupTypeStruct::operator=(const OsduAbstractWPCGroupTypeStruct& rhs)
{
    return *this = OsduAbstractWPCGroupTypeStruct(rhs);
}

OsduAbstractWPCGroupTypeStruct&
OsduAbstractWPCGroupTypeStruct::operator=(OsduAbstractWPCGroupTypeStruct&& rhs) noexcept
{
    datasets = std::move(rhs.datasets);
    ddmsDatasets = std::move(rhs.ddmsDatasets);
    artefacts = std::move(rhs.artefacts);
    isExtendedLoad = rhs.isExtendedLoad;
    isDiscoverable = rhs.isDiscoverable;
    technicalAssurances = std::move(rhs.technicalAssurances);
    nameAliases = std::move(rhs.nameAliases);

    setPartition(rhs.getPartition());

    return *this;
}

bool
OsduAbstractWPCGroupTypeStruct::isValid() const
{
    const bool datasetOK = !datasets.has_value() or !datasets.value().empty();
    const bool ddmsDatasetOK = !ddmsDatasets.has_value() or !ddmsDatasets.value().empty();

    return datasetOK and ddmsDatasetOK;
}

std::vector<std::string>
OsduAbstractWPCGroupTypeStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;

    if (!isValid()) {
        results.emplace_back("AbstractWPCGroupTypeStruct::isValid() failed.");
    }

    if (datasets.has_value() and datasets.value().empty()) {
        results.emplace_back("AbstractWPCGroupTypeStruct: Datasets should not be empty.");
    }
    if (ddmsDatasets.has_value() and ddmsDatasets.value().empty()) {
        results.emplace_back("AbstractWPCGroupTypeStruct: ddmsDatasets should not be empty.");
    }

    return results;
}

void
OsduAbstractWPCGroupTypeStruct::propagatePartition(const std::string& partition)
{
    if (artefacts.has_value()) {
        for (auto& artefact : artefacts.value()) {
            artefact.setPartition(partition);
        }
    }
    if (technicalAssurances.has_value()) {
        for (auto& ta : technicalAssurances.value()) {
            ta.setPartition(partition);
        }
    }
    if (nameAliases.has_value()) {
        for (auto& nameAlias : nameAliases.value()) {
            nameAlias.setPartition(partition);
        }
    }
}

} // namespace segysdk::metadata::osdu
