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

#include "OsduSeismicTraceData140.h"

#include "../Logger/Logger.h"
#include "../Logger/MessageGSM.h"
#include "../SegyIndex/GridCell.h"
#include "../Utilities/Authorization.h"
#include "../Utilities/BinGrid.h"
#include "../Utilities/JsonBinGrid.h"
#include "../Utilities/StorageSvcClient.h"
#include "../Utilities/StringUtils.h"
#include "OsduFieldNameLiterals.h"

#include <array>
#include <limits>
#include <math.h>

namespace segysdk::utilities::osdu
{

OsduSeismicTraceData140::OsduSeismicTraceData140(const Json::Value& root)
  : OsduSeismicTraceData100(root)
{
}

void
OsduSeismicTraceData140::checkAndUpdateMissingFields(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index,
                                                     const std::shared_ptr<segysdk::utilities::BinGrid>& grid)
{
    checkAndUpdateSpatialArea(index, grid);
    updateTraceDataFields(index);

    updateVersionToCurrent();
}

void
OsduSeismicTraceData140::updateVersionToCurrent()
{
    root_[OsduFieldNameLiterals::Kind] = kind().fullTypeName() + ":" + std::string(version_);
}

std::string 
getPartitionFromId(std::string id)
{
    const auto parts = segysdk::utilities::StringUtils::split(id, ":");
    if (parts.size() >= 2 and parts.size() <= 3) {
        return parts[0];
    }
    return "NoOsduPartition";
}

std ::string
getUoMString(TraceDomainUOM uom)
{
    switch (uom) {
    case TraceDomainUOM::meter:
        return "m";
    case TraceDomainUOM::feet:
        return "ft";
    case TraceDomainUOM::milliseconds:
        return "ms";
    case TraceDomainUOM::microseconds:
        return "us";
    case TraceDomainUOM::seconds:
        return "s";
    case TraceDomainUOM::undefined:
        return "undefined";
    }
    return "undefined";
}
    
void
OsduSeismicTraceData140::updateTraceDataFields(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index)
{
    auto& data = root_[OsduFieldNameLiterals::Data];
    const auto inlineExtent = index->getGeometry3DExtents().getJVectorExtent();
    data[OsduSeismicTraceDataLiterals::InlineMin] = inlineExtent.first();
    data[OsduSeismicTraceDataLiterals::InlineMax] = inlineExtent.last();
    data[OsduSeismicTraceDataLiterals::InlineIncrement] = inlineExtent.step();

    const auto crosslineExtent = index->getGeometry3DExtents().getIVectorExtent();
    data[OsduSeismicTraceDataLiterals::CrosslineMin] = crosslineExtent.first();
    data[OsduSeismicTraceDataLiterals::CrosslineMax] = crosslineExtent.last();
    data[OsduSeismicTraceDataLiterals::CrosslineIncrement] = crosslineExtent.step();

    const auto sampleExtent = index->getGeometry3DExtents().getSampleExtent();
    data[OsduSeismicTraceDataLiterals::SampleCount] = sampleExtent.count();

    auto depth = sampleExtent.first() + sampleExtent.count() * sampleExtent.step();
    if (std::fabs(depth - sampleExtent.last()) > 1E-6) { // good enough resolution of micro (s, m, ft)
        SEGYLOG_REPORT_ERROR("Calculated depth is not equal to Last Sample");
        return;
    }
    auto indexDomain = sampleExtent.domain();
    TraceDomainUOM indexUom = getTraceDomainUOMfromSegy(sampleExtent.zUoM());

    const auto targetUoM = getUoMString(getMetaTraceDomainUom("SampleInterval"));
    if (targetUoM != "undefined") {
        const auto partition = getPartitionFromId(root_[OsduFieldNameLiterals::Id].asString());
        data[OsduSeismicTraceDataLiterals::TraceDomainUOM] = partition + ":" + "reference-data--UnitOfMeasure:" + targetUoM + ":";
    }

    // Update the trace domain related fields.
    // Take the values from the SegyIndex, and unit convert to the UoM specified in the meta section
    // for the trace domain attributes: 
    // TraceLength, StartTime, EndTime and SampleInterval for Time domain or
    // TraceLength, StartDepth, EndDepth and SampleInterval for Depth domain.

    // Key challenge: What to do if one or all of these units are missing from Meta?
    // Ideally we would like to find one MetaElement where we have at least one of the 
    // trace based properties defined, and add missing property to that one. 
    // Otherwise create a new meta element holding all relevant properties.

    // In the SegyIndex, trace geometry is stored in seconds for time based seismic, or in 
    // meter or feet if it is depth based seismic.
    // Time based data is likely to want trace based properties stored in a more useful UoM, 
    // most likely as millisecods.
    auto setProperty = [this, &data, &indexUom, &indexDomain](std::string property, double value) {
        TraceDomainUOM targetUom = getMetaTraceDomainUom(property);
        double resultValue = value;
        if (targetUom == TraceDomainUOM::undefined) {
            // Not present in meta section, check data section:
            targetUom = getDataTraceDomainUom();
            if (targetUom == TraceDomainUOM::undefined) {
                // Not set in the data section as well. Update to the index one
                targetUom = indexUom;
                data[OsduSeismicTraceDataLiterals::TraceDomainUOM] = getTraceDomainUomString(indexUom);
            }
            updateMetaWithUOM(targetUom, property);
        }
        // If targetUoM from Meta or Data sections differs from what we have in index
        if (targetUom != indexUom) {
            // This is wrong UoM and should be reported
            // Units of counted value from Segy and in meta differ

            if (indexDomain == segyio::VerticalDomain::Depth) {
                segysdk::logger::MessageGSM msg{"SeismicTraceData record: UoM for " + property + " is specified as " + getTraceDomainUomString(targetUom) + ", while the SEGY is " + getTraceDomainUomString(indexUom)};
                SEGYLOG_GSM_ERROR(msg);

                switch (targetUom) {
                case TraceDomainUOM::microseconds:
                    resultValue = value * 1000;
                    break;
                case TraceDomainUOM::seconds:
                    resultValue = value / 1000;
                    break;
                default:
                    break;
                }
            }
            else {
                // The index stores sample extent values as seconds
                // myValue will be returned as 1000 * ms
                const auto myValue = getSEGYValue(value);
                switch (targetUom) {                  
                case TraceDomainUOM::microseconds:
                    resultValue = myValue;
                    break;
                case TraceDomainUOM::milliseconds:
                    resultValue = myValue / 1000;
                    break;
                case TraceDomainUOM::meter:
                case TraceDomainUOM::feet:
                    resultValue = myValue / 1000;
                    break;
                default:
                    break;
                }
            }
        }
        data[property] = resultValue;
    };
  
    auto domain = getTraceDomain();
    if (domain == TraceDomain::Undefined) {
        domain = indexDomain == segyio::VerticalDomain::Depth ? TraceDomain::Depth : TraceDomain::Time;
    }
    if (domain == TraceDomain::Depth) {
        setProperty(OsduSeismicTraceDataLiterals::StartDepth, sampleExtent.first());
        setProperty(OsduSeismicTraceDataLiterals::EndDepth, sampleExtent.last());
    }
    else {
        setProperty(OsduSeismicTraceDataLiterals::StartTime, sampleExtent.first());
        setProperty(OsduSeismicTraceDataLiterals::EndTime, sampleExtent.last());
    }
    setProperty(OsduSeismicTraceDataLiterals::SampleInterval, sampleExtent.step());
    setProperty(OsduSeismicTraceDataLiterals::TraceLength, sampleExtent.last() - sampleExtent.first());
}

double
OsduSeismicTraceData140::getSEGYValue(double value)
{
    return round_up(value * 1'000'000, 0);
} 

void
OsduSeismicTraceData140::checkAndUpdateSpatialArea(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index,
                                                   const std::shared_ptr<segysdk::utilities::BinGrid>& grid)
{
    auto& data = root_[OsduFieldNameLiterals::Data];

    if (not data.isMember(OsduSeismicTraceDataLiterals::SpatialArea)) {
        SEGYLOG_WARNING("No Spatial Area in Seismic Trace Data");
        return;
    }

    auto& spatialAreaJson = data[OsduSeismicTraceDataLiterals::SpatialArea];
    if (not spatialAreaJson.isMember(OsduFieldNameLiterals::AsIngestedCoordinates)) {
        SEGYLOG_WARNING("No AsIngestedCoordinates in Spatial Area of Seismic Trace Data");
        return;
    }
    auto& asIngested = spatialAreaJson[OsduFieldNameLiterals::AsIngestedCoordinates];
    // This field is required and should be taken from the Bin Grid if not present
    if (not asIngested.isMember(OsduSeismicTraceDataLiterals::PersistableReferenceCrs)) {
        asIngested[OsduSeismicTraceDataLiterals::PersistableReferenceCrs] = grid->persistableReferenceCrs;
    }

    if (not asIngested.isMember(OsduSeismicTraceDataLiterals::Features)) {
        SEGYLOG_WARNING("No features in Spatial Area of Seismic Trace Data");
        return;
    }

    auto& feature = asIngested[OsduSeismicTraceDataLiterals::Features][0];

    enum class Features {
        Points,
        Coordinates
    };
    auto getPointsJsonArray = [this, &index, &grid](Features f) {
        Json::Value jsonPolygon(Json::arrayValue);
        auto dataGrid = index->getGeometry3D().dataGrid;
        // This is a polygon in counter-clockwise order according to GeoJson format
        std::array<segysdk::segyindex::GridCell, 5> corners = {dataGrid.gridCell_11, dataGrid.gridCell_N1, dataGrid.gridCell_NN, dataGrid.gridCell_1N, dataGrid.gridCell_11};
        if (f == Features::Points) {
            std::for_each(corners.begin(), corners.end(), [&jsonPolygon](const segysdk::segyindex::GridCell& cell) {
                Json::Value point;
                point[OsduSeismicTraceDataLiterals::Inline] = cell.i;
                point[OsduSeismicTraceDataLiterals::Crossline] = cell.j;
                jsonPolygon.append(point);
            });
        }
        else {
            std::for_each(corners.begin(), corners.end(), [&jsonPolygon, &grid](const segysdk::segyindex::GridCell& cell) {
                Json::Value coordinate(Json::arrayValue);
                auto gridCoordinate = grid->getCoordinatesIJ(cell.i, cell.j);
                coordinate.append(gridCoordinate.easting);
                coordinate.append(gridCoordinate.northing);
                jsonPolygon.append(coordinate);
            });
        }
        return jsonPolygon;
    };
    // Coordinates
    if (not feature.isMember(OsduSeismicTraceDataLiterals::Geometry)) {
        Json::Value geometry;
        feature[OsduSeismicTraceDataLiterals::Geometry] = geometry;
    }
    auto& geometry = feature[OsduSeismicTraceDataLiterals::Geometry];
    if (not geometry.isMember(OsduSeismicTraceDataLiterals::Coordinates) or geometry[OsduSeismicTraceDataLiterals::Coordinates].empty()) {
        geometry[OsduSeismicTraceDataLiterals::Coordinates] = Json::arrayValue;
        geometry[OsduSeismicTraceDataLiterals::Coordinates].append(getPointsJsonArray(Features::Coordinates));
    }
    geometry[OsduSeismicTraceDataLiterals::Type] = "AnyCrsPolygon";

    // PointProperties
    if (not feature.isMember(OsduSeismicTraceDataLiterals::Properties) or feature[OsduSeismicTraceDataLiterals::Properties].empty()) {
        Json::Value properties;
        properties[OsduFieldNameLiterals::Kind] = "osdu:wks:AbstractGeoJson.PropertiesBinGridCorners:1.0.0";
        feature[OsduSeismicTraceDataLiterals::Properties] = properties;
    }
    auto& properties = feature[OsduSeismicTraceDataLiterals::Properties];
    if (not properties.isMember(OsduSeismicTraceDataLiterals::PointProperties)) {
        properties[OsduSeismicTraceDataLiterals::PointProperties] = getPointsJsonArray(Features::Points);
    }
}

void
OsduSeismicTraceData140::updateMetaWithUOM(const TraceDomainUOM& uom, std::string property)
{
    if (not root_.isMember(OsduFieldNameLiterals::Meta)) {
        root_[OsduFieldNameLiterals::Meta] = Json::arrayValue;
    }
    auto& meta = root_[OsduFieldNameLiterals::Meta];

    for (auto& metaEntry : meta) {
        // update properties in the meta if present
        if (getTraceDomainUom(metaEntry[OsduSeismicTraceMetaLiterals::unitOfMeasureID].asString(), false) == uom) {
            if (not metaEntry.isMember(OsduSeismicTraceMetaLiterals::propertyNames)) {
                metaEntry[OsduSeismicTraceMetaLiterals::propertyNames] = Json::arrayValue;
            }
            for (const auto& value : metaEntry[OsduSeismicTraceMetaLiterals::propertyNames]) {
                if (value.asString() == property) {
                    return;
                }
            }
            metaEntry[OsduSeismicTraceMetaLiterals::propertyNames].append(Json::Value(property));
            return;
        }
    }

    Json::Value metaEntry;
    metaEntry[OsduFieldNameLiterals::Kind] = "Unit";
    metaEntry[OsduSeismicTraceMetaLiterals::unitOfMeasureID] = getTraceDomainUomString(uom);
    metaEntry[OsduSeismicTraceMetaLiterals::propertyNames] = Json::arrayValue;

    switch (uom) {
    case TraceDomainUOM::meter:
        metaEntry[OsduSeismicTraceMetaLiterals::persistableReference] = "{\"abcd\":{\"a\":0.0,\"b\":1.0,\"c\":1.0,\"d\":0.0},\"symbol\":\"m\",\"baseMeasurement\":{\"ancestry\":\"L\",\"type\":\"UM\"},\"type\":\"UAD\"}";
        metaEntry[OsduFieldNameLiterals::Name] = "m";
        break;
    case TraceDomainUOM::feet:
        metaEntry[OsduSeismicTraceMetaLiterals::persistableReference] = "{\"abcd\":{\"a\":0.0,\"b\":0.3048,\"c\":1.0,\"d\":0.0},\"symbol\":\"ft\",\"baseMeasurement\":{\"ancestry\":\"L\",\"type\":\"UM\"},\"type\":\"UAD\"}";
        metaEntry[OsduFieldNameLiterals::Name] = "ft";
        break;
    case TraceDomainUOM::microseconds:
        metaEntry[OsduSeismicTraceMetaLiterals::persistableReference] = "{\"abcd\":{\"a\":0.0,\"b\":1e-06,\"c\":1.0,\"d\":0.0},\"symbol\":\"us\",\"baseMeasurement\":{\"ancestry\":\"T\",\"type\":\"UM\"},\"type\":\"UAD\"}";
        metaEntry[OsduFieldNameLiterals::Name] = "us";
        break;
    case TraceDomainUOM::milliseconds:
        metaEntry[OsduSeismicTraceMetaLiterals::persistableReference] = "{\"abcd\":{\"a\":0.0,\"b\":0.001,\"c\":1.0,\"d\":0.0},\"symbol\":\"ms\",\"baseMeasurement\":{\"ancestry\":\"T\",\"type\":\"UM\"},\"type\":\"UAD\"}";
        metaEntry[OsduFieldNameLiterals::Name] = "ms";
        break;
    case TraceDomainUOM::seconds:
        metaEntry[OsduSeismicTraceMetaLiterals::persistableReference] = "{\"abcd\":{\"a\":0.0,\"b\":1.0,\"c\":1.0,\"d\":0.0},\"symbol\":\"s\",\"baseMeasurement\":{\"ancestry\":\"T\",\"type\":\"UM\"},\"type\":\"UAD\"}";
        metaEntry[OsduFieldNameLiterals::Name] = "s";
        break;
    default:
        SEGYLOG_REPORT_ERROR("Unknown Trace Domain UOM.");
        return;
        break;
    }
    metaEntry[OsduSeismicTraceMetaLiterals::propertyNames].append(Json::Value(property));

    meta.append(metaEntry);
}

} // namespace segysdk::utilities::osdu
