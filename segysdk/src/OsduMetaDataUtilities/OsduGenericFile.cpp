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

#include "../OsduMetaDataUtilities/OsduGenericFile.h"
#include "../OsduMetaDataUtilities/OsduAcl.h"
#include "../OsduMetaDataUtilities/OsduLegal.h"
#include "../OsduMetaDataUtilities/OsduMetaArray.h"

#include "../OsduMetaDataUtilities/OsduGenericFileStruct.h"
#include "../OsduMetaDataUtilities/OsduGenericFileLiterals.h"

namespace segysdk::metadata::osdu
{
OsduGenericFile::OsduGenericFile(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), genericFileStruct_((OsduGenericFileStruct(osduPartition))) {}

OsduGenericFile::OsduGenericFile(const OsduGenericFile& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      genericFileStruct_(other.genericFileStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGenericFile::OsduGenericFile(OsduGenericFile&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      genericFileStruct_(std::move(other.genericFileStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGenericFile&
OsduGenericFile::operator=(const OsduGenericFile& rhs)
{
    return *this = OsduGenericFile(rhs);
}

OsduGenericFile&
OsduGenericFile::operator=(OsduGenericFile&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    genericFileStruct_ = std::move(rhs.genericFileStruct_);

    return *this;
}

OsduGenericFile::OsduGenericFile(OsduGenericFileStruct genericStruct)
    : OsduMetadataBase(genericStruct.getPartition(), false),
      genericFileStruct_(std::move(genericStruct))
{
    populateJsonObject();
}

OsduGenericFile::OsduGenericFile(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), genericFileStruct_(OsduGenericFileStruct(osduPartition))
{
    populateMetadataStruct();
}

/* Not optional */
OsduAclStruct
OsduGenericFile::getAclStruct() const
{
    OsduAcl acl(getParameter<Json::Value>(OsduGenericFileLiterals::Acl), getOsduPartition());
    const auto acl_structPtr = std::reinterpret_pointer_cast<OsduAclStruct>(acl.getMetaDataStruct());
    if (acl_structPtr != nullptr) {
        return *acl_structPtr;
    }

    return OsduAclStruct{getOsduPartition()};
}

/* Not optional */
OsduLegalStruct
OsduGenericFile::getLegalStruct() const
{
    OsduLegal legal(getParameter<Json::Value>(OsduGenericFileLiterals::Legal), getOsduPartition());
    const auto legal_structPtr = std::reinterpret_pointer_cast<OsduLegalStruct>(legal.getMetaDataStruct());
    if (legal_structPtr != nullptr) {
        return *legal_structPtr;
    }

    return OsduLegalStruct{getOsduPartition()};
}

std::optional<OsduAncestryStruct>
OsduGenericFile::getAncestryStruct() const
{
    const auto ancestryJson = getOptionalParameter<Json::Value>(OsduGenericFileLiterals::Ancestry);

    if (ancestryJson.has_value()) {
        auto ancestry = OsduAncestry(ancestryJson.value(), getOsduPartition());
        const auto legal_structPtr = std::reinterpret_pointer_cast<OsduAncestryStruct>(ancestry.getMetaDataStruct());
        if (legal_structPtr != nullptr) {
            return *legal_structPtr;
        }
    }

    return std::nullopt;
}

std::optional<OsduGenericFileDataStruct>
OsduGenericFile::getGenericFileDataStruct() const
{
    const auto dataJson = getOptionalParameter<Json::Value>(OsduGenericFileLiterals::Data);
    if (dataJson.has_value()) {
        OsduGenericFileData data(dataJson.value(), getOsduPartition());
        const auto data_structPtr = std::reinterpret_pointer_cast<OsduGenericFileDataStruct>(data.getMetaDataStruct());
        if (data_structPtr != nullptr) {
            return *data_structPtr;
        }
    }

    return std::nullopt;
}

std::optional<OsduMetaArrayStruct>
OsduGenericFile::getMetaArrayStruct() const
{
    const auto metaArrayJson = getOptionalParameter<Json::Value>(OsduGenericFileLiterals::Meta);
    if (metaArrayJson.has_value()) {
        auto metaArray = OsduMetaArray(metaArrayJson.value(), getOsduPartition());
        const auto meta_structPtr = std::reinterpret_pointer_cast<OsduMetaArrayStruct>(metaArray.getMetaDataStruct());
        if (meta_structPtr != nullptr) {
            return *meta_structPtr;
        }
    }

    return std::nullopt;
}

std::optional<OsduTagArrayStruct>
OsduGenericFile::getTagArrayStruct() const
{
    const auto tagArrayJson = getOptionalParameter<Json::Value>(OsduGenericFileLiterals::Tags);
    if (tagArrayJson.has_value()) {
        auto tagArray = OsduTagArray(tagArrayJson.value(), getOsduPartition());
        const auto tag_structPtr = std::reinterpret_pointer_cast<OsduTagArrayStruct>(tagArray.getMetaDataStruct());
        if (tag_structPtr != nullptr) {
            return *tag_structPtr;
        }
    }

    return std::nullopt;
}

bool
OsduGenericFile::isValidInput() const
{
    return genericFileStruct_.isValid();
}

void
OsduGenericFile::initializeJson()
{
    removeMember(OsduGenericFileLiterals::Acl);
    removeMember(OsduGenericFileLiterals::Legal);
    removeMember(OsduGenericFileLiterals::Ancestry);
    removeMember(OsduGenericFileLiterals::Meta);
}

void
OsduGenericFile::populateJsonObject()
{
    initializeJson();

    const auto acl = OsduAcl(genericFileStruct_.acl);

    setParameter(OsduGenericFileLiterals::Acl, acl.getAsJson());

    const auto legal = OsduLegal(genericFileStruct_.legal);
    setParameter(OsduGenericFileLiterals::Legal, legal.getAsJson());

    if (genericFileStruct_.ancestry.has_value()) {
        const auto ancestry = OsduAncestry(genericFileStruct_.ancestry.value());
        setParameter(OsduGenericFileLiterals::Ancestry, ancestry.getAsJson());
    }

    if (genericFileStruct_.data.has_value()) {
        const auto data = OsduGenericFileData(genericFileStruct_.data.value());
        setParameter(OsduGenericFileLiterals::Data, data.getAsJson());
    }

    if (genericFileStruct_.meta.has_value()) {
        const auto meta = OsduMetaArray(genericFileStruct_.meta.value());
        setParameter(OsduGenericFileLiterals::Legal, meta.getAsJson());
    }

    if (genericFileStruct_.tags.has_value()) {
        const auto tags = OsduTagArray(genericFileStruct_.tags.value());
        setParameter(OsduGenericFileLiterals::Tags, tags.getAsJson());
    }
}

void
OsduGenericFile::initializeMetadataStruct()
{
    genericFileStruct_ = OsduGenericFileStruct(getOsduPartition());
}

void
OsduGenericFile::populateMetadataStruct()
{
    initializeMetadataStruct();

    genericFileStruct_.acl = getAclStruct();
    genericFileStruct_.ancestry = getAncestryStruct();
    genericFileStruct_.legal = getLegalStruct();
    genericFileStruct_.data = getGenericFileDataStruct();

    if (isMember(OsduGenericFileLiterals::Meta)) {
        const auto metaArrayStruct = getMetaArrayStruct();
        if (metaArrayStruct.has_value()) {
            genericFileStruct_.meta = metaArrayStruct;
        }
    }
    if (isMember(OsduGenericFileLiterals::Tags)) {
        const auto tagsArrayStruct = getTagArrayStruct();
        if (tagsArrayStruct.has_value()) {
            genericFileStruct_.tags = tagsArrayStruct;
        }
    }
}

bool
OsduGenericFile::AllNodesValid() const
{
    return genericFileStruct_.allNodesValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduGenericFile::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduGenericFileStruct>(genericFileStruct_);
}

} // namespace segysdk::metadata::osdu
