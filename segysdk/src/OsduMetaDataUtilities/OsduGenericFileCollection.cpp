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

#include "../OsduMetaDataUtilities/OsduGenericFileCollection.h"

#include "../OsduMetaDataUtilities/OsduAcl.h"
#include "../OsduMetaDataUtilities/OsduLegal.h"
#include "../OsduMetaDataUtilities/OsduMetaArray.h"
#include "../OsduMetaDataUtilities/OsduGenericFileCollectionData.h"
#include "../OsduMetaDataUtilities/OsduGenericFileCollectionStruct.h"
#include "../OsduMetaDataUtilities/OsduGenericFileCollectionLiterals.h"

namespace segysdk::metadata::osdu
{

OsduGenericFileCollection::OsduGenericFileCollection(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), genericFileCollectionStruct_(OsduGenericFileCollectionStruct(osduPartition))
{
    /* Empty */
}

OsduGenericFileCollection::OsduGenericFileCollection(const OsduGenericFileCollection& other)
    : OsduMetadataBase(other.getOsduPartition()), genericFileCollectionStruct_(other.genericFileCollectionStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGenericFileCollection::OsduGenericFileCollection(OsduGenericFileCollection&& other) noexcept
    : OsduMetadataBase(other.getOsduPartition()), genericFileCollectionStruct_(std::move(other.genericFileCollectionStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGenericFileCollection&
OsduGenericFileCollection::operator=(const OsduGenericFileCollection& rhs)
{
    return *this = OsduGenericFileCollection(rhs);
}

OsduGenericFileCollection&
OsduGenericFileCollection::operator=(OsduGenericFileCollection&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    genericFileCollectionStruct_ = std::move(rhs.genericFileCollectionStruct_);

    return *this;
}

OsduGenericFileCollection::OsduGenericFileCollection(OsduGenericFileCollectionStruct genericStruct)
    : OsduMetadataBase(genericStruct.getPartition(), false), genericFileCollectionStruct_(std::move(genericStruct))
{
    populateJsonObject();
}

OsduGenericFileCollection::OsduGenericFileCollection(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), genericFileCollectionStruct_(OsduGenericFileCollectionStruct(osduPartition))
{
    populateMetadataStruct();
}

/* Not optional */
OsduAclStruct
OsduGenericFileCollection::getAclStruct() const
{
    OsduAcl acl(getParameter<Json::Value>(OsduGenericFileCollectionLiterals::Acl), getOsduPartition());
    const auto acl_structPtr = std::reinterpret_pointer_cast<OsduAclStruct>(acl.getMetaDataStruct());
    if (acl_structPtr != nullptr) {
        return *acl_structPtr;
    }

    return OsduAclStruct{getOsduPartition()};
}

/* Not optional */
OsduLegalStruct
OsduGenericFileCollection::getLegalStruct() const
{
    OsduLegal legal(getParameter<Json::Value>(OsduGenericFileCollectionLiterals::Legal), getOsduPartition());
    const auto legal_structPtr = std::reinterpret_pointer_cast<OsduLegalStruct>(legal.getMetaDataStruct());
    if (legal_structPtr != nullptr) {
        return *legal_structPtr;
    }

    return OsduLegalStruct{getOsduPartition()};
}

std::optional<OsduAncestryStruct>
OsduGenericFileCollection::getAncestryStruct() const
{
    const auto ancestryJson = getOptionalParameter<Json::Value>(OsduGenericFileCollectionLiterals::Ancestry);

    if (ancestryJson.has_value()) {
        auto ancestry = OsduAncestry(ancestryJson.value(), getOsduPartition());
        const auto legal_structPtr = std::reinterpret_pointer_cast<OsduAncestryStruct>(ancestry.getMetaDataStruct());
        if (legal_structPtr != nullptr) {
            return *legal_structPtr;
        }
    }
    return std::nullopt;
}

std::optional<OsduGenericFileCollectionDataStruct>
OsduGenericFileCollection::getGenericFileCollectionDataStruct() const
{
    const auto dataJson = getOptionalParameter<Json::Value>(OsduGenericFileCollectionLiterals::Data);
    if (dataJson.has_value()) {
        auto data = OsduGenericFileCollectionData(dataJson.value(), getOsduPartition());
        const auto data_structPtr = std::reinterpret_pointer_cast<OsduGenericFileCollectionDataStruct>(data.getMetaDataStruct());
        if (data_structPtr != nullptr) {
            return *data_structPtr;
        }
    }
    return std::nullopt;
}

std::optional<OsduMetaArrayStruct>
OsduGenericFileCollection::getMetaArrayStruct() const
{
    if (isMember(OsduGenericFileCollectionLiterals::Meta)) {
        const auto metaArrayJson = getParameter<Json::Value>(OsduGenericFileCollectionLiterals::Meta);

        OsduMetaArray metaArray(metaArrayJson, getOsduPartition());
        const auto meta_structPtr = std::reinterpret_pointer_cast<OsduMetaArrayStruct>(metaArray.getMetaDataStruct());
        if (meta_structPtr != nullptr) {
            return *meta_structPtr;
        }
    }

    return std::nullopt;
}

std::optional<OsduTagArrayStruct>
OsduGenericFileCollection::getTagArrayStruct() const
{
    if (isMember(OsduGenericFileCollectionLiterals::Tags)) {
        const auto tagArrayJson = getParameter<Json::Value>(OsduGenericFileCollectionLiterals::Tags);
        OsduTagArray tagArray(tagArrayJson, getOsduPartition());
        const auto tag_structPtr = std::reinterpret_pointer_cast<OsduTagArrayStruct>(tagArray.getMetaDataStruct());
        if (tag_structPtr != nullptr) {
            return *tag_structPtr;
        }
    }

    return std::nullopt;
}

bool
OsduGenericFileCollection::isValidInput() const
{

    return genericFileCollectionStruct_.isValid();
}

void
OsduGenericFileCollection::initializeJson()
{
    removeMember(OsduGenericFileCollectionLiterals::Acl);
    removeMember(OsduGenericFileCollectionLiterals::Legal);
    removeMember(OsduGenericFileCollectionLiterals::Ancestry);
    removeMember(OsduGenericFileCollectionLiterals::Meta);
}

void
OsduGenericFileCollection::populateJsonObject()
{
    clearJsonObject();

    const auto acl = OsduAcl(genericFileCollectionStruct_.acl);
    setParameter(OsduGenericFileCollectionLiterals::Acl, acl.getAsJson());

    const auto legal = OsduLegal(genericFileCollectionStruct_.legal);
    setParameter(OsduGenericFileCollectionLiterals::Legal, legal.getAsJson());

    // Optional from here
    if (genericFileCollectionStruct_.ancestry.has_value()) {
        const auto ancestry = OsduAncestry(genericFileCollectionStruct_.ancestry.value());
        setParameter(OsduGenericFileCollectionLiterals::Ancestry, ancestry.getAsJson());
    }

    if (genericFileCollectionStruct_.data.has_value()) {
        const OsduGenericFileCollectionData data(genericFileCollectionStruct_.data.value());
        setParameter(OsduGenericFileCollectionLiterals::Data, data.getAsJson());
    }

    if (genericFileCollectionStruct_.meta.has_value()) {
        const auto meta = OsduMetaArray(genericFileCollectionStruct_.meta.value());
        setParameter(OsduGenericFileCollectionLiterals::Meta, meta.getAsJson());
    }

    if (genericFileCollectionStruct_.tags.has_value()) {
        const auto tags = OsduTagArray(genericFileCollectionStruct_.tags.value());
        setParameter(OsduGenericFileCollectionLiterals::Tags, tags.getAsJson());
    }

}

void
OsduGenericFileCollection::initializeMetadataStruct()
{
    genericFileCollectionStruct_ = OsduGenericFileCollectionStruct(getOsduPartition());
}

void
OsduGenericFileCollection::populateMetadataStruct()
{
    initializeMetadataStruct();

    genericFileCollectionStruct_.acl = getAclStruct();
    genericFileCollectionStruct_.ancestry = getAncestryStruct();
    genericFileCollectionStruct_.legal = getLegalStruct();
    genericFileCollectionStruct_.data = getGenericFileCollectionDataStruct();

    if (isMember(OsduGenericFileCollectionLiterals::Meta)) {
        const auto metaArrayStruct = getMetaArrayStruct();
        if (metaArrayStruct.has_value()) {
            genericFileCollectionStruct_.meta = metaArrayStruct;
        }
    }
    if (isMember(OsduGenericFileCollectionLiterals::Tags)) {
        const auto tagsArrayStruct = getTagArrayStruct();
        if (tagsArrayStruct.has_value()) {
            genericFileCollectionStruct_.tags = tagsArrayStruct;
        }
    }
}

bool
OsduGenericFileCollection::AllNodesValid() const
{
    return genericFileCollectionStruct_.allNodesValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduGenericFileCollection::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduGenericFileCollectionStruct>(genericFileCollectionStruct_);
}

} // namespace segysdk::metadata::osdu
