/*
Copyright 2022-2025 Schlumberger

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

#include "DataPartition.h"
#include "Params.h"

#include "FileIO/GenericFileManager.h"
#include "Logger/Logger.h"
#include "NavigationExtractorPlugin/NavigationExtractorPlugin.h"
#include "OsduMetaDataUtilities/OsduSeismicTraceData.h"
#include "OsduMetaDataUtilities/OsduSeismicTraceDataStruct.h"
#include "SegyIO/SegyIOException.h"
#include "SegyIndex/SegyIndexImpl.h"
#include "SegyIndex/SeismicCubeExtents.h"
#include "StatisticsGeneratorPlugin/StatisticsGeneratorPlugin.h"
#include "StatisticsGeneratorPlugin/StatisticsInputParams.h"
#include "Utilities/Environment.h"
#include "Utilities/EnvironmentVariableStringConsts.h"
#include "Utilities/EnvironmentVariables.h"
#include "Utilities/InputParser.h"
#include "Utilities/JsonUtilities.h"
#include "Utilities/SessionUtilities.h"
#include "segysdk/SegySession.h"
#include "tools/SegyIndexerService/SegyIndexUtilities/SeismicTraceDataUtilities.h"

#include <cstdlib>
#include <filesystem>
#include <iostream>
#include <vector>

using SegyIndexPtr = std::shared_ptr<segysdk::segyindex::SegyIndex>;

static void
showGeometry(const SegyIndexPtr& index)
{
    if (index->hasIndexFile()) {
        segysdk::segyindex::SeismicCubeExtents const seismicCubeExtents = index->getFileLayoutExtents();

        SEGYLOG_REPORT_INFO(std::string("segyIndexerService")
                            + "; firstInline: " + std::to_string(seismicCubeExtents.getJVectorExtent().first())
                            + "; lastInline: " + std::to_string(seismicCubeExtents.getJVectorExtent().last())
                            + "; inlineStep: " + std::to_string(seismicCubeExtents.getJVectorExtent().step())
                            + "; firstCrossline: " + std::to_string(seismicCubeExtents.getIVectorExtent().first())
                            + "; lastCrossline: " + std::to_string(seismicCubeExtents.getIVectorExtent().last())
                            + "; crosslineStep: " + std::to_string(seismicCubeExtents.getIVectorExtent().step())
                            + "; firstSampleAt: " + std::to_string(seismicCubeExtents.getSampleExtent().first())
                            + "; lastSampleAt: " + std::to_string(seismicCubeExtents.getSampleExtent().last())
                            + "; sampleInterval: " + std::to_string(seismicCubeExtents.getSampleExtent().step()));
    }
}

static void
printUsage(const std::string& caller)
{
    std::cout << "\nUsage: \n";
    std::cout << "  For OSDU Data Platform: \n";
    std::cout << "    " + caller + " -osdu <FileCollection.SEGY.json> [-forceOverwrite]\n";
    std::cout << "  or \n";
    std::cout << "    " + caller + " -osdu_id <SDMS FileCollection id> [-forceOverwrite] [-seismic_trace_data </path/to/SeismicTraceData.json>]\n";
    std::cout << "    " + caller + " -osdu_id <SDMS FileCollection id> [-forceOverwrite] [-seismic_trace_data_id <SeismicTraceData_ID>]  [-work_product_id <WorkProduct_ID>]\n";
    std::cout << "  or \n";
    std::cout << "    " + caller + " -config <ConfigurationFile>\n";
    std::cout << "  forceOverwrite will overwrite an existing index\n";
    std::cout << "  <SeismicTraceData_ID> will be populated with indexer results\n";
    std::cout << "  <WorkProduct_ID> will be used to retrieve SeismicTraceData if SeismicTraceData reference is not available\n";
    std::cout << "To access data in cloud SDMS the following environment variables must be defined:\n";
    std::cout << "  " << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_URL + "\n";
    std::cout << "  " << segysdk::utilities::EnvironmentVariableStringConsts::STORAGE_SVC_URL + "\n";
    std::cout << "  " << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_API_KEY + "\n";
    std::cout << "  " << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_TOKEN + "\n";
    std::cout << "To compute Live Polygon Outline following environment variables could be defined:\n";
    std::cout << "  " << segysdk::utilities::EnvironmentVariableStringConsts::COMPUTE_POLYGON_OUTLINE + "\n";
    std::cout << "  " << segysdk::utilities::EnvironmentVariableStringConsts::OVERWRITE_POLYGON_OUTLINE + "\n";
    std::cout << "To enable generation of seismic trace histogram the following environment variables should be defined:\n";
    std::cout << "  " << segysdk::utilities::EnvironmentVariableStringConsts::CALCULATE_STATISTICS + "\n";
    std::cout << "  " << segysdk::utilities::EnvironmentVariableStringConsts::OVERWRITE_STATISTICS + "\n";
}

static void
updateSeismicTraceData(const indexer::Params& params,
                       const SegyIndexPtr& index,
                       const segysdk::library_loader::PluginConfig& pluginConfig)
{
    const auto& recordId = params.seismicTraceData_.empty() ? params.workProductId_ : params.seismicTraceData_;
    if (recordId.empty()) {
        SEGYLOG_REPORT_WARNING("can't update SeismicTraceData: no record reference provided");
        return;
    }
    SEGYLOG_REPORT_INFO("updating SeismicTraceData using reference: " + recordId);
    auto record = SegyIndexing::Utilities::SeismicTraceDataUtilities{recordId, pluginConfig};
    record.updateSeismicTraceData(index);
    if (segysdk::utilities::Environment::instance().computePolygonOutline()) {
        record.savePolygonOutlineToSeismicTraceData(index);
    }
}

static void
validateCommandLineOptions(const segysdk::utilities::InputParser& input)
{
    if (!input.cmdOptionExists("-osdu") && !input.cmdOptionExists("-osdu_id") && !input.cmdOptionExists("-config")) {
        throw segysdk::SegyInvalidArgumentException{"provide either -osdu or -osdu_id or -config command line parameters"};
    }
}

static void
promptForToken()
{
    std::cout << "Please enter " << segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_TOKEN << ": ";
    std::string token;
    std::cin >> token;
    segysdk::utilities::EnvironmentVariable::setStringEnv(segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_TOKEN, token);
    if (const auto test = segysdk::utilities::Environment::instance().sdToken(); test != token) {
        throw segysdk::segyio::SegyRuntimeException{"failed to setup " + segysdk::utilities::EnvironmentVariableStringConsts::SD_SVC_TOKEN};
    }
}

static void
beforeExitCleanUp()
{
    try {
        segysdk::logger::Logger::getInstance().stopLogWriterDispatcher();
        segysdk::fileio::GenericFileManager::reset();
    }
    catch (const std::exception& error) {
        std::cerr << __func__ << ':' << __LINE__ << ':' << error.what() << std::endl;
    }
}

static bool
isSegy2D(const std::string& seismicTraceData)
{
    const Json::Value json = segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(seismicTraceData);
    if (json.empty()) {
        return false;
    }
    const auto partition = indexer::getPartitionFromSeismicTraceData(json);
    auto osduSeismicTraceData = segysdk::metadata::osdu::OsduSeismicTraceData(json, partition);
    if (const auto traceData = std::reinterpret_pointer_cast<segysdk::metadata::osdu::OsduSeismicTraceDataStruct>(osduSeismicTraceData.getMetaDataStruct());
        traceData->seismicTraceDataSpecificProperties.has_value()) {
        const auto& properties = traceData->seismicTraceDataSpecificProperties.value();
        if (properties.seismicTraceDataDimensionalityTypeID.has_value()) {
            return properties.seismicTraceDataDimensionalityTypeID.value().find("2") != std::string::npos;
        }
        if (properties.binGridID.has_value()) {
            return false;
        }
        if (properties.preferred3DInterpretationSetID.has_value()) {
            return false;
        }
        if (properties.seismic2DName.has_value()) {
            return true;
        }
        if (properties.preferred2DInterpretationSetID.has_value()) {
            return true;
        }
    }
    return false;
}

static SegyIndexPtr
getSegyIndex(const indexer::Params& params)
{
    const auto& fileCollectionReference = params.fileCollectionPath_.empty() ? params.fileCollectionId_ : params.fileCollectionPath_;
    if (fileCollectionReference.empty()) {
        throw segysdk::SegyInvalidArgumentException{"need file collection path or ID"};
    }
    const auto session = segysdk::utilities::SessionUtilities::GetSegySessionFromFileCollectionJson(fileCollectionReference);
    if (!session) {
        throw segysdk::segyio::SegyRuntimeException{"failed to create SEG-Y session"};
    }
    auto result = session->getIndex();
    if (!result) {
        throw segysdk::segyio::SegyRuntimeException{"failed to get SEG-Y session index"};
    }
    session->setSegy2DFlag(params.isSegy2D);
    if (!result->hasIndexFile(true) || params.forceOverwrite_ || segysdk::utilities::Environment::instance().overwriteIndex()) {
        SEGYLOG_INFO("generating SEG-Y index file...");
        session->generateIndexFile(false, true);
    }
    else {
        SEGYLOG_REPORT_WARNING("using existing SEG-Y index file");
    }
    return result;
}

static void
generateStatisticsAndHistogram(const segysdk::library_loader::PluginConfig& pluginConfig, const indexer::Params& params)
{
    const auto pluginPath = pluginConfig.getCalculateStatisticsPluginPath().string();
    if (pluginPath.empty()) {
        throw segysdk::segyio::SegyRuntimeException{"failed to get statistics calculation plugin path"};
    }
    if (!std::filesystem::exists(pluginPath)) {
        throw segysdk::segyio::SegyRuntimeException{"statistics calculation plugin doesn't exist"};
    }
    segysdk::statistics_generator::StatisticsGeneratorPlugin statisticsPlugin(std::make_unique<segysdk::library_loader::DynamicLibraryLoader>(pluginPath));
    const bool forceOverwrite = segysdk::utilities::Environment::instance().overwriteStatistics();
    const bool verbosity = segysdk::utilities::Environment::instance().slbPluginsVerbosity();
    const auto& fileCollectionSegyId = params.fileCollectionId_.empty() ? params.fileCollectionPath_ : params.fileCollectionId_;
    const auto inputParams = segysdk::statistics_generator::StatisticsInputParams(
        fileCollectionSegyId,
        params.seismicTraceData_,
        params.workProductId_,
        forceOverwrite,
        verbosity);
    if (statisticsPlugin.GenerateForSegy(&inputParams) != 0) {
        throw segysdk::segyio::SegyRuntimeException{"failed to generate statistics"};
    }
}

static void
runIndexer(segysdk::logger::LoggerSettings& settings, const indexer::Params& params)
{
    const auto pluginConfig = segysdk::library_loader::PluginConfig(segysdk::utilities::Environment::instance().segysdkConfigurationFile());
    settings.setCorrelationId(segysdk::utilities::Environment::instance().correlationId());
    settings.setFileCollectionId(params.fileCollectionId_);
    settings.setLogWriterPluginPath(pluginConfig);
    segysdk::logger::initializeLogger(settings);
    if (const auto index = getSegyIndex(params); index->isSegy3D()) {
        showGeometry(index);
        if (segysdk::utilities::Environment::instance().calculateStatistics()) {
            generateStatisticsAndHistogram(pluginConfig, params);
        }
        updateSeismicTraceData(params, index, pluginConfig);
    }
}

static void
getOption(const segysdk::utilities::InputParser& input, const std::string& option, std::string& output)
{
    if (const auto tmp = input.getCmdOption("-" + option); !tmp.empty()) {
        output = tmp;
    }
}

static void
getPatternOption(const segysdk::utilities::InputParser& input, const std::string& option, std::string& output, std::string_view pattern)
{
    auto tmp = std::string{};
    getOption(input, option, tmp);
    if (tmp.find(pattern) != std::string::npos) {
        output = tmp;
    }
}

static auto
getIndexerParams(const segysdk::utilities::InputParser& input)
{
    auto result = std::vector<indexer::Params>{};
    if (const auto configFileName = std::filesystem::path{input.getCmdOption("-config")}; std::filesystem::exists(configFileName)) {
        const auto params = segysdk::navigation_extractor::InputParams{configFileName};
        for (const auto& [fileCollectionId, seismicTraceDataId] : params.getSegyLines()) {
            result.emplace_back(fileCollectionId, seismicTraceDataId);
            result.back().seismic2DInterpretationSet_ = params.getSeismic2DInterpretationSetId();
            result.back().forceOverwrite_ = params.getForceOverwrite();
            result.back().isSegy2D = params.isSegy2D();
        }
    }
    else {
        auto entry = indexer::Params{};
        getOption(input, "osdu", entry.fileCollectionPath_);
        getOption(input, "osdu_id", entry.fileCollectionId_);
        getPatternOption(input, "seismic_trace_data_id", entry.seismicTraceData_, ":work-product-component--SeismicTraceData:");
        getPatternOption(input, "seismic_trace_data", entry.seismicTraceData_, ".json");
        getPatternOption(input, "work_product_id", entry.workProductId_, ":work-product--WorkProduct:");
        getPatternOption(input, "seismic_2d_interpretation_set_id", entry.seismic2DInterpretationSet_, ":master-data--Seismic2DInterpretationSet:");
        entry.forceOverwrite_ = input.cmdOptionExists("-forceOverwrite");
        if (entry.seismicTraceData_.empty()) {
            SEGYLOG_REPORT_WARNING("SeismicTraceData is not provided, assuming 3D indexing");
        }
        else {
            entry.isSegy2D = isSegy2D(entry.seismicTraceData_);
        }
        result.emplace_back(entry);
    }
    return result;
}

static auto
commandLineToString(const int argc, const char* argv[])
{
    auto result = std::string{};
    for (auto i = 0; i < argc; ++i) {
        result.append(argv[i]).append(" ");
    }
    return result;
}

int
main(const int argc, const char* argv[])
{
    const auto input = segysdk::utilities::InputParser{argc, argv};
    if (input.cmdOptionExists("-h") || input.cmdOptionExists("-help")) {
        printUsage(argv[0]);
        return EXIT_SUCCESS;
    }
    auto settings = segysdk::logger::LoggerSettings{};
    segysdk::logger::initializeLogger(settings);
    segysdk::logger::Logger::setLogLevel(segysdk::utilities::Environment::instance().indexingVerbosity() == 0
                                             ? segysdk::logger::LogLevel::Info
                                             : segysdk::logger::LogLevel::Debug);
    auto result = EXIT_SUCCESS;
    auto workingParams = indexer::Params{};
    try {
        if (input.cmdOptionExists("-promptForToken")) {
            promptForToken();
        }
        validateCommandLineOptions(input);
        for (const auto& params : getIndexerParams(input)) {
            workingParams = params;
            runIndexer(settings, params);
        }
    }
    catch (const std::exception& error) {
        SEGYLOG_REPORT_INFO(error.what());
        printUsage(argv[0]);
        std::cout
            << "\ncalled as:\n  "
            << commandLineToString(argc, argv)
            << "\n\nworking parameters:\n"
            << workingParams.toString()
            << std::endl;
        result = EXIT_FAILURE;
    }
    beforeExitCleanUp();
    return result;
}
