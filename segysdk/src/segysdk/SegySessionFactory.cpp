/*
Copyright 2021-2024 Schlumberger

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

#include "../segysdk/SegySessionFactory.h"

#include "../FileIO/GenericFileManager.h"
#include "../Logger/Logger.h"
#include "../SegyFormat/External/LoadContextFromDelfiJson.h"
#include "../SegyFormat/External/LoadContextFromOSDUJson.h"
#include "../SegyIO/FileHeaderDefinitionFactory.h"
#include "../Telemetry/Connector.h"
#include "../Utilities/JsonUtilities.h"
#include "../segysdk/SegySessionImpl.h"

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <utility>
#include <vector>

namespace segysdk::api
{

constexpr auto segyVersionUndefined = -1;

struct ContextParams : public segyio::StorageAccessParams
{
    ContextParams& withJsonParams(const std::string& value)
    {
        jsonParams_ = value;
        return *this;
    }
    ContextParams& withSegyVersion(const int value)
    {
        segyVersion_ = value;
        return *this;
    }
    ContextParams& withSegyFileNames(const Strings& value)
    {
        segyFileNames_ = value;
        return *this;
    }
    ContextParams& withReaderOptions(const fileio::FileReaderOptions& value)
    {
        readerOptions_ = value;
        return *this;
    }
    ContextParams& withFirstFileName(const std::string& value)
    {
        firstFileName_ = value;
        return *this;
    }
    std::string jsonParams_{};
    int segyVersion_{segyVersionUndefined};
    Strings segyFileNames_{};
    fileio::FileReaderOptions readerOptions_{};
    std::string firstFileName_{};
};

std::shared_ptr<segyformat::external::SegyContext>
getSegyContextFromParameterString(const ContextParams& params);

int
getSegyVersionFromSegyFile(const ContextParams& params);

struct SegySessionFactory::Impl
{
    Impl(const std::vector<std::string>& segyFileNameList, std::string jsonParameters)
      : jsonParameters_(std::move(jsonParameters)),
        segyFileNameList_(segyFileNameList),
        segyVersion_(segyVersionUndefined)
    {
        if (segyFileNameList.empty())
            throw std::invalid_argument("SegySessionFactory: Input list of SEG-Y files cannot be empty");
    }

    explicit Impl(const std::string& osduFileCollectionSEGY)
      : jsonParameters_(osduFileCollectionSEGY),
        segyVersion_(segyVersionUndefined)
    {
        if (!jsonParameters_.empty() && jsonParameters_ != "{}") {

            Json::Value osduJson = utilities::JsonUtilities::getJsonFromString(jsonParameters_);
            if (osduJson.empty() || !osduJson.isMember("data")) {
                throw std::invalid_argument("SegySessionFactory: OSDU FileCollection.SEGY argument cannot be empty");
            }

            Json::Value dataNode = osduJson["data"];
            if (!dataNode.isMember("DatasetProperties")) {
                throw std::invalid_argument("SegySessionFactory: OSDU FileCollection.SEGY DataSetProperties argument cannot be empty");
            }
            Json::Value dsNode = dataNode["DatasetProperties"];
            if (!dsNode.isMember("FileSourceInfos")) {
                throw std::invalid_argument("SegySessionFactory: OSDU FileCollection.SEGY FileSourceInfos argument cannot be empty");
            }
            Json::Value fsNode = dsNode["FileSourceInfos"];
            for (Json::Value::ArrayIndex i = 0; i != fsNode.size(); i++) {
                if (fsNode[i].isMember("FileSource")) {
                    std::string fileName = fsNode[i]["FileSource"].asString();
                    segyFileNameList_.push_back(fileName);
                }
            }

            if (segyFileNameList_.empty())
                throw std::invalid_argument("SegySessionFactory: Input list of SEG-Y files cannot be empty");
            
        }
        else {
            throw std::invalid_argument("SegySessionFactory: OSDU FileCollection.SEGY argument cannot be empty");
        }
    }

    std::unique_ptr<SegySession>
    CreateSegySession(const segyio::StorageAccessParams& params)
    {
        SEGYLOG_DEBUG("Creating session for " + segyFileNameList_[0]);
        std::shared_ptr<segyformat::external::SegyContext> context = nullptr;
        const auto contextParams = ContextParams{params}
                                       .withJsonParams(jsonParameters_)
                                       .withSegyVersion(segyVersion_)
                                       .withSegyFileNames(segyFileNameList_)
                                       .withReaderOptions({params.sdCredentials_, params.cacheSizeOption_})
                                       .withFirstFileName(segyFileNameList_[0]);
        if (!jsonParameters_.empty() && jsonParameters_ != "{}") {
            context = getSegyContextFromParameterString(contextParams);
        }
        else {
            segyVersion_ = getSegyVersionFromSegyFile(contextParams);
            context = std::shared_ptr(std::make_shared<segyformat::external::SegyContext>(segyFileNameList_, segyVersion_));
        }

        const segyio::FileHeaderDescriptionFactory fhdFactory(context);

        auto binaryHeaders = fhdFactory.Generate_binaryHeaders();
        auto traceHeaders = fhdFactory.Generate_traceHeaders();
        auto binaryHeaderOverrides = fhdFactory.GetBinaryHeaderOverrides();
        auto traceHeaderOverrides = fhdFactory.GetTraceHeaderOverrides();

        return std::make_unique<SegySessionImpl>(SessionImplParams{params}
                                                     .withSegyContext(context)
                                                     .withBinaryHeaders(binaryHeaders)
                                                     .withTraceHeaders(traceHeaders)
                                                     .withBinaryHeaderOverrides(binaryHeaderOverrides)
                                                     .withTraceHeaderOverrides(traceHeaderOverrides));
    }

    std::string getFileName(const size_t index = 0)
    {
        if (index >= segyFileNameList_.size()) {
            return "";
        }
        return segyFileNameList_.at(index);
    }

private:
    std::string jsonParameters_;
    std::vector<std::string> segyFileNameList_;
    int segyVersion_;
};

SegySessionFactory::SegySessionFactory(const std::vector<std::string>& segyFileNameList, std::string jsonParameters)
  : impl_(std::make_shared<Impl>(segyFileNameList, std::move(jsonParameters))) {}

SegySessionFactory::SegySessionFactory(std::string osduFileCollectionSEGY)
  : impl_(std::make_shared<Impl>(std::move(osduFileCollectionSEGY))) {}

std::string
SegySessionFactory::getFileName(size_t index) const
{
    return impl_->getFileName(index);
}

std::unique_ptr<SegySession>
SegySessionFactory::CreateSegySession(const SDCredentials& sdCredentials, const CacheSizeOption& cacheSize) const
{
    return CreateSegySession(segyio::StorageAccessParams{}.withSDCredentials(sdCredentials).withCacheSizeOption(cacheSize));
}

std::unique_ptr<SegySession>
SegySessionFactory::CreateSegySession(const segyio::StorageAccessParams& params) const
{
    const auto& scope = START_TRACE_SPAN(SpanContext{});
    return impl_->CreateSegySession(params);
}

std::optional<int>
versionFromOverrides(Json::Value jsonParameters);

static int
getSegyVersion(const Json::Value& jsonParams, const ContextParams& params)
{
    const auto versionOverride = versionFromOverrides(jsonParams);
    if (versionOverride.has_value()) {
        return versionOverride.value();
    }
    else {
        return getSegyVersionFromSegyFile(params);
    }
}

std::shared_ptr<segyformat::external::SegyContext>
getSegyContextFromParameterString(const ContextParams& params)
{
    if (!params.jsonParams_.empty() && params.jsonParams_ != "{}") {

        Json::Value json = utilities::JsonUtilities::getJsonFromString(params.jsonParams_);
        if (json.empty()) {
            return std::make_shared<segyformat::external::SegyContext>();
        }

        if (json.isMember("kind") && json.isMember("data")) {
            std::string kindValue = json["kind"].asString();
            std::string osduSEGY = ":wks:dataset--FileCollection.SEGY";
            std::size_t found = kindValue.find(osduSEGY); // TODO substring search is incorrect approach
            if (found == std::string::npos) {
                throw std::invalid_argument("getSegyContextFromParameterString: Not a WKS OSDU FileCollection.SEGY parameter file");
            }

            Json::Value dataNode = json["data"];
            if (!dataNode.isMember("VectorHeaderMapping")) {
                return std::make_shared<segyformat::external::SegyContext>();
            }

            segyformat::external::LoadContextFromOSDUJson jsonParser(json);
            if (!jsonParser.canGetContext()) {
                std::cerr << "ED-Debug getSegyContextFromParameterString: Unable to create SEG-Y context from json string \n";
                return std::make_shared<segyformat::external::SegyContext>();
            }
            std::string edFileNames_ = params.segyFileNames_.empty()
                                           ? "{}"
                                           : params.segyFileNames_.at(0);

            return jsonParser.getContext(params.segyFileNames_, getSegyVersion(json, params));
        }

        if (json.isMember("TraceHeaderMapping") && json["TraceHeaderMapping"].isMember("Items")) {
            segyformat::external::LoadContextFromDelfiJson jsonParser(json);
            if (!jsonParser.canGetContext()) {
                return std::make_shared<segyformat::external::SegyContext>();
            }


            return jsonParser.getContext(params.segyFileNames_, getSegyVersion(json, params));
        }
    }
    return std::make_shared<segyformat::external::SegyContext>();
}

std::optional<int>
versionFromOverrides(Json::Value jsonParameters)
{
    if (!jsonParameters.isMember("BinaryHeaderOverrides")) {
        return std::nullopt;
    }
    Json::Value bhOverrides = jsonParameters["BinaryHeaderOverrides"];
    if (!bhOverrides.isMember("Items")) {
        return std::nullopt;
    }
    Json::Value items = bhOverrides["Items"];
    int value = 0;
    auto find_valid_revision = [&value](Json::Value& item) {
        if (item.isMember("KeyName")) {
            const std::string inputKey = item["KeyName"].asString();
            if (inputKey == "SEGYFormatRevisionNumber") {
                value = item["Value"].asInt();
                return value >= 0 && value <= 2;
            }
        }
        return false;
    };
    if (const auto it = std::find_if(items.begin(), items.end(), find_valid_revision); it != items.end()) {
        return value;
    }

    return std::nullopt;
}

int
getSegyVersionFromSegyFile(const ContextParams& params)
{

    if (params.segyVersion_ >= 0)
        return params.segyVersion_;

    const auto managerParams = fileio::FileManagerParams{
        fileio::FileAccessParams{
            request::Params{params}}
            .withFileName(params.firstFileName_)}
                                   .withReaderOptions(params.readerOptions_);
    const auto reader = fileio::GenericFileManager::createReader(managerParams);
    if (reader == nullptr)
        return 0;

    constexpr int offset = 300 + 3200; // 3200 byte textual header + binary header offset
    constexpr int length = 2;          // Length of field ..
    auto buffer = std::make_unique<uint8_t[]>(length);
    const auto read_bytes = reader->read(fileio::FileReadParams{request::Params{params}}.withOffset(offset).withSize(length));
    if (read_bytes.size() != length) {
        throw std::runtime_error("Failed to read " + std::to_string(length) + " bytes at offset " + std::to_string(offset) + ". Read " + std::to_string(read_bytes.size()));
    }

    if (buffer[0] == 0 && buffer[1] == 0)
        return 0; // Both should be zero - endianness should not matter
    if (buffer[0] != 0 && buffer[1] != 0)
        return 0; // Invalid so default to rev 0;

    if (buffer[0] == 0 || buffer[1] == 0) {
        return buffer[0] == 0 ? buffer[1] : buffer[0];
    }

    return 0;
}

} // namespace segysdk::api
