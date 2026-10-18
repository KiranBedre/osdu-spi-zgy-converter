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

#include "ComputePolygonPlugin/ComputePolygonPlugin.h"
#include "FileIO/GenericFileManager.h"
#include "LibraryLoader/PluginConfig.h"
#include "Logger/Logger.h"
#include "Logger/LoggerSettings.h"
#include "OsduMetaDataUtilities/OsduSeismicTraceData.h"
#include "OsduMetaDataUtilities/OsduSeismicTraceDataStruct.h"
#include "StatisticsGeneratorPlugin/StatisticsGeneratorPlugin.h"
#include "Utilities/Adapters.h"
#include "Utilities/BinGrid.h"
#include "Utilities/BrickSize.h"
#include "Utilities/Environment.h"
#include "Utilities/GuidGenerator.h"
#include "Utilities/JsonUtilities.h"
#include "Utilities/StringUtils.h"
#include "segysdk/SegySessionFactory.h"
#include "tools/SegyToZgy/VersionInfo.h"
#include "tools/SegyToZgy/library/BrickedConversion.h"
#include "tools/SegyToZgy/library/ConversionUnit.h"
#include "tools/SegyToZgy/library/SegyInputParameters.h"
#include "tools/SegyToZgy/library/ZgyMetaData.h"
#include "tools/SegyToZgy/library/ZgyOptions.h"
#include "tools/SegyToZgy/library/ZgyOptionsOsdu.h"
#include "tools/SegyToZgy/library/crs/CoordinateReferenceSystemResolver.hpp"
#include "tools/SegyToZgy/library/crs/PersistableReferenceCrs.h"

#include <openzgy/api.h>

#pragma warning(push)
#pragma warning(disable : 4251)
#include <SDAPI/SDGenericDataset.h>
#include <SDAPI/SDManager.h>
#pragma warning(pop)

#include <filesystem>
#include <sstream>
#include <thread>

static void
printBuildInfo(void)
{
    std::stringstream ss;

    /*
     * expected output:
     *
     * Application: <Product Name>
     * Version: <Major>.<Minor>.<Patch>.<Tweak>
     * Build date: <Date> <Time>
     * Copyright: <Copyright Message>
     *
     */
    ss << std::endl;
    ss << "Application: " << SEGY_TO_ZGY_PRODUCTNAME_STR << std::endl;
    ss << "Version: " << SEGY_TO_ZGY_PRODUCTVERSION_STR << std::endl;
    ss << "Build date: " << SEGY_TO_ZGY_TIMESTAMP_STR << std::endl;
    ss << "Copyright: " << SEGY_TO_ZGY_LEGALCOPYRIGHT_STR << std::endl;

    SEGYLOG_DEBUG(ss.str());
}

static void
logConfig(const segysdk::library_loader::PluginConfig& pluginConfig)
{
    std::stringstream ss;
    ss << SEGY_TO_ZGY_PRODUCTNAME_STR << "; ";
    ss << "version: " << SEGY_TO_ZGY_PRODUCTVERSION_STR << "; ";
    ss << "build: " << SEGY_TO_ZGY_TIMESTAMP_STR << "; ";
    ss << "logWriterPlugin:" << pluginConfig.isLogWriterPluginConfigured() << "; ";
    ss << "computePolygonPlugin:" << pluginConfig.isComputePolygonPluginConfigured() << "; ";
    ss << "extractNavigationPlugin:" << pluginConfig.isExtract2DNavigationPluginConfigured() << "; ";
    ss << "calculateStatisticsPlugin:" << pluginConfig.isCalculateStatisticsPluginConfigured() << "; ";
    ss << "calculateBinGridPlugin:" << pluginConfig.isCalculateBinGridPluginConfigured() << "; ";

    SEGYLOG_GSM_INFO(ss.str());
}

