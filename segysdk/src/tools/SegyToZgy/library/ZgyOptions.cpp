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

#include "ZgyOptions.h"

#include "ZgyOptionsDelfiCommandLine.h"
#include "ZgyOptionsOsdu.h"

#include "../FileIO/FileIOException.h"
#include "../FileIO/GenericFileManager.h"

#include "../Logger/Logger.h"
#include "../Utilities/Adapters.h"
#include "../Utilities/Environment.h"
#include "../Utilities/JsonUtilities.h"
#include "../Utilities/SessionUtilities.h"
#include "../Utilities/Settings.h"
#include "../Utilities/StringUtils.h"
#include "../Utilities/TokenSourceFactory.h"

#include <openzgy/api.h>
#include <openzgy/iocontext.h>

#include <SDAPI/SDGenericDataset.h>

#include <cstring>
#include <filesystem>
#include <memory>


ZgyOptions::~ZgyOptions()
{
    /* EMPTY */
}

std::shared_ptr<ZgyOptions>
ZgyOptions::parse(int argc, const char** argv, const segysdk::utilities::OptionsFileType oft)
{
    if (argc == 2 and strcmp(argv[1], "--createParameterTemplate") == 0) {
        dumpParameterTemplateJson();
        return nullptr;
    }

    if ((argc < 3) or ((argc >= 2) and (strcmp(argv[1], "--help") == 0))) {
        showUsageInfo(argc, argv);
        return nullptr;
    }

    auto const isOsdu = (argc >= 2) and (strcmp(argv[1], "--osdu") == 0);
    auto options = isOsdu ? std::static_pointer_cast<ZgyOptions>(std::make_shared<ZgyOptionsOsdu>(argc, argv, oft))
                          : std::static_pointer_cast<ZgyOptions>(std::make_shared<ZgyOptionsDelfiCommandLine>(argc, argv, oft));
    options->setIsOsdu(isOsdu);
    return options;
}

void
ZgyOptions::showUsageInfo(int argc, const char** argv)
{
    std::cout << "Usage:" << std::endl;
    std::cout << argv[0] << " --createParameterTemplate" << std::endl;
    std::cout << "\tDumps a template output format specification file." << std::endl
              << std::endl;

    std::cout << "OSDU:" << std::endl;
    std::cout << argv[0] << " --osdu  <FileCollection.SEGY ID> <SeismicTraceData ID>" << std::endl;
    std::cout << "e.g. " << std::endl;
    std::cout << argv[0] << "  --osdu opendes:dataset--FileCollection.SEGY:5c457ba177cff2680a65e059c89e9dfe opendes:opendes:work-product-component--SeismicTraceData:98821c3dd323219c49f03d31f238dd75" << std::endl;
    std::cout << std::endl;

    std::cout << "Environment variable \'SEGYTOZGY_ZGY_Output_Parameters\':  Path to json file describing ZGY output format." << std::endl;
    std::cout << "Dump output format specification Json file and modify to suit. " << std::endl;
    std::cout << "Save the Json file and set the environment variable SEGYTOZGY_ZGY_Output_Parameters to hold the path to this file." << std::endl;
    std::cout << std::endl;
    std::cout << "DELFI:" << std::endl;
    std::cout << argv[0] << " <input SEG-Y file> <Configuration> <Bin Grid>" << std::endl;
    std::cout << "e.g. " << std::endl;
    if (argc >= 1) {
        std::cout << argv[0] << "  D:\\Temp\\File1.sgy  D:\\Temp\\config.json  D:\\Temp\\BinGrid.json" << std::endl;
        std::cout << std::endl;
    }

    std::cout << argv[0] << " --help" << std::endl;
    std::cout << "\tDisplays this message" << std::endl;
    std::cout << std::endl;
}

void
ZgyOptions::dumpParameterTemplateJson()
{
    std::cout << "Parameter file for output control:" << std::endl
              << std::endl;
    dumpJsonSampleToConsoleOutput();
}

