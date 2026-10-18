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

#include "../OsduMetaDataUtilities/OsduAbstractSeismicPrecision.h"

#include "../OsduMetaDataUtilities/OsduAbstractSeismicPrecisionStruct.h"
#include "../OsduMetaDataUtilities/OsduAbstractSeismicPrecisionLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAbstractSeismicPrecision::OsduAbstractSeismicPrecision(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), seismicPrecisionStruct_(OsduAbstractSeismicPrecisionStruct(osduPartition)) {}

OsduAbstractSeismicPrecision::OsduAbstractSeismicPrecision(const OsduAbstractSeismicPrecision& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition()),
      seismicPrecisionStruct_(other.seismicPrecisionStruct_)
{
    setJson(other.getJson());
}

OsduAbstractSeismicPrecision::OsduAbstractSeismicPrecision(OsduAbstractSeismicPrecision&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition()),
      seismicPrecisionStruct_(std::move(other.seismicPrecisionStruct_))
{
    setJson(other.getJson());
}

OsduAbstractSeismicPrecision&
OsduAbstractSeismicPrecision::operator=(const OsduAbstractSeismicPrecision& rhs)
{
    return *this = OsduAbstractSeismicPrecision(rhs);
}

OsduAbstractSeismicPrecision&
OsduAbstractSeismicPrecision::operator=(OsduAbstractSeismicPrecision&& rhs) noexcept
{
    seismicPrecisionStruct_ = std::move(rhs.seismicPrecisionStruct_);
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());

    return *this;
}

OsduAbstractSeismicPrecision::OsduAbstractSeismicPrecision(OsduAbstractSeismicPrecisionStruct precisionStruct)
    : OsduMetadataBase(precisionStruct.getPartition()), seismicPrecisionStruct_(std::move(precisionStruct))
{
    populateJsonObject();
}

OsduAbstractSeismicPrecision::OsduAbstractSeismicPrecision(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), seismicPrecisionStruct_(OsduAbstractSeismicPrecisionStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractSeismicPrecision::isValidInput() const
{
    return seismicPrecisionStruct_.isValid();
}

void
OsduAbstractSeismicPrecision::initializeJson()
{
    removeMember(OsduAbstractSeismicPrecisionLiterals::WordWidth);
    removeMember(OsduAbstractSeismicPrecisionLiterals::WordFormat);
}

void
OsduAbstractSeismicPrecision::populateJsonObject()
{
    clearJsonObject();
    setOptionalParameter(OsduAbstractSeismicPrecisionLiterals::WordWidth,seismicPrecisionStruct_.wordWidth );
    setOptionalParameter(OsduAbstractSeismicPrecisionLiterals::WordFormat, seismicPrecisionStruct_.wordFormat);
}

void
OsduAbstractSeismicPrecision::initializeMetadataStruct()
{
    seismicPrecisionStruct_ = OsduAbstractSeismicPrecisionStruct(getOsduPartition());
}

void
OsduAbstractSeismicPrecision::populateMetadataStruct()
{
    initializeMetadataStruct();

    seismicPrecisionStruct_.wordWidth = getOptionalParameter<int>(OsduAbstractSeismicPrecisionLiterals::WordWidth);
    seismicPrecisionStruct_.wordFormat = getOptionalParameter<std::string>(OsduAbstractSeismicPrecisionLiterals::WordFormat);
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractSeismicPrecision::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractSeismicPrecisionStruct>(seismicPrecisionStruct_);
}

} // namespace segysdk::metadata::osdu