void
validate_transformation_code(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index_ptr, const ZgyOptions* options)
{
    if ((index_ptr->getGeometry3D().isRightHanded() && options->binGrid().transformationMethod != segysdk::utilities::BinGrid::TransformationMethod::RightHanded)
        || (!index_ptr->getGeometry3D().isRightHanded() && options->binGrid().transformationMethod != segysdk::utilities::BinGrid::TransformationMethod::LeftHanded)) {
        const std::string is_right_handed = index_ptr->getGeometry3D().isRightHanded() ? "True" : "False";
        SEGYLOG_REPORT_ERROR("*** Input SEG-Y volume geometry is inconsistent with SeismicBinGrid:\n"
                             "Specified Transformation: \n"
                             "SEG-Y isRightHanded = "
                             + is_right_handed + "  but SeismicBinGrid TransformationMethod = " + std::to_string(static_cast<int>(options->binGrid().transformationMethod)) + "\n");
    }
}

static OpenZGY::ZgyWriterArgs
createZgyWriterArgs(const ZgyOptions* options, const SegyInputParameters& p)
{
    const segysdk::utilities::BinGrid bin_grid = options->binGrid();
    SEGYLOG_DEBUG("Loaded bin grid");

    // The corners are ordered origin, last inline(i.e.i = last, j = 0),
    // last crossline, diagonal.
    // This is for the write layout, which for an inline sorted volume will have the first trace in the file as the origin, (File first inline, first crossline)
    // So, for decreasing increments we cannot just use the geometry values.

    const auto index_ptr = p.loadOrGenerateIndex();

    validate_transformation_code(index_ptr, options);

    const auto corner1 = bin_grid.getCoordinatesIJ(index_ptr->getFileLayout().JExtent.first(), index_ptr->getFileLayout().IExtent.first());

    const auto corner2 = bin_grid.getCoordinatesIJ(index_ptr->getFileLayout().JExtent.last(), index_ptr->getFileLayout().IExtent.first());

    const auto corner3 = bin_grid.getCoordinatesIJ(index_ptr->getFileLayout().JExtent.first(), index_ptr->getFileLayout().IExtent.last());

    const auto corner4 = bin_grid.getCoordinatesIJ(index_ptr->getFileLayout().JExtent.last(), index_ptr->getFileLayout().IExtent.last());

    const auto conversion_unit = getBinGridConversionUnit(bin_grid);
    SEGYLOG_DEBUG("Got conversion unit: " + conversion_unit.name);

    // We will populate the ZGY volume in the same trace order as we find in the SEG-Y file, so watch out for negative trace increments for either inlines or crosslines
    // as this will cause an incorrect cube layout to be generated! Inline Start and increment and Crossline start and increment should be taken from FileLayout in the index!

    OpenZGY::ZgyWriterArgs zgyWriterArgs = options->zgyWriterArgs()
                                               .corners({corner1.easting, corner1.northing, corner2.easting, corner2.northing, corner3.easting, corner3.northing, corner4.easting, corner4.northing})
                                               .datarange(p.getDataRangeLow(), p.getDataRangeHigh())
                                               .hunit(OpenZGY::UnitDimension::length, conversion_unit.name, conversion_unit.value)
                                               .ilinc(static_cast<float>(index_ptr->getFileLayout().JExtent.step()))
                                               .ilstart(static_cast<float>(index_ptr->getFileLayout().JExtent.first()))
                                               .size(p.getInlineCount(), p.getCrosslineCount(), p.getVerticalCount())
                                               .xlinc(static_cast<float>(index_ptr->getFileLayout().IExtent.step()))
                                               .xlstart(static_cast<float>(index_ptr->getFileLayout().IExtent.first()))
                                               .zinc(p.getVerticalIncrement())
                                               .zstart(p.getVerticalStart())
                                               .zunit(p.getVerticalUnitDimension(), p.getVerticalUnitName(), p.getVerticalUnitValue());

    std::ostringstream tmp;
    zgyWriterArgs.dump(tmp);
    SEGYLOG_DEBUG(tmp.str());

    return zgyWriterArgs;
}

static std::mutex progressMutex{};
static constexpr size_t progressWidth{80};
std::chrono::steady_clock::time_point progressLastAnnounced{std::chrono::steady_clock::now()};

static bool
finalizeProgress(uint64_t done, uint64_t total)
{
    if (total == 0)
        return true;

    const std::int64_t needed = (total <= 0) ? 1 : 1 + ((done * (progressWidth - 1)) / total);
    const std::string bar{"[" + std::string(needed, '#') + std::string(progressWidth - needed, '_') + "] " + std::to_string(100.0 * done / total) + "%"};

    std::lock_guard lk(progressMutex);
    const auto now = std::chrono::steady_clock::now();
    const auto elapsed = now - progressLastAnnounced;
    if (elapsed > std::chrono::minutes(1)) {
        SEGYLOG_DEBUG(bar);
        progressLastAnnounced = now;
    }

    return true;
}

