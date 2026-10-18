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

#include "../Utilities/BinGrid.h"
#include "../Utilities/Environment.h"
#include "../Utilities/JsonBinGrid.h"
#include "../Utilities/JsonUtilities.h"
#include "../Utilities/StringUtils.h"

#include "../Utilities/osdu/OsduFieldNameLiterals.h"
#include "../Utilities/osdu/OsduFileCollectionSegy100.h"
#include "../Utilities/osdu/OsduSeismicTraceData140.h"
#include "../Utilities/osdu/OsduSeismicTraceDataFactory.h"
#include "../segysdk/SegySessionFactory.h"
#include "../tests/UtilityLibrary/AutoCleanupTestFile.h"

#include <gtest/gtest.h>

#include <algorithm>
#include <filesystem>
#include <memory>
#include <fstream>

namespace segysdk::utilities::osdu::tests
{

class OsduSeismicTraceDataMock final : public segysdk::utilities::osdu::OsduSeismicTraceData140
{
public:
    explicit OsduSeismicTraceDataMock(const Json::Value& root)
      : OsduSeismicTraceData140(root)
    {
    }

    Json::Value
    getJsonRoot() const
    {
        return root_;
    }

    void clearSpatialAreaCoordinates()
    {
        auto& asIngested = root_[OsduFieldNameLiterals::Data][OsduSeismicTraceDataLiterals::SpatialArea][OsduFieldNameLiterals::AsIngestedCoordinates];
        auto& feature = asIngested[OsduSeismicTraceDataLiterals::Features][0];
        feature[OsduSeismicTraceDataLiterals::Geometry][OsduSeismicTraceDataLiterals::Coordinates].clear();
    }

    void clearSpatialAreaProperties()
    {
        auto& asIngested = root_[OsduFieldNameLiterals::Data][OsduSeismicTraceDataLiterals::SpatialArea][OsduFieldNameLiterals::AsIngestedCoordinates];
        asIngested[OsduSeismicTraceDataLiterals::Features][0][OsduSeismicTraceDataLiterals::Properties].clear();
    }

    void clearMeta()
    {
        auto& meta = root_[OsduFieldNameLiterals::Meta];
        meta.clear();
    }

    void clearTraceDomainAttributes(const TraceDomainUOM& uom) {
        auto& data = root_[OsduFieldNameLiterals::Data];
        data[OsduSeismicTraceDataLiterals::SampleInterval] = 12345.6;
        data[OsduSeismicTraceDataLiterals::TraceLength] = 12345.6;
        if (uom == TraceDomainUOM::meter or uom == TraceDomainUOM::feet) {
            // Depth
            data[OsduSeismicTraceDataLiterals::StartDepth] = 12345.6;
            data[OsduSeismicTraceDataLiterals::EndDepth] = 12345.6;
        }
        else {
            // Time
            data[OsduSeismicTraceDataLiterals::StartTime] = 12345.6;
            data[OsduSeismicTraceDataLiterals::EndTime] = 12345.6;
        }
    }

    void setSeismicDomainTypeId(const TraceDomainUOM uom)
    {
        const auto id = root_[OsduFieldNameLiterals::Id].asString();
        auto pos = id.find(":");
        const auto partition = pos != std::string::npos
                                   ? id.substr(0, pos)
                                   : "noPartition";

        auto& data = root_[OsduFieldNameLiterals::Data];
        auto domainValue = data[OsduSeismicTraceDataLiterals::SeismicDomainTypeID].asString();
        if (domainValue.empty()) {
            domainValue = partition + ":reference-data--SeismicDomainType:Time:";
        }
        if (uom == TraceDomainUOM::meter or uom == TraceDomainUOM::feet) {
            if (domainValue.find(":Depth:") == std::string::npos) {
                segysdk::utilities::StringUtils::ReplaceAll(domainValue, ":Time:", ":Depth:");
            }
        }
        else if (uom == TraceDomainUOM::seconds or uom == TraceDomainUOM::milliseconds or uom == TraceDomainUOM::microseconds) {
            if (domainValue.find(":Time:") == std::string::npos) {
                segysdk::utilities::StringUtils::ReplaceAll(domainValue, ":Depth:", ":Time:");
            }
        }
        data[OsduSeismicTraceDataLiterals::SeismicDomainTypeID] = domainValue;
    }

