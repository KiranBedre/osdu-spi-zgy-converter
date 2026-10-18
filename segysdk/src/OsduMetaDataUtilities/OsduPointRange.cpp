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

#include "../OsduMetaDataUtilities/OsduPointRange.h"

#include "../OsduMetaDataUtilities/OsduPointRangeLiterals.h"

namespace segysdk::metadata::osdu
{
OsduPointRange::OsduPointRange(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    pointRangeStruct_(osduPartition)
{
    /* Empty */
}
OsduPointRange::OsduPointRange(OsduPointRangeStruct pointRangeStruct)
  : OsduMetadataBase(pointRangeStruct.getPartition()), pointRangeStruct_(std::move(pointRangeStruct))
{
    populateJsonObject();
}

OsduPointRange::OsduPointRange(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), pointRangeStruct_(OsduPointRangeStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduPointRange::isValidInput() const
{
    return pointRangeStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduPointRange::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduPointRangeStruct>(pointRangeStruct_);
}

void
OsduPointRange::initializeJson()
{
    clearJsonObject();
}

void
OsduPointRange::populateJsonObject()
{
    initializeJson();

    setParameter(OsduPointRangeLiterals::Start, pointRangeStruct_.start);
    setParameter(OsduPointRangeLiterals::End, pointRangeStruct_.end);
}

void
OsduPointRange::initializeMetadataStruct()
{
    pointRangeStruct_ = OsduPointRangeStruct(getOsduPartition());
}

void
OsduPointRange::populateMetadataStruct()
{
    initializeMetadataStruct();

    pointRangeStruct_.start = getParameter<double>(OsduPointRangeLiterals::Start);
    pointRangeStruct_.end = getParameter<double>(OsduPointRangeLiterals::End);
}
} // namespace segysdk::metadata::osdu
