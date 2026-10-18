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

#include "../OsduMetaDataUtilities/OsduFileHistogram.h"

#include "../OsduMetaDataUtilities/OsduCommonMetadata.h"
#include "../OsduMetaDataUtilities/OsduFileHistogramData.h"

#include "../OsduMetaDataUtilities/OsduFileHistogramLiterals.h"

namespace segysdk::metadata::osdu
{
OsduFileHistogram::OsduFileHistogram(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    fileHistogramStruct_(OsduFileHistogramStruct(osduPartition))
{
    /* Empty */
}

OsduFileHistogram::OsduFileHistogram(OsduFileHistogramStruct fileHistogramStruct)
    : OsduMetadataBase(fileHistogramStruct.getPartition(), false), fileHistogramStruct_(std::move(fileHistogramStruct))
{
    populateJsonObject();
}

OsduFileHistogram::OsduFileHistogram(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), fileHistogramStruct_(OsduFileHistogramStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduFileHistogram::isValidInput() const
{
    return fileHistogramStruct_.isValid();
}

void
OsduFileHistogram::initializeJson()
{
    removeMember(OsduFileHistogramLiterals::Acl);
    removeMember(OsduFileHistogramLiterals::Ancestry);
    removeMember(OsduFileHistogramLiterals::CreateUser);
    removeMember(OsduFileHistogramLiterals::CreateTime);
    removeMember(OsduFileHistogramLiterals::Data);
    removeMember(OsduFileHistogramLiterals::Id);
    removeMember(OsduFileHistogramLiterals::Kind);
    removeMember(OsduFileHistogramLiterals::Legal);
    removeMember(OsduFileHistogramLiterals::Meta);
    removeMember(OsduFileHistogramLiterals::ModifyUser);
    removeMember(OsduFileHistogramLiterals::ModifyTime);
    removeMember(OsduFileHistogramLiterals::Tags);
    removeMember(OsduFileHistogramLiterals::Version);
}

void
OsduFileHistogram::populateJsonObject()
{
    clearJsonObject();

    setStructParameter<OsduCommonMetadata, OsduCommonMetadataStruct>("", fileHistogramStruct_.commonMetadataStruct);
    if (fileHistogramStruct_.dataStruct.has_value()) {
        Json::Value additionJson = Json::objectValue;
        additionJson[OsduFileHistogramLiterals::Data] = OsduFileHistogramData(fileHistogramStruct_.dataStruct.value()).getAsJson();
        mergeInJsonRecursively(additionJson);
    }
}

void
OsduFileHistogram::initializeMetadataStruct()
{
    // This bit should be done for composite structures to allow the partition to be propagated to sub-elements.
    if (isMember(OsduFileHistogramLiterals::Id)) {
        const auto partition = OsduFileHistogramStruct::getPartitionFromReference(getParameter<std::string>(OsduFileHistogramLiterals::Id));
        setOsduPartition(partition);
    }

    fileHistogramStruct_ = OsduFileHistogramStruct(getOsduPartition());
}

void
OsduFileHistogram::populateMetadataStruct()
{
    initializeMetadataStruct();

    fileHistogramStruct_.commonMetadataStruct = getStructParameter<OsduCommonMetadata, OsduCommonMetadataStruct>();
    fileHistogramStruct_.dataStruct = getOptionalStructParameter<OsduFileHistogramData, OsduFileHistogramDataStruct>(OsduFileHistogramLiterals::Data);
}

std::shared_ptr<OsduMetadataStructBase>
OsduFileHistogram::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduFileHistogramStruct>(fileHistogramStruct_);
}
} // namespace segysdk::metadata::osdu
