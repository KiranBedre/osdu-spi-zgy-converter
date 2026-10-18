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

#include "../OsduMetaDataUtilities/OsduAbstractGeoContextStruct.h"

#include "OsduAbstractGeoContextLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAbstractGeoContextStruct::OsduAbstractGeoContextStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{}

OsduAbstractGeoContextStruct::OsduAbstractGeoContextStruct(const OsduAbstractGeoContextStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    geoTypeID(other.geoTypeID),
    geoPoliticalEntityID(other.geoPoliticalEntityID),
    basinID(other.basinID),
    fieldID(other.fieldID),
    playID(other.playID),
    prospectID(other.prospectID)
{
    /* Empty */
}

OsduAbstractGeoContextStruct::OsduAbstractGeoContextStruct(OsduAbstractGeoContextStruct&& other) noexcept
    : OsduMetadataStructBase(other.getPartition()),
    geoTypeID(std::move(other.geoTypeID)),
    geoPoliticalEntityID(std::move(other.geoPoliticalEntityID)),
    basinID(std::move(other.basinID)),
    fieldID(std::move(other.fieldID)),
    playID(std::move(other.playID)),
    prospectID(std::move(other.prospectID))
{
    /* Empty */
}

OsduAbstractGeoContextStruct&
OsduAbstractGeoContextStruct::operator=(const OsduAbstractGeoContextStruct& rhs)
{
    return *this = OsduAbstractGeoContextStruct(rhs);
}

OsduAbstractGeoContextStruct&
OsduAbstractGeoContextStruct::operator=(OsduAbstractGeoContextStruct&& rhs) noexcept
{
    geoTypeID = std::move(rhs.geoTypeID);
    geoPoliticalEntityID = std::move(rhs.geoPoliticalEntityID);
    basinID = std::move(rhs.basinID);
    fieldID = std::move(rhs.fieldID);
    playID = std::move(rhs.playID);
    prospectID = std::move(rhs.prospectID);

    setPartition(rhs.getPartition());

    return *this;
}

bool
OsduAbstractGeoContextStruct::isValid() const
{
    return geoTypeID.has_value() and (geoPoliticalEntityID.has_value() or basinID.has_value() or fieldID.has_value() or playID.has_value() or prospectID.has_value());
}

void
OsduAbstractGeoContextStruct::performValidityScan(std::vector<std::string>& results, const std::optional<std::string>& parameter, const std::string& tag) const
{
    if (parameter.has_value() and !isValidOSDUReferenceIdString(parameter.value())) {
        appendStringVector(results, {"GeoContext: " + tag + " is not a valid OSDU reference string."});
    }
}


std::vector<std::string>
OsduAbstractGeoContextStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;

    if (!isValid()) {
        results.emplace_back("OsduAbstractGeoContextStruct::isValid() failed.");
    }
    if (!geoTypeID.has_value()) {
        results.emplace_back("OsduAbstractGeoContextStruct:: geoTypeID should have a value.");
    }
    if (!geoPoliticalEntityID.has_value() and !basinID.has_value() and !fieldID.has_value() and !playID.has_value() and !prospectID.has_value()) {
        results.emplace_back("OsduAbstractGeoContextStruct:: One of [geoPoliticalEntityID, basinID,fieldID,  playID, prospectID] should have a value.");
    }
    performValidityScan(results, geoTypeID, OsduAbstractGeoContextLiterals::GeoTypeID);
    performValidityScan(results, geoPoliticalEntityID, OsduAbstractGeoContextLiterals::GeoPoliticalEntityID);
    performValidityScan(results, basinID, OsduAbstractGeoContextLiterals::BasinID);
    performValidityScan(results, fieldID, OsduAbstractGeoContextLiterals::FieldID);
    performValidityScan(results, playID, OsduAbstractGeoContextLiterals::PlayID);
    performValidityScan(results, prospectID, OsduAbstractGeoContextLiterals::ProspectID);

    return results;
}

} // namespace segysdk::metadata::osdu
