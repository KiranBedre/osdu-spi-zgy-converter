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

#include "tools/StorageConnector/StorageConnector.h"

#include "NavigationExtractorPlugin/InputParams.h"
#include "OsduMetaDataUtilities/OsduFileImagePng.h"
#include "OsduMetaDataUtilities/OsduFileImagePngLiterals.h"
#include "Utilities/Containers.h"
#include "Utilities/JsonParser.h"
#include "Utilities/osdu/WellKnownTypes.h"
#include "tools/Error/Error.h"
#include "tools/FileCollection/FileCollection.h"
#include "tools/Keywords/Keywords.h"
#include "tools/StorageClient/StorageClient.h"

#include <filesystem>

namespace tool
{

StorageConnector::~StorageConnector() = default;

StorageConnector::StorageConnector(StorageClientPtr&& client)
  : client_{std::move(client)}
{
}

static std::string
trimTrailingColons(std::string_view source)
{
    auto result = std::string{source};
    while (!result.empty() && result.back() == ':') {
        result.pop_back();
    }
    return result;
}

static segysdk::utilities::Strings
getSeismicTraceDataIds(const Params& params, const StorageClient& client)
{
    if (!params.seismicTraceDataId_.empty()) {
        return {params.seismicTraceDataId_};
    }
    auto result = segysdk::utilities::Strings{};
    const auto& workProduct = segysdk::parseFromStringRequired(client.getRecord(params.workProductId_));
    for (const auto& component : workProduct[keywords::data][keywords::components]) {
        if (const auto& componentId = trimTrailingColons(component.asString());
            componentId.find(segysdk::utilities::osdu::WellKnownTypes::SeismicTraceData) != std::string::npos) {
            result.push_back(componentId);
        }
    }
    return result;
}

static void
addDescriptor(Context& dest, const segysdk::navigation_extractor::SegyLine& line, const StorageConnector& connector)
{
    dest.seismicTraceDataDescriptors_.push_back(
        SeismicTraceDataDescriptor{line.seismicTraceDataId_}
            .withFileCollectionDescriptor(
                FileCollectionDescriptor{line.fileCollectionId_, connector.getJsonRecord(line.fileCollectionId_)})
            .withSeismicLineGeometryId(line.seismicLineGeometryId_));
}

static auto
getContextFromConfigFile(const std::string& configFile, const StorageConnector& connector)
{
    auto result = Context{};
    for (const auto& line : segysdk::navigation_extractor::InputParams{configFile}.getLines()) {
        addDescriptor(result, line, connector);
    }
    return result;
}

static auto
getContextFromTraceDataIds(const Params& params, const segysdk::utilities::Strings& ids, const StorageConnector& connector)
{
    auto result = Context{};
    for (const auto& seismicTraceDataId : ids) {
        const auto& seismicTraceData = connector.getJsonRecord(seismicTraceDataId);
        for (const auto& dataset : seismicTraceData[keywords::data][keywords::datasets]) {
            const auto& datasetId = trimTrailingColons(dataset.asString());
            if (isFileCollectionSEGY(datasetId) || isFileCollectionSlbOpenZGY(datasetId)) {
                addDescriptor(
                    result,
                    segysdk::navigation_extractor::SegyLine{}
                        .withSeismicTraceDataId(seismicTraceDataId)
                        .withFileCollectionId(datasetId)
                        .withSeismicLineGeometryId(
                            params.seismicLineGeometryId_.empty()
                                ? seismicTraceData[keywords::data][keywords::seismicLineGeometryId].asString()
                                : params.seismicLineGeometryId_),
                    connector);
            }
        }
    }
    return result;
}

Context
StorageConnector::getContext(const Params& params) const
{
    auto result = std::filesystem::exists(params.configFile_)
                      ? getContextFromConfigFile(params.configFile_, *this)
                      : getContextFromTraceDataIds(params, getSeismicTraceDataIds(params, *client_), *this);
    if (result.seismicTraceDataDescriptors_.empty()) {
        throw Error{std::string{"no valid SeismicTraceData references found"}.append(params.toString())};
    }
    return result;
}

std::string
StorageConnector::addFileImagePngRecord(const Params& params, const std::string& fileSource, const std::uint64_t fileSize) const
{
    using namespace segysdk::metadata::osdu;
    return client_->storeRecord(
        OsduFileImagePng{client_->getRecord(params.fileCollectionId_)}
            .asString(OsduFileImagePngParams{}
                          .withFileSource(fileSource)
                          .withFileSize(fileSize)
                          .withDataFlowId(params.correlationId_)
                          .withSliceType(params.sliceType_)));
}

static auto
parseDataPartition(std::string_view input)
{
    const auto pos = input.find(':');
    if (pos == std::string::npos) {
        throw Error{std::string{"failed to parse data partition from '"}.append(input).append("'")};
    }
    return std::string{input.substr(0, pos)};
}

static auto
appendIdentifierVersion(std::string_view identifier)
{
    // {identifier}:{version}, where empty version means the latest one
    return std::string{identifier}.append(":");
}

std::string
StorageConnector::addFileImagePngArtefacts(const Params& params, const segysdk::Strings& resourceIds) const
{
    using namespace segysdk::metadata::osdu;
    auto seismicTraceData = getJsonRecord(params.seismicTraceDataId_);
    for (const auto& resourceId : resourceIds) {
        auto artefact = Json::Value{};
        artefact[OsduFileImagePngLiterals::resourceId] = appendIdentifierVersion(resourceId);
        artefact[OsduFileImagePngLiterals::resourceKind] = OsduFileImagePng::getResourceKind();
        artefact[OsduFileImagePngLiterals::roleId] = appendIdentifierVersion(
            parseDataPartition(resourceId).append(":reference-data--ArtefactRole:EmbeddedImage"));
        seismicTraceData[OsduFileImagePngLiterals::data][OsduFileImagePngLiterals::artefacts].append(artefact);
    }
    return client_->storeRecord(seismicTraceData.toStyledString());
}

Json::Value
StorageConnector::getJsonRecord(const std::string& recordId) const
{
    auto it = recordsCache_.find(recordId);
    if (it == recordsCache_.end()) {
        it = recordsCache_.try_emplace(recordId, segysdk::parseFromStringRequired(client_->getRecord(recordId))).first;
    }
    return it->second;
}

std::string
StorageConnector::storeJsonRecord(const Json::Value& record) const
{
    return client_->storeRecord(record.toStyledString());
}

} // namespace tool
