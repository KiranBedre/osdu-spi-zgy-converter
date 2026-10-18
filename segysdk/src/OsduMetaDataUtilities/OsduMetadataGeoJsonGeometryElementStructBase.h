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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include <optional>
#include <vector>

namespace segysdk::metadata::osdu
{

struct SEGYSDK_DLL_PUBLIC OsduMetadataGeoJsonGeometryElementStructBase
{
    OsduMetadataGeoJsonGeometryElementStructBase() = default;
    virtual ~OsduMetadataGeoJsonGeometryElementStructBase() = default;

protected:
    virtual bool CoordinatesValid(const std::optional<std::vector<double>>& coordinatesPoint, const std::optional<std::vector<std::vector<double>>>& coordinatesLine, const std::optional<std::vector<std::vector<std::vector<double>>>>& coordinatesPolygon, const std::optional<std::vector<std::vector<std::vector<std::vector<double>>>>>& coordinatesMultiPolygon) const;
    virtual bool coordinates_pointValid(const std::optional<std::vector<double>>& coordinatesPoint) const;
    virtual bool coordinates_lineValid(const std::optional<std::vector<std::vector<double>>>& coordinatesLine) const;
    virtual bool coordinates_polygonValid(const std::optional<std::vector<std::vector<std::vector<double>>>>& coordinatesPolygon) const;
    virtual bool coordinates_multipolygonValid(const std::optional<std::vector<std::vector<std::vector<std::vector<double>>>>>& coordinatesMultiPolygon) const;

public:
    inline static constexpr const char* const geometry_Point = "AnyCrsPoint";
    inline static constexpr const char* const geometry_LineString = "AnyCrsLineString";
    inline static constexpr const char* const geometry_Polygon = "AnyCrsPolygon";
    inline static constexpr const char* const geometry_MultiPoint = "AnyCrsMultiPoint";
    inline static constexpr const char* const geometry_MultilineString = "AnyCrsMultiLineString";
    inline static constexpr const char* const geometry_MultiPolygon = "AnyCrsMultiPolygon";
    inline static constexpr const char* const geometry_Collection = "AnyCrsGeometryCollection";
};

} // namespace segysdk::metadata::osdu