    void setMetaUom(const osdu::TraceDomainUOM& uom, std::string property)
    {
        updateMetaWithUOM(uom, property);
    }
};

class OsduSeismicTraceDataTests : public testing::Test
{
public:
    void SetUp() override
    {
        loadSeismicTraceData();
        prepareTestData();
    }

    void updateSeismicTraceData()
    {
        seismicTraceData_->checkAndUpdateMissingFields(index_, grid_);
    }

    void checkInitialValues()
    {
        double const defaultDouble = 12345.6;
        EXPECT_DOUBLE_EQ(inlineMin(), defaultDouble);
        EXPECT_DOUBLE_EQ(inlineMax(), defaultDouble);
        EXPECT_DOUBLE_EQ(inlineIncrement(), defaultDouble);

        EXPECT_DOUBLE_EQ(crosslineMin(), defaultDouble);
        EXPECT_DOUBLE_EQ(crosslineMax(), defaultDouble);
        EXPECT_DOUBLE_EQ(crosslineIncrement(), defaultDouble);

        EXPECT_DOUBLE_EQ(sampleCount(), 2);
        EXPECT_DOUBLE_EQ(sampleInterval(), defaultDouble);

        EXPECT_DOUBLE_EQ(startTime(), defaultDouble);
        EXPECT_DOUBLE_EQ(endTime(), defaultDouble);

        EXPECT_DOUBLE_EQ(startDepth(), defaultDouble);
        EXPECT_DOUBLE_EQ(endDepth(), defaultDouble);

        EXPECT_EQ(traceUOM(), "namespace:reference-data--UnitOfMeasure:m:");
        EXPECT_DOUBLE_EQ(traceLength(), defaultDouble);

        auto coordinates = spatialAreaCoordinates();
        EXPECT_DOUBLE_EQ(coordinates[0].asDouble(), defaultDouble);
        EXPECT_DOUBLE_EQ(coordinates[1].asDouble(), defaultDouble);

        auto properties = spatialAreaProperties();
        EXPECT_TRUE(properties.empty());

        EXPECT_STREQ(kindVersion().c_str(), "1.0.0");

        // Check meta and some entries are present
        EXPECT_EQ(getMeta().size(), 3);
    }

    std::string kindVersion() const
    {
        return seismicTraceData_->kind().version();
    }

    double inlineMin() const
    {
        return seismicTraceData_->getJsonRoot()["data"][OsduSeismicTraceDataLiterals::InlineMin].asDouble();
    }

    double inlineMax() const
    {
        return seismicTraceData_->getJsonRoot()["data"][OsduSeismicTraceDataLiterals::InlineMax].asDouble();
    }

    double inlineIncrement() const
    {
        return seismicTraceData_->getJsonRoot()["data"][OsduSeismicTraceDataLiterals::InlineIncrement].asDouble();
    }

    double crosslineMin() const
    {
        return seismicTraceData_->getJsonRoot()["data"][OsduSeismicTraceDataLiterals::CrosslineMin].asDouble();
    }

    double crosslineMax() const
    {
        return seismicTraceData_->getJsonRoot()["data"][OsduSeismicTraceDataLiterals::CrosslineMax].asDouble();
    }

    double crosslineIncrement() const
    {
        return seismicTraceData_->getJsonRoot()["data"][OsduSeismicTraceDataLiterals::CrosslineIncrement].asDouble();
    }

    double sampleCount() const
    {
        return seismicTraceData_->getJsonRoot()["data"][OsduSeismicTraceDataLiterals::SampleCount].asDouble();
    }

    double sampleInterval() const
    {
        return seismicTraceData_->getJsonRoot()["data"][OsduSeismicTraceDataLiterals::SampleInterval].asDouble();
    }

