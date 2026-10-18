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

#include "../OsduMetaDataUtilities/OsduShotPointCommonMidPointRatio.h"

#include "../OsduMetaDataUtilities/OsduShotPointCommonMidPointRatioLiterals.h"

namespace segysdk::metadata::osdu
{
OsduShotPointCommonMidPointRatio::OsduShotPointCommonMidPointRatio(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    shotPointCommonMidPointRatioStruct_(osduPartition)
{
    /* Empty */
}
OsduShotPointCommonMidPointRatio::OsduShotPointCommonMidPointRatio(OsduShotPointCommonMidPointRatioStruct shotPointCommonMidPointRatioStruct)
  : OsduMetadataBase(shotPointCommonMidPointRatioStruct.getPartition()), shotPointCommonMidPointRatioStruct_(std::move(shotPointCommonMidPointRatioStruct))
{
    populateJsonObject();
}

OsduShotPointCommonMidPointRatio::OsduShotPointCommonMidPointRatio(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), shotPointCommonMidPointRatioStruct_(OsduShotPointCommonMidPointRatioStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduShotPointCommonMidPointRatio::isValidInput() const
{
    return shotPointCommonMidPointRatioStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduShotPointCommonMidPointRatio::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduShotPointCommonMidPointRatioStruct>(shotPointCommonMidPointRatioStruct_);
}

void
OsduShotPointCommonMidPointRatio::initializeJson()
{
    clearJsonObject();
}

void
OsduShotPointCommonMidPointRatio::populateJsonObject()
{
    initializeJson();

    setParameter(OsduShotPointCommonMidPointRatioLiterals::ShotPoint, shotPointCommonMidPointRatioStruct_.shotPoint);
    setParameter(OsduShotPointCommonMidPointRatioLiterals::CommonMidPoint, shotPointCommonMidPointRatioStruct_.midPoint);
    setOptionalParameter(OsduShotPointCommonMidPointRatioLiterals::RoundingError, shotPointCommonMidPointRatioStruct_.roundingError);
}

void
OsduShotPointCommonMidPointRatio::initializeMetadataStruct()
{
    shotPointCommonMidPointRatioStruct_ = OsduShotPointCommonMidPointRatioStruct(getOsduPartition());
}

void
OsduShotPointCommonMidPointRatio::populateMetadataStruct()
{
    initializeMetadataStruct();

    shotPointCommonMidPointRatioStruct_.shotPoint = getParameter<double>(OsduShotPointCommonMidPointRatioLiterals::ShotPoint);
    shotPointCommonMidPointRatioStruct_.midPoint = getParameter<double>(OsduShotPointCommonMidPointRatioLiterals::CommonMidPoint);
    shotPointCommonMidPointRatioStruct_.roundingError = getOptionalParameter<double>(OsduShotPointCommonMidPointRatioLiterals::RoundingError);
}
} // namespace segysdk::metadata::osdu
