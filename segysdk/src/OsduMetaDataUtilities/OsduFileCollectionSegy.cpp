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

#include <utility>

#include "../OsduMetaDataUtilities/OsduFileCollectionSegy.h"

#include "../OsduMetaDataUtilities/OsduCommonMetadata.h"
#include "../OsduMetaDataUtilities/OsduFileCollectionSegyData.h"

#include "../OsduMetaDataUtilities/OsduFileCollectionSegyLiterals.h"

namespace segysdk::metadata::osdu
{
OsduFileCollectionSegy::OsduFileCollectionSegy(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    fileCollectionSegyStruct_(OsduFileCollectionSegyStruct(osduPartition))
{
    /* Empty */
}

OsduFileCollectionSegy::OsduFileCollectionSegy(OsduFileCollectionSegyStruct fileCollectionSegyStruct)
  : OsduMetadataBase(fileCollectionSegyStruct.getPartition(), false), fileCollectionSegyStruct_(std::move(fileCollectionSegyStruct))
{
    populateJsonObject();
}

OsduFileCollectionSegy::OsduFileCollectionSegy(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), fileCollectionSegyStruct_(OsduFileCollectionSegyStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduFileCollectionSegy::isValidInput() const
{
    return fileCollectionSegyStruct_.isValid();
}

void
OsduFileCollectionSegy::initializeJson()
{
    removeMember(OsduFileCollectionSegyLiterals::Acl);
    removeMember(OsduFileCollectionSegyLiterals::Ancestry);
    removeMember(OsduFileCollectionSegyLiterals::CreateUser);
    removeMember(OsduFileCollectionSegyLiterals::CreateTime);
    removeMember(OsduFileCollectionSegyLiterals::Data);
    removeMember(OsduFileCollectionSegyLiterals::Id);
    removeMember(OsduFileCollectionSegyLiterals::Kind);
    removeMember(OsduFileCollectionSegyLiterals::Legal);
    removeMember(OsduFileCollectionSegyLiterals::Meta);
    removeMember(OsduFileCollectionSegyLiterals::ModifyUser);
    removeMember(OsduFileCollectionSegyLiterals::ModifyTime);
    removeMember(OsduFileCollectionSegyLiterals::Tags);
    removeMember(OsduFileCollectionSegyLiterals::Version);
}

void
OsduFileCollectionSegy::populateJsonObject()
{
    initializeJson();

    setStructParameter<OsduCommonMetadata>("", fileCollectionSegyStruct_.commonMetadataStruct);
    if (fileCollectionSegyStruct_.dataStruct.has_value()) {
        Json::Value additionJson = Json::objectValue;
        additionJson[OsduFileCollectionSegyLiterals::Data] = OsduFileCollectionSegyData(fileCollectionSegyStruct_.dataStruct.value()).getAsJson();
        mergeInJsonRecursively(additionJson);
    }
}

void
OsduFileCollectionSegy::initializeMetadataStruct()
{
    // This bit should be done for composite structures to allow the partition to be propagated to sub-elements.
    if (isMember(OsduFileCollectionSegyLiterals::Id)) {
        const auto partition = OsduFileCollectionSegyStruct::getPartitionFromReference(getParameter<std::string>(OsduFileCollectionSegyLiterals::Id));
        setOsduPartition(partition);
    }

    fileCollectionSegyStruct_ = OsduFileCollectionSegyStruct(getOsduPartition());
}

void
OsduFileCollectionSegy::populateMetadataStruct()
{
    initializeMetadataStruct();

    fileCollectionSegyStruct_.commonMetadataStruct = getStructParameter<OsduCommonMetadata, OsduCommonMetadataStruct>();
    fileCollectionSegyStruct_.dataStruct = getOptionalStructParameter<OsduFileCollectionSegyData, OsduFileCollectionSegyDataStruct>(OsduFileCollectionSegyLiterals::Data);
}

std::shared_ptr<OsduMetadataStructBase>
OsduFileCollectionSegy::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduFileCollectionSegyStruct>(fileCollectionSegyStruct_);
}

} // namespace segysdk::metadata::osdu
