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

#include "../OsduMetaDataUtilities/OsduSeismicTraceData.h"

#include "../OsduMetaDataUtilities/OsduCommonMetadata.h"
#include "../OsduMetaDataUtilities/OsduSeismicTraceDataSpecificProperties.h"

#include "../OsduMetaDataUtilities/OsduSeismicTraceDataLiterals.h"

namespace segysdk::metadata::osdu
{
OsduSeismicTraceData::OsduSeismicTraceData(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    seismicTraceDataStruct_(OsduSeismicTraceDataStruct(osduPartition))
{
    /* Empty */
}

OsduSeismicTraceData::OsduSeismicTraceData(OsduSeismicTraceDataStruct seismicTraceDataStruct)
    : OsduMetadataBase(seismicTraceDataStruct.getPartition()), seismicTraceDataStruct_(std::move(seismicTraceDataStruct))
{
    populateJsonObject();
}

OsduSeismicTraceData::OsduSeismicTraceData(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), seismicTraceDataStruct_(OsduSeismicTraceDataStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduSeismicTraceData::isValidInput() const
{
    return seismicTraceDataStruct_.isValid();
}

void
OsduSeismicTraceData::initializeJson()
{
    removeMember(OsduSeismicTraceDataLiterals::Acl);
    removeMember(OsduSeismicTraceDataLiterals::Ancestry);
    removeMember(OsduSeismicTraceDataLiterals::CreateUser);
    removeMember(OsduSeismicTraceDataLiterals::CreateTime);
    removeMember(OsduSeismicTraceDataLiterals::Data);
    removeMember(OsduSeismicTraceDataLiterals::Id);
    removeMember(OsduSeismicTraceDataLiterals::Kind);
    removeMember(OsduSeismicTraceDataLiterals::Legal);
    removeMember(OsduSeismicTraceDataLiterals::Meta);
    removeMember(OsduSeismicTraceDataLiterals::ModifyUser);
    removeMember(OsduSeismicTraceDataLiterals::ModifyTime);
    removeMember(OsduSeismicTraceDataLiterals::Tags);
    removeMember(OsduSeismicTraceDataLiterals::Version);
}

void
OsduSeismicTraceData::populateJsonObject()
{
    initializeJson();

    setStructParameter<OsduCommonMetadata>("", seismicTraceDataStruct_.commonMetadata);

    if (seismicTraceDataStruct_.seismicTraceDataSpecificProperties.has_value()) {
        Json::Value additionJson{Json::objectValue};
        additionJson[OsduSeismicTraceDataLiterals::Data] = OsduSeismicTraceDataSpecificProperties(seismicTraceDataStruct_.seismicTraceDataSpecificProperties.value()).getAsJson();
        mergeInJsonRecursively(additionJson);
    }
}

void
OsduSeismicTraceData::initializeMetadataStruct()
{
    // This bit should be done for composite structures to allow the partition to be propagated to sub-elements.
    if (isMember(OsduSeismicTraceDataLiterals::Id)) {
        const auto partition = OsduSeismicTraceDataStruct::getPartitionFromReference(getParameter<std::string>(OsduSeismicTraceDataLiterals::Id));
        setOsduPartition(partition);
    }

    seismicTraceDataStruct_ = OsduSeismicTraceDataStruct(getOsduPartition());
}

void
OsduSeismicTraceData::populateMetadataStruct()
{
    initializeMetadataStruct();

    seismicTraceDataStruct_.commonMetadata = getStructParameter<OsduCommonMetadata, OsduCommonMetadataStruct>();
    seismicTraceDataStruct_.seismicTraceDataSpecificProperties = getOptionalStructParameter<OsduSeismicTraceDataSpecificProperties, OsduSeismicTraceDataSpecificPropertiesStruct>(OsduSeismicTraceDataLiterals::Data);
}

std::shared_ptr<OsduMetadataStructBase>
OsduSeismicTraceData::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduSeismicTraceDataStruct>(seismicTraceDataStruct_);
}
} // namespace segysdk::metadata::osdu
