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

#include "../OsduMetaDataUtilities/OsduVectorHeaderMapping.h"

#include "../OsduMetaDataUtilities/OsduVectorHeaderMappingElement.h"
#include "../OsduMetaDataUtilities/OsduVectorHeaderMappingStruct.h"

#include <algorithm>
#include <utility>

namespace segysdk::metadata::osdu
{
OsduVectorHeaderMapping::OsduVectorHeaderMapping(std::string osduPartition)
    : OsduMetadataBase(std::move(osduPartition), true), vhmStruct_(OsduVectorHeaderMappingStruct(getOsduPartition()))
{
    /* Empty */
}

OsduVectorHeaderMapping::OsduVectorHeaderMapping(const OsduVectorHeaderMapping& other)
    : OsduMetadataBase(other.getOsduPartition(), true),
      vhmStruct_(other.vhmStruct_)
{
    populateJsonObject();
}

OsduVectorHeaderMapping::OsduVectorHeaderMapping(OsduVectorHeaderMapping&& other) noexcept
    : OsduMetadataBase(other.getOsduPartition(), true),
      vhmStruct_(std::move(other.vhmStruct_))
{
    populateJsonObject();
}

OsduVectorHeaderMapping&
OsduVectorHeaderMapping::operator=(const OsduVectorHeaderMapping& rhs)
{
    return *this = OsduVectorHeaderMapping(rhs);
}

OsduVectorHeaderMapping&
OsduVectorHeaderMapping::operator=(OsduVectorHeaderMapping&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());

    vhmStruct_ = std::move(rhs.vhmStruct_);

    return *this;

}

OsduVectorHeaderMapping::OsduVectorHeaderMapping(OsduVectorHeaderMappingStruct aclStruct)
    : OsduMetadataBase(aclStruct.getPartition(), true), vhmStruct_(std::move(aclStruct))
{
    populateJsonObject();
}

OsduVectorHeaderMapping::OsduVectorHeaderMapping(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition, true), vhmStruct_(OsduVectorHeaderMappingStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduVectorHeaderMapping::isValidInput() const
{
    return vhmStruct_.isValid();
}

void
OsduVectorHeaderMapping::initializeJson()
{
    clearJsonObject();
    InitializeJsonRootAsArray();
}

void
OsduVectorHeaderMapping::populateJsonObject()
{
    initializeJson();

    for (const auto& mapping : vhmStruct_.vectorHeaderMapping_) {
        const OsduVectorHeaderMappingElement vhm(mapping);
        appendRootArrayElement(vhm.getAsJson());
    }
}

void
OsduVectorHeaderMapping::initializeMetadataStruct()
{
    vhmStruct_ = OsduVectorHeaderMappingStruct(getOsduPartition());
}

void
OsduVectorHeaderMapping::populateMetadataStruct()
{
    initializeMetadataStruct();

    if (isRootJsonArray()) {
        for (const auto& map : getAllRootArrayElements<Json::Value>()) {
            OsduVectorHeaderMappingElement vhm(map, getOsduPartition());
            const auto mapStruct = std::reinterpret_pointer_cast<OsduVectorHeaderMappingElementStruct>(vhm.getMetaDataStruct());
            vhmStruct_.vectorHeaderMapping_.emplace_back(*mapStruct);
        }
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduVectorHeaderMapping::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduVectorHeaderMappingStruct>(vhmStruct_);
}

} // namespace segysdk::metadata::osdu
