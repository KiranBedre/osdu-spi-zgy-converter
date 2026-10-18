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

#include "../OsduMetaDataUtilities/OsduMetaArray.h"

#include "../OsduMetaDataUtilities/OsduGenericFileLiterals.h"
#include "../OsduMetaDataUtilities/OsduMetaElement.h"

namespace segysdk::metadata::osdu
{

OsduMetaArray::OsduMetaArray(std::string osduPartition)
    : OsduMetadataBase(std::move(osduPartition), true), metaArrayStruct_(OsduMetaArrayStruct(getOsduPartition()))
{
    populateJsonObject();
}

OsduMetaArray::OsduMetaArray(const OsduMetaArray& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      metaArrayStruct_(other.metaArrayStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduMetaArray::OsduMetaArray(OsduMetaArray&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      metaArrayStruct_(std::move(other.metaArrayStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduMetaArray&
OsduMetaArray::operator=(const OsduMetaArray& rhs)
{
    return *this = OsduMetaArray(rhs);
}

OsduMetaArray&
OsduMetaArray::operator=(OsduMetaArray&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    metaArrayStruct_ = std::move(rhs.metaArrayStruct_);

    return *this;
}

OsduMetaArray::OsduMetaArray(OsduMetaArrayStruct metaArray)
    : OsduMetadataBase(metaArray.getPartition()), metaArrayStruct_(std::move(metaArray))
{
    populateJsonObject();
}

OsduMetaArray::OsduMetaArray(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), metaArrayStruct_(OsduMetaArrayStruct(osduPartition))
{
    if (json.isArray()) {
        assignRootJsonObject(json);
    }
    else if (json.isMember(OsduGenericFileLiterals::Meta) and json[OsduGenericFileLiterals::Meta].isArray()) {
        assignRootJsonObject(json[OsduGenericFileLiterals::Meta]);
    }

    populateMetadataStruct();
}

bool
OsduMetaArray::isValidInput() const
{
    return metaArrayStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduMetaArray::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduMetaArrayStruct>(metaArrayStruct_);
}

void
OsduMetaArray::initializeJson()
{
    clearJsonObject();
    InitializeJsonRootAsArray();
}

void
OsduMetaArray::initializeMetadataStruct()
{
    metaArrayStruct_.metaStructs.clear();
}

void
OsduMetaArray::populateMetadataStruct()
{
    if (!isRootJsonArray()) {
        return;
    }

    std::vector<OsduMetaElementStruct> result;
    const auto metaElementsJson = getAllRootArrayElements<Json::Value>();
    for (auto& json : metaElementsJson) {

        OsduMetaElement metaElement(json, getOsduPartition());
        const auto metaStructPtr = std::reinterpret_pointer_cast<OsduMetaElementStruct>(metaElement.getMetaDataStruct());
        if (metaStructPtr != nullptr and metaStructPtr->isValid()) {
            result.emplace_back(*metaStructPtr);
        }
    }
    
    metaArrayStruct_.metaStructs = result;
}

void
OsduMetaArray::populateJsonObject()
{
    initializeJson();
    for (const auto& m : metaArrayStruct_.metaStructs) {
        OsduMetaElement me(m);
        if (me.isValidInput()) {
            const auto m_json = me.getAsJson();
            appendRootArrayElement(m_json);
        }
    }
}

} // namespace segysdk::metadata::osdu
