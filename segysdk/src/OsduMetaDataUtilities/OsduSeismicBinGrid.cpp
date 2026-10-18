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

#include "../OsduMetaDataUtilities/OsduSeismicBinGrid.h"

#include "../OsduMetaDataUtilities/OsduCommonMetadata.h"
#include "../OsduMetaDataUtilities/OsduSeismicBinGridData.h"

#include "../OsduMetaDataUtilities/OsduSeismicBinGridLiterals.h"

namespace segysdk::metadata::osdu
{
OsduSeismicBinGrid::OsduSeismicBinGrid(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    seismicBinGridStruct_(OsduSeismicBinGridStruct(osduPartition))
{
    /* Empty */
}

OsduSeismicBinGrid::OsduSeismicBinGrid(OsduSeismicBinGridStruct seismicBinGridStruct)
  : OsduMetadataBase(seismicBinGridStruct.getPartition()), seismicBinGridStruct_(std::move(seismicBinGridStruct))
{
    populateJsonObject();
}

OsduSeismicBinGrid::OsduSeismicBinGrid(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), seismicBinGridStruct_(OsduSeismicBinGridStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduSeismicBinGrid::isValidInput() const
{
    return seismicBinGridStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduSeismicBinGrid::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduSeismicBinGridStruct>(seismicBinGridStruct_);
}

void
OsduSeismicBinGrid::initializeJson()
{
    clearJsonObject();
}

void
OsduSeismicBinGrid::populateJsonObject()
{
    initializeJson();

    OsduCommonMetadata commonMetadata(seismicBinGridStruct_.commonMetadataStruct);
    mergeInJson(commonMetadata.getAsJson());

    if (seismicBinGridStruct_.dataStruct.has_value()) {
        Json::Value additionJson{Json::objectValue};
        additionJson[OsduSeismicBinGridLiterals::Data] = OsduSeismicBinGridData(seismicBinGridStruct_.dataStruct.value()).getAsJson();
        mergeInJsonRecursively(additionJson);
    }
}

void
OsduSeismicBinGrid::initializeMetadataStruct()
{
    // This bit should be done for composite structures to allow the partition to be propagated to sub-elements.
    if (isMember(OsduSeismicBinGridLiterals::Id)) {
        const auto partition = OsduSeismicBinGridStruct::getPartitionFromReference(getParameter<std::string>(OsduSeismicBinGridLiterals::Id));
        setOsduPartition(partition);
    }

    seismicBinGridStruct_ = OsduSeismicBinGridStruct(getOsduPartition());
}

void
OsduSeismicBinGrid::populateMetadataStruct()
{
    initializeMetadataStruct();

    seismicBinGridStruct_.commonMetadataStruct = getStructParameter<OsduCommonMetadata, OsduCommonMetadataStruct>();
    seismicBinGridStruct_.dataStruct = getOptionalStructParameter<OsduSeismicBinGridData, OsduSeismicBinGridDataStruct>(OsduSeismicBinGridLiterals::Data);
}

} // namespace segysdk::metadata::osdu