    double startTime() const
    {
        return seismicTraceData_->getJsonRoot()["data"][OsduSeismicTraceDataLiterals::StartTime].asDouble();
    }

    double startDepth() const
    {
        return seismicTraceData_->getJsonRoot()["data"][OsduSeismicTraceDataLiterals::StartDepth].asDouble();
    }

    double endTime() const
    {
        return seismicTraceData_->getJsonRoot()["data"][OsduSeismicTraceDataLiterals::EndTime].asDouble();
    }

    double endDepth() const
    {
        return seismicTraceData_->getJsonRoot()["data"][OsduSeismicTraceDataLiterals::EndDepth].asDouble();
    }

    double traceLength() const
    {
        return seismicTraceData_->getJsonRoot()["data"][OsduSeismicTraceDataLiterals::TraceLength].asDouble();
    }

    std::string traceUOM() const
    {
        return seismicTraceData_->getJsonRoot()["data"][OsduSeismicTraceDataLiterals::TraceDomainUOM].asString();
    }

    Json::Value spatialAreaCoordinates() const
    {
        auto asIngested = seismicTraceData_->getJsonRoot()["data"][OsduSeismicTraceDataLiterals::SpatialArea][OsduFieldNameLiterals::AsIngestedCoordinates];
        auto feature = asIngested[OsduSeismicTraceDataLiterals::Features][0];
        return feature[OsduSeismicTraceDataLiterals::Geometry][OsduSeismicTraceDataLiterals::Coordinates];
    }

    Json::Value spatialAreaProperties() const
    {
        auto asIngested = seismicTraceData_->getJsonRoot()["data"][OsduSeismicTraceDataLiterals::SpatialArea][OsduFieldNameLiterals::AsIngestedCoordinates];
        return asIngested[OsduSeismicTraceDataLiterals::Features][0][OsduSeismicTraceDataLiterals::Properties];
    }

    Json::Value getMeta() const
    {
        return seismicTraceData_->getJsonRoot()[OsduFieldNameLiterals::Meta];
    }

    void clearSpatialAreaCoordinates()
    {
        seismicTraceData_->clearSpatialAreaCoordinates();
    }

    void clearMeta()
    {
        seismicTraceData_->clearMeta();
    }

    void clearTraceDomainAttributes(const TraceDomainUOM& uom)
    {
        seismicTraceData_->clearTraceDomainAttributes(uom);
    }

    void updateSeismicDomainType(const TraceDomainUOM& uom) 
    {
        seismicTraceData_->setSeismicDomainTypeId(uom);
    }

    void setMetaUom(const osdu::TraceDomainUOM& uom, std::string property)
    {
        seismicTraceData_->setMetaUom(uom, property);
    }

    const auto& getIndex() const
    {
        return index_;
    }

    const auto& getGrid() const
    {
        return grid_;
    }

    std::map<std::string, double>
    getProperties(const osdu::TraceDomainUOM& uom) const 
    {
        if (uom == TraceDomainUOM::meter or uom == TraceDomainUOM::feet) {
            // Depth domain
            return {
                {OsduSeismicTraceDataLiterals::StartDepth, 0.0},
                {OsduSeismicTraceDataLiterals::EndDepth, 8.192},
                {OsduSeismicTraceDataLiterals::TraceLength, 8.192},
                {OsduSeismicTraceDataLiterals::SampleInterval, 0.032},
            };
        }

        // Time domain
        return {
            {OsduSeismicTraceDataLiterals::StartTime, 0.0},
            {OsduSeismicTraceDataLiterals::EndTime, 8.192},
            {OsduSeismicTraceDataLiterals::TraceLength, 8.192},
            {OsduSeismicTraceDataLiterals::SampleInterval, 0.032},
        };
    }

 private:
    void loadSeismicTraceData()
    {
        auto const filePath = segysdk::utilities::Environment::instance().SegyLocalTestDataPath();
        const auto inputFilename = std::filesystem::path{filePath} / "OSDU" / "SeismicTraceData.1.0.0.json";
        auto jsonData = segysdk::utilities::JsonUtilities::GetJsonObjectFromFile(inputFilename.string());
        seismicTraceData_ = std::make_shared<OsduSeismicTraceDataMock>(jsonData);
    }

