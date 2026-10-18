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

#include "OsduSeismicTraceData100.h"
#include "OsduFieldNameLiterals.h"

#include "../FileIO/GenericFileManager.h"
#include "../Logger/Logger.h"
#include "../Utilities/StringUtils.h"

#include <json/json.h>
#include <algorithm>
#include <cmath>

namespace segysdk::utilities::osdu
{

OsduSeismicTraceData100::OsduSeismicTraceData100(const Json::Value& root)
  : OsduObject(root)
{
}

std::string
OsduSeismicTraceData100::binGridId() const
{
    return root_[OsduFieldNameLiterals::Data][OsduFieldNameLiterals::BinGridID].asString();
}

std::vector<std::string>
OsduSeismicTraceData100::datasetIds() const
{
    std::vector<std::string> ret;
    for (const auto& id : root_[OsduFieldNameLiterals::Data][OsduFieldNameLiterals::Datasets]) {
        ret.push_back(id.asString());
    }
    return ret;
}

std::vector<std::string>
OsduSeismicTraceData100::artefactIds() const
{
    const auto& data = root_[OsduFieldNameLiterals::Data];
    std::vector<std::string> ret;
    for (const auto& artefact : data[OsduFieldNameLiterals::Artefacts]) {
        ret.push_back(artefact[OsduFieldNameLiterals::ResourceID].asString());
    }
    return ret;
}

void
OsduSeismicTraceData100::addCompletedOpenZGYDataset(const std::string& newId, const OsduKind& newObjectKind)
{
    // TODO handle if there is already a converted dataset
    auto& data = root_[OsduFieldNameLiterals::Data];
    if (!data.isMember(OsduFieldNameLiterals::Artefacts)) {
        data[OsduFieldNameLiterals::Artefacts] = Json::arrayValue;
    }
    Json::Value newArtefact;
    newArtefact[OsduFieldNameLiterals::RoleID] = ns() + ":reference-data--ArtefactRole:ConvertedContent:";
    newArtefact[OsduFieldNameLiterals::ResourceKind] = newObjectKind.kind();
    newArtefact[OsduFieldNameLiterals::ResourceID] = newId + ":";
    data[OsduFieldNameLiterals::Artefacts].append(newArtefact);
}

void
OsduSeismicTraceData100::addLiveTraceOutline(const Json::Value& jsonPolygons, bool isMultiple)
{
    auto& data = root_[OsduFieldNameLiterals::Data];
    if (!data.isMember(OsduFieldNameLiterals::LiveTraceOutline)) {
        data[OsduFieldNameLiterals::LiveTraceOutline] = Json::objectValue;
    }
    auto& liveTrace = data[OsduFieldNameLiterals::LiveTraceOutline];
    if (!liveTrace.isMember(OsduFieldNameLiterals::AsIngestedCoordinates)) {
        liveTrace[OsduFieldNameLiterals::AsIngestedCoordinates] = Json::objectValue;
    }
    liveTrace[OsduFieldNameLiterals::AsIngestedCoordinates][OsduSeismicTraceDataLiterals::Type] = "AnyCrsFeatureCollection";

    auto& ingestedCoords = liveTrace[OsduFieldNameLiterals::AsIngestedCoordinates];
    if (!ingestedCoords.isMember(OsduSeismicTraceDataLiterals::Features)) {
        ingestedCoords[OsduSeismicTraceDataLiterals::Features] = Json::arrayValue;
    }
    // Clear any previous data
    ingestedCoords[OsduSeismicTraceDataLiterals::Features].clear();

    Json::Value anyCrsFeature;
    anyCrsFeature[OsduSeismicTraceDataLiterals::Type] = "AnyCrsFeature";
    anyCrsFeature[OsduSeismicTraceDataLiterals::Properties] = Json::objectValue;
    anyCrsFeature[OsduSeismicTraceDataLiterals::Geometry] = Json::objectValue;
    
    auto& geometry = anyCrsFeature[OsduSeismicTraceDataLiterals::Geometry];
    geometry[OsduSeismicTraceDataLiterals::Type] = isMultiple ? "AnyCrsMultiPolygon" : "AnyCrsPolygon";
    geometry[OsduSeismicTraceDataLiterals::Coordinates] = jsonPolygons;

    ingestedCoords[OsduSeismicTraceDataLiterals::Features].append(anyCrsFeature);

    //
    // Copy CRS from SpatialArea
    if (!data.isMember(OsduSeismicTraceDataLiterals::SpatialArea)) {
        SEGYLOG_REPORT_WARNING("updateSeismicTraceData; addLiveTraceOutline: No spatial area in seismic trace data");
        return;
    }

    auto& spatialAreaJson = data[OsduSeismicTraceDataLiterals::SpatialArea];
    if (spatialAreaJson.isMember(OsduFieldNameLiterals::AsIngestedCoordinates)) {
        auto& asIngested = spatialAreaJson[OsduFieldNameLiterals::AsIngestedCoordinates];
        // This field is required and should be in Spatial Area
        ingestedCoords[OsduSeismicTraceDataLiterals::PersistableReferenceCrs] = asIngested[OsduSeismicTraceDataLiterals::PersistableReferenceCrs];
        if (asIngested.isMember(OsduSeismicTraceDataLiterals::CoordinateReferenceSystemID)) {
            ingestedCoords[OsduSeismicTraceDataLiterals::CoordinateReferenceSystemID] = asIngested[OsduSeismicTraceDataLiterals::CoordinateReferenceSystemID];
        }
    }
    else {
        SEGYLOG_REPORT_WARNING("updateSeismicTraceData; addLiveTraceOutline: No ingested coordinates in spatial area");
        return;
    }
}

std::string
OsduSeismicTraceData100::getTraceDomainUoMString(const std::string& referenceUom)
{
    if (segysdk::utilities::StringUtils::keyEndsWith(referenceUom, "reference-data--UnitOfMeasure:m:")) {
        return "m";
    }
    if (segysdk::utilities::StringUtils::keyEndsWith(referenceUom, "reference-data--UnitOfMeasure:ft:")) {
        return "ft";
    }
    if (segysdk::utilities::StringUtils::keyEndsWith(referenceUom, "reference-data--UnitOfMeasure:s:")) {
        return "s";
    }
    if (segysdk::utilities::StringUtils::keyEndsWith(referenceUom, "reference-data--UnitOfMeasure:ms:")) {
        return "ms";
    }
    else if (segysdk::utilities::StringUtils::keyEndsWith(referenceUom, "reference-data--UnitOfMeasure:us:")) {
        return "us";
    }

    // Default to ms
    return "ms";
}

double
OsduSeismicTraceData100::getUnitConversionFactor(const std::string& traceDomainUOM, double zUomToSIFactor)
{
    // Convert to SI then back to the unit to be used in the SeismicTraceData metadata file

    if (traceDomainUOM == "m") {
        // This is the SI unit for depth, so just use zUomToSIFactor
        
        return zUomToSIFactor;
    }
    else if (traceDomainUOM == "ft") {
        // Depth units are normally whole meter or feet
        // Convert to SI and divide by 0.3048 to get ft
        // 
        return zUomToSIFactor / 0.3048;

    }
    else if (traceDomainUOM == "s") {
        // This is the SI unit for time, so just use zUomToSIFactor

        return zUomToSIFactor;
    }
    else if (traceDomainUOM == "ms") {
        // Convert to SI and multiply by 1000 to get ms
        //
        return zUomToSIFactor * 1000;
    }
    else if (traceDomainUOM == "us") {
        // Convert to SI and multiply by 1000000 to get us
        //
        return zUomToSIFactor * 1000000;
    }

    // Should never get here ... 
    return zUomToSIFactor;
}

static int
findMetaEntry(const std::string& traceDomainUOMString, const std::string& osduTraceDomainUOM, const Json::Value& metaJson)
{
    int ix = 0;
    for (const auto& meta : metaJson) {
        if (!meta.isMember("kind") or !meta.isMember("name") or !meta.isMember("propertyNames") or !meta.isMember("unitOfMeasureID")) {
            ix++;
            continue;
        }

        if (meta["kind"].asString() == "Unit") {
            if (meta["name"].asString() == traceDomainUOMString or meta["unitOfMeasureID"].asString() == osduTraceDomainUOM) {
                return ix; 
            }
        }
        ix++;
    }

    return -1;
}

void
OsduSeismicTraceData100::updateMetaSection(const std::vector<std::string>& propertyList, std::string& traceDomainUOMString, const std::string& osduTraceDomainUOM, Json::Value& meta)
{
    // Assume the meta section exist, and is not empty...
    if (meta.empty()) {
        return;
    }
    const auto metaIx = findMetaEntry(traceDomainUOMString, osduTraceDomainUOM, meta);
    if (metaIx < 0) {
        return;
    }

    for (const auto& prop : propertyList) {
        bool found = false;
        for (const auto& mprop : meta[metaIx]["propertyNames"]) {
            if (mprop.asString() == prop) {
                found = true;
                break;
            }
        }
        if (not found) {
            meta[metaIx]["propertyNames"].append(prop);
        }
    }
}

void
OsduSeismicTraceData100::updateExtentFields(const segyindex::CubeExtent& cubeExtents)
{
    auto& data = root_[OsduFieldNameLiterals::Data];
    
    std::string traceDomainUOMString = "ms";
    std::string osduTraceDomainUOM = "ms";
    bool isTimeSeismic = true;
  
    if (data.isMember("TraceDomainUOM")) {
        osduTraceDomainUOM = data["TraceDomainUOM"].asString();
        isTimeSeismic = !segysdk::utilities::StringUtils::keyEndsWith(osduTraceDomainUOM, "reference-data--UnitOfMeasure:m:") and !segysdk::utilities::StringUtils::keyEndsWith(osduTraceDomainUOM, "reference-data--UnitOfMeasure:ft:");
        traceDomainUOMString = getTraceDomainUoMString(osduTraceDomainUOM);
    }

    auto propertyList = isTimeSeismic 
        ? std::vector<std::string>{"SampleInterval", "StartTime", "EndTime", "TraceLength"} 
        : std::vector<std::string>{"SampleInterval", "StartDepth", "EndDepth", "TraceLength"};

    if (root_.isMember("meta")) {
        updateMetaSection(propertyList, traceDomainUOMString, osduTraceDomainUOM, root_["meta"]);
    }

    if (!data.isMember("SampleCount") or data["SampleCount"].asInt() == 0) {
        data["SampleCount"] = static_cast<unsigned int>(cubeExtents.traceExtent.sampleCount);
    }

    if (!data.isMember("InlineMin") or data["InlineMin"].asInt() == 0) {
        data["InlineMin"] = cubeExtents.InlineMin;
    }
    if (!data.isMember("InlineMax") or data["InlineMax"].asInt() == 0) {
        data["InlineMax"] = cubeExtents.InlineMax;
    }
    if (!data.isMember("InlineIncrement") or data["InlineIncrement"].asDouble() == 0) {
        data["InlineIncrement"] = cubeExtents.InlineIncrement;
    }
    if (!data.isMember("CrosslineMin") or data["CrosslineMin"].asInt() == 0) {
        data["CrosslineMin"] = cubeExtents.CrosslineMin;
    }
    if (!data.isMember("CrosslineMax") or data["CrosslineMax"].asInt() == 0) {
        data["CrosslineMax"] = cubeExtents.CrosslineMax;
    }
    if (!data.isMember("CrosslineIncrement") or data["CrosslineIncrement"].asDouble() == 0) {
        data["CrosslineIncrement"] = cubeExtents.CrosslineIncrement;
    }

    const auto zUnitScaleFactor = getUnitConversionFactor(traceDomainUOMString, cubeExtents.traceExtent.zUnitToSIFactor);
    data["SampleInterval"] = cubeExtents.traceExtent.sampleInterval * zUnitScaleFactor;

    double startDepth = cubeExtents.traceExtent.firstSampleDepth * zUnitScaleFactor;
    double endDepth = cubeExtents.traceExtent.lastSampleDepth * zUnitScaleFactor;

    // TraceLength field is common for both time and depth seismic and calculated
    // the same way for the two domains.
    data["TraceLength"] = endDepth - startDepth;
    
    if (isTimeSeismic) {
        data["StartTime"] = startDepth;
        data["EndTime"] = endDepth;
    }
    else {
        data["StartDepth"] = startDepth;
        data["EndDepth"] = endDepth;
    }
    
}

Json::Value
OsduSeismicTraceData100::postProcessPolygons(const Json::Value& polygon)
{
    Json::Value myPolygons;
    if (!polygon.isArray() or polygon.empty()) {
        return myPolygons;
    }

    for (auto& poly : polygon) {
        Json::Value myCoords;

        for (auto& coords : poly) {
            Json::Value myCoord;
            for (auto& coord : coords) {
                if (coord.isNumeric()) {
                    const auto value = round_up(coord.asDouble(), 2);
                    myCoord.append(value);
                }
            }
            myCoords.append(myCoord);
        }
        myPolygons.append(myCoords);
    }

    return myPolygons;
}

std::list<OsduSeismicTraceData100::PolygonCoordinates>
OsduSeismicTraceData100::getLiveTraceOutlinePolygons() const
{
    std::list<OsduSeismicTraceData100::PolygonCoordinates> retPolygons;

    auto& data = root_[OsduFieldNameLiterals::Data];
    if (!data.isMember(OsduFieldNameLiterals::LiveTraceOutline)) {
        return {};
    }
    auto& liveTrace = data[OsduFieldNameLiterals::LiveTraceOutline];
    if (!liveTrace.isMember(OsduFieldNameLiterals::AsIngestedCoordinates)) {
        return {};
    }
    auto& ingestedCoords = liveTrace[OsduFieldNameLiterals::AsIngestedCoordinates];
    if (!ingestedCoords.isMember(OsduSeismicTraceDataLiterals::Features)) {
        return {};
    }

    // Is it always the first element?
    if (ingestedCoords[OsduSeismicTraceDataLiterals::Features].empty()) {
        return {};
    }
    if (!ingestedCoords[OsduSeismicTraceDataLiterals::Features][0].isMember(OsduSeismicTraceDataLiterals::Geometry)) {
        return {};
    }
    auto& geometry = ingestedCoords[OsduSeismicTraceDataLiterals::Features][0][OsduSeismicTraceDataLiterals::Geometry];
    if (!geometry.isMember(OsduSeismicTraceDataLiterals::Coordinates)) {
        return {};
    }

    auto collectPolyCoordinates = [&retPolygons](const Json::Value& jsonPolygons) {
        for (const auto& jsonPoly : jsonPolygons) {
            PolygonCoordinates coordinates;
            for (const auto& polyCoord : jsonPoly) {
                std::array<double, 2> coords = {polyCoord[0].asDouble(), polyCoord[1].asDouble()};
                coordinates.emplace_back(std::move(coords));
            }
            retPolygons.emplace_back(coordinates);
        }
    };
    const std::string anyCrsMultiPolygon = "AnyCrsMultiPolygon";
    if (geometry[OsduSeismicTraceDataLiterals::Type].asString() == anyCrsMultiPolygon) {
        for (const auto& relatedPolygons : geometry[OsduSeismicTraceDataLiterals::Coordinates]) {
            collectPolyCoordinates(relatedPolygons);
        }
    }
    else {
        collectPolyCoordinates(geometry[OsduSeismicTraceDataLiterals::Coordinates]);
    }
    return retPolygons;
}

TraceDomainUOM
OsduSeismicTraceData100::getBackupUoM(const Json::Value root) const
{
    if (!root.isMember(OsduFieldNameLiterals::Data)) {
        return TraceDomainUOM::undefined;
    }
    auto& data = root[OsduFieldNameLiterals::Data];

    if (!data.isMember(OsduSeismicTraceDataLiterals::SeismicDomainTypeID)) {
        return TraceDomainUOM::undefined;
    }
    auto traceDomain = data[OsduSeismicTraceDataLiterals::SeismicDomainTypeID].asString();

    if (traceDomain.find(":Time:")) {
        return TraceDomainUOM::milliseconds;
    }

    if (traceDomain.find(":Depth:")) {
        return TraceDomainUOM::meter;
    }

    // last default
    return TraceDomainUOM::undefined;
}

TraceDomainUOM
OsduSeismicTraceData100::getMetaTraceDomainUom(std::string dataField) const
{
    auto backupUoM = getBackupUoM(root_);

    if (not root_.isMember(OsduFieldNameLiterals::Meta)) {
        return backupUoM;
    }
    auto& meta = root_[OsduFieldNameLiterals::Meta];

    for (const auto& metaEntry : meta) {
        if (not metaEntry.isMember(OsduSeismicTraceMetaLiterals::propertyNames) or metaEntry[OsduSeismicTraceMetaLiterals::propertyNames].empty()) {
            continue;
        }
        for (const auto& property : metaEntry[OsduSeismicTraceMetaLiterals::propertyNames]) {
            if (dataField == property.asString()) {
                auto myUoM = getTraceDomainUom(metaEntry[OsduSeismicTraceMetaLiterals::unitOfMeasureID].asString());
                return myUoM != TraceDomainUOM::undefined ? myUoM : backupUoM;
            }
        }
    }
    return TraceDomainUOM::undefined;
}

TraceDomainUOM
OsduSeismicTraceData100::getDataTraceDomainUom() const
{
    auto& data = root_[OsduFieldNameLiterals::Data];
    if (!data.isMember(OsduSeismicTraceDataLiterals::TraceDomainUOM)) {
        SEGYLOG_REPORT_WARNING("updateSeismicTraceData; getDataTraceDomain: TraceDomainUOM is not set in the SeismicTraceData data section.");
        return TraceDomainUOM::undefined;
    }

    std::string traceDomainUomId = data[OsduSeismicTraceDataLiterals::TraceDomainUOM].asString();
    return getTraceDomainUom(traceDomainUomId);
}

TraceDomainUOM
OsduSeismicTraceData100::getTraceDomainUom(std::string traceUomId, bool displayWarning) const
{
    static const std::map<std::string, TraceDomainUOM> traceUOMs = {
        {"m", TraceDomainUOM::meter},
        {"ft", TraceDomainUOM::feet},
        {"ftUS", TraceDomainUOM::feet},
        {"us", TraceDomainUOM::microseconds},
        {"ms", TraceDomainUOM::milliseconds},
        {"s", TraceDomainUOM::seconds}
    };

    const char* const traceUomFormat = "<namespace>:reference-data--UnitOfMeasure:[m|ft|ftUS|us|ms|s]:";
    const std::string errorMsg = "updateSeismicTraceData; getTraceDomain: TraceDomainUOM ID is not recognized:" + traceUomId + ". ID should be of the following format:" + traceUomFormat;

    size_t pos = traceUomId.find("UnitOfMeasure:");
    if (pos == std::string::npos) {
        if (displayWarning) {
            SEGYLOG_REPORT_WARNING(errorMsg);
        }     
        return TraceDomainUOM::undefined;
    }
    const std::string partUnit = traceUomId.substr(pos);
    pos = partUnit.find(":");
    const std::string unit = partUnit.substr(pos + 1, partUnit.rfind(":") - pos - 1);

    if (traceUOMs.find(unit) == traceUOMs.end()) {
        if (displayWarning) {
            SEGYLOG_REPORT_WARNING(errorMsg);
        }   
        return TraceDomainUOM::undefined;
    }
    return traceUOMs.at(unit);
}

TraceDomain
OsduSeismicTraceData100::getTraceDomain() const
{
    static const std::map<std::string, TraceDomain> traceDomains = {
        {"Depth", TraceDomain::Depth},
        {"Time", TraceDomain::Time},
    };

    auto& data = root_[OsduFieldNameLiterals::Data];
    if (!data.isMember(OsduSeismicTraceDataLiterals::SeismicDomainTypeID)) {
        SEGYLOG_REPORT_WARNING("updateSeismicTraceData; getTraceDomain: SeismicDomainTypeID is not set in the SeismicTraceData data section.");
        return TraceDomain::Undefined;
    }

    std::string traceDomain = data[OsduSeismicTraceDataLiterals::SeismicDomainTypeID].asString();
    const char* const traceDomainFormat = "<namespace>:reference-data--SeismicDomainType:[Time|Depth]:";
    const std::string errorMsg = "updateSeismicTraceData; getTraceDomain: SeismicDomainTypeID ID is not recognized:" + traceDomain + ". ID should be of the following format:" + traceDomainFormat;

    if (traceDomain.find(":") == std::string::npos) {
        return TraceDomain::Undefined;
    }
    const auto domainParts = segysdk::utilities::StringUtils::split(traceDomain, ":");
    const auto domain = domainParts.at(2);

    if (traceDomains.find(domain) == traceDomains.end()) {
        SEGYLOG_REPORT_WARNING(errorMsg);
        return TraceDomain::Undefined;
    }
    return traceDomains.at(domain);
}

TraceDomainUOM
OsduSeismicTraceData100::getTraceDomainUOMfromSegy(const segyio::VerticalUOM& segyUOM) const
{
    static const std::map<segyio::VerticalUOM, TraceDomainUOM> recordUOMs = {
        {segyio::VerticalUOM::meter, TraceDomainUOM::meter},
        {segyio::VerticalUOM::feet, TraceDomainUOM::feet},
        {segyio::VerticalUOM::seconds, TraceDomainUOM::seconds}
    };

    return recordUOMs.at(segyUOM);
}

std::string
OsduSeismicTraceData100::getTraceDomainUomString(const TraceDomainUOM& traceUom) const
{
    static const std::map<TraceDomainUOM, std::string> recordUOMs = {
        {TraceDomainUOM::meter, ":reference-data--UnitOfMeasure:m:"},
        {TraceDomainUOM::feet, ":reference-data--UnitOfMeasure:ft:"},
        {TraceDomainUOM::seconds, ":reference-data--UnitOfMeasure:s:"},
        {TraceDomainUOM::milliseconds, ":reference-data--UnitOfMeasure:ms:"},
        {TraceDomainUOM::microseconds, ":reference-data--UnitOfMeasure:us:"}
    };

    return ns() + recordUOMs.at(traceUom);
}

bool
OsduSeismicTraceData100::isDimensionalityType2D() const
{
    auto& data = root_[OsduFieldNameLiterals::Data];
    if (!data.isMember(OsduSeismicTraceDataLiterals::SeismicTraceDataDimensionalityTypeID)) {
        SEGYLOG_REPORT_WARNING("updateSeismicTraceData; dimensionalityType: SeismicTraceDataDimensionalityTypeID is not set in the SeismicTraceData data section.");
        return false;
    }

    std::string dataType = data[OsduSeismicTraceDataLiterals::SeismicTraceDataDimensionalityTypeID].asString();

    size_t pos = dataType.find("SeismicTraceDataDimensionalityType:");
    if (pos == std::string::npos) {
        const char* const typeFormat = "<namespace>:reference-data--SeismicTraceDataDimensionalityType:[3D|2D|4D]:";
        const std::string errorMsg = "updateSeismicTraceData; dimensionalityType: SeismicTraceDataDimensionalityTypeID ID is not recognized:" + dataType + ". ID should be of the following format:" + typeFormat;

        SEGYLOG_REPORT_WARNING(errorMsg);
        return false;
    }
    const std::string dim = dataType.substr(pos);
    pos = dim.find(":");
    const std::string dimension = dim.substr(pos + 1, dim.rfind(":") - pos - 1);

    return dimension == "2D";
}

double
OsduSeismicTraceData100::round_up(double value, int decimal_places)
{
    const double multiplier = std::pow(10.0, decimal_places);
    const int part1 = static_cast<int>(std::ceil(value * multiplier));
    const auto result = part1 / multiplier;
    return result;
}

} // namespace segysdk::utilities::osdu