static void
copySeismicData(const segysdk::utilities::BrickSize brickSize,
                segysdk::api::SegySession* segySession,
                const SegyInputParameters& p,
                const OpenZGY::ZgyWriterArgs& zgyWriterArgs)
{
    const auto zgy_writer = OpenZGY::IZgyWriter::open(zgyWriterArgs);

    SEGYLOG_DEBUG("ZGY writer created");

    BrickedConversion bc(segySession, p, zgy_writer.get(), brickSize);

    bc.run();

    SEGYLOG_DEBUG("Closing ZGY");

    zgy_writer->finalize({}, finalizeProgress);
    zgy_writer->close();
}

static void
updateSeismicMetadata(const ZgyOptions* options)
{
    if (not segysdk::utilities::isSDDataset(options->targetFile())) {
        std::string msg = "Target file " + options->targetFile() + " is not a SD data set. Metadata not updated.";
        SEGYLOG_REPORT_INFO(msg);
        return;
    }
    // Get latest version of metdata from fileService:
    // const auto osduOptions = static_cast<const ZgyOptionsOsdu*>(options);
    // auto stdJson = segysdk::utilities::JsonUtilities::getJsonStringFromPathOrOsduId(osduOptions->getSeismicTraceDataId());

    std::string stdJson = "";
    const auto manager = options->createSDManager();
    if (options->isOsdu()) {
        std::vector<std::string> segy_file_names(options->sourceFiles());
        auto sourceDataset = seismicdrive::SDGenericDataset(manager.get(), segy_file_names.at(0));
        sourceDataset.open(seismicdrive::SDDatasetDisposition::READ_ONLY);
        stdJson = sourceDataset.getMetaData();
        SEGYLOG_REPORT_INFO("stdJson:");
        SEGYLOG_REPORT_INFO(stdJson);
        return;
    }
    SEGYLOG_WARNING("Updating seismic metadata");

    if (ZgyMetaData::clone(manager.get(), stdJson, options->targetFile())) {
        SEGYLOG_REPORT_INFO("Seismic metadata successfully updated");
    }
    else {
        const std::string msg = "Seismic metadata for " + options->targetFile() + " was not correctly updated!";
        SEGYLOG_REPORT_WARNING(msg);
    }
}

static void
updateSeismicTraceData(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index, const ZgyOptions* options)
{
    if (options->isOsdu()) {
        const auto osduOptions = static_cast<const ZgyOptionsOsdu*>(options);
        osduOptions->updateSeismicTraceData(index);
    }
}

static void
createPolygonOutlines(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index,
                      const ZgyOptions* options,
                      const segysdk::library_loader::PluginConfig& pluginConfig)
{
    if (!segysdk::utilities::Environment::instance().computePolygonOutline()) {
        return;
    }

    const auto pluginPath = pluginConfig.getComputePolygonPluginPath();
    if (pluginPath.empty()) {
        return;
    }
    try {
        auto libraryLoader = std::make_unique<segysdk::library_loader::DynamicLibraryLoader>(pluginPath.string());
        segysdk::compute_polygon::ComputePolygonPlugin polygonPlugin(std::move(libraryLoader));
        bool isMultiple = false;
        auto binGrid = std::make_shared<segysdk::utilities::BinGrid>(options->binGrid());
        auto jsonPolys = polygonPlugin.computeForSegy(index, binGrid, isMultiple);

        SEGYLOG_INFO("Computed Polygons:" + jsonPolys.toStyledString());

        if (options->isOsdu()) {
            const auto osduOptions = static_cast<const ZgyOptionsOsdu*>(options);
            osduOptions->updateSeismicTraceDataLiveOutline(jsonPolys, isMultiple);
        }
    }
    catch (const std::exception& err) {
        // Don't fail a conversion if the polygon could not be computed. Just report
        std::cerr << err.what() << std::endl;
        SEGYLOG_REPORT_ERROR(err.what());
    }
}

