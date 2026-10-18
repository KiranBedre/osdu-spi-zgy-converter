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

#include "../OsduMetaDataUtilities/OsduMetadataGeoJsonGeometryElementStructBase.h"


namespace segysdk::metadata::osdu
{

bool
OsduMetadataGeoJsonGeometryElementStructBase::CoordinatesValid(const std::optional<std::vector<double>>& coordinatesPoint, const std::optional<std::vector<std::vector<double>>>& coordinatesLine, const std::optional<std::vector<std::vector<std::vector<double>>>>& coordinatesPolygon, const std::optional<std::vector<std::vector<std::vector<std::vector<double>>>>>& coordinatesMultiPolygon) const
{
    bool coord1Valid = coordinates_pointValid(coordinatesPoint) and !coordinates_lineValid(coordinatesLine) and !coordinates_polygonValid(coordinatesPolygon) and !coordinates_multipolygonValid(coordinatesMultiPolygon);
    bool coord2Valid = coordinates_lineValid(coordinatesLine) and !coordinates_pointValid(coordinatesPoint) and !coordinates_polygonValid(coordinatesPolygon) and !coordinates_multipolygonValid(coordinatesMultiPolygon);
    bool coord3Valid = coordinates_polygonValid(coordinatesPolygon) and !coordinates_pointValid(coordinatesPoint) and !coordinates_lineValid(coordinatesLine) and !coordinates_multipolygonValid(coordinatesMultiPolygon);
    bool coord4Valid = coordinates_multipolygonValid(coordinatesMultiPolygon) and !coordinates_pointValid(coordinatesPoint) and !coordinates_lineValid(coordinatesLine) and !coordinates_polygonValid(coordinatesPolygon);

    return coord1Valid or coord2Valid or coord3Valid or coord4Valid;
}

bool
OsduMetadataGeoJsonGeometryElementStructBase::coordinates_pointValid(const std::optional<std::vector<double>>& coordinatesPoint) const
{
    return coordinatesPoint.has_value() && !coordinatesPoint.value().empty();
}

bool
OsduMetadataGeoJsonGeometryElementStructBase::coordinates_lineValid(const std::optional<std::vector<std::vector<double>>>& coordinatesLine) const
{
    return coordinatesLine.has_value() && !coordinatesLine.value().empty();
}

bool
OsduMetadataGeoJsonGeometryElementStructBase::coordinates_polygonValid(const std::optional<std::vector<std::vector<std::vector<double>>>>& coordinatesPolygon) const
{
    return coordinatesPolygon.has_value() && !coordinatesPolygon.value().empty();
}

bool
OsduMetadataGeoJsonGeometryElementStructBase::coordinates_multipolygonValid(const std::optional<std::vector<std::vector<std::vector<std::vector<double>>>>>& coordinatesMultiPolygon) const
{
    return coordinatesMultiPolygon.has_value() && !coordinatesMultiPolygon.value().empty();
}

} // namespace segysdk::metadata::osdu
