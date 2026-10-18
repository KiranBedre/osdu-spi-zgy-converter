/*
Copyright 2021-2023 Schlumberger

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

#include "OsduObject.h"
#include "../segysdk_details/DLL_EXPORT.h"
#include "../SegyIO/VerticalUOM.h"
#include "../SegyIndex/CubeExtent.h"

#include <array>
#include <list>
#include <string>

namespace segysdk::utilities::osdu
{

class OsduSeismicTraceDataLiterals
{
public:
    inline static constexpr const char* const SpatialArea = "SpatialArea";
    inline static constexpr const char* const PersistableReferenceCrs = "persistableReferenceCrs";
    inline static constexpr const char* const CoordinateReferenceSystemID = "CoordinateReferenceSystemID";
    inline static constexpr const char* const Features = "features";
    inline static constexpr const char* const Geometry = "geometry";
    inline static constexpr const char* const Coordinates = "coordinates";
    inline static constexpr const char* const Type = "type";
    inline static constexpr const char* const Properties = "properties";
    inline static constexpr const char* const PointProperties = "PointProperties";
    inline static constexpr const char* const Inline = "Inline";
    inline static constexpr const char* const Crossline = "Crossline";
    inline static constexpr const char* const InlineMin = "InlineMin";
    inline static constexpr const char* const InlineMax = "InlineMax";
    inline static constexpr const char* const InlineIncrement = "InlineIncrement";
    inline static constexpr const char* const CrosslineMin = "CrosslineMin";
    inline static constexpr const char* const CrosslineMax = "CrosslineMax";
    inline static constexpr const char* const CrosslineIncrement = "CrosslineIncrement";
    inline static constexpr const char* const SampleCount = "SampleCount";
    inline static constexpr const char* const SampleInterval = "SampleInterval";
    inline static constexpr const char* const TraceLength = "TraceLength";
    inline static constexpr const char* const StartTime = "StartTime";
    inline static constexpr const char* const EndTime = "EndTime";
    inline static constexpr const char* const StartDepth = "StartDepth";
    inline static constexpr const char* const EndDepth = "EndDepth";
    inline static constexpr const char* const RangeAmplitudeMin = "RangeAmplitudeMin";
    inline static constexpr const char* const RangeAmplitudeMax = "RangeAmplitudeMax";
    inline static constexpr const char* const TraceDomainUOM = "TraceDomainUOM";
    inline static constexpr const char* const SeismicDomainTypeID = "SeismicDomainTypeID";
    inline static constexpr const char* const SeismicTraceDataDimensionalityTypeID = "SeismicTraceDataDimensionalityTypeID";
};

class OsduSeismicTraceMetaLiterals
{
public:
    inline static constexpr const char* const unitOfMeasureID = "unitOfMeasureID";
    inline static constexpr const char* const persistableReference = "persistableReference";
    inline static constexpr const char* const propertyNames = "propertyNames";
};

enum class TraceDomainUOM {
    meter,
    feet,
    microseconds,
    milliseconds,
    seconds,
    undefined
};

enum class TraceDomain {
    Depth,
    Time,
    Undefined
};

class SEGYSDK_DLL_PUBLIC OsduSeismicTraceData100 : public OsduObject
{
protected:
    static constexpr std::string_view const version_ = "1.0.0";
    double round_up(double value, int decimal_places);

public:
    using PolygonCoordinates = std::list<std::array<double, 2>>;

    explicit OsduSeismicTraceData100(const Json::Value& root);

    std::string binGridId() const;
    std::vector<std::string> datasetIds() const;
    std::vector<std::string> artefactIds() const;
    void addCompletedOpenZGYDataset(const std::string& newId, const OsduKind& newObjectKind);
    void addLiveTraceOutline(const Json::Value& jsonPolygons, bool isMultiple = false);
    void updateExtentFields(const segyindex::CubeExtent& cubeExtents);
    TraceDomainUOM getMetaTraceDomainUom(std::string dataField) const;
    TraceDomainUOM getDataTraceDomainUom() const;
    TraceDomainUOM getTraceDomainUom(std::string traceUomId, bool displayWarning = true) const;
    TraceDomain getTraceDomain() const;
    std::string getTraceDomainUoMString(const std::string& referenceUom);
    double getUnitConversionFactor(const std::string& traceDomainUOM, double zUomToSIFactor);
    TraceDomainUOM getTraceDomainUOMfromSegy(const segyio::VerticalUOM& segyUOM) const;
    std::string getTraceDomainUomString(const TraceDomainUOM& traceUomId) const;

    bool isDimensionalityType2D() const;

    std::list<PolygonCoordinates> getLiveTraceOutlinePolygons() const;

private:
    TraceDomainUOM getBackupUoM(const Json::Value root) const;
    Json::Value postProcessPolygons(const Json::Value& polygon);
    void updateMetaSection(const std::vector<std::string>& propertyList, std::string& traceDomainUOMString, const std::string& osduTraceDomainUOM, Json::Value& meta);
};

} // namespace segysdk::utilities::osdu
