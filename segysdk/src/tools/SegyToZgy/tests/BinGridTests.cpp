/*
Copyright 2021-2022 Schlumberger

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

#include "Utilities/BinGrid.h"
#include "Utilities/Environment.h"
#include "Utilities/JsonBinGrid.h"
#include "tools/SegyToZgy/library/ConversionUnit.h"
#include "tools/SegyToZgy/library/crs/CoordinateReferenceSystemResolver.hpp"
#include "tools/SegyToZgy/library/crs/PersistableReferenceCrs.h"
#include "tools/SegyToZgy/library/crs/WktCoordinateSystem.h"

#include <gtest/gtest.h>

#include <filesystem>


TEST(BinGridTests, test_right_handed_original_example)
{
    segysdk::utilities::BinGrid sut;
    sut.transformationMethod = segysdk::utilities::BinGrid::TransformationMethod::RightHanded;
    sut.binGridOriginI = 1;
    sut.binGridOriginJ = 1;
    sut.binGridOriginEasting = 456781;
    sut.binGridOriginNorthing = 5836723;
    sut.binGridScaleFactor = 0.99984;
    sut.binWidthI = 25;
    sut.binWidthJ = 12.5;
    sut.mapGridBearingJ = 20;
    sut.binNodeIncrementI = 1;
    sut.binNodeIncrementJ = 1;

    const double I = 300, J = 247;
    const auto coordinates = sut.getCoordinatesIJ(I, J);

    EXPECT_NEAR(coordinates.easting, 464855.62, 0.01);
    EXPECT_NEAR(coordinates.northing, 5837055.90, 0.01);
}

TEST(BinGridTests, test_left_handed_original_example)
{
    segysdk::utilities::BinGrid sut;
    sut.transformationMethod = segysdk::utilities::BinGrid::TransformationMethod::LeftHanded;
    sut.binGridOriginI = 5000;
    sut.binGridOriginJ = 0;
    sut.binGridOriginEasting = 871200;
    sut.binGridOriginNorthing = 10280160;
    sut.binGridScaleFactor = 1;
    sut.binWidthI = 82.5;
    sut.binWidthJ = 41.25;
    sut.mapGridBearingJ = 340;
    sut.binNodeIncrementI = 1;
    sut.binNodeIncrementJ = 1;

    const double I = 4700, J = 247;
    const auto coordinates = sut.getCoordinatesIJ(I, J);

    EXPECT_NEAR(coordinates.easting, 890972.63, 0.01);
    EXPECT_NEAR(coordinates.northing, 10298199.29, 0.01);
}

TEST(BinGridTests, test_load_OSDU_JSON_v1_0_0)
{
    auto const filePath = segysdk::utilities::Environment::instance().SegyLocalTestDataPath();
    const auto inputFilename = std::filesystem::path{filePath} / "BinGrid" / "osdu" / "SeismicBinGrid.1.0.0.json";
    const auto sut = segysdk::utilities::JsonBinGrid::loadBinGridFromFile(inputFilename.string());
    EXPECT_EQ(sut.binGridOriginI, 1.1);
    EXPECT_EQ(sut.binGridOriginJ, 2.2);
    EXPECT_EQ(sut.binGridOriginEasting, 3.3);
    EXPECT_EQ(sut.binGridOriginNorthing, 4.4);
    EXPECT_EQ(sut.binGridScaleFactor, 5.5);
    EXPECT_EQ(sut.binWidthI, 6.6);
    EXPECT_EQ(sut.binWidthJ, 7.7);
    EXPECT_EQ(sut.mapGridBearingJ, 8.8);
    EXPECT_EQ(sut.binNodeIncrementI, 9);
    EXPECT_EQ(sut.binNodeIncrementJ, 10);
    EXPECT_EQ(sut.coordinateReferenceSystemId, "namespace:reference-data--CoordinateReferenceSystem:BoundCRS.SLB.32021.15851:");
    EXPECT_EQ(sut.persistableReferenceCrs, "{\"lateBoundCRS\":{\"wkt\":\"PROJCS[\\\"NAD_1927_StatePlane_North_Dakota_South_FIPS_3302\\\",GEOGCS[\\\"GCS_North_American_1927\\\",DATUM[\\\"D_North_American_1927\\\",SPHEROID[\\\"Clarke_1866\\\",6378206.4,294.9786982]],PRIMEM[\\\"Greenwich\\\",0.0],UNIT[\\\"Degree\\\",0.0174532925199433]],PROJECTION[\\\"Lambert_Conformal_Conic\\\"],PARAMETER[\\\"False_Easting\\\",2000000.0],PARAMETER[\\\"False_Northing\\\",0.0],PARAMETER[\\\"Central_Meridian\\\",-100.5],PARAMETER[\\\"Standard_Parallel_1\\\",46.1833333333333],PARAMETER[\\\"Standard_Parallel_2\\\",47.4833333333333],PARAMETER[\\\"Latitude_Of_Origin\\\",45.6666666666667],UNIT[\\\"Foot_US\\\",0.304800609601219],AUTHORITY[\\\"EPSG\\\",32021]]\",\"ver\":\"PE_10_3_1\",\"name\":\"NAD_1927_StatePlane_North_Dakota_South_FIPS_3302\",\"authCode\":{\"auth\":\"EPSG\",\"code\":\"32021\"},\"type\":\"LBC\"},\"singleCT\":{\"wkt\":\"GEOGTRAN[\\\"NAD_1927_To_WGS_1984_79_CONUS\\\",GEOGCS[\\\"GCS_North_American_1927\\\",DATUM[\\\"D_North_American_1927\\\",SPHEROID[\\\"Clarke_1866\\\",6378206.4,294.9786982]],PRIMEM[\\\"Greenwich\\\",0.0],UNIT[\\\"Degree\\\",0.0174532925199433]],GEOGCS[\\\"GCS_WGS_1984\\\",DATUM[\\\"D_WGS_1984\\\",SPHEROID[\\\"WGS_1984\\\",6378137.0,298.257223563]],PRIMEM[\\\"Greenwich\\\",0.0],UNIT[\\\"Degree\\\",0.0174532925199433]],METHOD[\\\"NADCON\\\"],PARAMETER[\\\"Dataset_conus\\\",0.0],AUTHORITY[\\\"EPSG\\\",15851]]\",\"ver\":\"PE_10_3_1\",\"name\":\"NAD_1927_To_WGS_1984_79_CONUS\",\"authCode\":{\"auth\":\"EPSG\",\"code\":\"15851\"},\"type\":\"ST\"},\"ver\":\"PE_10_3_1\",\"name\":\"NAD27 * OGP-Usa Conus / North Dakota South [32021,15851]\",\"authCode\":{\"auth\":\"SLB\",\"code\":\"32021079\"},\"type\":\"EBC\"}");
    EXPECT_EQ(sut.transformationMethod, segysdk::utilities::BinGrid::TransformationMethod::LeftHanded);
}

TEST(BinGridTests, test_load_WKS_JSON)
{
    auto const filePath = segysdk::utilities::Environment::instance().SegyLocalTestDataPath();
    const auto inputFilename = std::filesystem::path{filePath} / "BinGrid" / "wks" / "slb_wke_binGrid.data.json";
    const auto sut = segysdk::utilities::JsonBinGrid::loadBinGridFromFile(inputFilename.string());
    EXPECT_EQ(sut.binGridOriginI, 11.11);
    EXPECT_EQ(sut.binGridOriginJ, 22.22);
    EXPECT_EQ(sut.binGridOriginEasting, 33.33);
    EXPECT_EQ(sut.binGridOriginNorthing, 44.44);
    EXPECT_EQ(sut.binGridScaleFactor, 55.55);
    EXPECT_EQ(sut.binWidthI, 66.66);
    EXPECT_EQ(sut.binWidthJ, 77.77);
    EXPECT_EQ(sut.mapGridBearingJ, 88.88);
    EXPECT_EQ(sut.binNodeIncrementI, 99);
    EXPECT_EQ(sut.binNodeIncrementJ, 1010);
    EXPECT_EQ(sut.persistableReferenceCrs, "{\"lateBoundCRS\":{\"wkt\":\"PROJCS[\\\"NAD_1927_StatePlane_Louisiana_South_FIPS_1702\\\",GEOGCS[\\\"GCS_North_American_1927\\\",DATUM[\\\"D_North_American_1927\\\",SPHEROID[\\\"Clarke_1866\\\",6378206.4,294.9786982]],PRIMEM[\\\"Greenwich\\\",0.0],UNIT[\\\"Degree\\\",0.0174532925199433]],PROJECTION[\\\"Lambert_Conformal_Conic\\\"],PARAMETER[\\\"False_Easting\\\",2000000.0],PARAMETER[\\\"False_Northing\\\",0.0],PARAMETER[\\\"Central_Meridian\\\",-91.3333333333333],PARAMETER[\\\"Standard_Parallel_1\\\",29.3],PARAMETER[\\\"Standard_Parallel_2\\\",30.7],PARAMETER[\\\"Latitude_Of_Origin\\\",28.6666666666667],UNIT[\\\"Foot_US\\\",0.304800609601219],AUTHORITY[\\\"EPSG\\\",26782]]\",\"ver\":\"PE_10_3_1\",\"name\":\"NAD_1927_StatePlane_Louisiana_South_FIPS_1702\",\"authCode\":{\"auth\":\"EPSG\",\"code\":\"26782\"},\"type\":\"LBC\"},\"singleCT\":{\"wkt\":\"GEOGTRAN[\\\"NAD_1927_To_WGS_1984_79_CONUS\\\",GEOGCS[\\\"GCS_North_American_1927\\\",DATUM[\\\"D_North_American_1927\\\",SPHEROID[\\\"Clarke_1866\\\",6378206.4,294.9786982]],PRIMEM[\\\"Greenwich\\\",0.0],UNIT[\\\"Degree\\\",0.0174532925199433]],GEOGCS[\\\"GCS_WGS_1984\\\",DATUM[\\\"D_WGS_1984\\\",SPHEROID[\\\"WGS_1984\\\",6378137.0,298.257223563]],PRIMEM[\\\"Greenwich\\\",0.0],UNIT[\\\"Degree\\\",0.0174532925199433]],METHOD[\\\"NADCON\\\"],PARAMETER[\\\"Dataset_conus\\\",0.0],AUTHORITY[\\\"EPSG\\\",15851]]\",\"ver\":\"PE_10_3_1\",\"name\":\"NAD_1927_To_WGS_1984_79_CONUS\",\"authCode\":{\"auth\":\"EPSG\",\"code\":\"15851\"},\"type\":\"ST\"},\"ver\":\"PE_10_3_1\",\"name\":\"NAD27 * OGP-Usa Conus / Louisiana South [26782,15851]\",\"authCode\":{\"auth\":\"SLB\",\"code\":\"26782079\"},\"type\":\"EBC\"}");
    EXPECT_EQ(sut.transformationMethod, segysdk::utilities::BinGrid::TransformationMethod::LeftHanded);
}

TEST(BinGridTests, parse_crs_from_string)
{
    auto const filePath = segysdk::utilities::Environment::instance().SegyLocalTestDataPath();
    const auto inputFilename = std::filesystem::path{filePath} / "BinGrid" / "wks" / "slb_wke_binGrid.data.json";
    const auto crsString = segysdk::utilities::JsonBinGrid::loadBinGridFromFile(inputFilename.string()).persistableReferenceCrs;
    const auto sut = PersistableReferenceCrs(crsString);

    const auto wkt = sut.getWkt();
    EXPECT_EQ(wkt, "PROJCS[\"NAD_1927_StatePlane_Louisiana_South_FIPS_1702\",GEOGCS[\"GCS_North_American_1927\",DATUM[\"D_North_American_1927\",SPHEROID[\"Clarke_1866\",6378206.4,294.9786982]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Lambert_Conformal_Conic\"],PARAMETER[\"False_Easting\",2000000.0],PARAMETER[\"False_Northing\",0.0],PARAMETER[\"Central_Meridian\",-91.3333333333333],PARAMETER[\"Standard_Parallel_1\",29.3],PARAMETER[\"Standard_Parallel_2\",30.7],PARAMETER[\"Latitude_Of_Origin\",28.6666666666667],UNIT[\"Foot_US\",0.304800609601219],AUTHORITY[\"EPSG\",26782]]");

    const auto unit = sut.getConversionUnit();
    EXPECT_EQ(unit.name, "Foot_US");
    EXPECT_EQ(unit.value, 0.304800609601219);

    // TODO need test cases for fallbacks (earlyBoundCRS, compoundCRS)
}

TEST(BinGridTests, parse_wgs84_crs_from_string)
{
    const char* filePath = std::getenv("SEGY_LOCALTESTDATA_PATH");
    const auto inputFilename = std::filesystem::path{filePath} / "BinGrid" / "wks" / "slb_wke_binGrid.data.json";
    const auto crsString = "{\"wkt\":\"PROJCS[\\\"WGS_1984_UTM_Zone_43S\\\",GEOGCS[\\\"GCS_WGS_1984\\\",DATUM[\\\"D_WGS_1984\\\",SPHEROID[\\\"WGS_1984\\\",6378137.0,298.257223563]],PRIMEM[\\\"Greenwich\\\",0.0],UNIT[\\\"Degree\\\",0.0174532925199433]],PROJECTION[\\\"Transverse_Mercator\\\"],PARAMETER[\\\"False_Easting\\\",500000.0],PARAMETER[\\\"False_Northing\\\",10000000.0],PARAMETER[\\\"Central_Meridian\\\",75.0],PARAMETER[\\\"Scale_Factor\\\",0.9996],PARAMETER[\\\"Latitude_Of_Origin\\\",0.0],UNIT[\\\"Meter\\\",1.0],AUTHORITY[\\\"EPSG\\\",32743]]\",\"ver\":\"PE_10_3_1\",\"name\":\"WGS_1984_UTM_Zone_43S\",\"authCode\":{\"auth\":\"EPSG\",\"code\":\"32743\"},\"type\":\"LBC\"}";
    const auto sut = PersistableReferenceCrs(crsString);

    const auto wkt = sut.getWkt();
    EXPECT_EQ(wkt, "PROJCS[\"WGS_1984_UTM_Zone_43S\",GEOGCS[\"GCS_WGS_1984\",DATUM[\"D_WGS_1984\",SPHEROID[\"WGS_1984\",6378137.0,298.257223563]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]],PROJECTION[\"Transverse_Mercator\"],PARAMETER[\"False_Easting\",500000.0],PARAMETER[\"False_Northing\",10000000.0],PARAMETER[\"Central_Meridian\",75.0],PARAMETER[\"Scale_Factor\",0.9996],PARAMETER[\"Latitude_Of_Origin\",0.0],UNIT[\"Meter\",1.0],AUTHORITY[\"EPSG\",32743]]");

    const auto unit = sut.getConversionUnit();
    EXPECT_EQ(unit.name, "Meter");
    EXPECT_EQ(unit.value, 1.0);

    // TODO need test cases for fallbacks (earlyBoundCRS, compoundCRS)
}

TEST(BinGridTests, get_conversion_unit_from_coordinate_reference_system_record)
{
    auto const filePath = segysdk::utilities::Environment::instance().SegyLocalTestDataPath();
    segysdk::utilities::BinGrid sut;
    sut.coordinateReferenceSystemId = (std::filesystem::path{filePath} / "BinGrid" / "osdu" / "CoordinateReferenceSystem.with_persistable_reference.json").string();

    const auto unit = getBinGridConversionUnit(sut);
    EXPECT_EQ(unit.name, "Meter");
    EXPECT_EQ(unit.value, 1.0);
}

TEST(BinGridTests, fallback_to_bin_grid_persistable_reference_crs)
{
    auto const filePath = segysdk::utilities::Environment::instance().SegyLocalTestDataPath();
    const auto inputFilename = std::filesystem::path{filePath} / "BinGrid" / "wks" / "slb_wke_binGrid.data.json";

    segysdk::utilities::BinGrid sut;
    sut.coordinateReferenceSystemId = (std::filesystem::path{filePath} / "BinGrid" / "osdu" / "CoordinateReferenceSystem.missing_persistable_reference.json").string();
    sut.persistableReferenceCrs = segysdk::utilities::JsonBinGrid::loadBinGridFromFile(inputFilename.string()).persistableReferenceCrs;

    const auto unit = getBinGridConversionUnit(sut);
    EXPECT_EQ(unit.name, "Foot_US");
    EXPECT_EQ(unit.value, 0.304800609601219);
}

TEST(BinGridTests, parse_unit_from_GEOGCS_wkt_no_linear_unit)
{
    EXPECT_THROW(
        WktCoordinateSystem sut("GEOGCS[\"GCS_North_American_1927\",DATUM[\"D_North_American_1927\",SPHEROID[\"Clarke_1866\",6378206.4,294.9786982]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433]]"), std::exception);
}

TEST(BinGridTests, parse_unit_from_GEOGCS_wkt_with_linear_unit)
{
    WktCoordinateSystem sut("GEOGCS[\"GCS_North_American_1927\",DATUM[\"D_North_American_1927\",SPHEROID[\"Clarke_1866\",6378206.4,294.9786982]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Degree\",0.0174532925199433],UNIT[\"Foot_US\",0.304800609601219]]");

    const auto unit = sut.getConversionUnit();
    EXPECT_EQ(unit.name, "Foot_US");
    EXPECT_EQ(unit.value, 0.304800609601219);
}

TEST(BinGridTests, parse_unit_from_GEOCCS_wkt)
{
    WktCoordinateSystem sut("GEOCCS[\"GCS_North_American_1927\",DATUM[\"D_North_American_1927\",SPHEROID[\"Clarke_1866\",6378206.4,294.9786982]],PRIMEM[\"Greenwich\",0.0],UNIT[\"Foot_US\",-12.34E5]]");

    const auto unit = sut.getConversionUnit();
    EXPECT_EQ(unit.name, "Foot_US");
    EXPECT_EQ(unit.value, -1234000.);
}
