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

#include "../OsduMetaDataUtilities/OsduGenericLabels.h"
#include "../OsduMetaDataUtilities/OsduGenericLabelsLiterals.h"

namespace segysdk::metadata::osdu
{
OsduGenericLabels::OsduGenericLabels(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    genericLabelsStruct_(osduPartition)
{
    /* Empty */
}
OsduGenericLabels::OsduGenericLabels(OsduGenericLabelsStruct genericLabelsStruct)
  : OsduMetadataBase(genericLabelsStruct.getPartition()), genericLabelsStruct_(std::move(genericLabelsStruct))
{
    populateJsonObject();
}

OsduGenericLabels::OsduGenericLabels(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), genericLabelsStruct_(OsduGenericLabelsStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduGenericLabels::isValidInput() const
{
    return genericLabelsStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduGenericLabels::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduGenericLabelsStruct>(genericLabelsStruct_);
}

void
OsduGenericLabels::initializeJson()
{
    clearJsonObject();
}

void
OsduGenericLabels::populateJsonObject()
{
    initializeJson();

    setParameter(OsduGenericLabelsLiterals::First, genericLabelsStruct_.first);
    setParameter(OsduGenericLabelsLiterals::Last, genericLabelsStruct_.last);
}

void
OsduGenericLabels::initializeMetadataStruct()
{
    genericLabelsStruct_ = OsduGenericLabelsStruct(getOsduPartition());
}

void
OsduGenericLabels::populateMetadataStruct()
{
    initializeMetadataStruct();

    genericLabelsStruct_.first = getParameter<std::string>(OsduGenericLabelsLiterals::First);
    genericLabelsStruct_.last = getParameter<std::string>(OsduGenericLabelsLiterals::Last);
}
} // namespace segysdk::metadata::osdu
