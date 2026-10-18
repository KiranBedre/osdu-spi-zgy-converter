/*
Copyright 2021 Schlumberger

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

#include "LoadContextFromOSDUJson.h"

#include "SegyHeaderField.h"
#include "SegyTraceHeaderStringConsts.h"
#include "OsduLegalLiterals.h"
#include "OSDUReferenceData.h"
#include "OsduVectorHeaderMappingLiterals.h"
#include "OsduOverrideByConstantValueLiterals.h"
#include "OsduOverrideByFormulaLiterals.h"

#include "../segysdk_details/SegyException.h"
#include "../Logger/Logger.h"

#include "../Utilities/StringUtils.h"

#include <iostream>

using namespace segysdk;
namespace segysdk::segyformat::external
{

LoadContextFromOSDUJson::LoadContextFromOSDUJson(const Json::Value& json)
  : LoadContextFromJson(json)
{

    SetDefaultScalarMapper(getScalarMapper());
    SetExternalToStandardKeyMapper(getInterfaceToStandardKeyMapper());
}

/*
     {
      "KeyName": "dev-weu-des-prod-testing-eu:reference-data--HeaderKeyName:INLINE:",
      "WordFormat": "dev-weu-des-prod-testing-eu:reference-data--WordFormatType:INT:",
      "WordWidth": 4,
      "Position": 189,
      "ScalarIndicator": "NOSCALE",
      "UoM":  "dev-weu-des-prod-testing-eu:reference-data--UnitOfMeasure:m:"
    },


*/

