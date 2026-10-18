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

#include "Options.h"

#include "Adapters.h"

#include "OptionsDelfiCommandLine.h"
#include "OptionsOsdu.h"

#include <SDAPI/SDGenericDataset.h>
#include <SDAPI/SDManager.h>
#include "../Utilities/Authorization.h"
#include "../Utilities/Environment.h"
#include "../Utilities/JsonUtilities.h"
#include "../Utilities/Settings.h"
#include "../Utilities/auth/TokenSource.h"

#include <cstring>
#include <memory>

namespace segysdk::utilities
{

Options::~Options()
{
    /* EMPTY */
}

std::shared_ptr<Options>
Options::parse(int argc, const char** argv, const OptionsFileType oft)
{
    auto const isOsdu = (argc >= 2) && (strcmp(argv[1], "--osdu") == 0);
    auto options = isOsdu
                       ? std::static_pointer_cast<Options>(std::make_shared<OptionsOsdu>(argc, argv, oft))
                       : std::static_pointer_cast<Options>(std::make_shared<OptionsDelfiCommandLine>(argc, argv, oft));
    options->setIsOsdu(isOsdu);
    return options;
}

OptionsFileType
Options::optionsFileType() const
{
    return oft_;
}

std::unique_ptr<seismicdrive::SDManager>
Options::createSDManager() const
{
    if (!segysdk::utilities::isSDDataset(targetFile())) {
        return nullptr;
    }

    auto manager = std::make_unique<seismicdrive::SDManager>(sdCredentials_.url(), sdCredentials_.apiKey());
    auto httpHeaders = segysdk::utilities::Settings::instance().getHttpHeaders();
    if (!httpHeaders.empty()) {
        manager->setHttpHeaders(httpHeaders);
    }

    manager->setAuthProviderCallback(&sdAuthCallback, sdCredentials_.tokenSource().get());

    return manager;
}

BrickSize
Options::brickSize()
{
    const auto outputParams = segysdk::utilities::Environment::instance().zgyOutputParameters();
    const auto params = getOutputParamsFromJson(outputParams);
    return params.getBrickSize();
}

SDCredentials const&
Options::sdCredentials() const
{
    return sdCredentials_;
}

bool
Options::isOsdu() const
{
    return isOsdu_;
}

void
Options::setIsOsdu(bool isOsdu)
{
    isOsdu_ = isOsdu;
}

Options::Options(const OptionsFileType oft)
  : token_source_(Authorization::sdTokenSource()),
    sdCredentials_(SDCredentials(
        Environment::instance().sdUrl(),
        Environment::instance().sdApiKey(),
        Authorization::sdTokenSource(), 
        Settings::instance().getHttpHeaders())),
    oft_(oft)
{
    /* EMPTY */
}

Options::Options()
  : token_source_(Authorization::sdTokenSource()),
    sdCredentials_(SDCredentials(
        Environment::instance().sdUrl(),
        Environment::instance().sdApiKey(),
        Authorization::sdTokenSource(), 
        Settings::instance().getHttpHeaders())),
    oft_(OptionsFileType::Uninitialized)
{
    /* EMPTY */
}

std::string
Options::sdAuthCallback(const void* data)
{
    const auto tokenSource = static_cast<const segysdk::utilities::auth::TokenSource*>(data);
    return tokenSource->token();
}

OpenZGY::SampleDataType
Options::getSampleSize(int value)
{
    switch (value) {
    case 32:
        return OpenZGY::SampleDataType::float32;

    case 16:
        return OpenZGY::SampleDataType::int16;

    case 8:
        return OpenZGY::SampleDataType::int8;

    default:
        // 64-bit sample size not currently supported
        return OpenZGY::SampleDataType::unknown;
    }
}

std::string
Options::getBrickSize(Json::Value brick)
{
    const std::string ILabel = "INLINES_PER_BRICK";
    const int i_size = brick.isMember(ILabel) ? brick[ILabel].asInt() : 64;

    const std::string XLabel = "CROSSLINES_PER_BRICK";
    const int j_size = brick.isMember(XLabel) ? brick[XLabel].asInt() : 64;

    const std::string ZLabel = "TRACE_SAMPLES_PER_BRICK";
    const int z_size = brick.isMember(ZLabel) ? brick[ZLabel].asInt() : 64;

    std::string brickSize = std::to_string(i_size) + "X" + std::to_string(j_size) + "X" + std::to_string(z_size);

    return brickSize;
}

ZgyOutputParams
Options::getOutputParamsFromJson(const std::string& json_file)
{
    std::string json_string = JsonUtilities::GetJsonStringFromFile(json_file);

    if (json_string.empty() || json_string == "{}") {
        return {};
    }

    auto json = JsonUtilities::getJsonFromString(json_string);

    const std::string zgyQualityLabel = "ZGY_QUALITY";
    OpenZGY::SampleDataType sampleSize = json.isMember(zgyQualityLabel)
                                             ? getSampleSize(json[zgyQualityLabel].asInt())
                                             : OpenZGY::SampleDataType::float32;

    const std::string UseZgyCompressionLabel = "USE_ZFP_COMPRESSION";
    const bool useZgyCompression = json.isMember(UseZgyCompressionLabel)
                                       ? json[UseZgyCompressionLabel].asBool()
                                       : false;

    const std::string CompressionGroupLabel = "Compression";
    const std::string ZgyCompressionLabel = "ZFP_SNR";
    float zgyCompression = 100.0F;
    if (json.isMember(CompressionGroupLabel)) {
        Json::Value compressionJson = json[CompressionGroupLabel];
        zgyCompression = useZgyCompression && compressionJson.isMember(ZgyCompressionLabel)
                             ? compressionJson[ZgyCompressionLabel].asFloat()
                             : 100.0F;
    }

    const std::string LodCompressionGroupLabel = "LOD_Compression";
    const std::string UseLodCompressionLabel = "USE_LOD_COMPRESSION";
    bool useLodCompression = false;
    float lodCompression = zgyCompression;
    if (json.isMember(LodCompressionGroupLabel)) {
        Json::Value lodCompressionJson = json[LodCompressionGroupLabel];
        useLodCompression = lodCompressionJson.isMember(UseLodCompressionLabel)
                                ? lodCompressionJson[UseLodCompressionLabel].asBool()
                                : useZgyCompression;

        const std::string LodCompressionLabel = "ZFP_LOD_SNR";
        lodCompression = lodCompressionJson.isMember(LodCompressionLabel)
                             ? lodCompressionJson[LodCompressionLabel].asFloat()
                             : zgyCompression;
    }
    const std::string CustomBrickSizeGroupLabel = "CUSTOM_BRICK_SIZE";
    std::string brick_size = json.isMember(CustomBrickSizeGroupLabel)
                                 ? getBrickSize(json[CustomBrickSizeGroupLabel])
                                 : "";

    if (useZgyCompression || useLodCompression) {
        sampleSize = OpenZGY::SampleDataType::float32;
    }

    return {sampleSize, useZgyCompression, zgyCompression, useLodCompression, lodCompression, brick_size};
}

} // namespace segysdk::utilities
