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

#include "../OsduMetaDataUtilities/OsduAbstractDatasetCollectionStruct.h"

namespace segysdk::metadata::osdu
{
OsduAbstractDatasetCollectionStruct::OsduAbstractDatasetCollectionStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition)),
    helper_(getPartition())
{
    /* Empty */
}

OsduAbstractDatasetCollectionStruct::OsduAbstractDatasetCollectionStruct(const OsduAbstractDatasetCollectionStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    genericFileDataStruct(other.genericFileDataStruct),
    datasetCollectionPropertiesStruct(other.datasetCollectionPropertiesStruct),
    vectorHeaderMappingStruct(other.vectorHeaderMappingStruct),
    binaryHeaderOverridesStruct(other.binaryHeaderOverridesStruct),
    traceHeaderOverridesStruct(other.traceHeaderOverridesStruct)
{
    /* Empty */
}

OsduAbstractDatasetCollectionStruct::OsduAbstractDatasetCollectionStruct(OsduAbstractDatasetCollectionStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    genericFileDataStruct(std::move(other.genericFileDataStruct)),
    datasetCollectionPropertiesStruct(std::move(other.datasetCollectionPropertiesStruct)),
    vectorHeaderMappingStruct(std::move(other.vectorHeaderMappingStruct)),
    binaryHeaderOverridesStruct(std::move(other.binaryHeaderOverridesStruct)),
    traceHeaderOverridesStruct(std::move(other.traceHeaderOverridesStruct))
{
    /* Empty */
}

OsduAbstractDatasetCollectionStruct&
OsduAbstractDatasetCollectionStruct::operator=(const OsduAbstractDatasetCollectionStruct& rhs)
{
    return *this = OsduAbstractDatasetCollectionStruct(rhs);
}

OsduAbstractDatasetCollectionStruct&
OsduAbstractDatasetCollectionStruct::operator=(OsduAbstractDatasetCollectionStruct&& rhs) noexcept
{
    genericFileDataStruct = std::move(rhs.genericFileDataStruct);
    datasetCollectionPropertiesStruct = std::move(rhs.datasetCollectionPropertiesStruct);
    vectorHeaderMappingStruct = std::move(rhs.vectorHeaderMappingStruct);
    binaryHeaderOverridesStruct = std::move(rhs.binaryHeaderOverridesStruct);
    traceHeaderOverridesStruct = std:;move(rhs.traceHeaderOverridesStruct);
    helper_ = std::move(helper_);
    setPartition(rhs.getPartition());

    return *this;
}

bool
OsduAbstractDatasetCollectionStruct::genericFileDataValid() const
{
    return !genericFileDataStruct.has_value() ? true : genericFileDataStruct->isValid();
}

bool
OsduAbstractDatasetCollectionStruct::datasetPropertiesValid() const
{
    return !datasetCollectionPropertiesStruct.has_value() ? true : datasetCollectionPropertiesStruct.value().isValid();
}

bool
OsduAbstractDatasetCollectionStruct::vectorHeaderMappingValid() const
{
    return !vectorHeaderMappingStruct.has_value() ? true : vectorHeaderMappingStruct.value().isValid();
}

bool
OsduAbstractDatasetCollectionStruct::binaryHeaderOverridesValid() const
{
    return !binaryHeaderOverridesStruct.has_value() ? true : binaryHeaderOverridesStruct.value().isValid();
}

bool
OsduAbstractDatasetCollectionStruct::traceHeaderOverridesValid() const
{
    return !traceHeaderOverridesStruct.has_value() ? true : traceHeaderOverridesStruct.value().isValid();
}


bool
OsduAbstractDatasetCollectionStruct::allPropertiesValid() const
{
    return genericFileDataValid() and datasetPropertiesValid() and  vectorHeaderMappingValid() and binaryHeaderOverridesValid() and traceHeaderOverridesValid();
}


bool
OsduAbstractDatasetCollectionStruct::isValid() const
{
    return allPropertiesValid();
}

std::vector<std::string>
OsduAbstractDatasetCollectionStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (isValid()) {
        return results;    
    }

    results.emplace_back("OsduAbstractDatasetCollectionStruct::isValid() failed.");

    if (!datasetPropertiesValid()) {
        results.emplace_back("DatasetProperties test failed.");
    }
    if (!genericFileDataValid()) {
        results.emplace_back("genericFileDataValid test failed.");
    }
    if (!vectorHeaderMappingValid()) {
        results.emplace_back("VectorHeaderMapping test failed.");
    }
    if (!binaryHeaderOverridesValid()) {
        results.emplace_back("BinaryheaderOverrides test failed.");
    }
    if (!traceHeaderOverridesValid()) {
        results.emplace_back("TraceheaderOverrides test failed.");
    }
    return results;
}

void
OsduAbstractDatasetCollectionStruct::propagatePartition(const std::string& partition)
{
    if (datasetCollectionPropertiesStruct.has_value()) {
        datasetCollectionPropertiesStruct.value().setPartition(partition);
    }
    if (genericFileDataStruct.has_value()) {
        genericFileDataStruct.value().setPartition(partition);
    }
    if (vectorHeaderMappingStruct.has_value()) {
        vectorHeaderMappingStruct.value().setPartition(partition);
    }
    if (binaryHeaderOverridesStruct.has_value()) {
        binaryHeaderOverridesStruct.value().setPartition(partition);
    }
    if (traceHeaderOverridesStruct.has_value()) {
        traceHeaderOverridesStruct.value().setPartition(partition);
    }
}

} // namespace segysdk::metadata::osdu
