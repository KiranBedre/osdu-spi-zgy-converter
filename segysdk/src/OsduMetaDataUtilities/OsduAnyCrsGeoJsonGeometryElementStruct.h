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

#pragma once

#include "../OsduMetaDataUtilities/OsduMetadataGeoJsonGeometryElementStructBase.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"
#include "../OsduMetaDataUtilities/OsduMetadataStructBase.h"

#include <optional>

/*
 "SpatialArea": {
            "AppliedOperations": [
                "conversion from ED_1950_UTM_Zone_34N to GCS_European_1950; 5 points converted",
                "transformation GCS_European_1950 to GCS_WGS_1984 using ED_1950_To_WGS_1984_2; 5 points successfully transformed"
            ],
            "AsIngestedCoordinates": {
                "CoordinateReferenceSystemID": "opendes:reference-data--CoordinateReferenceSystem:BoundCRS::opendes::23031024:",
                "features": [
                    {
                        "geometry": {
                            "coordinates": [
                                [
                                    [
                                        438302.76,
                                        6475310.75
                                    ],
                                    [
                                        429582.2271600945,
                                        6477485.023036702
                                    ],
                                    [
                                        430791.8366380928,
                                        6477485.023036702
                                    ],
                                    [
                                        439512.36947799835,
                                        6480162.22863138
                                    ]
                                ]
                            ],
                            "type": "AnyCrsPolygon"
                        },
                        "properties": {
                            "ijNumbers": [
                                [
                                    9961,
                                    1961
                                ],
                                [
                                    10361,
                                    1961
                                ],
                                [
                                    10361,
                                    2680
                                ],
                                [
                                    9961,
                                    2680
                                ]
                            ]
                        },
                        "type": "AnyCrsFeature"
                    }
                ],
                "persistableReferenceCrs": "{\"authCode\":{\"auth\":\"OSDU\",\"code\":\"23031024\"},\"lateBoundCRS\":{\"authCode\":{\"auth\":\"EPSG\",\"code\":\"23031\"},\"name\":\"ED_1950_UTM_Zone_31N\",\"type\":\"LBC\",\"ver\":\"PE_10_3_1\",\"wkt\":\"PROJCS[\\\"ED_1950_UTM_Zone_31N\\\",GEOGCS[\\\"GCS_European_1950\\\",DATUM[\\\"D_European_1950\\\",SPHEROID[\\\"International_1924\\\",6378388.0,297.0]],PRIMEM[\\\"Greenwich\\\",0.0],UNIT[\\\"Degree\\\",0.0174532925199433]],PROJECTION[\\\"Transverse_Mercator\\\"],PARAMETER[\\\"False_Easting\\\",500000.0],PARAMETER[\\\"False_Northing\\\",0.0],PARAMETER[\\\"Central_Meridian\\\",3.0],PARAMETER[\\\"Scale_Factor\\\",0.9996],PARAMETER[\\\"Latitude_Of_Origin\\\",0.0],UNIT[\\\"Meter\\\",1.0],AUTHORITY[\\\"EPSG\\\",23031]]\"},\"name\":\"ED50 / UTM zone 31N * EPSG-Nor S62 2001 [23031,1613]\",\"singleCT\":{\"authCode\":{\"auth\":\"EPSG\",\"code\":\"1613\"},\"name\":\"ED_1950_To_WGS_1984_24\",\"type\":\"ST\",\"ver\":\"PE_10_3_1\",\"wkt\":\"GEOGTRAN[\\\"ED_1950_To_WGS_1984_24\\\",GEOGCS[\\\"GCS_European_1950\\\",DATUM[\\\"D_European_1950\\\",SPHEROID[\\\"International_1924\\\",6378388.0,297.0]],PRIMEM[\\\"Greenwich\\\",0.0],UNIT[\\\"Degree\\\",0.0174532925199433]],GEOGCS[\\\"GCS_WGS_1984\\\",DATUM[\\\"D_WGS_1984\\\",SPHEROID[\\\"WGS_1984\\\",6378137.0,298.257223563]],PRIMEM[\\\"Greenwich\\\",0.0],UNIT[\\\"Degree\\\",0.0174532925199433]],METHOD[\\\"Position_Vector\\\"],PARAMETER[\\\"X_Axis_Translation\\\",-90.365],PARAMETER[\\\"Y_Axis_Translation\\\",-101.13],PARAMETER[\\\"Z_Axis_Translation\\\",-123.384],PARAMETER[\\\"X_Axis_Rotation\\\",0.333],PARAMETER[\\\"Y_Axis_Rotation\\\",0.077],PARAMETER[\\\"Z_Axis_Rotation\\\",0.894],PARAMETER[\\\"Scale_Difference\\\",1.994],AUTHORITY[\\\"EPSG\\\",1613]]\"},\"type\":\"EBC\",\"ver\":\"PE_10_3_1\"}",
                "type": "AnyCrsFeatureCollection"
            },
            "SpatialGeometryTypeID": "opendes:reference-data--SpatialGeometryType:Polygon:",
            "SpatialParameterTypeID": "opendes:reference-data--SpatialParameterType:Outline:",
            "Wgs84Coordinates": {
                "features": [
                    {
                        "geometry": {
                            "coordinates": [
                                [
                                    [
                                        1.943967222,
                                        58.414205
                                    ],
                                    [
                                        1.794071944,
                                        58.43241222
                                    ],
                                    [
                                        1.814781667,
                                        58.43260528
                                    ],
                                    [
                                        1.9633875,
                                        58.45794
                                    ],
                                    [
                                        1.943967222,
                                        58.414205
                                    ]
                                ]
                            ],
                            "type": "Polygon"
                        },
                        "properties": {
                            "ijNumbers": [
                                [
                                    9961,
                                    1961
                                ],
                                [
                                    10361,
                                    1961
                                ],
                                [
                                    10361,
                                    2680
                                ],
                                [
                                    9961,
                                    2680
                                ],
                                [
                                    9961,
                                    1961
                                ]
                            ]
                        },
                        "type": "Feature"
                    }
                ],
                "type": "FeatureCollection"
            }
        },
 */