    void prepareTestData()
    {
        const AutoCleanupTestFile temp_file("tiny.sgy");
        temp_file.create_index();
        const auto segy_session = api::SegySessionFactory({temp_file.get_path()}, temp_file.get_segy_config_json_string()).CreateSegySession();
        segy_session->loadIndexFile();
        index_ = segy_session->getIndex();

        // Update the fields based on an index and bin grid
        auto const filePath = segysdk::utilities::Environment::instance().SegyLocalTestDataPath();
        const auto inputFilename = std::filesystem::path{filePath} / "BinGrid" / "osdu" / "SeismicBinGrid.1.0.0.json";
        const auto jsonBinGrid = segysdk::utilities::JsonBinGrid::loadBinGridFromFile(inputFilename.string());
        grid_ = std::make_shared<segysdk::utilities::BinGrid>(jsonBinGrid);
    }

private:
    std::shared_ptr<OsduSeismicTraceDataMock> seismicTraceData_ = nullptr;
    std::shared_ptr<segysdk::utilities::BinGrid> grid_ = nullptr;
    std::shared_ptr<segysdk::segyindex::SegyIndex> index_ = nullptr;
};

TEST_F(OsduSeismicTraceDataTests, initialFile)
{
    checkInitialValues();
}

TEST_F(OsduSeismicTraceDataTests, seismicDataUpdate)
{
    // Clear present coordinates from the Seismic Trace Data
    clearSpatialAreaCoordinates();
    // Clear meta that was loaded
    clearMeta();
    updateSeismicTraceData();

    EXPECT_DOUBLE_EQ(inlineMin(), 3990);
    EXPECT_DOUBLE_EQ(inlineMax(), 4246);
    EXPECT_DOUBLE_EQ(inlineIncrement(), 256);

    EXPECT_DOUBLE_EQ(crosslineMin(), 3850);
    EXPECT_DOUBLE_EQ(crosslineMax(), 8714);
    EXPECT_DOUBLE_EQ(crosslineIncrement(), 256);

    EXPECT_DOUBLE_EQ(sampleCount(), 256);
    EXPECT_DOUBLE_EQ(sampleInterval(), 32.);

    EXPECT_DOUBLE_EQ(startTime(), 0.0);
    EXPECT_DOUBLE_EQ(endTime(), 8192.);

    EXPECT_DOUBLE_EQ(traceLength(), 8192.);
    EXPECT_EQ(traceUOM(), "namespace:reference-data--UnitOfMeasure:m:");

    auto meta = getMeta();
    EXPECT_EQ(meta.size(), 1);
    EXPECT_EQ(meta[0][OsduSeismicTraceMetaLiterals::unitOfMeasureID].asString(), traceUOM());
    const char* const persistableReference = "{\"abcd\":{\"a\":0.0,\"b\":1.0,\"c\":1.0,\"d\":0.0},\"symbol\":\"m\",\"baseMeasurement\":{\"ancestry\":\"L\",\"type\":\"UM\"},\"type\":\"UAD\"}";
    EXPECT_EQ(meta[0][OsduSeismicTraceMetaLiterals::persistableReference].asString(), persistableReference);
    EXPECT_EQ(meta[0][OsduFieldNameLiterals::Name].asString(), "m");
    EXPECT_EQ(meta[0][OsduFieldNameLiterals::Kind].asString(), "Unit");

    std::list<std::string> expected = {OsduSeismicTraceDataLiterals::StartTime, OsduSeismicTraceDataLiterals::EndTime,
                                         OsduSeismicTraceDataLiterals::TraceLength, OsduSeismicTraceDataLiterals::SampleInterval};
    const auto& metaPropeties = meta[0][OsduSeismicTraceMetaLiterals::propertyNames];
    EXPECT_EQ(metaPropeties.size(), expected.size());
    for (const auto& property : metaPropeties) {
        EXPECT_TRUE(std::find(expected.begin(), expected.end(), property.asString()) != expected.end()) << property.asString() + ": unexpected";
    }

    auto coordinates = spatialAreaCoordinates()[0];
    auto gridCells = getIndex()->getGeometry3D().dataGrid;

    constexpr int polygonSize = 5;
    EXPECT_EQ(coordinates.size(), polygonSize);

    const auto grid = getGrid();
    auto coordinates11 = grid->getCoordinatesIJ(gridCells.gridCell_11.i, gridCells.gridCell_11.j);
    EXPECT_DOUBLE_EQ(coordinates[0][0].asDouble(), coordinates11.easting);
    EXPECT_DOUBLE_EQ(coordinates[0][1].asDouble(), coordinates11.northing);

    auto coordinatesN1 = grid->getCoordinatesIJ(gridCells.gridCell_N1.i, gridCells.gridCell_N1.j);
    EXPECT_DOUBLE_EQ(coordinates[1][0].asDouble(), coordinatesN1.easting);
    EXPECT_DOUBLE_EQ(coordinates[1][1].asDouble(), coordinatesN1.northing);

    auto coordinatesNN = grid->getCoordinatesIJ(gridCells.gridCell_NN.i, gridCells.gridCell_NN.j);
    EXPECT_DOUBLE_EQ(coordinates[2][0].asDouble(), coordinatesNN.easting);
    EXPECT_DOUBLE_EQ(coordinates[2][1].asDouble(), coordinatesNN.northing);

    auto coordinates1N = grid->getCoordinatesIJ(gridCells.gridCell_1N.i, gridCells.gridCell_1N.j);
    EXPECT_DOUBLE_EQ(coordinates[3][0].asDouble(), coordinates1N.easting);
    EXPECT_DOUBLE_EQ(coordinates[3][1].asDouble(), coordinates1N.northing);

    EXPECT_DOUBLE_EQ(coordinates[4][0].asDouble(), coordinates11.easting);
    EXPECT_DOUBLE_EQ(coordinates[4][1].asDouble(), coordinates11.northing);

    auto properties = spatialAreaProperties();
    EXPECT_STREQ(properties["kind"].asCString(), "osdu:wks:AbstractGeoJson.PropertiesBinGridCorners:1.0.0");

    auto& pointProperties = properties[OsduSeismicTraceDataLiterals::PointProperties];
    EXPECT_EQ(pointProperties.size(), polygonSize);
    EXPECT_EQ(pointProperties[0][OsduSeismicTraceDataLiterals::Inline].asInt(), gridCells.gridCell_11.i);
    EXPECT_EQ(pointProperties[0][OsduSeismicTraceDataLiterals::Crossline].asInt(), gridCells.gridCell_11.j);

    EXPECT_EQ(pointProperties[1][OsduSeismicTraceDataLiterals::Inline].asInt(), gridCells.gridCell_N1.i);
    EXPECT_EQ(pointProperties[1][OsduSeismicTraceDataLiterals::Crossline].asInt(), gridCells.gridCell_N1.j);

    EXPECT_EQ(pointProperties[2][OsduSeismicTraceDataLiterals::Inline].asInt(), gridCells.gridCell_NN.i);
    EXPECT_EQ(pointProperties[2][OsduSeismicTraceDataLiterals::Crossline].asInt(), gridCells.gridCell_NN.j);

    EXPECT_EQ(pointProperties[3][OsduSeismicTraceDataLiterals::Inline].asInt(), gridCells.gridCell_1N.i);
    EXPECT_EQ(pointProperties[3][OsduSeismicTraceDataLiterals::Crossline].asInt(), gridCells.gridCell_1N.j);

    EXPECT_EQ(pointProperties[4][OsduSeismicTraceDataLiterals::Inline].asInt(), gridCells.gridCell_11.i);
    EXPECT_EQ(pointProperties[4][OsduSeismicTraceDataLiterals::Crossline].asInt(), gridCells.gridCell_11.j);

    EXPECT_STREQ(kindVersion().c_str(), "1.4.0");
}

TEST_F(OsduSeismicTraceDataTests, conversionUsingUOMfromMeta)
{
    // Clear present coordinates from the Seismic Trace Data
    clearSpatialAreaCoordinates();
    // Clear meta that was loaded
    clearMeta();

    std::map<osdu::TraceDomainUOM, double> testSet = {
        {osdu::TraceDomainUOM::feet, 1000.},
        {osdu::TraceDomainUOM::meter, 1000.},
        {osdu::TraceDomainUOM::microseconds, 1'000'000.},
        {osdu::TraceDomainUOM::milliseconds, 1000.},
        {osdu::TraceDomainUOM::seconds, 1.}};

    for (const auto& [uom, ratio] : testSet) {
        std::map<std::string, double> properties = getProperties(uom); 

        // Ensure all required properties and their uoms are captured in meta[]
        for (const auto& [property, value] : properties) {
            setMetaUom(uom, property);
        }
        updateSeismicDomainType(uom);
        updateSeismicTraceData();

        EXPECT_DOUBLE_EQ(sampleInterval(), properties[OsduSeismicTraceDataLiterals::SampleInterval] * ratio);
        if (uom == TraceDomainUOM::meter or uom == TraceDomainUOM::feet) {
            EXPECT_DOUBLE_EQ(startDepth(), properties[OsduSeismicTraceDataLiterals::StartDepth] * ratio);
            EXPECT_DOUBLE_EQ(endDepth(), properties[OsduSeismicTraceDataLiterals::EndDepth] * ratio);
            // the following attributes should not have been set
            EXPECT_DOUBLE_EQ(startTime(), 12345.6);
            EXPECT_DOUBLE_EQ(endTime(), 12345.6);
        }
        else {
            EXPECT_DOUBLE_EQ(startTime(), properties[OsduSeismicTraceDataLiterals::StartTime] * ratio);
            EXPECT_DOUBLE_EQ(endTime(), properties[OsduSeismicTraceDataLiterals::EndTime] * ratio);
            // the following attributes should not have been set
            EXPECT_DOUBLE_EQ(startDepth(), 12345.6);
            EXPECT_DOUBLE_EQ(endDepth(), 12345.6);
        }

        EXPECT_DOUBLE_EQ(traceLength(), properties[OsduSeismicTraceDataLiterals::TraceLength] * ratio);

        // Some attributes are set, others are not

        clearMeta();
        clearTraceDomainAttributes(uom);
    }

    std::map<std::string, double> properties = {
        {OsduSeismicTraceDataLiterals::StartTime, 0.0},
        {OsduSeismicTraceDataLiterals::EndTime, 8.192},
        {OsduSeismicTraceDataLiterals::TraceLength, 8.192},
        {OsduSeismicTraceDataLiterals::SampleInterval, 0.032},
    };

    // Some properties are set, some are not
    setMetaUom(osdu::TraceDomainUOM::feet, OsduSeismicTraceDataLiterals::SampleInterval);
    setMetaUom(osdu::TraceDomainUOM::microseconds, OsduSeismicTraceDataLiterals::EndTime);

    updateSeismicTraceData();

    EXPECT_DOUBLE_EQ(properties[OsduSeismicTraceDataLiterals::SampleInterval] * testSet[osdu::TraceDomainUOM::feet],
                     sampleInterval());
    EXPECT_DOUBLE_EQ(properties[OsduSeismicTraceDataLiterals::EndTime] * testSet[osdu::TraceDomainUOM::microseconds],
                     endTime());
    EXPECT_DOUBLE_EQ(properties[OsduSeismicTraceDataLiterals::StartTime],
                     startTime());

    // Data Trace UoM will be used which is in m
    // index is time based and in seconds
    EXPECT_DOUBLE_EQ(properties[OsduSeismicTraceDataLiterals::TraceLength] * testSet[osdu::TraceDomainUOM::meter],
                     traceLength());
}

} // namespace segysdk::utilities::osdu::tests