std::map<std::string, std::string>
LoadContextFromOSDUJson::getInterfaceToStandardKeyMapper() const
{
    std::map<std::string, std::string> mapper;

    /*
    CMPX          X coordinate of bin center
    CMPY          Y coordinate of bin center
    SRCX          X coordinate of energy source
    SRCY          Y coordinate of energy source
    RECX          X coordinate of receiver
    RECY          Y coordinate of receiver
    CMP           Bin number for 2D
    SP            Shotpoint number
    INLINE        Inline coordinate of bin center
    CROSSLINE     Crossline coordinate of bin center
    XYSCALAR      Scalar for position coordinates
    ZSCALAR       Scalar for elevations
    SPSCALAR      Scalar for Shotpoint number
    NSAMP         Number of samples
    SAMPINT       Sample interval
    STARTTIME     The time associated with the first sample, often the delay recording time.
    TIMEDATUM     Elevation at zero time, usually referenced to MSL.
    RECELEV       Receiver elevation
    SRCELEV       Source ground elevation
    SRCZ          Source depth
    RECWATERZ     Receiver water depth
    SRCWATERZ     Source water depth
    DATUMELEVSRC  Elevation of vertical datum at source, referenced to geodetic datum of CRS
    DATUMELEVREC  Elevation of vertical datum at receiver, referenced to geodetic datum of CRS
    */

    // For now, we want to restrict use of coordinate fields to only SourceCoordinateX/Y
    // We need a rework of the logic in the segyio level to handle multiple, independent coordinate fields
    // as we need to know which one to use
    constexpr bool const mapToSourceCoordinatesOnly = true;

    if constexpr (mapToSourceCoordinatesOnly) {
        // All coordinate fields are mapped as being SourceCoordinate.
        // This should be valid for OSDU where only one set of coordinate locations should be mapped in the FileCollection.SEGY json struct
        mapper.try_emplace("CMPX", SegyTraceHeaderStringConsts::SourceCoordinateX);
        mapper.try_emplace("CMPY", SegyTraceHeaderStringConsts::SourceCoordinateY);
        mapper.try_emplace("SRCX", SegyTraceHeaderStringConsts::SourceCoordinateX);
        mapper.try_emplace("SRCY", SegyTraceHeaderStringConsts::SourceCoordinateY);
        mapper.try_emplace("RECX", SegyTraceHeaderStringConsts::SourceCoordinateX);
        mapper.try_emplace("RECY", SegyTraceHeaderStringConsts::SourceCoordinateY);
    }
    else 
    {
        // All coordinate fields are mapped to the correct coordinate definition.
        // This will currently NOT work with the SegySDK segyIO layer as it is only using SourceCoordinateX/Y as the coordinate location
        mapper.try_emplace("CMPX", SegyTraceHeaderStringConsts::XCoordinateEnsemble);
        mapper.try_emplace("CMPY", SegyTraceHeaderStringConsts::YCoordinateEnsemble);
        mapper.try_emplace("SRCX", SegyTraceHeaderStringConsts::SourceCoordinateX);
        mapper.try_emplace("SRCY", SegyTraceHeaderStringConsts::SourceCoordinateY);
        mapper.try_emplace("RECX", SegyTraceHeaderStringConsts::ReceiverGroupCoordinateX);
        mapper.try_emplace("RECY", SegyTraceHeaderStringConsts::ReceiverGroupCoordinateY);
    }

    mapper.try_emplace("CMP", SegyTraceHeaderStringConsts::EnsembleNumber);
    mapper.try_emplace("SP", SegyTraceHeaderStringConsts::EnergySourcePointNumber);
    mapper.try_emplace("SPScaled", SegyTraceHeaderStringConsts::ShotpointNumber);
    mapper.try_emplace("Inline", SegyTraceHeaderStringConsts::InlineNumber);
    mapper.try_emplace("Crossline", SegyTraceHeaderStringConsts::CrosslineNumber);
    mapper.try_emplace("XYSCALAR", SegyTraceHeaderStringConsts::ScalarForCoordinates);
    mapper.try_emplace("ZSCALAR", SegyTraceHeaderStringConsts::ScalarForElevations);
    mapper.try_emplace("SPSCALAR", SegyTraceHeaderStringConsts::ScalarForShotpoint);
    mapper.try_emplace("NSAMP", SegyTraceHeaderStringConsts::NumberOfSamplesIntrace);
    mapper.try_emplace("SAMPINT", SegyTraceHeaderStringConsts::SampleInterval);

    mapper.try_emplace("STARTTIME", SegyTraceHeaderStringConsts::DelayRecordingTime);
    mapper.try_emplace("TIMEDATUM", SegyTraceHeaderStringConsts::SurfaceElevationAtSource);
    mapper.try_emplace("RECELEV", SegyTraceHeaderStringConsts::ReceiverGroupElevation);
    mapper.try_emplace("SRCELEV", SegyTraceHeaderStringConsts::SurfaceElevationAtSource);
    mapper.try_emplace("SRCZ", SegyTraceHeaderStringConsts::WaterDepthAtSource);
    mapper.try_emplace("RECWATERZ", SegyTraceHeaderStringConsts::WaterDepthAtReceiverGroup);
    mapper.try_emplace("SRCWATERZ", SegyTraceHeaderStringConsts::SurfaceElevationAtSource);
    mapper.try_emplace("DATUMELEVSRC", SegyTraceHeaderStringConsts::DatumElevationAtSource);
    mapper.try_emplace("DATUMELEVREC", SegyTraceHeaderStringConsts::DatumElevationAtReceiverGroup);

    // No binary header fields to map yet ...
    return mapper;
}

