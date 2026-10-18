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

#include "../OsduMetaDataUtilities/OsduStackRanges.h"

#include "../OsduMetaDataUtilities/OsduStackRangesLiterals.h"

namespace segysdk::metadata::osdu
{

OsduStackRanges::OsduStackRanges(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), stackRangesStruct_(OsduStackRangesStruct(osduPartition))
{
    /* Empty */
}

OsduStackRanges::OsduStackRanges(const OsduStackRanges& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      stackRangesStruct_(other.stackRangesStruct_)
{
    setOsduPartition(other.getOsduPartition());
    setJson(other.getJson());
}

OsduStackRanges::OsduStackRanges(OsduStackRanges&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      stackRangesStruct_(std::move(other.stackRangesStruct_))
{
    setOsduPartition(other.getOsduPartition());
    setJson(other.getJson());
}

OsduStackRanges&
OsduStackRanges::operator=(const OsduStackRanges& rhs)
{
    return *this = OsduStackRanges(rhs);
}

OsduStackRanges&
OsduStackRanges::operator=(OsduStackRanges&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setIsArrayElement(rhs.getIsArrayElement());
    setJson(rhs.getAsJson());

    stackRangesStruct_ = std::move(rhs.stackRangesStruct_);

    return *this;
}

OsduStackRanges::OsduStackRanges(OsduStackRangesStruct stackRangeStruct)
    : OsduMetadataBase(stackRangeStruct.getPartition(), false), stackRangesStruct_(std::move(stackRangeStruct))
{
    populateJsonObject();
}

OsduStackRanges::OsduStackRanges(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), stackRangesStruct_(OsduStackRangesStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduStackRanges::isValidInput() const
{
    return stackRangesStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduStackRanges::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduStackRangesStruct>(stackRangesStruct_);
}

void
OsduStackRanges::initializeJson()
{
    clearJsonObject();
}

void
OsduStackRanges::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduStackRangesLiterals::Minimum, stackRangesStruct_.minimum);
    setOptionalParameter(OsduStackRangesLiterals::Maximum, stackRangesStruct_.maximum);
}

void
OsduStackRanges::initializeMetadataStruct() {
    stackRangesStruct_ = OsduStackRangesStruct(getOsduPartition());
}

void
OsduStackRanges::populateMetadataStruct()
{
    stackRangesStruct_.minimum = getOptionalParameter<double>(OsduStackRangesLiterals::Minimum);
    stackRangesStruct_.maximum = getOptionalParameter<double>(OsduStackRangesLiterals::Maximum);
}

} // namespace segysdk::metadata::osdu
