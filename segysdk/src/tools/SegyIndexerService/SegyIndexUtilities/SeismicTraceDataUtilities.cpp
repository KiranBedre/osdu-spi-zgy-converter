/*
Copyright 2023 Schlumberger

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

#include "../ComputePolygonPlugin/ComputePolygonPlugin.h"
#include "../FileIO/GenericFileManager.h"
#include "../Logger/Logger.h"
#include "../tools/SegyIndexerService/SegyIndexUtilities/SeismicTraceDataUtilities.h"
#include "../Utilities/Authorization.h"
#include "../Utilities/JsonBinGrid.h"
#include "../Utilities/JsonUtilities.h"
#include "../Utilities/osdu/OsduSeismicTraceDataFactory.h"
#include "../Utilities/osdu/OsduWorkProduct100.h"
#include "../Utilities/osdu/WellKnownTypes.h"
#include "../Utilities/SessionUtilities.h"
#include "../Utilities/Settings.h"
#include "../Utilities/TokenSourceFactory.h"

namespace SegyIndexing::Utilities
{

SeismicTraceDataUtilities::SeismicTraceDataUtilities(std::string recordId, const segysdk::library_loader::PluginConfig& pluginConfig)
  : pluginPath_(pluginConfig.getComputePolygonPluginPath().string()),
    workProductId_(),
    seismicTraceDataId_(),
    storageSvcClient_(segysdk::utilities::Authorization::storageSvcTokenProvider())
{
    if (recordId.empty()) {
        return;
    }
    
    if (recordId.find(segysdk::utilities::osdu::WellKnownTypes::SeismicTraceData) != std::string::npos) {
        seismicTraceDataId_ = recordId;
    }
    else if (recordId.find(segysdk::utilities::osdu::WellKnownTypes::WorkProduct) != std::string::npos) {
        workProductId_ = recordId;
    }

    // Handle case where recordId is a local file or sd file and not an OSDU record id.
    const std::string jsonExt = ".json";
    const auto pos = recordId.find(jsonExt);
    if (pos != std::string::npos) {        
        const auto expPos = recordId.size() - jsonExt.size();
        if (pos == expPos) {
            const auto json = segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(recordId);
            if (json.isMember("kind")) {
                const auto kind = json["kind"].asString();
                if (kind.find(segysdk::utilities::osdu::WellKnownTypes::SeismicTraceData) != std::string::npos) {
                    seismicTraceDataId_ = recordId;
                }
                else if (kind.find(segysdk::utilities::osdu::WellKnownTypes::WorkProduct) != std::string::npos) {
                    workProductId_ = recordId;
                }
            }
        }
    }
}

void
SeismicTraceDataUtilities::updateSeismicTraceData(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index)
{
    auto seismicTraceDataOr = getSeismicTraceDataFromId();
    if (not seismicTraceDataOr.has_value()) {
        return;
    } 

    auto seismicTraceData = seismicTraceDataOr.value();

    const auto json = segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(seismicTraceData.binGridId());
    if (json.empty()) {
        std::string errorMsg = "Unable to get SeismicBinGrid from SeismicTraceData : [" + seismicTraceData.binGridId() + "]";
        SEGYLOG_REPORT_ERROR(errorMsg);
        return;
    }
    auto grid = std::make_shared<segysdk::utilities::BinGrid>(segysdk::utilities::JsonBinGrid::loadBinGridFromContent(json.toStyledString()));

    seismicTraceData.checkAndUpdateMissingFields(index, grid);

    StoreOrSaveRecord(seismicTraceData.id(), seismicTraceData.content());
    
}

void
SeismicTraceDataUtilities::savePolygonOutlineToSeismicTraceData(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index)
{
    if (!segysdk::utilities::Environment::instance().computePolygonOutline()) {
        return;
    }

    if (pluginPath_.empty()) {
        SEGYLOG_REPORT_WARNING("savePolygon; No plugin path set for computing Polygon Outline");
        return;
    }

    auto seismicTraceDataOr = getSeismicTraceDataFromId();
    if (!seismicTraceDataOr.has_value()) {
        return;
    }

    auto seismicTraceData = seismicTraceDataOr.value();
    
    if (not seismicTraceData.getLiveTraceOutlinePolygons().empty() and not segysdk::utilities::Environment::instance().overwritePolygonOutline()) {
        SEGYLOG_DEBUG("Seismic Trace Data has already LivePolygons computed. No overwrite specified");
        return;
    }
    legalTag_ = seismicTraceData.legalTag();
    std::shared_ptr<segysdk::utilities::BinGrid> grid = nullptr;
    if (segysdk::utilities::Environment::instance().useBinGridForComputePolygonOutline()) {
        auto binGridId = seismicTraceData.binGridId();
        if (binGridId.empty()) {
            std::string errorMsg = "Bin Grid could not be empty";
            SEGYLOG_REPORT_ERROR(errorMsg);
            return;
        }
        const auto binGridJson = segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(binGridId);
        grid = std::make_shared<segysdk::utilities::BinGrid>(segysdk::utilities::JsonBinGrid::loadBinGridFromContent(binGridJson.toStyledString()));
    }
    try {
        auto libraryLoader = std::make_unique<segysdk::library_loader::DynamicLibraryLoader>(pluginPath_);
        segysdk::compute_polygon::ComputePolygonPlugin polygonPlugin(std::move(libraryLoader));
        bool isMultiple = false;
        auto jsonPolys = polygonPlugin.computeForSegy(index, grid, isMultiple);

        seismicTraceData.addLiveTraceOutline(jsonPolys, isMultiple);
        StoreOrSaveRecord(seismicTraceDataId_, seismicTraceData.content());
    }
    catch (const std::exception& err) {
        // Don't fail indexation if the polygon could not be computed. Just report
        std::cerr << err.what() << std::endl;
        SEGYLOG_REPORT_ERROR(err.what());
    }
}

std::optional<segysdk::utilities::osdu::OsduSeismicTraceData140>
SeismicTraceDataUtilities::extractSeismicTraceDataFromWorkProductId(const std::string& workProductId)
{
    const segysdk::utilities::osdu::OsduWorkProduct100 workProductJson(segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(workProductId));

    for (const auto& componentId : workProductJson.componentIds()) {
        const auto json = segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(componentId);
        const auto objectKind = segysdk::utilities::osdu::OsduObject(json).kind();
        if (objectKind.shortTypeName() == segysdk::utilities::osdu::WellKnownTypes::SeismicTraceData) {
            return segysdk::utilities::osdu::OsduSeismicTraceDataFactory::getOsduSeismicTraceData(json);
        }
    }
    return std::nullopt;
}

std::optional<segysdk::utilities::osdu::OsduSeismicTraceData140>
SeismicTraceDataUtilities::getSeismicTraceDataFromId()
{
    std::optional<segysdk::utilities::osdu::OsduSeismicTraceData140> seismicTraceDataOr{std::nullopt};

    if (!seismicTraceDataId_.empty()) {
        seismicTraceDataOr = segysdk::utilities::osdu::OsduSeismicTraceDataFactory::getOsduSeismicTraceData(segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(seismicTraceDataId_));
    }
    else if (!workProductId_.empty()) {
        seismicTraceDataOr = extractSeismicTraceDataFromWorkProductId(workProductId_);
    }

    if (not seismicTraceDataOr.has_value()) {
        const auto recordId = !seismicTraceDataId_.empty() ? seismicTraceDataId_ : workProductId_;
        std::string errorMsg = "Seismic Trace Data not found in: " + recordId;
        SEGYLOG_REPORT_ERROR(errorMsg);
    }

    return seismicTraceDataOr;
}


bool
SeismicTraceDataUtilities::isMetadataID(const std::string& fileNameOrId)
{
    // Reference should look like this: 
    // "opendes:dataset--FileCollection.SEGY:1429f828654449fb8f7e635543b65860"
    size_t hits = 0;
    size_t offset = 0;
    auto pos = fileNameOrId.find(":");
    while (pos != std::string::npos) {
        hits++;
        offset += pos + 1;
        const auto mySearchId = fileNameOrId.substr(offset);
        pos = mySearchId.find(":");
    }

    return hits >= 2;
}

bool
SeismicTraceDataUtilities::isLocalFile(const std::string& fileNameOrId)
{
    if (fileNameOrId.find("sd://")) {
        return false;
    }

    if (isMetadataID(fileNameOrId)) {
        return false;
    }

    return std::filesystem::exists(fileNameOrId);
}

segysdk::fileio::FileReaderOptions
SeismicTraceDataUtilities::getFileReaderOptions(const std::string& fileName, const std::string& legalTag)
{
    if (isLocalFile(fileName)) {
        return segysdk::fileio::FileReaderOptions::localOnly();
    }

    const segysdk::SDCredentials credentials(
        segysdk::utilities::Environment::instance().sdUrl(),
        segysdk::utilities::Environment::instance().sdApiKey(),
        segysdk::utilities::TokenSourceFactory::getTokenSource(),
        segysdk::utilities::Settings::instance().getHttpHeaders());

    return {credentials, segysdk::utilities::SessionUtilities::getSDCacheSize(), legalTag};

}

void
SeismicTraceDataUtilities::StoreOrSaveRecord(const std::string& metadataId, const std::string& metadataRecord)
{
    if (isMetadataID(metadataId)) {
        // metadata id
        storageSvcClient_.storeRecord(metadataRecord);
    }
    else {
        auto fro = getFileReaderOptions(metadataId);
        segysdk::fileio::FileReference fi(metadataId, fro);
        segysdk::fileio::GenericFileManager::writeTextFile(fi, metadataRecord, true);
    }
}

} // namespace SegyIndexing::Utilities