std::string
LoadContextFromOSDUJson::getInputKeyFromKey(const std::string& key) const
{
    /*
        CMPX          X coordinate of bin center
        CMPY          Y coordinate of bin center
        SRCX          X coordinate of energy source
        SRCY          Y coordinate of energy source
        RECX          X coordinate of receiver
        RECY          Y coordinate of receiver
        CMP           Bin number for 2D
        SP            Shotpoint number
        Inline        Inline coordinate of bin center
        Crossline     Crossline coordinate of bin center
        XYSCALAR      Scalar for position coordinates
        ZSCALAR       Scalar for elevations
        SPSCALAR      Scalar for Shotpoint number
        NSAMP         Number of samples
        SAMPINT       Sample interval
        STARTTIME     The time associated with the first sample, often the delay recording time.
        TIMEDATUM     Elevation at zero time, usually referenced to MSL.
        RECELEV       Receiver elevation
        SRCELEV       Source ground elevation
        SRCZ          Source depth
        RECWATERZ     Receiver water depth
        SRCWATERZ     Source water depth
        DATUMELEVSRC  Elevation of vertical datum at source, referenced to geodetic datum of CRS
        DATUMELEVREC  Elevation of vertical datum at receiver, referenced to geodetic datum of CRS
    */

    const auto osduKey = OSDUKeyNameReference(key);
    if (!osduKey.isValidReference()) {
        const std::string msg = "loadContext; Header definition key does not match expected osdu value : " + key;
        SEGYLOG_REPORT_ERROR(msg);
        return "UNDEFINED";
    }

    if (osduKey.getReferenceValueToUpper() == "CMPX" || osduKey.getReferenceValueToUpper() == "CDPX") {
        return "CMPX";
    }
    if (osduKey.getReferenceValueToUpper() == "CMPY" || osduKey.getReferenceValueToUpper() == "CDPY") {
        return "CMPY";
    }
    if (osduKey.getReferenceValueToUpper() == "SRCX") {
        return "SRCX";
    }
    if (osduKey.getReferenceValueToUpper() == "SRCY") {
        return "SRCY";
    }
    if (osduKey.getReferenceValueToUpper() == "RECX") {
        return "RECX";
    }
    if (osduKey.getReferenceValueToUpper() == "RECY") {
        return "RECY";
    }
    if (osduKey.getReferenceValueToUpper() == "CMP" || osduKey.getReferenceValueToUpper() == "CDP") {
        return "CMP";
    }
    if (osduKey.getReferenceValueToUpper() == "SP") {
        return "SP";
    }
    if (osduKey.getReferenceValueToUpper() == "SPSCALED") {
        return "SPScaled";
    }
    if (osduKey.getReferenceValueToUpper() == "INLINE") {
        return "Inline";
    }
    if (osduKey.getReferenceValueToUpper() == "CROSSLINE") {
        return "Crossline";
    }
    if (osduKey.getReferenceValueToUpper() == "XYSCALAR") {
        return "XYSCALAR";
    }
    if (osduKey.getReferenceValueToUpper() == "SPSCALAR") {
        return "SPSCALAR";
    }
    if (osduKey.getReferenceValueToUpper() == "ZSCALAR") {
        return "ZSCALAR";
    }
    if (osduKey.getReferenceValueToUpper() == "NSAMP") {
        return "NSAMP";
    }
    if (osduKey.getReferenceValueToUpper() == "SAMPINT") {
        return "SAMPINT";
    }
    if (osduKey.getReferenceValueToUpper() == "STARTTIME") {
        return "STARTTIME";
    }
    if (osduKey.getReferenceValueToUpper() == "TIMEDATUM") {
        return "TIMEDATUM";
    }
    if (osduKey.getReferenceValueToUpper() == "RECELEV") {
        return "RECELEV";
    }
    if (osduKey.getReferenceValueToUpper() == "SRCELEV") {
        return "SRCELEV";
    }
    if (osduKey.getReferenceValueToUpper() == "SRCZ") {
        return "SRCZ";
    }
    if (osduKey.getReferenceValueToUpper() == "RECWATERZ") {
        return "RECWATERZ";
    }
    if (osduKey.getReferenceValueToUpper() == "SRCWATERZ") {
        return "SRCWATERZ";
    }
    if (osduKey.getReferenceValueToUpper() == "DATUMELEVSRC") {
        return "DATUMELEVSRC";
    }
    if (osduKey.getReferenceValueToUpper() == "DATUMELEVREC") {
        return "DATUMELEVREC";
    }

    const std::string msg = "loadContext; Unable to identify specified header key : " + key;
    SEGYLOG_REPORT_ERROR(msg);
    return "UNDEFINED";
}