bool
checkIfStatisticsShouldBeGenerated(const std::string& osduSeismicTraceDataId, bool forceOverWrite)
{
    const std::string sdStart = "sd://";
    const std::string histogramArtefact = "dataset--File.SeismicHistogram";

    if (osduSeismicTraceDataId.empty()) {
        return false;
    }
    const auto json = segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(osduSeismicTraceDataId);
    std ::string osduPartition = "NoPartition";
    auto pos = osduSeismicTraceDataId.find("sd://");
    if (pos != std::string::npos) {
        auto start = pos + sdStart.size();
        const auto tmpStr = osduSeismicTraceDataId.substr(start + 1);
        pos = tmpStr.find('/');
        if (pos != std::string::npos) {
            osduPartition = tmpStr.substr(0, pos - 1);
        }
    }
    else {
        auto parts = segysdk::utilities::StringUtils::split(osduSeismicTraceDataId, ":");
        if (parts.size() >= 2 and parts.size() <= 3) {
            osduPartition = parts[0];
        }
    }
    auto std = segysdk::metadata::osdu::OsduSeismicTraceData(json, osduPartition);
    auto stdStruct = std::reinterpret_pointer_cast<segysdk::metadata::osdu::OsduSeismicTraceDataStruct>(std.getMetaDataStruct());

    if (stdStruct->seismicTraceDataSpecificProperties.has_value()) {
        if (stdStruct->seismicTraceDataSpecificProperties.value().rangeAmplitudeMax.has_value()
            and stdStruct->seismicTraceDataSpecificProperties.value().rangeAmplitudeMin.has_value()) {
            return forceOverWrite;
        }
        if (stdStruct->seismicTraceDataSpecificProperties.value().wpcGroupTypeStruct.has_value()
            and !stdStruct->seismicTraceDataSpecificProperties.value().wpcGroupTypeStruct.value().artefacts.has_value()) {
            return true;
        }
        else {
            for (const auto& artefact : stdStruct->seismicTraceDataSpecificProperties.value().wpcGroupTypeStruct.value().artefacts.value()) {
                if (artefact.resourceID.has_value() and artefact.resourceID.value().find(histogramArtefact) != std::string::npos) {
                    return forceOverWrite;
                }
            }
        }
    }

    return true;
}

static void
generateStatisticsAndMissingTracesInfo(const ZgyOptions* options,
                                       const segysdk::library_loader::PluginConfig& pluginConfig)
{
    if (!segysdk::utilities::Environment::instance().calculateStatistics() || !options->isOsdu()) {
        return;
    }

    const auto pluginPath_ = pluginConfig.getCalculateStatisticsPluginPath().string();
    if (pluginPath_.empty()) {
        return;
    }
    if (!std::filesystem::exists(pluginPath_)) {
        return;
    }
    auto libraryLoader = std::make_unique<segysdk::library_loader::DynamicLibraryLoader>(pluginPath_);
    segysdk::statistics_generator::StatisticsGeneratorPlugin statisticsPlugin(std::move(libraryLoader));

    const bool forceOverwrite = segysdk::utilities::Environment::instance().overwriteStatistics() > 0;
    const bool verbosity = segysdk::utilities::Environment::instance().slbPluginsVerbosity();

    const auto osduOptions = static_cast<const ZgyOptionsOsdu*>(options);

    const auto fileCollectionSegyId = osduOptions->getFileCollectionId();
    const auto osduSeismicTraceDataId = osduOptions->getSeismicTraceDataId();

    if (!checkIfStatisticsShouldBeGenerated(osduSeismicTraceDataId, forceOverwrite)) {
        return;
    }

    const auto osduWorkProductId = "";
    segysdk::statistics_generator::StatisticsInputParams inputParams(fileCollectionSegyId, osduSeismicTraceDataId, osduWorkProductId, forceOverwrite, verbosity);

    const auto status = statisticsPlugin.GenerateForSegy(&inputParams);
    if (status == 0) {
        SEGYLOG_REPORT_ERROR("Generation of Statistics and Histogram completed successfully.");
    }
    else {
        SEGYLOG_REPORT_ERROR("** Generation of Statistics and Histogram failed! **\nReturn error code: " + std::to_string(status));
    }
    return;
}