namespace segysdk::metadata::osdu
{

struct SEGYSDK_DLL_PUBLIC OsduAnyCrsGeoJsonGeometryElementStruct final : public OsduMetadataStructBase, public OsduMetadataGeoJsonGeometryElementStructBase

{
    explicit OsduAnyCrsGeoJsonGeometryElementStruct(std::string osduPartition);
    OsduAnyCrsGeoJsonGeometryElementStruct(const OsduAnyCrsGeoJsonGeometryElementStruct& other);
    OsduAnyCrsGeoJsonGeometryElementStruct(OsduAnyCrsGeoJsonGeometryElementStruct&& other) noexcept;
    OsduAnyCrsGeoJsonGeometryElementStruct& operator=(const OsduAnyCrsGeoJsonGeometryElementStruct& rhs);
    OsduAnyCrsGeoJsonGeometryElementStruct& operator=(OsduAnyCrsGeoJsonGeometryElementStruct&& rhs) noexcept;
    ~OsduAnyCrsGeoJsonGeometryElementStruct() override = default;

    std::string type;
    std::optional<std::vector<double>> coordinates_point;
    std::optional<std::vector<std::vector<double>>> coordinates_line;
    std::optional<std::vector<std::vector<std::vector<double>>>> coordinates_polygon;
    std::optional<std::vector<std::vector<std::vector<std::vector<double>>>>> coordinates_multipolygon;
    std::optional<std::vector<double>> bbox;

    bool isValid() const override;

    std::vector<std::string> performAttributeValidityScan() const override;

public:
    inline static constexpr const char* const geometry_Point = "AnyCrsPoint";
    inline static constexpr const char* const geometry_LineString = "AnyCrsLineString";
    inline static constexpr const char* const geometry_Polygon = "AnyCrsPolygon";
    inline static constexpr const char* const geometry_MultiPoint = "AnyCrsMultiPoint";
    inline static constexpr const char* const geometry_MultilineString = "AnyCrsMultiLineString";
    inline static constexpr const char* const geometry_MultiPolygon = "AnyCrsMultiPolygon";
    inline static constexpr const char* const geometry_Collection = "AnyCrsGeometryCollection";

public:
    static std::string getClassPartition() { return partition_; }
    std::string getClassAreaPath() const { return classAreaPath_; }
    std::string getClassName() const { return className_; }
    std::string getClassVersion() const { return classVersion_; }

public:
    inline static constexpr const char* const partition_{"osdu"};
    inline static constexpr const char* const classAreaPath_{"wks"};
    inline static constexpr const char* const className_{"AnyCrsGeoJsonGeometryElement"};
    inline static constexpr const char* const classVersion_{"1.0.0"};

    
};

} // namespace segysdk::metadata::osdu