void
ZgyOptions::dumpJsonSampleToConsoleOutput()
{
    std::cout << "{" << std::endl;
    std::cout << "\t\"_comment\": [" << std::endl;
    std::cout << "\t\t\"ZGY Quality will only be used when USE_ZFP_COMPRESSION: false. \"," << std::endl;
    std::cout << "\t\t\"Legal values are 8, 16 or 32. \"," << std::endl;
    std::cout << "\t\t\"For compressed file this will always be 32 - Bit.\"" << std::endl;
    std::cout << "\t]," << std::endl;
    std::cout << "\t\"ZGY_QUALITY\": 16," << std::endl;

    std::cout << "\t\"USE_ZFP_COMPRESSION\":  true," << std::endl;
    std::cout << "\t\"Compression\": {" << std::endl;
    std::cout << "\t\t\"_comment1\": [" << std::endl;
    std::cout << "\t\t\t\"ZFP_SNR specifies desired [approximate] signal-to-noise ratio for lossy compression of high resolution data, valid range [10..70]. \"," << std::endl;
    std::cout << "\t\t\t\"If invalid or not set then loss-less compression will be used (silently).\"" << std::endl;
    std::cout << "\t\t]," << std::endl;
    std::cout << "\t\t\"ZFP_SNR\": " << std::to_string(60.0) << std::endl;
    std::cout << "\t}," << std::endl;

    std::cout << "\t\"USE_LOD_COMPRESSION\": true," << std::endl;
    std::cout << "\t\"LOD_Compression\": {" << std::endl;
    std::cout << "\t\t\"_comment2\": [" << std::endl;
    std::cout << "\t\t\t\"ZFP_LOD_SNR specifies desired [approximate] signal-to-noise ratio for lossy compression of low-resolution data, valid range [10..70].\", " << std::endl;
    std::cout << "\t\t\t\"If invalid or not set then loss-less compression will be used (silently).\"" << std::endl;
    std::cout << "\t\t]," << std::endl;
    std::cout << "\t\t\"ZFP_LOD_SNR\": " << std::to_string(30.0) << std::endl;
    std::cout << "\t}," << std::endl;

    std::cout << "\t\"USE_CUSTOM_BRICK_SIZE\":  true," << std::endl;
    std::cout << "\t\"CUSTOM_BRICK_SIZE\": {" << std::endl;
    std::cout << "\t\t\"_comment3\": [" << std::endl;
    std::cout << "\t\t\t\"USE_CUSTOM_BRICK_SIZE brick size can be specified by specifying required values for CUSTOM_BRICK_SIZE. \", " << std::endl;
    std::cout << "\t\t\t\"Required parameters are INLINES_PER_BRICK , CROSSLINES_PER_BRICK and TRACE_SAMPLES_PER_BRICK.\", " << std::endl;
    std::cout << "\t\t\t\"Using non-standard values might have a significant effect on your target application's performance. \", " << std::endl;
    std::cout << "\t\t\t\"Delete this section for default values.\"" << std::endl;
    std::cout << "\t\t]," << std::endl;

    std::cout << "\t\t\"INLINES_PER_BRICK\": " << std::to_string(64) << "," << std::endl;
    std::cout << "\t\t\"CROSSLINES_PER_BRICK\": " << std::to_string(64) << "," << std::endl;
    std::cout << "\t\t\"TRACE_SAMPLES_PER_BRICK\": " << std::to_string(64) << std::endl;
    std::cout << "\t}" << std::endl;

    std::cout << "}" << std::endl
              << std::endl;
}

OpenZGY::ZgyWriterArgs
ZgyOptions::zgyWriterArgs() const
{
    const auto outputParams = segysdk::utilities::Environment::instance().zgyOutputParameters();

    const bool isCloudBased = segysdk::utilities::isSDDataset(outputParams);

    if (isCloudBased && !outputParams.empty()) {
        SEGYLOG_DEBUG("zgyWriterArgs: Get output parameters from json : [" + outputParams + "]");
    }
    else if (!isCloudBased && !outputParams.empty() && std::filesystem::exists(outputParams)) {
        SEGYLOG_DEBUG("zgyWriterArgs: Get output parameters from json : [" + outputParams + "]");
    }
    else {
        SEGYLOG_DEBUG("zgyWriterArgs: Taking output parameters from environment variables");
    }

    const segysdk::utilities::ZgyOutputParams zgyOutputParams = (!outputParams.empty() && isCloudBased) || std::filesystem::exists(outputParams)
                                                                    ? getOutputParamsFromJson(outputParams)
                                                                    : segysdk::utilities::ZgyOutputParams();

    OpenZGY::ZgyWriterArgs zgyWriterArgs = OpenZGY::ZgyWriterArgs()
                                               .filename(targetFile())
                                               .datatype(zgyOutputParams.getZgySampleSize())
                                               .bricksize(static_cast<int64_t>(zgyOutputParams.getBrickSize().sizeI()),
                                                          static_cast<int64_t>(zgyOutputParams.getBrickSize().sizeJ()),
                                                          static_cast<int64_t>(zgyOutputParams.getBrickSize().sizeK()))
                                               .iocontext(createOpenZGYIoContext()); // we'll leak an IO context object per conversion
    if (zgyOutputParams.UseCompression()) {
        SEGYLOG_DEBUG("Set ZFP compression with SnR " + std::to_string(zgyOutputParams.getZgyCompressionSnR()));
        zgyWriterArgs.zfp_compressor(zgyOutputParams.getZgyCompressionSnR());
    }
    if (zgyOutputParams.UseLodCompression()) {
        SEGYLOG_DEBUG("Set ZFP LoD compression with SnR " + std::to_string(zgyOutputParams.getLodCompressionSnR()));
        zgyWriterArgs.zfp_lodcompressor(zgyOutputParams.getLodCompressionSnR());
    }
    return zgyWriterArgs;
}

