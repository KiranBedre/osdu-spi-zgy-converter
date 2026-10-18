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

#include "JsonLoaderTestDataProvider.h"

#include "../SegyFormat/External/SegyTraceHeaderStringConsts.h"
#include "../SegyFormat/External/UoMStringConsts.h"
#include "../Utilities/StringUtils.h"

#include <json/json.h>
#include <algorithm>

namespace segysdk::segyformat::external::tests
{

using segysdk::utilities::StringUtils;

Json::Value
JsonLoaderTestDataProvider::getCorrectVectorHeaderMappingOSDU()
{
    Json::Value mappingJson;

    Json::Value item;
    item["KeyName"] = "opendes:reference-data--HeaderKeyName:Inline:";
    item["WordFormat"] = "opendes:reference-data--WordFormatType:INT:";
    item["WordWidth"] = 4;
    item["Position"] = 221;
    item["UoM"] = "Unitless";
    item["ScalarIndicator"] = "NOSCALE";
    item["ScalarOverride"] = 0;
    mappingJson.append(item);

    item.clear();

    item["KeyName"] = "opendes:reference-data--HeaderKeyName:Crossline:";
    item["WordFormat"] = "opendes:reference-data--WordFormatType:INT:";
    item["WordWidth"] = 4;
    item["Position"] = 21;
    item["UoM"] = "Unitless";
    item["ScalarIndicator"] = "NOSCALE";
    item["ScalarOverride"] = 0;
    mappingJson.append(item);

    item.clear();

    item["KeyName"] = "opendes:reference-data--HeaderKeyName:CDPX:";
    item["WordFormat"] = "opendes:reference-data--WordFormatType:INT:";
    item["WordWidth"] = 4;
    item["Position"] = 73;
    item["UoM"] = "opendes:reference-data--UnitOfMeasure:m:";
    item["ScalarIndicator"] = "STANDARD";
    item["ScalarOverride"] = 0;
    mappingJson.append(item);

    item.clear();

    item["KeyName"] = "opendes:reference-data--HeaderKeyName:CDPY:";
    item["WordFormat"] = "opendes:reference-data--WordFormatType:INT:";
    item["WordWidth"] = 4;
    item["Position"] = 77;
    item["UoM"] = "opendes:reference-data--UnitOfMeasure:m:";
    item["ScalarIndicator"] = "STANDARD";
    item["ScalarOverride"] = 0;
    mappingJson.append(item);

    item.clear();

    item["KeyName"] = "opendes:reference-data--HeaderKeyName:STARTTIME:";
    item["WordFormat"] = "opendes:reference-data--WordFormatType:INT:";
    item["WordWidth"] = 2;
    item["Position"] = 109;
    item["UoM"] = "opendes:reference-data--UnitOfMeasure:ms:";
    item["ScalarIndicator"] = "NOSCALE";
    item["ScalarOverride"] = 0;
    mappingJson.append(item);

    item.clear();

    item["KeyName"] = "opendes:reference-data--HeaderKeyName:NSAMP:";
    item["WordFormat"] = "opendes:reference-data--WordFormatType:UINT:";
    item["WordWidth"] = 2;
    item["Position"] = 115;
    item["UoM"] = "Unitless";
    item["ScalarIndicator"] = "NOSCALE";
    item["ScalarOverride"] = 0;
    mappingJson.append(item);

    return mappingJson;
}

Json::Value
JsonLoaderTestDataProvider::getIncorrectVectorHeaderMappingOSDU()
{
    Json::Value mappingJson;

    Json::Value item;
    item["KeyName"] = "opendes:reference-data--HeaderKeyName:Inline";
    item["WordFormat"] = "opendes:reference-data--WordFormatType:INT";
    item["WordWidth"] = 4;
    item["Position"] = 221;
    item["UoM"] = "Unitless";
    item["ScalarIndicator"] = "NOSCALE";
    item["ScalarOverride"] = 0;
    mappingJson.append(item);

    item.clear();

    item["KeyName"] = "opendes:reference-data--HeaderKeyName:Crossline";
    item["WordFormat"] = "opendes:reference-data--WordFormatType:Integer";
    item["WordWidth"] = 4;
    item["Position"] = 21;
    item["UoM"] = "Unitless";
    item["ScalarIndicator"] = "NOSCALE";
    item["ScalarOverride"] = 0;
    mappingJson.append(item);

    item.clear();

    item["KeyName"] = "opendes:reference-data--HeaderKeyName:CDPX";
    item["WordFormat"] = "opendes:reference-data--WordFormatType:INT";
    item["WordWidth"] = 4;
    item["Position"] = 73;
    item["UoM"] = "opendes:reference-data--UnitOfMeasure:m";
    item["ScalarIndicator"] = "STANDARD";
    item["ScalarOverride"] = 0;
    mappingJson.append(item);

    item.clear();

    item["KeyName"] = "opendes:reference-data--HeaderKeyName:CDPY";
    item["WordFormat"] = "opendes:reference-data--WordFormatType:INT";
    item["WordWidth"] = 4;
    item["Position"] = 77;
    item["UoM"] = "opendes:reference-data--UnitOfMeasure:m";
    item["ScalarIndicator"] = "STANDARD";
    item["ScalarOverride"] = 0;
    mappingJson.append(item);

    item.clear();

    item["KeyName"] = "opendes:reference-data--HeaderKeyName:STARTTIME";
    item["WordFormat"] = "opendes:reference-data--WordFormatType:INT";
    item["WordWidth"] = 2;
    item["Position"] = 109;
    item["UoM"] = "opendes:reference-data--UnitOfMeasure:ms";
    item["ScalarIndicator"] = "NOSCALE";
    item["ScalarOverride"] = 0;
    mappingJson.append(item);

    item.clear();

    item["KeyName"] = "opendes:reference-data--HeaderKeyName:NSAMP";
    item["WordFormat"] = "opendes:reference-data--WordFormatType:UINT ";
    item["WordWidth"] = 2;
    item["Position"] = 115;
    item["UoM"] = "Unitless";
    item["ScalarIndicator"] = "NOSCALE";
    item["ScalarOverride"] = 0;
    mappingJson.append(item);

    return mappingJson;
}

Json::Value
JsonLoaderTestDataProvider::illegalHeaderNameMapping()

{
    Json::Value mappingJson;

    Json::Value item;
    item["KeyName"] = "opendes:reference-data--HeaderKeyName:BADITEM";
    item["WordFormat"] = "opendes:reference-data--WordFormatType:INT";
    item["WordWidth"] = 2;
    item["Position"] = 237;
    item["UoM"] = "opendes:reference-data--UnitOfMeasure:ms";
    item["ScalarIndicator"] = "STANDARD";
    item["ScalarOverride"] = 0;
    mappingJson.append(item);

    item.clear();

    return mappingJson;
}

Json::Value
JsonLoaderTestDataProvider::illegalOffsetMapping()

{
    Json::Value mappingJson;

    Json::Value item;
    item["KeyName"] = "opendes:reference-data--HeaderKeyName:Inline";
    item["WordFormat"] = "opendes:reference-data--WordFormatType:INT";
    item["WordWidth"] = 4;
    item["Position"] = 239;
    item["UoM"] = "Unitless";
    item["ScalarIndicator"] = "STANDARD";
    item["ScalarOverride"] = 0;
    mappingJson.append(item);

    item.clear();

    return mappingJson;
}

Json::Value
JsonLoaderTestDataProvider::getFileSourceInfo()
{

    Json::Value fsJson;

    fsJson["FileSource"] = "sd://opendes/sntc/segy/mig.sgy";
    fsJson["PreloadFilePath"] = "C:/Users/edahl2/Downloads/seismic-to-osdu/Petrel/OSDU_SEGY";
    fsJson["Name"] = "OSDU_SEGY";
    fsJson["FileSize"] = "224235504";
    return fsJson;
}

Json::Value
JsonLoaderTestDataProvider::getFileSourceInfos()
{

    Json::Value fsiJson;

    fsiJson["FileSourceInfos"].append(getFileSourceInfo());
    fsiJson["Checksum"] = "data.default.viewers@opendes.enterprisedata.cloud.slb-ds.com";
    return fsiJson;
}

Json::Value
JsonLoaderTestDataProvider::getDatasetProperties()
{

    Json::Value dsJson;

    dsJson["FileSourceInfos"] = getFileSourceInfos();
    dsJson["viewers"] = "data.default.viewers@opendes.enterprisedata.cloud.slb-ds.com";
    return dsJson;
}

Json::Value
JsonLoaderTestDataProvider::getJsonOSDUData()
{

    Json::Value dataJson;
    dataJson["Name"] = "OSDU_SEGY";
    dataJson["Description"] = "As originally delivered by ACME.com.";
    dataJson["TotalSize"] = "26869760";
    dataJson["SchemaFormatTypeID"] = "opendes:reference-data--SchemaFormatType:SEGY";
    dataJson["Endian"] = "BIG";
    dataJson["DatasetProperties"] = getDatasetProperties();
    dataJson["VectorHeaderMapping"] = getCorrectVectorHeaderMappingOSDU();
    dataJson["SEGYRevision"] = "rev 0";
    dataJson["ExtensionProperties"] = Json::Value();

    return dataJson;
}

Json::Value
JsonLoaderTestDataProvider::getJsonOSDUAcl()
{

    Json::Value aclJson;
    aclJson["owners"] = "data.default.owners@opendes.enterprisedata.cloud.slb-ds.com";
    aclJson["viewers"] = "data.default.viewers@opendes.enterprisedata.cloud.slb-ds.com";
    return aclJson;
}

Json::Value
JsonLoaderTestDataProvider::getJsonOSDULegal()
{

    Json::Value legalJson;
    legalJson["legaltags"].append("opendes-default-legal");

    legalJson["otherRelevantDataCountries"] = "NO";
    legalJson["status"] = "compliant";

    return legalJson;
}

Json::Value
JsonLoaderTestDataProvider::getMetaData()
{
    Json::Value metadataJson;

    return metadataJson;
}

Json::Value
JsonLoaderTestDataProvider::SetupFileCollectionJson()
{
    Json::Value fcJson;

    fcJson["kind"] = "osdu:wks:dataset--FileCollection.SEGY:1.0.0";
    fcJson["version"] = static_cast<Json::UInt64>(1562066009929332);
    fcJson["acl"] = getJsonOSDUAcl();
    fcJson["legal"] = getJsonOSDULegal();
    fcJson["data"] = getJsonOSDUData();
    fcJson["meta"] = getMetaData();
    fcJson["createUser"] = "edahl2@slb.com";
    fcJson["createTime"] = "2021-05-09T09:55:31.323Z";
    fcJson["modifyUser"] = "edahl2@slb.com";
    fcJson["modifyTime"] = "2021-05-09T09:58:31.323Z";

    return fcJson;
}

bool
JsonLoaderTestDataProvider::ListContains(const std::vector<std::string>& list, const std::string& member)
{
    return std::find(list.begin(), list.end(), member) != list.end();
}

std::string
JsonLoaderTestDataProvider::getKeyFromOSDUString(std::string& osduKey)
{
    const std::string osduHeaderReference = "opendes:reference-data--HeaderKeyName:";

    const auto pos = osduKey.find(osduHeaderReference);
    if (pos == std::string::npos) {
        const std::string msg = "getFormatFromOSDUParameters:  Unable to get valid key name from " + osduKey;
        throw std::invalid_argument(msg);
    }

    StringUtils::ReplaceAll(osduKey, osduHeaderReference, "");
    StringUtils::ReplaceAll(osduKey, ":", "");

    if (StringUtils::ToUppercase(osduKey) == "INLINE")
        return SegyTraceHeaderStringConsts::InlineNumber;
    if (StringUtils::ToUppercase(osduKey) == "CROSSLINE")
        return SegyTraceHeaderStringConsts::CrosslineNumber;
    if (StringUtils::ToUppercase(osduKey) == "CDPX")
        return SegyTraceHeaderStringConsts::SourceCoordinateX;
    if (StringUtils::ToUppercase(osduKey) == "CDPY")
        return SegyTraceHeaderStringConsts::SourceCoordinateY;
    if (StringUtils::ToUppercase(osduKey) == "STARTTIME")
        return SegyTraceHeaderStringConsts::DelayRecordingTime;
    if (StringUtils::ToUppercase(osduKey) == "NSAMP")
        return SegyTraceHeaderStringConsts::NumberOfSamplesIntrace;

    return osduKey;
}

DataFormat
JsonLoaderTestDataProvider::getFormatFromString(std::string dataFormat)
{
    const std::string osduHeaderReference = "opendes:reference-data--WordFormatType:";

    const auto pos = dataFormat.find(osduHeaderReference);
    if (pos == std::string::npos) {
        const std::string msg = "getFormatFromOSDUParameters:  Unable to get valid data format from " + dataFormat;
        throw std::invalid_argument(msg);
    }

    StringUtils::ReplaceAll(dataFormat, osduHeaderReference, "");
    StringUtils::ReplaceAll(dataFormat, ":", "");

    const std::string parseFormat = StringUtils::ToUppercase(dataFormat);

    if (StringUtils::keyMatches(parseFormat, "UINT"))
        return DataFormat::UnsignedInt;
    if (StringUtils::keyMatches(parseFormat, "INT"))
        return DataFormat::Integer;
    if (StringUtils::keyMatches(parseFormat, "IBMFLOAT"))
        return DataFormat::FloatIBM;
    if (StringUtils::keyMatches(parseFormat, "IEEEFLOAT"))
        return DataFormat::FloatIEEE;

    const std::string msg = "getFormatFromOSDUParameters:  Unable to convert data format " + dataFormat + " to valid internal representation";
    throw std::invalid_argument(msg);
}

std::string
JsonLoaderTestDataProvider::getUoMFromString(std::string osduUoM)
{
    if (osduUoM.empty() || osduUoM == "Unitless")
        return osduUoM;

    const std::string osduHeaderReference = "opendes:reference-data--UnitOfMeasure:";

    if (StringUtils::keyMatches(osduUoM, osduHeaderReference)) {

        StringUtils::ReplaceAll(osduUoM, osduHeaderReference, "");
        StringUtils::ReplaceAll(osduUoM, ":", "");

        if (osduUoM == "ms")
            return UoMStringConsts::ms;
        if (osduUoM == "us")
            return UoMStringConsts::us;
        if (osduUoM == "m")
            return UoMStringConsts::m;
        if (osduUoM == "ft")
            return UoMStringConsts::ft;
        if (osduUoM == "ftUS")
            return UoMStringConsts::ft;
    }
    else if (StringUtils::keyMatches(osduUoM, "UnitOfMeasure:")) {
        StringUtils::ReplaceAll(osduUoM, "UnitOfMeasure", "");

        if (StringUtils::keyMatches(osduUoM, ":ms"))
            return UoMStringConsts::ms;
        if (StringUtils::keyMatches(osduUoM, ":us"))
            return UoMStringConsts::us;
        if (StringUtils::keyEndsWith(osduUoM, ":m") || StringUtils::keyMatches(osduUoM, ":meter"))
            return UoMStringConsts::m;
        if (StringUtils::keyMatches(osduUoM, ":ft"))
            return UoMStringConsts::ft;
        if (StringUtils::keyMatches(StringUtils::ToLowercase(osduUoM), ":ftus"))
            return UoMStringConsts::ft;
    }

    const std::string msg = "getFormatFromOSDUParameters:  Unable to convert unit of measurement " + osduUoM + " to valid internal representation";
    throw std::invalid_argument(msg);
}

std::string
JsonLoaderTestDataProvider::getScalarFieldFromKey(const std::string& key)
{
    if (StringUtils::keyMatches(key, "Coordinate"))
        return SegyTraceHeaderStringConsts::ScalarForCoordinates;

    return "";
}

TraceHeaderField
JsonLoaderTestDataProvider::getHeaderDefinitionFromJson(Json::Value& item)
{
    auto memberList = item.getMemberNames();
    auto osduKey = item["KeyName"].asString();
    auto pos = item["Position"].asInt();
    auto wordLength = item["WordWidth"].asUInt();
    auto wordFormat = item["WordFormat"].asString();
    auto osduUoM = item["UoM"].asString();
    auto scalarIndicator = item["ScalarIndicator"].asString();
    auto scalarOverride = item["ScalarOverride"].asInt();

    std::string key;
    key = getKeyFromOSDUString(osduKey);
    DataFormat dataFormat = DataFormat::Integer;
    dataFormat = getFormatFromString(wordFormat);

    std::string uom;
    uom = getUoMFromString(osduUoM);

    TraceHeaderField def;
    if (osduUoM.empty() && (scalarIndicator.empty() || scalarIndicator == "NOSCALE")) {
        def = TraceHeaderField(key, osduKey, dataFormat, static_cast<unsigned short>(wordLength), pos);
    }
    else if (!osduUoM.empty() && (scalarIndicator.empty() || scalarIndicator == "NOSCALE")) {
        def = TraceHeaderField(key, osduKey, dataFormat, static_cast<unsigned short>(wordLength), pos, uom);
    }
    else if (!osduUoM.empty() && scalarIndicator == "STANDARD") {
        std::string scalarField = getScalarFieldFromKey(key);

        def = TraceHeaderField(key, osduKey, dataFormat, static_cast<unsigned short>(wordLength), pos, ScalarIndicator::STANDARD, scalarField, uom);
    }
    else if (!osduUoM.empty() && !scalarIndicator.empty() && scalarIndicator == "OVERRIDE" && scalarOverride != 0) {
        def = TraceHeaderField(key, osduKey, dataFormat, static_cast<unsigned short>(wordLength), pos, ScalarIndicator::OVERRIDE, "", scalarOverride, uom);
    }

    return def;
}

TraceHeaderField
JsonLoaderTestDataProvider::getMatchingHeaderFromInputSet(std::vector<TraceHeaderField>& inputHdrs, const std::shared_ptr<SegyHeaderField>& ctxtHdr)
{
    for (auto hdr : inputHdrs) {
        if (hdr.getKey() == ctxtHdr->getKey())
            return hdr;
    }

    return TraceHeaderField();
}

} // namespace segysdk::segyformat::external::tests
