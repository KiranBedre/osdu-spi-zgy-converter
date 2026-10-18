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

#include "../OsduMetaDataUtilities/OsduAbstractMaster.h"

#include "../OsduMetaDataUtilities/OsduAbstractNameAlias.h"
#include "../OsduMetaDataUtilities/OsduAbstractGeoContext.h"
#include "../OsduMetaDataUtilities/OsduAbstractSpatialLocation.h"
#include "../OsduMetaDataUtilities/OsduAbstractTechnicalAssurance.h"

#include "../OsduMetaDataUtilities/OsduAbstractMasterLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAbstractMaster::OsduAbstractMaster(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    masterStruct_(osduPartition)
{
    /* Empty */
}
OsduAbstractMaster::OsduAbstractMaster(OsduAbstractMasterStruct masterStruct)
  : OsduMetadataBase(masterStruct.getPartition(), false), masterStruct_(std::move(masterStruct))
{
    populateJsonObject();
}

OsduAbstractMaster::OsduAbstractMaster(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), masterStruct_(OsduAbstractMasterStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduAbstractMaster::isValidInput() const
{
    return masterStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduAbstractMaster::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAbstractMasterStruct>(masterStruct_);
}

void
OsduAbstractMaster::initializeJson()
{
    clearJsonObject();
}

void
OsduAbstractMaster::populateJsonObject()
{
    initializeJson();

    setOptionalArrayStructParameter<OsduAbstractNameAlias>(OsduAbstractMasterLiterals::NameAliases, masterStruct_.nameAliasesStruct);
    setOptionalArrayStructParameter<OsduAbstractGeoContext>(OsduAbstractMasterLiterals::GeoContexts, masterStruct_.geoContextsStruct);
    setOptionalStructParameter<OsduAbstractSpatialLocation>(OsduAbstractMasterLiterals::SpatialLocation, masterStruct_.spatialLocationStruct);
    setOptionalParameter(OsduAbstractMasterLiterals::VersionCreationReason, masterStruct_.versionCreationReason);
    setOptionalArrayStructParameter<OsduAbstractTechnicalAssurance>(OsduAbstractMasterLiterals::TechnicalAssurances, masterStruct_.technicalAssurancesStruct);
}

void
OsduAbstractMaster::initializeMetadataStruct()
{
    masterStruct_ = OsduAbstractMasterStruct(getOsduPartition());
}

void
OsduAbstractMaster::populateMetadataStruct()
{
    initializeMetadataStruct();

    masterStruct_.nameAliasesStruct = getOptionalArrayStructParameter<OsduAbstractNameAlias, OsduAbstractNameAliasStruct>(OsduAbstractMasterLiterals::NameAliases);
    masterStruct_.geoContextsStruct = getOptionalArrayStructParameter<OsduAbstractGeoContext, OsduAbstractGeoContextStruct>(OsduAbstractMasterLiterals::GeoContexts);
    masterStruct_.spatialLocationStruct = getOptionalStructParameter<OsduAbstractSpatialLocation, OsduAbstractSpatialLocationStruct>(OsduAbstractMasterLiterals::SpatialLocation);
    masterStruct_.versionCreationReason = getOptionalParameter<std::string>(OsduAbstractMasterLiterals::VersionCreationReason);
    masterStruct_.technicalAssurancesStruct = getOptionalArrayStructParameter<OsduAbstractTechnicalAssurance, OsduAbstractTechnicalAssuranceStruct>(OsduAbstractMasterLiterals::TechnicalAssurances);
}
} // namespace segysdk::metadata::osdu