ZgyOptions::ZgyOptions(const segysdk::utilities::OptionsFileType oft)
  : segysdk::utilities::Options(oft)
{
    /* EMPTY */
}

ZgyOptions::ZgyOptions()
  : segysdk::utilities::Options()
{
    /* EMPTY */
}

const OpenZGY::IOContext*
ZgyOptions::createOpenZGYIoContext() const
{
    if (!segysdk::utilities::isSDDataset(targetFile()))
        return nullptr;

    auto ret = std::make_unique<OpenZGY::SeismicStoreIOContext>();
    ret->sdurl(sdCredentials().url());
    ret->sdapikey(sdCredentials().apiKey());
    const auto tokenSource = sdCredentials().tokenSource().get();
    ret->sdtokencb([tokenSource]() {
        return tokenSource->token();
    },
                   "?"); // TODO what does type mean here?

    const auto sourcePath{sourceFiles()[0]};
    if (segysdk::utilities::isSDDataset(sourcePath)) {
        const auto sdMgr = createSDManager();
        seismicdrive::SDGenericDataset sdDataset(sdMgr.get(), sourcePath);
        sdDataset.open(seismicdrive::SDDatasetDisposition::READ_ONLY);
        const auto sourceLegalTag = sdDataset.getLegalTag();
        SEGYLOG_DEBUG("Legal tag of source [" + sourcePath + "] : [" + sourceLegalTag + "]");
        ret->legaltag(sourceLegalTag);
    }

    // Specify IOCONTEXT for OpenZGY. Beware, using the default settings will allocate 2GB of memory!!
    // Note: OpenZGY cache size determined by segsize (MB) * segsplit (Write processes).
    // The default setting for the environment variables used below requires 64MB * 16 = 1GB RAM
    const size_t iocontext_buffer_size = segysdk::utilities::Environment::instance().openZgyBufferSize();
    const size_t iocontext_write_threads = segysdk::utilities::Environment::instance().openZgyWriteThreads();
    const size_t iocontext_cpu_threads = segysdk::utilities::Environment::instance().openZgyCPUThreads();
    const size_t iocontext_io_threads = segysdk::utilities::Environment::instance().openZgyIOThreads();

    ret->segsize(iocontext_buffer_size);
    ret->segsplit(iocontext_write_threads);
    ret->cputhreads(iocontext_cpu_threads);
    ret->iothreads(iocontext_io_threads);

    return ret.release();
}

bool
ZgyOptions::isLocalFile(const std::string& fileName)
{
    const auto fileNameParts = segysdk::utilities::StringUtils::split(fileName, ":");
    bool isLocal = fileNameParts.size() == 2 and (fileName.find(':') == 1 or fileName.find("\\\\") == 0);

    // sd:// file or OSDU id string...
    // bool inCloud = not isLocal and fileNameParts.size() >= 3 or fileName.find("sd://") == 0;
    return isLocal;
}

segysdk::fileio::FileReaderOptions
ZgyOptions::getFileReaderOptions(const std::string& fileName, const std::string& legalTag = "") 
{

    if (isLocalFile(fileName)) {
        return segysdk::fileio::FileReaderOptions::localOnly();
    }

    const segysdk::SDCredentials credentials(
        segysdk::utilities::Environment::instance().sdUrl(),
        segysdk::utilities::Environment::instance().sdApiKey(),
        segysdk::utilities::TokenSourceFactory::getTokenSource(),
        segysdk::utilities::Settings::instance().getHttpHeaders());

    if (legalTag.empty()) {
        throw segysdk::fileio::FileIOException("legal tag cannot be empty!");
    }
    return {credentials, segysdk::utilities::SessionUtilities::getSDCacheSize(), legalTag};
}

segysdk::fileio::FileReference
ZgyOptions::getFileReference(const std::string& fileName)
{
    auto fro = getFileReaderOptions(fileName);
    return segysdk::fileio::FileReference(fileName, fro);
}

std::string
ZgyOptions::storeLocalRecord(const std::string& fileName, const std::string& content)
{
    segysdk::fileio::FileReference metaFileRef = getFileReference(fileName);

    // sdapi does not support Truncate mode ...
    if (std::filesystem::exists(fileName)) {
        std::filesystem::remove(fileName);
    }

    auto writer = segysdk::fileio::GenericFileManager::createWriter(metaFileRef, segysdk::fileio::WriteMode::Create);
    if (writer == nullptr) {
        return "";
    }

    // Write meta data section.
    writer->write(reinterpret_cast<const uint8_t*>(content.c_str()), content.size());
    writer->close();

    return fileName;
}
