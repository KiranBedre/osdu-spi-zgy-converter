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

#include "../OsduMetaDataUtilities/OsduTimeLapse.h"

#include "../OsduMetaDataUtilities/OsduTimeLapseLiterals.h"

namespace segysdk::metadata::osdu
{

OsduTimeLapse::OsduTimeLapse(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), timeLapseStruct_(OsduTimeLapseStruct(osduPartition))
{
    /* Empty */
}

OsduTimeLapse::OsduTimeLapse(const OsduTimeLapse& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      timeLapseStruct_(other.timeLapseStruct_)
{
    setOsduPartition(other.getOsduPartition());
    setJson(other.getJson());
}

OsduTimeLapse::OsduTimeLapse(OsduTimeLapse&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      timeLapseStruct_(std::move(other.timeLapseStruct_))
{
    setOsduPartition(other.getOsduPartition());
    setJson(other.getJson());
}

OsduTimeLapse&
OsduTimeLapse::operator=(const OsduTimeLapse& rhs)
{
    return *this = OsduTimeLapse(rhs);
}

OsduTimeLapse&
OsduTimeLapse::operator=(OsduTimeLapse&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setIsArrayElement(rhs.getIsArrayElement());
    setJson(rhs.getAsJson());

    timeLapseStruct_ = std::move(rhs.timeLapseStruct_);

    return *this;
}

OsduTimeLapse::OsduTimeLapse(OsduTimeLapseStruct tagElementStruct)
    : OsduMetadataBase(tagElementStruct.getPartition(), false), timeLapseStruct_(std::move(tagElementStruct))
{
    populateJsonObject();
}

OsduTimeLapse::OsduTimeLapse(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), timeLapseStruct_(OsduTimeLapseStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduTimeLapse::isValidInput() const
{
    return timeLapseStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduTimeLapse::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduTimeLapseStruct>(timeLapseStruct_);
}

void
OsduTimeLapse::initializeJson()
{
    clearJsonObject();
}

void
OsduTimeLapse::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduTimeLapseLiterals::TimeSeriesID, timeLapseStruct_.timeSeriesID);
    setOptionalParameter(OsduTimeLapseLiterals::TimeIndex, timeLapseStruct_.timeIndex);
}

void
OsduTimeLapse::initializeMetadataStruct() {
    timeLapseStruct_ = OsduTimeLapseStruct(getOsduPartition());
}

void
OsduTimeLapse::populateMetadataStruct()
{
    timeLapseStruct_.timeSeriesID = getOptionalParameter<std::string>(OsduTimeLapseLiterals::TimeSeriesID);
    timeLapseStruct_.timeIndex = getOptionalParameter<int>(OsduTimeLapseLiterals::TimeIndex);
}

} // namespace segysdk::metadata::osdu