DataFormat
LoadContextFromOSDUJson::getDataFormatFromNode(const std::string& formatString) const
{
    const auto osduFormat = OSDUWordFormatReference(formatString);
    if (!osduFormat.isValidReference()) {
        const std::string msg = "loadContext; Field format definition key does not match expected osdu value : " + formatString;
        SEGYLOG_REPORT_ERROR(msg);
        return DataFormat::Integer;
    }

    if (osduFormat.getReferenceValueToUpper() == "INT" || osduFormat.getReferenceValueToUpper() == "INTEGER") {
        return DataFormat::Integer;
    }
    if (osduFormat.getReferenceValueToUpper() == "UNSIGNED_INT" || osduFormat.getReferenceValueToUpper() == "UINT") {
        return DataFormat::UnsignedInt;
    }
    if (osduFormat.getReferenceValueToUpper() == "IBMFLOAT") {
        return DataFormat::FloatIBM;
    }
    if (osduFormat.getReferenceValueToUpper() == "IEEEFLOAT") {
        return DataFormat::FloatIEEE;
    }

    const std::string msg = "loadContext; Invalid data format in header definition: " + formatString;
    SEGYLOG_REPORT_ERROR(msg);
    return DataFormat::Integer;
}

std::string
LoadContextFromOSDUJson::getUoMFromNode(const std::string& unitString) const
{

    if (utilities::StringUtils::keyMatches(unitString, "Unitless") || utilities::StringUtils::keyMatches(unitString, "Euc"))
        return UoMStringConsts::Unitless;

    const auto osduUnit = OSDUUnitOfMeasureReference(unitString);
    if (!osduUnit.isValidReference()) {
        const std::string msg = "loadContext; Field Unit of measurement definition key does not match expected osdu value : " + unitString;
        SEGYLOG_REPORT_ERROR(msg);
        return UoMStringConsts::Unitless;
    }

    if (osduUnit.getReferenceValueToUpper() == "UNITLESS" || osduUnit.getReferenceValueToUpper() == "EUC")
        return UoMStringConsts::Unitless;

    // Depth/Length units
    if (osduUnit.getReferenceValueToUpper() == "M" || osduUnit.getReferenceValueToUpper() == "METER") {
        return UoMStringConsts::m;
    }
    if (osduUnit.getReferenceValueToUpper() == "FT" || osduUnit.getReferenceValueToUpper() == "FEET" || osduUnit.getReferenceValueToUpper() == "FOOT") {
        return UoMStringConsts::ft;
    }
    if (utilities::StringUtils::keyMatches(osduUnit.getReferenceValueToUpper(), "FT") || utilities::StringUtils::keyMatches(osduUnit.getReferenceValueToUpper(), "FEET") || utilities::StringUtils::keyMatches(osduUnit.getReferenceValueToUpper(), "FOOT")) {
        return UoMStringConsts::ft;
    }

    // Time units
    if (osduUnit.getReferenceValueToUpper() == "MS") {
        return UoMStringConsts::ms;
    }
    if (osduUnit.getReferenceValueToUpper() == "US") {
        return UoMStringConsts::us;
    }

    return osduUnit.getReferenceValueToUpper();
}

bool
LoadContextFromOSDUJson::isHeaderDefinitionsNode(const std::string& key, const Json::Value& node) const
{
    return key == osdu::OsduVectorHeaderMappingLiterals::Data && node.isMember(osdu::OsduVectorHeaderMappingLiterals::VectorHeaderMapping);
}

std::shared_ptr<SegyContext>
LoadContextFromOSDUJson::getContext(const std::vector<std::string>& segyFileList, const int segyVersion)
{
    auto context = LoadContextFromJson::getContext(segyFileList, segyVersion);
    if (!context) {
        std::cerr << "LoadContextFromOSDUJson: Unable to create valid SEG-Y context\n";  
        return context;
    }
    const std::string legal_node {osdu::OsduLegalLiterals::Legal};
    auto node = getJsonNode(legal_node);
    if (node.isMember(osdu::OsduLegalLiterals::Legaltags)) {
        context->setLegalTag(node[osdu::OsduLegalLiterals::Legaltags][0].asString());
    }

    return context;
}

