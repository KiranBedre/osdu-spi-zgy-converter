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

#include "../OsduMetaDataUtilities/OsduGeoJsonGeometryElement.h"

#include "../OsduMetaDataUtilities/OsduGeoJsonGeometryElementStruct.h"
#include "../OsduMetaDataUtilities/OsduGeoJsonGeometryElementLiterals.h"

namespace segysdk::metadata::osdu
{
OsduGeoJsonGeometryElement::OsduGeoJsonGeometryElement(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), geometryElementStruct_(OsduGeoJsonGeometryElementStruct(osduPartition)) {}

OsduGeoJsonGeometryElement::OsduGeoJsonGeometryElement(const OsduGeoJsonGeometryElement& other)
    : OsduMetadataBase(other.getOsduPartition()), geometryElementStruct_(other.geometryElementStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGeoJsonGeometryElement::OsduGeoJsonGeometryElement(OsduGeoJsonGeometryElement&& other) noexcept
    : OsduMetadataBase(other.getOsduPartition()), geometryElementStruct_(std::move(other.geometryElementStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduGeoJsonGeometryElement&
OsduGeoJsonGeometryElement::operator=(const OsduGeoJsonGeometryElement& rhs)
{
    return *this = OsduGeoJsonGeometryElement(rhs);
}

OsduGeoJsonGeometryElement&
OsduGeoJsonGeometryElement::operator=(OsduGeoJsonGeometryElement&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    geometryElementStruct_ = std::move(rhs.geometryElementStruct_);

    return *this;
}

OsduGeoJsonGeometryElement::OsduGeoJsonGeometryElement(OsduGeoJsonGeometryElementStruct geometryElementStruct)
  : OsduMetadataBase(geometryElementStruct.getPartition()), geometryElementStruct_(std::move(geometryElementStruct))
{
    populateJsonObject();
}

OsduGeoJsonGeometryElement::OsduGeoJsonGeometryElement(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), geometryElementStruct_(OsduGeoJsonGeometryElementStruct(osduPartition))
{
    populateMetadataStruct();
}


bool
OsduGeoJsonGeometryElement::isValidInput() const
{
    return geometryElementStruct_.isValid();
}

void
OsduGeoJsonGeometryElement::initializeJson()
{
    removeMember(OsduGeoJsonGeometryElementLiterals::type);
    removeMember(OsduGeoJsonGeometryElementLiterals::coordinates);
    removeMember(OsduGeoJsonGeometryElementLiterals::geometries);
    removeMember(OsduGeoJsonGeometryElementLiterals::bbox);
}

void
OsduGeoJsonGeometryElement::populateJsonObject()
{
    clearJsonObject();
    setParameter(OsduGeoJsonGeometryElementLiterals::type, geometryElementStruct_.type);

    if (geometryElementStruct_.type != OsduGeoJsonGeometryElementStruct::geometry_Collection) {

        Json::Value jsonCoords;
        if (geometryElementStruct_.type == OsduGeoJsonGeometryElementStruct::geometry_Point) {
            // Single coordinate, [X and Y ]
            if (geometryElementStruct_.coordinates_point.has_value()) {
                for (const auto& coord : geometryElementStruct_.coordinates_point.value()) {
                    jsonCoords.append(coord);
                }
            }
        }
        else if (geometryElementStruct_.type == OsduGeoJsonGeometryElementStruct::geometry_LineString
                 or geometryElementStruct_.type == OsduGeoJsonGeometryElementStruct::geometry_MultiPoint) {
            //  Pair of coordinates [[X1, Y1], [Xn, Yn], ... ]
            if (geometryElementStruct_.coordinates_line.has_value()) {
                for (const auto& coord : geometryElementStruct_.coordinates_line.value()) {
                    Json::Value json;
                    for (const auto& c : coord) {
                        json.append(c);
                    }
                    jsonCoords.append(json);
                }
            }
        }
        else if (geometryElementStruct_.type == OsduGeoJsonGeometryElementStruct::geometry_Polygon
                 or geometryElementStruct_.type == OsduGeoJsonGeometryElementStruct::geometry_MultilineString) {
            if (geometryElementStruct_.coordinates_polygon.has_value()) {
                //  Multiple sets of coordinates [[[X1, Y1], [Xn, Yn]],[[X1, Y1], [Xn, Yn]], ... ]
                for (const auto& coord_poly : geometryElementStruct_.coordinates_polygon.value()) {
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
        else if (geometryElementStruct_.type == OsduGeoJsonGeometryElementStruct::geometry_MultiPolygon) {
            if (geometryElementStruct_.coordinates_multipolygon.has_value()) {
                //  Multiple, Multiple sets of coordinates [[[[X1, Y1], [Xn, Yn]],[[X1, Y1], [Xn, Yn]]],[[[X1, Y1], [Xn, Yn]],[[X1, Y1], [Xn, Yn]]], ...]
                for (const auto& coord_multipoly : geometryElementStruct_.coordinates_multipolygon.value()) {
                    Json::Value json1;
                    for (const auto& coord_poly : coord_multipoly) {
                        Json::Value json2;
                        for (const auto& coords : coord_poly) {
                            Json::Value json3;
                            for (const auto& c : coords) {
                                json3.append(c);
                            }
                            json2.append(json2);
                        }
                        json1.append(json2);
                    }
                    jsonCoords.append(json1);
                }
            }
        }

        setParameter(OsduGeoJsonGeometryElementLiterals::coordinates, jsonCoords);
    }

    if (geometryElementStruct_.geometries.has_value()) {
        InitializeJsonArray(OsduGeoJsonGeometryElementLiterals::geometries);
        for (auto& geometryStruct : geometryElementStruct_.geometries.value()) {
            OsduGeoJsonGeometryElement ge(geometryStruct);
            appendArrayElement(OsduGeoJsonGeometryElementLiterals::geometries, ge.getAsJson());
        }
    }
    if (geometryElementStruct_.bbox.has_value()) {
        InitializeJsonArray(OsduGeoJsonGeometryElementLiterals::bbox);
        for (const auto& value : geometryElementStruct_.bbox.value()) {
            appendArrayElement(OsduGeoJsonGeometryElementLiterals::bbox, value);
        }
    }
}

void
OsduGeoJsonGeometryElement::initializeMetadataStruct()
{
    geometryElementStruct_ = OsduGeoJsonGeometryElementStruct(getOsduPartition());
}

std::optional<std::vector<double>>
OsduGeoJsonGeometryElement::getPointCoordinates(const Json::Value& jsonCoords)
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
OsduGeoJsonGeometryElement::getLineCoordinates(const Json::Value& jsonCoords)
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
OsduGeoJsonGeometryElement::getPolygonCoordinates(const Json::Value& jsonCoords)
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
OsduGeoJsonGeometryElement::getMultiPolygonCoordinates(const Json::Value& jsonCoords)
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
OsduGeoJsonGeometryElement::populateMetadataStruct()
{
    initializeMetadataStruct();

    geometryElementStruct_.type = getParameter<std::string>(OsduGeoJsonGeometryElementLiterals::type);

    std::vector<std::vector<double>> result;
    const auto jsonCoords = getParameter<Json::Value>(OsduGeoJsonGeometryElementLiterals::coordinates);
    if (geometryElementStruct_.type == OsduGeoJsonGeometryElementStruct::geometry_Point) {
        geometryElementStruct_.coordinates_point = getPointCoordinates(jsonCoords);
    }
    else if (geometryElementStruct_.type == OsduGeoJsonGeometryElementStruct::geometry_LineString
        or geometryElementStruct_.type == OsduGeoJsonGeometryElementStruct::geometry_MultiPoint) {
        geometryElementStruct_.coordinates_line = getLineCoordinates(jsonCoords);
    }
    else if (geometryElementStruct_.type == OsduGeoJsonGeometryElementStruct::geometry_Polygon
        or geometryElementStruct_.type == OsduGeoJsonGeometryElementStruct::geometry_MultilineString) {
        geometryElementStruct_.coordinates_polygon = getPolygonCoordinates(jsonCoords);
    }
    else if (geometryElementStruct_.type == OsduGeoJsonGeometryElementStruct::geometry_MultiPolygon) {
        geometryElementStruct_.coordinates_multipolygon = getMultiPolygonCoordinates(jsonCoords);
    }
   
    if (isMember(OsduGeoJsonGeometryElementLiterals::geometries)) {
        std::vector<OsduGeoJsonGeometryElementStruct> geometries;
        const auto jsonArray = getAllArrayElements<Json::Value>(OsduGeoJsonGeometryElementLiterals::geometries);
        for (auto& json: jsonArray) {
            OsduGeoJsonGeometryElement ge(json, getOsduPartition());
            const auto ptr = std::reinterpret_pointer_cast<OsduGeoJsonGeometryElementStruct>(ge.getMetaDataStruct());
            if (ptr != nullptr) {
                geometries.emplace_back(*ptr);
            }
        }
        geometryElementStruct_.geometries = geometries;
    }

    if (isMember(OsduGeoJsonGeometryElementLiterals::bbox)) {
        geometryElementStruct_.bbox = getAllArrayElements<double>(OsduGeoJsonGeometryElementLiterals::bbox);
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduGeoJsonGeometryElement::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduGeoJsonGeometryElementStruct>(geometryElementStruct_);
}

} // namespace segysdk::metadata::osdu
