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

#include "LoadContextFromDelfiJson.h"

#include "SegyHeaderField.h"
#include "SegyTraceHeaderStringConsts.h"
#include "../segysdk_details/SegyException.h"
#include "../Logger/Logger.h"

using namespace segysdk;
namespace segysdk::segyformat::external
{

LoadContextFromDelfiJson::LoadContextFromDelfiJson(const Json::Value& json)
  : LoadContextFromJson(json)
{

    SetDefaultScalarMapper(getScalarMapper());
    SetExternalToStandardKeyMapper(getInterfaceToStandardKeyMapper());
}

/*
    {
        "KeyName": "CDPX",
        "WordFormat": "Integer",
        "WordWidth": 4,
        "Position": 181,
        "UoM": "m",
        "ScalarIndicator": "STANDARD",
        "ScalarOverride": 0
    },
*/

std::map<std::string, std::string>
LoadContextFromDelfiJson::getInterfaceToStandardKeyMapper() const
{
    std::map<std::string, std::string> mapper;

    /*
      OSDU defined key names to be used:
      --------------------------------------
      CDPX	X coordinate of bin center
      CDPY	Y coordinate of bin center
      SOUX	X coordinate of energy source
      SOUY	Y coordinate of energy source
      RECX	X coordinate of receiver
      RECY	Y coordinate of receiver
      RECELEV	Receiver elevation
      SOUELEV	Source ground elevation
      SOUZ	Source depth
      RECWATERZ	Receiver water depth
      SOUWATERZ	Source water depth
      CDP	Bin number for 2D
      SP	Shotpoint number
      INLINE	Inline coordinate of bin center
      CROSSLINE	Crossline coordinate of bin center
      XYSCALAR	Scalar for position coordinates
      ZSCALAR	Scalar for elevations
      SPSCALAR	Scalar for Shotpoint number
    */

    mapper.try_emplace("CDPX", segyformat::external::SegyTraceHeaderStringConsts::XCoordinateEnsemble);
    mapper.try_emplace("CDPY", segyformat::external::SegyTraceHeaderStringConsts::YCoordinateEnsemble);
    mapper.try_emplace("SOUX", segyformat::external::SegyTraceHeaderStringConsts::SourceCoordinateX);
    mapper.try_emplace("SOUY", segyformat::external::SegyTraceHeaderStringConsts::SourceCoordinateY);
    mapper.try_emplace("RECX", segyformat::external::SegyTraceHeaderStringConsts::ReceiverGroupCoordinateX);
    mapper.try_emplace("RECY", segyformat::external::SegyTraceHeaderStringConsts::ReceiverGroupCoordinateY);
    mapper.try_emplace("RECELEV", segyformat::external::SegyTraceHeaderStringConsts::ReceiverGroupElevation);
    mapper.try_emplace("SOUELEV", segyformat::external::SegyTraceHeaderStringConsts::SurfaceElevationAtSource);
    mapper.try_emplace("SOUZ", segyformat::external::SegyTraceHeaderStringConsts::WaterDepthAtSource);
    mapper.try_emplace("RECWATERZ", segyformat::external::SegyTraceHeaderStringConsts::WaterDepthAtReceiverGroup);
    mapper.try_emplace("SOUWATERZ", segyformat::external::SegyTraceHeaderStringConsts::SurfaceElevationAtSource);
    mapper.try_emplace("CDP", segyformat::external::SegyTraceHeaderStringConsts::EnsembleNumber);
    mapper.try_emplace("SP", segyformat::external::SegyTraceHeaderStringConsts::ShotpointNumber);
    mapper.try_emplace("INLINE", segyformat::external::SegyTraceHeaderStringConsts::InlineNumber);
    mapper.try_emplace("CROSSLINE", segyformat::external::SegyTraceHeaderStringConsts::CrosslineNumber);
    mapper.try_emplace("XYSCALAR", segyformat::external::SegyTraceHeaderStringConsts::ScalarForCoordinates);
    mapper.try_emplace("ZSCALAR", segyformat::external::SegyTraceHeaderStringConsts::ScalarForElevations);
    mapper.try_emplace("SPSCALAR", segyformat::external::SegyTraceHeaderStringConsts::ScalarForShotpoint);

    // No binary header fields to map yet ...
    return mapper;
}

std::string
LoadContextFromDelfiJson::getInputKeyFromKey(const std::string& key) const
{
    return key;
}

DataFormat
LoadContextFromDelfiJson::getDataFormatFromNode(const std::string& value) const
{
    if (value == "Integer") {
        return DataFormat::Integer;
    }
    if (value == "UnsignedInt") {
        return DataFormat::UnsignedInt;
    }
    if (value == "FloatIBM") {
        return DataFormat::FloatIBM;
    }
    if (value == "FloatIEEE") {
        return DataFormat::FloatIEEE;
    }

    return DataFormat::Integer;
}

std::string
LoadContextFromDelfiJson::getUoMFromNode(const std::string& value) const
{
    if (value == "m") {
        return UoMStringConsts::m;
    }
    if (value == "ft") {
        return UoMStringConsts::ft;
    }
    if (value == "ms") {
        return UoMStringConsts::ms;
    }
    if (value == "us") {
        return UoMStringConsts::us;
    }

    return UoMStringConsts::ft;
}

bool
LoadContextFromDelfiJson::isHeaderDefinitionsNode(const std::string& key, const Json::Value& node) const
{
    (void)node;
    return key == "TraceHeaderMapping";
}

std::vector<std::shared_ptr<SegyOverrideField>>
LoadContextFromDelfiJson::updateBinaryHeaderOverridesFromJSON(const Json::Value& node) const
{
    const auto memberNames = node.getMemberNames();
    std::vector<std::shared_ptr<SegyOverrideField>> overrides;

    if (!memberNames.empty() && node.isMember("Items")) {

        Json::Value items = node["Items"];
        for (Json::Value::ArrayIndex i = 0; i != items.size(); i++) {
            std::string inputKey;
            double value = -999;

            if (items[i].isMember("KeyName")) {
                inputKey = items[i]["KeyName"].asString();
            }

            if (items[i].isMember("Value")) {
                value = items[i]["Value"].asDouble();
            }

            if (!inputKey.empty() && value != -999) {
                std::string key = getStandardKeyFromInterfaceKey(inputKey);
                auto inputHdr = std::make_shared<SegyOverrideField>(key, value);
                overrides.push_back(inputHdr);
            }
        }
    }

    return overrides;
}

std::vector<std::shared_ptr<SegyOverrideField>>
LoadContextFromDelfiJson::updateTraceHeaderOverridesFromJSON(const Json::Value& node) const
{
    const auto memberNames = node.getMemberNames();
    std::vector<std::shared_ptr<SegyOverrideField>> overrides;

    if (!memberNames.empty() && node.isMember("Items")) {
        /*
        {
            "KeyName": "SamplesPerTrace",
            "Value": 1501.0
            },
    */

        Json::Value items = node["Items"];
        for (Json::Value::ArrayIndex i = 0; i != items.size(); i++) {
            std::string inputKey;
            double value = -999;

            if (items[i].isMember("KeyName")) {
                inputKey = items[i]["KeyName"].asString();
            }

            if (items[i].isMember("Value")) {
                value = items[i]["Value"].asDouble();
            }

            if (!inputKey.empty() && value != -999) {
                std::string key = getStandardKeyFromInterfaceKey(inputKey);
                auto inputHdr = std::make_shared<SegyOverrideField>(key, value);
                overrides.push_back(inputHdr);
            }
        }
    }

    return overrides;
}

std::vector<std::shared_ptr<SegyHeaderField>>
LoadContextFromDelfiJson::updateTraceHeadersFromJSON(const Json::Value& node, const std::shared_ptr<SegyContext>& context)
{
    auto memberNames = node.getMemberNames();

    if (memberNames.empty() || !node.isMember("Items")) {
        return context->getTraceHeaderDefinitions();
    }

    std::vector<std::shared_ptr<SegyHeaderField>> inputHeaders;
    Json::Value items = node["Items"];
    for (auto& item : items) {
        std::string inputKey;
        std::string uom = UoMStringConsts::Unitless;
        int offset = 0;
        unsigned short length = 0;
        int scalarOverride = 0;
        DataFormat format = DataFormat::Integer;
        ScalarIndicator scalarIndicator = ScalarIndicator::NOSCALE;

        if (item.isMember("KeyName")) {
            auto key = item["KeyName"].asString();
            inputKey = getInputKeyFromKey(key);
        }
        if (item.isMember("WordFormat")) {
            std::string fmt = item["WordFormat"].asString();
            format = getDataFormatFromNode(fmt);
        }
        if (item.isMember("WordWidth")) {

            unsigned int tmp = item["WordWidth"].asUInt();
            if (tmp < 65536)
                length = static_cast<unsigned short>(tmp);
            else
                length = 4;
        }
        if (item.isMember("Position")) {
            offset = item["Position"].asInt();
        }
        if (item.isMember("UoM")) {
            std::string uomStr = item["UoM"].asString();
            uom = getUoMFromNode(uomStr);
        }
        if (item.isMember("ScalarIndicator")) {
            std::string scind = item["ScalarIndicator"].asString();
            scalarIndicator = getScalarIndicatorFromNode(scind);
        }
        if (item.isMember("ScalarOverride")) {
            scalarOverride = item["ScalarOverride"].asInt();
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
