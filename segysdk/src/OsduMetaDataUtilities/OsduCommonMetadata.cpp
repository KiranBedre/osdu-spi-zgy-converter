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

#include <utility>

#include "../OsduMetaDataUtilities/OsduCommonMetadata.h"

#include "../OsduMetaDataUtilities/OsduAcl.h"
#include "../OsduMetaDataUtilities/OsduLegal.h"
#include "../OsduMetaDataUtilities/OsduTagArray.h"
#include "../OsduMetaDataUtilities/OsduAncestry.h"
#include "../OsduMetaDataUtilities/OsduMetaArray.h"
#include "../OsduMetaDataUtilities/OsduCommonMetadataData.h"

#include "../OsduMetaDataUtilities/OsduCommonMetadataLiterals.h"

namespace segysdk::metadata::osdu
{
OsduCommonMetadata::OsduCommonMetadata(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    commonMetadataStruct_(OsduCommonMetadataStruct(osduPartition)) {}

OsduCommonMetadata::OsduCommonMetadata(OsduCommonMetadataStruct commonMetadataStruct)
  : OsduMetadataBase(commonMetadataStruct.getPartition()), commonMetadataStruct_(std::move(commonMetadataStruct))
{
    populateJsonObject();
}

OsduCommonMetadata::OsduCommonMetadata(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), commonMetadataStruct_(OsduCommonMetadataStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduCommonMetadata::isValidInput() const
{
    return commonMetadataStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduCommonMetadata::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduCommonMetadataStruct>(commonMetadataStruct_);
}

void
OsduCommonMetadata::initializeJson()
{
    clearJsonObject();
}

void
OsduCommonMetadata::populateJsonObject()
{
    clearJsonObject();

    setOptionalParameter(OsduCommonMetadataLiterals::Id, commonMetadataStruct_.id);
    setParameter(OsduCommonMetadataLiterals::Kind, commonMetadataStruct_.kind);
    setOptionalParameter(OsduCommonMetadataLiterals::Version, commonMetadataStruct_.version);

    const auto acl = OsduAcl(commonMetadataStruct_.aclStruct);
    setParameter(OsduCommonMetadataLiterals::Acl, acl.getAsJson());

    const auto legal = OsduLegal(commonMetadataStruct_.legalStruct);
    setParameter(OsduCommonMetadataLiterals::Legal, legal.getAsJson());

    if (commonMetadataStruct_.tagsStruct.has_value()) {
        const auto tags = OsduTagArray(commonMetadataStruct_.tagsStruct.value());
        setParameter(OsduCommonMetadataLiterals::Tags, tags.getAsJson());
    }

    setOptionalParameter(OsduCommonMetadataLiterals::CreateTime, commonMetadataStruct_.createTime);
    setOptionalParameter(OsduCommonMetadataLiterals::CreateUser, commonMetadataStruct_.createUser);
    setOptionalParameter(OsduCommonMetadataLiterals::ModifyTime, commonMetadataStruct_.modifyTime);
    setOptionalParameter(OsduCommonMetadataLiterals::ModifyUser, commonMetadataStruct_.modifyUser);

    if (commonMetadataStruct_.ancestryStruct.has_value()) {
        const auto ancestry = OsduAncestry(commonMetadataStruct_.ancestryStruct.value());
        setParameter(OsduCommonMetadataLiterals::Ancestry, ancestry.getAsJson());
    }

    const auto meta = OsduMetaArray(commonMetadataStruct_.metaStruct);
    setParameter(OsduCommonMetadataLiterals::Meta, meta.getAsJson());

    if (commonMetadataStruct_.dataStruct.has_value()) {
        const auto data = OsduCommonMetadataData(commonMetadataStruct_.dataStruct.value());
        setParameter(OsduCommonMetadataLiterals::Data, data.getAsJson());
    }
}

void
OsduCommonMetadata::initializeMetadataStruct()
{
    // This bit should be done for composite structures to allow the partition to be propagated to sub-elements.
    if (isMember(OsduCommonMetadataLiterals::Id)) {
        const auto partition = OsduCommonMetadataStruct::getPartitionFromReference(getParameter<std::string>(OsduCommonMetadataLiterals::Id));
        setOsduPartition(partition);
    }

    commonMetadataStruct_ = OsduCommonMetadataStruct(getOsduPartition());
}

void
OsduCommonMetadata::populateMetadataStruct()
{
    initializeMetadataStruct();

    commonMetadataStruct_.id = getOptionalParameter<std::string>(OsduCommonMetadataLiterals::Id);
    commonMetadataStruct_.kind = getParameter<std::string>(OsduCommonMetadataLiterals::Kind);
    commonMetadataStruct_.version = getOptionalParameter<int64_t>(OsduCommonMetadataLiterals::Version);

    commonMetadataStruct_.aclStruct = getStructParameter<OsduAcl, OsduAclStruct>(OsduCommonMetadataLiterals::Acl);
    commonMetadataStruct_.legalStruct = getStructParameter<OsduLegal, OsduLegalStruct>(OsduCommonMetadataLiterals::Legal);
    commonMetadataStruct_.tagsStruct = getOptionalStructParameter<OsduTagArray, OsduTagArrayStruct>(OsduCommonMetadataLiterals::Tags);

    commonMetadataStruct_.createUser = getOptionalParameter<std::string>(OsduCommonMetadataLiterals::CreateUser);
    commonMetadataStruct_.createTime = getOptionalParameter<std::string>(OsduCommonMetadataLiterals::CreateTime);
    commonMetadataStruct_.modifyUser = getOptionalParameter<std::string>(OsduCommonMetadataLiterals::ModifyUser);
    commonMetadataStruct_.modifyTime = getOptionalParameter<std::string>(OsduCommonMetadataLiterals::ModifyTime);

    commonMetadataStruct_.ancestryStruct = getOptionalStructParameter<OsduAncestry, OsduAncestryStruct>(OsduCommonMetadataLiterals::Ancestry);
    commonMetadataStruct_.metaStruct = getStructParameter<OsduMetaArray, OsduMetaArrayStruct>(OsduCommonMetadataLiterals::Meta);
    commonMetadataStruct_.dataStruct = getOptionalStructParameter<OsduCommonMetadataData, OsduCommonMetadataDataStruct>(OsduCommonMetadataLiterals::Data);
}
} // namespace segysdk::metadata::osdu
