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

#include "../OsduMetaDataUtilities/OsduGeoJsonGeometryType.h"

#include "../OsduMetaDataUtilities/OsduGeoJsonGeometryTypeStruct.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonGeometryTypeLiterals.h"

namespace segysdk::metadata::osdu
{
OsduGeoJsonGeometryType::OsduGeoJsonGeometryType(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), geometryTypeStruct_(OsduGeoJsonGeometryTypeStruct(osduPartition)) {}

OsduGeoJsonGeometryType::OsduGeoJsonGeometryType(const OsduGeoJsonGeometryType& other)
    : OsduMetadataBase(other.getOsduPartition()), geometryTypeStruct_(other.geometryTypeStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGeoJsonGeometryType::OsduGeoJsonGeometryType(OsduGeoJsonGeometryType&& other) noexcept
    : OsduMetadataBase(other.getOsduPartition()), geometryTypeStruct_(std::move(other.geometryTypeStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGeoJsonGeometryType&
OsduGeoJsonGeometryType::operator=(const OsduGeoJsonGeometryType& rhs)
{
    return *this = OsduGeoJsonGeometryType(rhs);
}

OsduGeoJsonGeometryType&
OsduGeoJsonGeometryType::operator=(OsduGeoJsonGeometryType&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

     geometryTypeStruct_ = std::move(rhs.geometryTypeStruct_);

    return *this;
}

OsduGeoJsonGeometryType::OsduGeoJsonGeometryType(OsduGeoJsonGeometryTypeStruct constantValueOverrideStruct)
    : OsduMetadataBase(constantValueOverrideStruct.getPartition()), geometryTypeStruct_(std::move(constantValueOverrideStruct))
{
    populateJsonObject();
}

OsduGeoJsonGeometryType::OsduGeoJsonGeometryType(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), geometryTypeStruct_(OsduGeoJsonGeometryTypeStruct(osduPartition))
{
    populateMetadataStruct();
}


bool
OsduGeoJsonGeometryType::isValidInput() const
{
    return geometryTypeStruct_.isValid();
}

void
OsduGeoJsonGeometryType::initializeJson()
{
    removeMember(OsduGeoJsonGeometryTypeLiterals::type);
    removeMember(OsduGeoJsonGeometryTypeLiterals::coordinates);
    removeMember(OsduGeoJsonGeometryTypeLiterals::geometries);
    removeMember(OsduGeoJsonGeometryTypeLiterals::bbox);
}

void
OsduGeoJsonGeometryType::populateJsonObject()
{
    clearJsonObject();
    setParameter(OsduGeoJsonGeometryTypeLiterals::type, geometryTypeStruct_.type);
    InitializeJsonArray(OsduGeoJsonGeometryTypeLiterals::coordinates);
    for (const auto& coord : geometryTypeStruct_.coordinates) {
        Json::Value array;
        for (const auto& value : coord) {
            array.append(value);
        }
        appendArrayElement(OsduGeoJsonGeometryTypeLiterals::coordinates, array);
    }
    if (geometryTypeStruct_.bbox.has_value()) {
        InitializeJsonArray(OsduGeoJsonGeometryTypeLiterals::bbox);
        for (const auto& value : geometryTypeStruct_.bbox.value()) {
            appendArrayElement(OsduGeoJsonGeometryTypeLiterals::bbox, value);
        }
    }
}

void
OsduGeoJsonGeometryType::initializeMetadataStruct()
{
    geometryTypeStruct_ = OsduGeoJsonGeometryTypeStruct(getOsduPartition());
}

void
OsduGeoJsonGeometryType::populateMetadataStruct()
{
    initializeMetadataStruct();

    geometryTypeStruct_.type = getParameter<std::string>(OsduGeoJsonGeometryTypeLiterals::type);

    const auto jsonArray = getAllArrayElements<Json::Value>(OsduGeoJsonGeometryTypeLiterals::coordinates);
    std::vector<std::vector<double>> coordinates;
    for (const Json::Value& coords : jsonArray) {
        if (coords.isArray()) {
            std::vector<double> coordValues;
            for (const auto& value : coords) {
                coordValues.emplace_back(value.asDouble());
            }
            coordinates.emplace_back(coordValues);
        }
    }
    geometryTypeStruct_.coordinates = coordinates;

    if (isMember(OsduGeoJsonGeometryTypeLiterals::bbox)) {
        geometryTypeStruct_.bbox = getAllArrayElements<double>(OsduGeoJsonGeometryTypeLiterals::bbox);
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduGeoJsonGeometryType::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduGeoJsonGeometryTypeStruct>(geometryTypeStruct_);
}

} // namespace segysdk::metadata::osdu