static void
doConversion(const ZgyOptions* options, const segysdk::library_loader::PluginConfig& pluginConfig)
{
    const auto segy_file_names(options->sourceFiles());
    const auto brickSize = ZgyOptions::brickSize();

    const auto segy_config_json_string = options->segyConfig();

    SEGYLOG_DEBUG("Read " + std::to_string(segy_config_json_string.length()) + " chars of SEG-Y config JSON");

    const segysdk::CacheSizeOption cacheSize(
        segysdk::utilities::Environment::instance().sdReadCachePageSize(),
        segysdk::utilities::Environment::instance().sdReadCacheMaxPages(),
        segysdk::utilities::Environment::instance().sdReadCacheBlocksPerPage(),
        segysdk::utilities::Environment::instance().sdReadCachePageSize());
    SEGYLOG_DEBUG("Cache buffers created.");

    SEGYLOG_GSM_INFO("segy-to-zgy: create session for " + segy_file_names[0]);

    const auto segy_session = segysdk::api::SegySessionFactory(segy_file_names, segy_config_json_string).CreateSegySession(options->sdCredentials(), cacheSize);
    SEGYLOG_DEBUG("SEG-Y session initiated.");

    if (segysdk::utilities::Environment::instance().overwriteIndex()) {
        segy_session->generateIndexFile(false, true);
        SEGYLOG_DEBUG("Index file generated (overwrite flag == true)");
    }

    // Index will be loaded or generated (overwrite flag == false) in the SegyInputParameters constructor.
    const SegyInputParameters p(*segy_session);
    SEGYLOG_DEBUG("Parsed parameters from input");

    auto segyIndex = segy_session->getIndex();

    updateSeismicTraceData(segyIndex, options);

    generateStatisticsAndMissingTracesInfo(options, pluginConfig);

    createPolygonOutlines(segyIndex, options, pluginConfig);

    const OpenZGY::ZgyWriterArgs zgyWriterArgs = createZgyWriterArgs(options, p);

    copySeismicData(brickSize, segy_session.get(), p, zgyWriterArgs);

    // Currently the below is essentially a no-op
    updateSeismicMetadata(options);

    SEGYLOG_DEBUG("Finalizing records");
    options->finalize();
    SEGYLOG_DEBUG("Success");
}

void
beforeExitCleanUp()
{
    // Report and gracefully stop the Logger in case of exit call
    auto& log = segysdk::logger::Logger::getInstance();
    log.stopLogWriterDispatcher();

    segysdk::fileio::GenericFileManager::reset();
}

int
main(int argc, const char** argv)
{
    int errorCode = 0;
    try {
        // First initialize the log system to get help output ...
        const segysdk::library_loader::PluginConfig pluginConfig(segysdk::utilities::Environment::instance().segysdkConfigurationFile());
        segysdk::logger::LoggerSettings settings{};
        settings.setCorrelationId(segysdk::utilities::Environment::instance().correlationId());
        settings.setLogWriterPluginPath(pluginConfig);
        segysdk::logger::initializeLogger(settings);

        if (argc > 2) {
            // Give log system time to get initialized ...
            using namespace std::chrono_literals;
            std::this_thread::sleep_for(500ms);

            printBuildInfo();

            const std::filesystem::path cwd = std::filesystem::current_path();
            SEGYLOG_DEBUG("Current working directory: " + cwd.string());

            SEGYLOG_DEBUG("Init");
            segysdk::utilities::Environment::instance().dump();
            SEGYLOG_ARGS(argc, argv);
        }

        const auto options = ZgyOptions::parse(argc, argv, segysdk::utilities::OptionsFileType::SEGY);
        if (options == nullptr) {
            return 0;
        }

        SEGYLOG_DEBUG("Parsed options");
        const std::string fileCollectionId = options->isOsdu() ? std::dynamic_pointer_cast<ZgyOptionsOsdu>(options)->getFileCollectionId() : "";
        settings.setFileCollectionId(fileCollectionId);

        logConfig(pluginConfig);
        doConversion(options.get(), pluginConfig);
    }
    catch (const std::exception& err) {
        std::cerr << err.what() << std::endl;
        SEGYLOG_REPORT_ERROR(err.what());
        errorCode = 1;
    }

    beforeExitCleanUp();

    return errorCode;
}