std::vector<std::shared_ptr<SegyOverrideField>>
LoadContextFromOSDUJson::updateBinaryHeaderOverridesFromJSON(const Json::Value& node) const
{
    std::vector<std::shared_ptr<SegyOverrideField>> overrides;

    if (!node.isArray()) {
        return overrides;
    }

    /*
      [
        {
            "KeyName": "namespace:reference-data--HeaderKeyName:NSAMP",
            "OverrideByConstant": 1501.0
        },
      ]
    */

    // OSDU Binary headers are all constant value overrides
    for (const auto& field : node) {
        std::string inputKey;
        double value = -999;

        if (field.isMember(osdu::OsduOverrideByConstantValueLiterals::KeyName)) {
            inputKey = field[osdu::OsduOverrideByConstantValueLiterals::KeyName].asString();
        }

        if (field.isMember(osdu::OsduOverrideByConstantValueLiterals::OverrideByConstant)) {
            value = field[osdu::OsduOverrideByConstantValueLiterals::OverrideByConstant].asDouble();
        }

        if (!inputKey.empty() && value != -999) {
            std::string tmpKey = getInputKeyFromKey(inputKey);
            std::string key = getStandardKeyFromInterfaceKey(tmpKey);
            auto inputHdr = std::make_shared<SegyOverrideField>(key, value);
            overrides.push_back(inputHdr);
        }
    }

    return overrides;
}

std::vector<std::shared_ptr<SegyOverrideField>>
LoadContextFromOSDUJson::updateTraceHeaderOverridesFromJSON(const Json::Value& node) const
{
    std::vector<std::shared_ptr<SegyOverrideField>> overrides;

    if (!node.isArray()) {
        return overrides;
    }

    /*
      [
        {
            "KeyName": "namespace:reference-data--HeaderKeyName:NSAMP",
            "OverrideByConstant": 1501.0
        },
        "OverrideByExpressions": [
          {
            "Input": "namespace:reference-data--HeaderKeyName:CMPX:",
            "Operator1": "Plus",
            "Operand1Constant": 12345.6,
            "Operand1Variable": "namespace:reference-data--HeaderKeyName:CMPX:",
            "Operator2": "Plus",
            "Operand2Constant": 12345.6,
            "Operand2Variable": "namespace:reference-data--HeaderKeyName:CMPX:",
            "Remark": "Example Remark"
          }
        ]
      ]
    */
    for (const auto& field : node) {
        std::string inputKey;
        double value = -999;

        if (field.isMember(osdu::OsduOverrideByConstantValueLiterals::KeyName)) {
            inputKey = field[osdu::OsduOverrideByConstantValueLiterals::KeyName].asString();
        }

        if (field.isMember(osdu::OsduOverrideByConstantValueLiterals::OverrideByConstant)) {
            value = field[osdu::OsduOverrideByConstantValueLiterals::OverrideByConstant].asDouble();
        }

        if (!inputKey.empty() && value != -999) {
            std::string tmpKey = getInputKeyFromKey(inputKey);
            std::string key = getStandardKeyFromInterfaceKey(tmpKey);
            auto inputHdr = std::make_shared<SegyOverrideField>(key, value);
            overrides.push_back(inputHdr);
            
            // We have a constant value override, so no need to look for formula override
            continue;
        }

        if (field.isMember(osdu::OsduOverrideByFormulaLiterals::OverrideByExpressions)) {
            std::string msg = inputKey + ": Support for TraceHeaderOverridesByExpression is not yet implemented.";
            SEGYLOG_GSM_WARNING(msg);
        }
    }

    return overrides;
}

