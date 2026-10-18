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

#pragma once


#include "../SegyFormat/External/SegyHeaderField.h"
#include "../SegyFormat/External/TraceHeaderField.h"

#include <vector>

namespace Json
{
class Value;
}

namespace segysdk::segyformat::external::tests
{

using namespace segyformat::external;

class JsonLoaderTestDataProvider
{

public:
    static Json::Value getCorrectVectorHeaderMappingOSDU();

    static Json::Value getIncorrectVectorHeaderMappingOSDU();

    static Json::Value illegalHeaderNameMapping();

    static Json::Value illegalOffsetMapping();

    static Json::Value getFileSourceInfo();

    static Json::Value getFileSourceInfos();

    static Json::Value getDatasetProperties();

    static Json::Value getJsonOSDUData();

    static Json::Value getJsonOSDUAcl();

    static Json::Value getJsonOSDULegal();

    static Json::Value getMetaData();

    static Json::Value SetupFileCollectionJson();

    static bool ListContains(const std::vector<std::string>& list, const std::string& member);

    static std::string getKeyFromOSDUString(std::string& osduKey);

    static DataFormat getFormatFromString(std::string dataFormat);

    static std::string getUoMFromString(std::string osduUoM);

    static std::string getScalarFieldFromKey(const std::string& key);

    static TraceHeaderField getHeaderDefinitionFromJson(Json::Value& item);

    static TraceHeaderField getMatchingHeaderFromInputSet(std::vector<TraceHeaderField>& inputHdrs, const std::shared_ptr<SegyHeaderField>& ctxtHdr);
};
} // namespace segysdk::segyformat::external::tests
