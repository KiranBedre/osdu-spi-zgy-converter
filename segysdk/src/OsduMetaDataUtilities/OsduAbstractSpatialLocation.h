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

#include "../OsduMetaDataUtilities/OsduAbstractSpatialLocationStruct.h"
#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>
#include <string>

namespace segysdk::metadata::osdu
{
/*

 "LineageAssertions": [
            {
                "ID": "namespace:any-group-type--AnyIndividualType:SomeUniqueAnyIndividualTypeID:",
                "LineageRelationshipType": "namespace:reference-data--LineageRelationshipType:Direct:"
            }
        ],
        "LiveTraceOutline": {
            "AppliedOperations": [
                "conversion from ED_1950_UTM_Zone_31N to GCS_European_1950; 1 points converted",
                "transformation GCS_European_1950 to GCS_WGS_1984 using ED_1950_To_WGS_1984_24; 1 points successfully transformed"
            ],
            "AsIngestedCoordinates": {
                "bbox": [
                    12345.6,
                    12345.6,
                    12345.6,
                    12345.6
                ],
                "CoordinateReferenceSystemID": "namespace:reference-data--CoordinateReferenceSystem:BoundProjected:EPSG::32021_EPSG::15851:",
                "features": [
                    {
                        "bbox": [
                            12345.6,
                            12345.6,
                            12345.6,
                            12345.6
                        ],
                        "geometry": {
                            "bbox": [
                                12345.6,
                                12345.6,
                                12345.6,
                                12345.6
                            ],
                            "coordinates": [
                                12345.6,
                                12345.6
                            ],
                            "type": "AnyCrsPoint"
                        },
                        "properties": {
                        },
                        "type": "AnyCrsFeature"
                    }
                ],
                "persistableReferenceCrs": "{\"authCode\":{\"auth\":\"OSDU\",\"code\":\"32021079\"},\"lateBoundCRS\":{\"authCode\":{\"auth\":\"EPSG\",\"code\":\"32021\"},\"name\":\"NAD_1927_StatePlane_North_Dakota_South_FIPS_3302\",\"type\":\"LBC\",\"ver\":\"PE_10_9_1\",\"wkt\":\"PROJCS[\\\"NAD_1927_StatePlane_North_Dakota_South_FIPS_3302\\\",GEOGCS[\\\"GCS_North_American_1927\\\",DATUM[\\\"D_North_American_1927\\\",SPHEROID[\\\"Clarke_1866\\\",6378206.4,294.9786982]],PRIMEM[\\\"Greenwich\\\",0.0],UNIT[\\\"Degree\\\",0.0174532925199433]],PROJECTION[\\\"Lambert_Conformal_Conic\\\"],PARAMETER[\\\"False_Easting\\\",2000000.0],PARAMETER[\\\"False_Northing\\\",0.0],PARAMETER[\\\"Central_Meridian\\\",-100.5],PARAMETER[\\\"Standard_Parallel_1\\\",46.18333333333333],PARAMETER[\\\"Standard_Parallel_2\\\",47.48333333333333],PARAMETER[\\\"Latitude_Of_Origin\\\",45.66666666666666],UNIT[\\\"Foot_US\\\",0.3048006096012192],AUTHORITY[\\\"EPSG\\\",32021]]\"},\"name\":\"NAD27 * OGP-Usa Conus / North Dakota CS27 South zone [32021,15851]\",\"singleCT\":{\"authCode\":{\"auth\":\"EPSG\",\"code\":\"15851\"},\"name\":\"NAD_1927_To_WGS_1984_79_CONUS\",\"type\":\"ST\",\"ver\":\"PE_10_9_1\",\"wkt\":\"GEOGTRAN[\\\"NAD_1927_To_WGS_1984_79_CONUS\\\",GEOGCS[\\\"GCS_North_American_1927\\\",DATUM[\\\"D_North_American_1927\\\",SPHEROID[\\\"Clarke_1866\\\",6378206.4,294.9786982]],PRIMEM[\\\"Greenwich\\\",0.0],UNIT[\\\"Degree\\\",0.0174532925199433]],GEOGCS[\\\"GCS_WGS_1984\\\",DATUM[\\\"D_WGS_1984\\\",SPHEROID[\\\"WGS_1984\\\",6378137.0,298.257223563]],PRIMEM[\\\"Greenwich\\\",0.0],UNIT[\\\"Degree\\\",0.0174532925199433]],METHOD[\\\"NADCON\\\"],PARAMETER[\\\"Dataset_conus\\\",0.0],OPERATIONACCURACY[5.0],AUTHORITY[\\\"EPSG\\\",15851]]\"},\"type\":\"EBC\",\"ver\":\"PE_10_9_1\"}",
                "persistableReferenceUnitZ": "{\"scaleOffset\":{\"scale\":1.0,\"offset\":0.0},\"symbol\":\"m\",\"baseMeasurement\":{\"ancestry\":\"Length\",\"type\":\"UM\"},\"type\":\"USO\"}",
                "persistableReferenceVerticalCrs": "{\"authCode\":{\"auth\":\"EPSG\",\"code\":\"5714\"},\"name\":\"MSL_Height\",\"type\":\"LBC\",\"ver\":\"PE_10_9_1\",\"wkt\":\"VERTCS[\\\"MSL_Height\\\",VDATUM[\\\"Mean_Sea_Level\\\"],PARAMETER[\\\"Vertical_Shift\\\",0.0],PARAMETER[\\\"Direction\\\",1.0],UNIT[\\\"Meter\\\",1.0],AUTHORITY[\\\"EPSG\\\",5714]]\"}",
                "type": "AnyCrsFeatureCollection",
                "VerticalCoordinateReferenceSystemID": "namespace:reference-data--CoordinateReferenceSystem:Vertical:EPSG::5714:",
                "VerticalUnitID": "namespace:reference-data--UnitOfMeasure:m:"
            },
            "CoordinateQualityCheckDateTime": "2020-02-13T09:13:15.55Z",
            "CoordinateQualityCheckPerformedBy": "Example CoordinateQualityCheckPerformedBy",
            "CoordinateQualityCheckRemarks": [
                "Example CoordinateQualityCheckRemarks"
            ],
            "QualitativeSpatialAccuracyTypeID": "namespace:reference-data--QualitativeSpatialAccuracyType:Assumed:",
            "QuantitativeAccuracyBandID": "namespace:reference-data--QuantitativeAccuracyBand:Length.LessThan1m:",
            "SpatialGeometryTypeID": "namespace:reference-data--SpatialGeometryType:Point:",
            "SpatialLocationCoordinatesDate": "2020-02-13T09:13:15.55Z",
            "SpatialParameterTypeID": "namespace:reference-data--SpatialParameterType:Outline:",
            "Wgs84Coordinates": {
                "bbox": [
                    12345.6,
                    12345.6,
                    12345.6,
                    12345.6
                ],
                "features": [
                    {
                        "bbox": [
                            12345.6,
                            12345.6,
                            12345.6,
                            12345.6
                        ],
                        "geometry": {
                            "bbox": [
                                12345.6,
                                12345.6,
                                12345.6,
                                12345.6
                            ],
                            "coordinates": [
                                12345.6,
                                12345.6
                            ],
                            "type": "Point"
                        },
                        "properties": {
                        },
                        "type": "Feature"
                    }
                ],
                "type": "FeatureCollection"
            }
        },
  
*/

class SEGYSDK_DLL_PUBLIC OsduAbstractSpatialLocation final : public OsduMetadataBase
{
public:
    OsduAbstractSpatialLocation(const std::string& osduPartition);
    OsduAbstractSpatialLocation(const OsduAbstractSpatialLocation& other);
    OsduAbstractSpatialLocation(OsduAbstractSpatialLocation&& other) noexcept;
    OsduAbstractSpatialLocation& operator=(const OsduAbstractSpatialLocation& rhs);
    OsduAbstractSpatialLocation& operator=(OsduAbstractSpatialLocation&& rhs) noexcept;
    ~OsduAbstractSpatialLocation() override = default;
    OsduAbstractSpatialLocation(OsduAbstractSpatialLocationStruct constantValueOverrideStruct);
    explicit OsduAbstractSpatialLocation(Json::Value json, const std::string& osduPartition);

public:
    bool isValidInput() const override;
    std::shared_ptr<OsduMetadataStructBase> getMetaDataStruct() override;

private:
    void initializeJson() override;
    void populateJsonObject() override;
    void initializeMetadataStruct() override;
    std::shared_ptr<OsduAbstractAnyCrsFeatureCollectionStruct> getOsduAbstractAnyCrsFeatureCollectionStruct(const Json::Value& json) const;
    std::shared_ptr<OsduAbstractFeatureCollectionStruct> getOsduAbstractFeatureCollectionStruct(const Json::Value& json) const;
    void populateMetadataStruct() override;

private:
    OsduAbstractSpatialLocationStruct spatialLocationStruct_;

};

}
