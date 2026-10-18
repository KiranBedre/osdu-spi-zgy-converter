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

#include "../OsduMetaDataUtilities/OsduAnyCrsGeoJsonGeometryElement.h"

#include "../OsduMetaDataUtilities/OsduAnyCrsGeoJsonGeometryElementStruct.h"
#include "../OsduMetaDataUtilities/OsduAnyCrsGeoJsonGeometryElementLiterals.h"

namespace segysdk::metadata::osdu
{
OsduAnyCrsGeoJsonGeometryElement::OsduAnyCrsGeoJsonGeometryElement(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), anyCrsGeometryElementStruct_(OsduAnyCrsGeoJsonGeometryElementStruct(osduPartition)) {}

OsduAnyCrsGeoJsonGeometryElement::OsduAnyCrsGeoJsonGeometryElement(const OsduAnyCrsGeoJsonGeometryElement& other)
    : OsduMetadataBase(other.getOsduPartition()), anyCrsGeometryElementStruct_(other.anyCrsGeometryElementStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAnyCrsGeoJsonGeometryElement::OsduAnyCrsGeoJsonGeometryElement(OsduAnyCrsGeoJsonGeometryElement&& other) noexcept
    : OsduMetadataBase(other.getOsduPartition()), anyCrsGeometryElementStruct_(std::move(other.anyCrsGeometryElementStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduAnyCrsGeoJsonGeometryElement&
OsduAnyCrsGeoJsonGeometryElement::operator=(const OsduAnyCrsGeoJsonGeometryElement& rhs)
{
    return *this = OsduAnyCrsGeoJsonGeometryElement(rhs);
}

OsduAnyCrsGeoJsonGeometryElement&
OsduAnyCrsGeoJsonGeometryElement::operator=(OsduAnyCrsGeoJsonGeometryElement&& rhs) noexcept
{
    anyCrsGeometryElementStruct_ = std::move(rhs.anyCrsGeometryElementStruct_);
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    return *this;
}

OsduAnyCrsGeoJsonGeometryElement::OsduAnyCrsGeoJsonGeometryElement(OsduAnyCrsGeoJsonGeometryElementStruct constantValueOverrideStruct)
    : OsduMetadataBase(constantValueOverrideStruct.getPartition()), anyCrsGeometryElementStruct_(std::move(constantValueOverrideStruct))
{
    populateJsonObject();
}

OsduAnyCrsGeoJsonGeometryElement::OsduAnyCrsGeoJsonGeometryElement(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), anyCrsGeometryElementStruct_(OsduAnyCrsGeoJsonGeometryElementStruct(osduPartition))
{
    populateMetadataStruct();
}


bool
OsduAnyCrsGeoJsonGeometryElement::isValidInput() const
{
    return anyCrsGeometryElementStruct_.isValid();
}

void
OsduAnyCrsGeoJsonGeometryElement::initializeJson()
{
    removeMember(OsduAnyCrsGeoJsonGeometryElementLiterals::type);
    removeMember(OsduAnyCrsGeoJsonGeometryElementLiterals::coordinates);
    removeMember(OsduAnyCrsGeoJsonGeometryElementLiterals::bbox);
}

void
OsduAnyCrsGeoJsonGeometryElement::populateJsonObject()
{
    clearJsonObject();
    setParameter(OsduAnyCrsGeoJsonGeometryElementLiterals::type, anyCrsGeometryElementStruct_.type);

    if (anyCrsGeometryElementStruct_.type != OsduAnyCrsGeoJsonGeometryElementStruct::geometry_Collection) {

        Json::Value jsonCoords;
        if (anyCrsGeometryElementStruct_.type == OsduAnyCrsGeoJsonGeometryElementStruct::geometry_Point) {
            // Single coordinate, [X and Y ]
            if (anyCrsGeometryElementStruct_.coordinates_point.has_value()) {
                for (const auto& coord : anyCrsGeometryElementStruct_.coordinates_point.value()) {
                    jsonCoords.append(coord);
                }
            }
        }
        else if (anyCrsGeometryElementStruct_.type == OsduAnyCrsGeoJsonGeometryElementStruct::geometry_LineString
                 or anyCrsGeometryElementStruct_.type == OsduAnyCrsGeoJsonGeometryElementStruct::geometry_MultiPoint) {
            //  Pair of coordinates [[X1, Y1], [Xn, Yn], ... ]
            if (anyCrsGeometryElementStruct_.coordinates_line.has_value()) {
                for (const auto& coord : anyCrsGeometryElementStruct_.coordinates_line.value()) {
                    Json::Value json;
                    for (const auto& c : coord) {
                        json.append(c);
                    }
                    jsonCoords.append(json);
                }
            }
        }
        else if (anyCrsGeometryElementStruct_.type == OsduAnyCrsGeoJsonGeometryElementStruct::geometry_Polygon
                 or anyCrsGeometryElementStruct_.type == OsduAnyCrsGeoJsonGeometryElementStruct::geometry_MultilineString) {
            if (anyCrsGeometryElementStruct_.coordinates_polygon.has_value()) {
                //  Multiple sets of coordinates [[[X1, Y1], [Xn, Yn]],[[X1, Y1], [Xn, Yn]], ... ]
                for (const auto& coord_poly : anyCrsGeometryElementStruct_.coordinates_polygon.value()) {
                    Json::Value json1;
                    for (const auto& coords : coord_poly) {
                        Json::Value json2;
                        for (const auto& c : coords) {
                            json2.append(c);
                        }
                        json1.append(json2);
                    }
                    jsonCoords.append(json1);
                }
            }
        }
        else if (anyCrsGeometryElementStruct_.type == OsduAnyCrsGeoJsonGeometryElementStruct::geometry_MultiPolygon) {
            if (anyCrsGeometryElementStruct_.coordinates_multipolygon.has_value()) {
                //  Multiple, Multiple sets of coordinates [[[[X1, Y1], [Xn, Yn]],[[X1, Y1], [Xn, Yn]]],[[[X1, Y1], [Xn, Yn]],[[X1, Y1], [Xn, Yn]]], ...]
                for (const auto& coord_multipoly : anyCrsGeometryElementStruct_.coordinates_multipolygon.value()) {
                    Json::Value json1;
                    for (const auto& coord_poly : coord_multipoly) {
                        Json::Value json2;
                        for (const auto& coords : coord_poly) {
                            Json::Value json3;
                            for (const auto& c : coords) {
                                json3.append(c);
                            }
                            json2.append(json3);
                        }
                        json1.append(json2);
                    }
                    jsonCoords.append(json1);
                }
            }
        }

        setParameter(OsduAnyCrsGeoJsonGeometryElementLiterals::coordinates, jsonCoords);
    }

    if (anyCrsGeometryElementStruct_.bbox.has_value()) {
        InitializeJsonArray(OsduAnyCrsGeoJsonGeometryElementLiterals::bbox);
        for (const auto& value : anyCrsGeometryElementStruct_.bbox.value()) {
            appendArrayElement(OsduAnyCrsGeoJsonGeometryElementLiterals::bbox, value);
        }
    }
}

void
OsduAnyCrsGeoJsonGeometryElement::initializeMetadataStruct()
{
    anyCrsGeometryElementStruct_ = OsduAnyCrsGeoJsonGeometryElementStruct(getOsduPartition());
}

std::optional<std::vector<double>>
OsduAnyCrsGeoJsonGeometryElement::getPointCoordinates(const Json::Value& jsonCoords)
{
    std::vector<double> result;
    if (jsonCoords.empty() || !jsonCoords.isArray()) {
        return std::nullopt;
    }

    for (const auto& coordJson :jsonCoords) {
        result.emplace_back(coordJson.asDouble());
    }

    return result;
}

std::optional<std::vector<std::vector<double>>>
OsduAnyCrsGeoJsonGeometryElement::getLineCoordinates(const Json::Value& jsonCoords)
{
    if (jsonCoords.empty() || !jsonCoords.isArray()) {
        return std::nullopt;
    }

    std::vector<std::vector<double>> result;

    for (const auto& coordArrayJson : jsonCoords) {
        if (coordArrayJson.isArray()) {
            std::vector<double> array1;

            for (const auto& coordJson : coordArrayJson) {
                array1.emplace_back(coordJson.asDouble());
            }
            result.emplace_back(array1);
        }
    }

    return result;
}

std::optional < std::vector<std::vector<std::vector<double>>>>
OsduAnyCrsGeoJsonGeometryElement::getPolygonCoordinates(const Json::Value& jsonCoords)
{
    if (jsonCoords.empty() || !jsonCoords.isArray()) {
        return std::nullopt;
    }

    std::vector<std::vector<std::vector<double>>> result;

     for (const auto& coordArrayArrayJson : jsonCoords) {
        if (coordArrayArrayJson.isArray()) {
            std::vector<std::vector<double>> array2;
            for (const auto& coordArrayJson : coordArrayArrayJson) {
                if (coordArrayJson.isArray()) {
                    std::vector<double> array1;

                    for (const auto& coordJson : coordArrayJson) {
                        array1.emplace_back(coordJson.asDouble());
                    }

                    array2.emplace_back(array1);
                }
            }
            result.emplace_back(array2);
        }
    }

    return result;
}

std::optional < std::vector<std::vector<std::vector<std::vector<double>>>>>
OsduAnyCrsGeoJsonGeometryElement::getMultiPolygonCoordinates(const Json::Value& jsonCoords)
{
    if (jsonCoords.empty() || !jsonCoords.isArray()) {
        return std::nullopt;
    }

    std::vector<std::vector<std::vector<std::vector<double>>>> result;

    for (const auto& coordArrayArrayArrayJson : jsonCoords) {
        if (coordArrayArrayArrayJson.isArray()) {
            std::vector < std::vector<std::vector<double>>> array3;
            for (const auto& coordArrayArrayJson : coordArrayArrayArrayJson) {
                if (coordArrayArrayJson.isArray()) {
                    std::vector < std::vector<double>> array2;

                    for (const auto& coordArrayJson : coordArrayArrayJson) {
                        if (coordArrayJson.isArray()) {
                            std::vector<double> array1;
                            for (const auto& coordJson : coordArrayJson) {
                                array1.emplace_back(coordJson.asDouble());
                            }
                            array2.emplace_back(array1);
                        }
                    }

                    array3.emplace_back(array2);
                }
            }
            result.emplace_back(array3);
        }
    }

    return result;
}


void
OsduAnyCrsGeoJsonGeometryElement::populateMetadataStruct()
{
    initializeMetadataStruct();

    anyCrsGeometryElementStruct_.type = getParameter<std::string>(OsduAnyCrsGeoJsonGeometryElementLiterals::type);

    std::vector<std::vector<double>> result;
    const auto jsonCoords = getParameter<Json::Value>(OsduAnyCrsGeoJsonGeometryElementLiterals::coordinates);
    if (anyCrsGeometryElementStruct_.type == OsduAnyCrsGeoJsonGeometryElementStruct::geometry_Point) {
        anyCrsGeometryElementStruct_.coordinates_point = getPointCoordinates(jsonCoords);
    }
    else if (anyCrsGeometryElementStruct_.type == OsduAnyCrsGeoJsonGeometryElementStruct::geometry_LineString
        or anyCrsGeometryElementStruct_.type == OsduAnyCrsGeoJsonGeometryElementStruct::geometry_MultiPoint) {
        anyCrsGeometryElementStruct_.coordinates_line = getLineCoordinates(jsonCoords);
    }
    else if (anyCrsGeometryElementStruct_.type == OsduAnyCrsGeoJsonGeometryElementStruct::geometry_Polygon
        or anyCrsGeometryElementStruct_.type == OsduAnyCrsGeoJsonGeometryElementStruct::geometry_MultilineString) {
        anyCrsGeometryElementStruct_.coordinates_polygon = getPolygonCoordinates(jsonCoords);
    }
    else if (anyCrsGeometryElementStruct_.type == OsduAnyCrsGeoJsonGeometryElementStruct::geometry_MultiPolygon) {
        anyCrsGeometryElementStruct_.coordinates_multipolygon = getMultiPolygonCoordinates(jsonCoords);
    }
   

    if (isMember(OsduAnyCrsGeoJsonGeometryElementLiterals::bbox)) {
        anyCrsGeometryElementStruct_.bbox = getAllArrayElements<double>(OsduAnyCrsGeoJsonGeometryElementLiterals::bbox);
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduAnyCrsGeoJsonGeometryElement::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduAnyCrsGeoJsonGeometryElementStruct>(anyCrsGeometryElementStruct_);
}

} // namespace segysdk::metadata::osdu
