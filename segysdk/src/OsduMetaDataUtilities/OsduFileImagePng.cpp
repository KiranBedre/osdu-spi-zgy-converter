/*
Copyright 2025 Schlumberger

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

#include "OsduMetaDataUtilities/OsduFileImagePng.h"

#include "OsduMetaDataUtilities/OsduFileImagePngLiterals.h"
#include "Utilities/JsonParser.h"

namespace segysdk::metadata::osdu
{

OsduFileImagePngParams&
OsduFileImagePngParams::withFileSource(std::string_view value)
{
    fileSource_ = value;
    return *this;
}

OsduFileImagePngParams&
OsduFileImagePngParams::withFileSize(const std::uint64_t value)
{
    fileSize_ = value;
    return *this;
}

OsduFileImagePngParams&
OsduFileImagePngParams::withDataFlowId(std::string_view value)
{
    dataFlowId_ = value;
    return *this;
}

OsduFileImagePngParams&
OsduFileImagePngParams::withSliceType(std::string_view value)
{
    sliceType_ = value;
    return *this;
}

OsduFileImagePng::OsduFileImagePng(std::string&& fileCollectionJson)
  : fileCollectionJson_{std::move(fileCollectionJson)}
{
}

static void
copyField(const Json::Value& source, const std::string& key, Json::Value& destination)
{
    if (source.isMember(key)) {
        destination[key] = source[key];
    }
}

static Json::Value
copyCommonFields(const Json::Value& source)
{
    auto result = Json::Value{};
    copyField(source, OsduFileImagePngLiterals::acl, result);
    copyField(source, OsduFileImagePngLiterals::legal, result);
    copyField(source, OsduFileImagePngLiterals::tags, result);
    copyField(source, OsduFileImagePngLiterals::meta, result);
    copyField(source, OsduFileImagePngLiterals::ancestry, result);
    return result;
}

std::string
OsduFileImagePng::asString(const OsduFileImagePngParams& params) const
{
    auto info = Json::Value{};
    info[OsduFileImagePngLiterals::fileSource] = params.fileSource_;
    info[OsduFileImagePngLiterals::fileSize] = static_cast<Json::UInt64>(params.fileSize_);
    auto result = copyCommonFields(parseFromStringRequired(fileCollectionJson_));
    result[OsduFileImagePngLiterals::data][OsduFileImagePngLiterals::isExtendedLoad] = false;
    result[OsduFileImagePngLiterals::data][OsduFileImagePngLiterals::datasetProperties][OsduFileImagePngLiterals::fileSourceInfo] = info;
    result[OsduFileImagePngLiterals::kind] = getResourceKind();
    result[OsduFileImagePngLiterals::tags][OsduFileImagePngLiterals::dataFlowId] = params.dataFlowId_;
    result[OsduFileImagePngLiterals::tags][OsduFileImagePngLiterals::sliceType] = params.sliceType_;
    return result.toStyledString();
}

std::string
OsduFileImagePng::getResourceKind()
{
    return "osdu:wks:dataset--File.Image.PNG:1.1.0";
}

} // namespace segysdk::metadata::osdu