std::vector<std::shared_ptr<SegyHeaderField>>
LoadContextFromOSDUJson::updateTraceHeadersFromJSON(const Json::Value& node, const std::shared_ptr<SegyContext>& context)
{
    auto memberNames = node.getMemberNames();

    if (memberNames.empty() || !node.isMember(osdu::OsduVectorHeaderMappingLiterals::VectorHeaderMapping)) {
        return context->getTraceHeaderDefinitions();
    }

    std::vector<std::shared_ptr<SegyHeaderField>> inputHeaders;
    Json::Value items = node[osdu::OsduVectorHeaderMappingLiterals::VectorHeaderMapping];
    for (const auto& item : items) {
        std::string inputKey;
        std::string uom = UoMStringConsts::Unitless;
        int offset = 0;
        unsigned short length = 0;
        int scalarOverride = 0;

        DataFormat format = DataFormat::Integer;
        ScalarIndicator scalarIndicator = ScalarIndicator::NOSCALE;

        if (item.isMember(osdu::OsduVectorHeaderMappingLiterals::KeyName)) {
            auto key = item[osdu::OsduVectorHeaderMappingLiterals::KeyName].asString();
            inputKey = getInputKeyFromKey(key);
        }
        if (item.isMember(osdu::OsduVectorHeaderMappingLiterals::WordFormat)) {
            std::string fmt = item[osdu::OsduVectorHeaderMappingLiterals::WordFormat].asString();
            format = getDataFormatFromNode(fmt);
        }
        if (item.isMember(osdu::OsduVectorHeaderMappingLiterals::WordWidth)) {

            unsigned int tmp = item[osdu::OsduVectorHeaderMappingLiterals::WordWidth].asUInt();
            if (tmp < 65536)
                length = static_cast<unsigned short>(tmp);
            else
                length = 4;
        }
        if (item.isMember(osdu::OsduVectorHeaderMappingLiterals::Position)) {
            offset = item[osdu::OsduVectorHeaderMappingLiterals::Position].asInt();
        }
        if (item.isMember(osdu::OsduVectorHeaderMappingLiterals::UoM)) {
            std::string uomStr = item[osdu::OsduVectorHeaderMappingLiterals::UoM].asString();
            uom = getUoMFromNode(uomStr);
        }
        if (item.isMember(osdu::OsduVectorHeaderMappingLiterals::ScalarIndicator)) {
            std::string scind = item[osdu::OsduVectorHeaderMappingLiterals::ScalarIndicator].asString();
            scalarIndicator = getScalarIndicatorFromNode(scind);
        }
        if (item.isMember(osdu::OsduVectorHeaderMappingLiterals::ScalarOverride)) {
            scalarOverride = item[osdu::OsduVectorHeaderMappingLiterals::ScalarOverride].asInt();
        }

        if (!inputKey.empty() and offset > 0 and length > 0) {
            std::string key = getStandardKeyFromInterfaceKey(inputKey);
            std::string scalarKey = getDefaultScalarField(key, scalarIndicator);
            if (offset < 0 or offset > TraceHeaderSize) {
                std::string msg = "loadContext; updateTraceHeadersFromJSON: Illegal SEG-Y byte definition: Offset must be a value between 0 and " + std::to_string(TraceHeaderSize - 3) + ". Field=" + key;
                SEGYLOG_REPORT_ERROR(msg);
                throw SegyInvalidArgumentException(msg);
            }
            if (length < 1 or length > 8) {
                std::string msg = "loadContext; updateTraceHeadersFromJSON: Illegal SEG-Y byte definition: field size must be a value between 1 and 8 bytes. Field=" + key;
                SEGYLOG_REPORT_ERROR(msg);
                throw SegyInvalidArgumentException(msg);
            }
            if (offset + length >= TraceHeaderSize) {
                std::string msg = "loadContext; updateTraceHeadersFromJSON: Illegal SEG-Y byte definition: Offset + field size must add up to be a value less than the size of the trace header. Field=" + key;
                SEGYLOG_REPORT_ERROR(msg);
                throw SegyInvalidArgumentException(msg);
            }
            auto inputHdr = std::make_shared<TraceHeaderField>(key, inputKey, format, length, offset, scalarIndicator, scalarKey, scalarOverride, uom);
            inputHeaders.push_back(inputHdr);
        }
    }
    std::sort(inputHeaders.begin(), inputHeaders.end(),
              [](const std::shared_ptr<SegyHeaderField>& lhs, const std::shared_ptr<SegyHeaderField>& rhs) {
                  return lhs->getOffset() < rhs->getOffset();
              });

    return mergeInputHeadersWithStandard(inputHeaders, context->getSegyVersion());
}

} // namespace segysdk::segyformat::external
