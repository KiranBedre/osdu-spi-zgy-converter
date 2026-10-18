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

#include "ZgyOptionsOsdu.h"
#include "ZgyMetaData.h"

#include "../FileIO/FileIOException.h"
#include "../FileIO/GenericFileManager.h"
#include "../Logger/Logger.h"
#include "../OsduMetaDataUtilities/OsduMetafileApi.h"
#include "../segysdk_details/SegyException.h"
#include "../tools/SegyToZgy/library/OpenZGYFileProperties.h"
#include "../Utilities/Authorization.h"
#include "../Utilities/BinGrid.h"
#include "../Utilities/Environment.h"
#include "../Utilities/GuidGenerator.h"
#include "../Utilities/JsonBinGrid.h"
#include "../Utilities/JsonUtilities.h"
#include "../Utilities/osdu/OsduFileCollectionSegy100.h"
#include "../Utilities/osdu/OsduObject.h"
#include "../Utilities/osdu/OsduSeismicTraceDataFactory.h"
#include "../Utilities/osdu/OsduWorkProduct100.h"
#include "../Utilities/osdu/WellKnownTypes.h"
#include "../Utilities/SessionUtilities.h"
#include "../Utilities/StorageSvcClient.h"
#include "../Utilities/StringUtils.h"
#include "../Utilities/TokenSourceFactory.h"

#include <fstream>
#include <iostream>
#include <openzgy/api.h>

static auto
findSeismicTraceDataWpc(const segysdk::utilities::osdu::OsduWorkProduct100& workProduct, const segysdk::utilities::StorageSvcClient& storageSvcClient)
{
    for (const auto& componentId : workProduct.componentIds()) {
        SEGYLOG_DEBUG("Fetching work product component [" + componentId + "]");
        const auto json = storageSvcClient.getRecordAsJson(componentId);
        const auto objectKind = segysdk::utilities::osdu::OsduObject(json).kind();
        SEGYLOG_DEBUG("Work product component [" + componentId + "] kind: [" + objectKind.kind() + "]");
        if (objectKind.shortTypeName() == segysdk::utilities::osdu::WellKnownTypes::SeismicTraceData) {
            return segysdk::utilities::osdu::OsduSeismicTraceDataFactory::getOsduSeismicTraceData(json);
        }
    }
    throw std::runtime_error("Work product does not contain a SeismicTraceData component.");
}

static auto
getSeismicTraceData(const Json::Value recordJson, const segysdk::utilities::StorageSvcClient& storageSvcClient)
{
    const auto objectKind = segysdk::utilities::osdu::OsduObject(recordJson).kind();

    if (objectKind.shortTypeName() == segysdk::utilities::osdu::WellKnownTypes::SeismicTraceData) {
        return segysdk::utilities::osdu::OsduSeismicTraceDataFactory::getOsduSeismicTraceData(recordJson);
    }
    else if (objectKind.shortTypeName() == segysdk::utilities::osdu::WellKnownTypes::WorkProduct) {
        const segysdk::utilities::osdu::OsduWorkProduct100 workProductJson(recordJson);
        return findSeismicTraceDataWpc(workProductJson, storageSvcClient);
    }
    throw segysdk::SegyInvalidArgumentException("Argument should be either WorkProduct or SeismicTraceData Record Id");
}

OsduVersionStruct::OsduVersionStruct(int myMajor, int myMinor, int myIncremental)
  : major(myMajor), minor(myMinor), incremental(myIncremental)
{

}

ZgyOptionsOsdu::Args::Args(int argc, const char** argv, const segysdk::utilities::StorageSvcClient& storageSvcClient)
{
    if (argc != 4) {
        help(argv[0]);
        exit(1);
    }

    fileCollectionId_ = argv[2];
    const std::string recordId = argv[3];
    SEGYLOG_DEBUG("Fetching Record [" + recordId + "]");
    const auto json = segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(recordId);
    const auto seismicTraceData = getSeismicTraceData(json, storageSvcClient);
    seismicTraceDataId_ = seismicTraceData.id();
    SEGYLOG_DEBUG("Seismic trace data ID: [" + seismicTraceDataId_ + "]");
    binGridId_ = seismicTraceData.binGridId();
    SEGYLOG_DEBUG("Using bin grid ID: [" + binGridId_ + "] from seismic trace data");
}

std::string
ZgyOptionsOsdu::Args::fileCollectionId() const
{
    return fileCollectionId_;
}

std::string
ZgyOptionsOsdu::Args::seismicTraceDataId() const
{
    return seismicTraceDataId_;
}

std::string
ZgyOptionsOsdu::Args::binGridId() const
{
    return binGridId_;
}

void
ZgyOptionsOsdu::Args::help(const std::string& programName)
{
    std::cerr << "Usage: " << programName << " --osdu <FileCollection.SEGY Id> <SeismicTraceData Id>" << std::endl;
}

ZgyOptionsOsdu::ZgyOptionsOsdu(int argc, const char** argv, const segysdk::utilities::OptionsFileType oft)
  : ZgyOptions(oft),
    storageSvcClient_(segysdk::utilities::Authorization::storageSvcTokenProvider()),
    args_(argc, argv, storageSvcClient_),
    runId_(segysdk::utilities::GuidGenerator::newGuid()),
    fileCollection_(segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(args_.fileCollectionId())),
    binGridContent_(segysdk::utilities::JsonUtilities::getJsonStringFromPathOrOsduId(args_.binGridId()))
{
    SEGYLOG_DEBUG("Run Id: " + runId_);
}

std::vector<std::string>
ZgyOptionsOsdu::sourceFiles() const
{
    return fileCollection_.segyFileNames();
}

std::string
ZgyOptionsOsdu::segyConfig() const
{
    return fileCollection_.content();
}

segysdk::utilities::BinGrid
ZgyOptionsOsdu::binGrid() const
{
    return segysdk::utilities::JsonBinGrid::loadBinGridFromContent(binGridContent_);
}

std::string
ZgyOptionsOsdu::targetFile() const
{
    return segysdk::utilities::Environment::instance().outputPathOverrideOrDefault(fileCollection_.desiredZgyFileName(runId_));
}

std::string
ZgyOptionsOsdu::getFileCollectionId() const
{
    return args_.fileCollectionId();
}

std::string
ZgyOptionsOsdu::getSeismicTraceDataId() const
{
    return args_.seismicTraceDataId();
}

void
ZgyOptionsOsdu::updateSeismicTraceData(const std::shared_ptr<segysdk::segyindex::SegyIndex>& index) const
{
    const auto binGridContent = segysdk::utilities::JsonUtilities::getJsonStringFromPathOrOsduId(args_.binGridId());
    auto grid = std::make_shared<segysdk::utilities::BinGrid>(segysdk::utilities::JsonBinGrid::loadBinGridFromContent(binGridContent));
    const auto stdId = args_.seismicTraceDataId();
    auto std = segysdk::utilities::osdu::OsduSeismicTraceDataFactory::getOsduSeismicTraceData(segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(args_.seismicTraceDataId()));

    std.checkAndUpdateMissingFields(index, grid);
    if (isLocalFile(stdId)) {
        storeLocalRecord(stdId, std.content());
    }
    else {
        storageSvcClient_.storeRecord(std.content());
    }
}

void
ZgyOptionsOsdu::updateSeismicTraceDataLiveOutline(const Json::Value& coordinates, bool isMultiple) const
{
    auto std = segysdk::utilities::osdu::OsduSeismicTraceDataFactory::getOsduSeismicTraceData(segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(args_.seismicTraceDataId()));
    if (not std.getLiveTraceOutlinePolygons().empty() and not segysdk::utilities::Environment::instance().overwritePolygonOutline()) {
        return;
    }
    std.addLiveTraceOutline(coordinates, isMultiple);
    if (isLocalFile(args_.seismicTraceDataId())) {
        storeLocalRecord(args_.seismicTraceDataId(), std.content());
    }
    else {
        storageSvcClient_.storeRecord(std.content());
    }
}

OsduVersionStruct
ZgyOptionsOsdu::getOsduSeismicTraceDataVersion(const std::shared_ptr<segysdk::metadata::osdu::OsduSeismicTraceDataStruct> seismicTraceDataStruct) const
{
    segysdk::metadata::osdu::OsduMetadataHelper helper(seismicTraceDataStruct->getPartition());
    if (!helper.isValidOSDUReferenceIdString(seismicTraceDataStruct->commonMetadata.kind)) {
        return {1, 0, 0};
    }

    const std::vector<std::string> parts = segysdk::metadata::osdu::OsduMetadataHelper::parseReferenceAttribute(seismicTraceDataStruct->commonMetadata.kind);
    const auto kindVersion = parts[3];

    const std::vector<std::string> versionParts = segysdk::utilities::StringUtils::split(kindVersion, ".");
    if (versionParts.size() >= 3) {
        const int major = std::stoi(versionParts[0].c_str());
        const int minor = std::stoi(versionParts[1].c_str());
        const int incremental = std::stoi(versionParts[2]);

        return {major, minor, incremental};
    }

    return {1, 0, 0};
}

std::string
getLocalMetafileName(const std::string& inputFile,  std::string metafileId)
{
    if (!ZgyOptions::isLocalFile(inputFile)) {
        return inputFile;
    }
    
    const std::string segyType = "FileCollection.SEGY";
    const std::string zgyType = "FileCollection.slb.OpenZGY";
    if (ZgyOptions::isLocalFile(metafileId) and metafileId.find(segyType) != std::string::npos) {
        segysdk::utilities::StringUtils::ReplaceAll(metafileId, segyType, zgyType);
        return metafileId;
    }

    auto fs = std::filesystem::path(inputFile);
    auto myExt = fs.extension();
    auto myLen = fs.filename().string().size() - myExt.string().size();
    auto myStub = fs.filename().string().substr(0, myLen) + zgyType;
    return myStub;
}    
    
void
ZgyOptionsOsdu::finalize() const
{
    const OpenZGYFileProperties fp(targetFile(), *this);
    const auto osduPartition = segysdk::utilities::Environment::instance().osduDataPartitionId();

    segysdk::metadata::osdu::OsduFileCollectionSegy fileCollectionSegy(segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(args_.fileCollectionId()), osduPartition);
    const auto fcsStructPtr = std::reinterpret_pointer_cast<segysdk::metadata::osdu::OsduFileCollectionSegyStruct>(fileCollectionSegy.getMetaDataStruct());
    if (fcsStructPtr == nullptr) {
        SEGYLOG_REPORT_ERROR("Finalize: Unable to get FileCollectionSegy metadata file ... ");
        return;
    }
    if (fcsStructPtr->commonMetadataStruct.kind.find(segysdk::utilities::osdu::WellKnownTypes::FileCollectionSEGY) == std::string::npos) {
        SEGYLOG_REPORT_ERROR("Finalize: FileCollectionSegy metadata file has illegal kind... ");
        return;
    }

    segysdk::metadata::osdu::OsduFileCollectionOpenZgy zgyFile(*fcsStructPtr);
    const auto zgyStructPtr = std::reinterpret_pointer_cast<segysdk::metadata::osdu::OsduFileCollectionOpenZgyStruct>(zgyFile.getMetaDataStruct());

    zgyStructPtr->addFileProperties(fp.fileName(), fp.fileSize());

    segysdk::metadata::osdu::OsduSeismicTraceData seismicTraceData(segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(args_.seismicTraceDataId()), osduPartition);
    const auto seismicTraceDataStruct = std::reinterpret_pointer_cast<segysdk::metadata::osdu::OsduSeismicTraceDataStruct>(seismicTraceData.getMetaDataStruct());
    if (seismicTraceDataStruct == nullptr) {
        SEGYLOG_REPORT_ERROR("Finalize: Could not get SeismicTraceData struct");
        return;
    }
    if (seismicTraceDataStruct->commonMetadata.kind.find(segysdk::utilities::osdu::WellKnownTypes::SeismicTraceData) == std::string::npos) {
        SEGYLOG_REPORT_ERROR("Finalize: SeismicTraceData metadata file has illegal kind... ");
        return;
    }

    const auto stdKindVersion = getOsduSeismicTraceDataVersion(seismicTraceDataStruct);

    if (stdKindVersion.major > 1 or (stdKindVersion.major == 1 and stdKindVersion.minor >= 4)) {
        const segysdk::utilities::ZgyOutputParams zgyOutputParameters = segysdk::utilities::ZgyOutputParams::getZgyOutputParameters();
        const auto zgySampleSize = zgyOutputParameters.getZgySampleSize();
        segysdk::metadata::osdu::OsduAbstractSeismicPrecisionStruct precisionStruct(osduPartition);
        precisionStruct.setPrecision(zgySampleSize);
        zgyStructPtr->addPrecision(precisionStruct);

        if (!segysdk::utilities::Environment::instance().zgyOutputParameters().empty() && zgyOutputParameters.UseCompression()) {
            segysdk::metadata::osdu::OsduAbstractCompressionInfoStruct compressionInfo(osduPartition);
            compressionInfo.setCompressionInfo(zgyOutputParameters, fp.fileSize());
            zgyStructPtr->addCompressionInfo(compressionInfo);
        }

        zgyStructPtr->commonMetadataStruct.kind = segysdk::metadata::osdu::OsduFileCollectionOpenZgyStruct::getOsduKind("1.1.0");
    }
    zgyFile = segysdk::metadata::osdu::OsduFileCollectionOpenZgy(*zgyStructPtr);
    const auto zgyStyled_string = zgyFile.getAsFormattedString();
    SEGYLOG_DEBUG("FileCollection.OpenZGY:\n" + zgyStyled_string);

    std::string zgyFileRef = getLocalMetafileName(fp.fileName(), args_.fileCollectionId());

    const auto zgyId = isLocalFile(zgyFileRef)
                           ? storeLocalRecord(zgyFileRef, zgyStyled_string)
                           : storageSvcClient_.storeRecord(zgyStyled_string);

    seismicTraceDataStruct->addArtefact(zgyId, zgyStructPtr->commonMetadataStruct.kind);
    seismicTraceData = segysdk::metadata::osdu::OsduSeismicTraceData(*seismicTraceDataStruct);
    const auto stdStyled_string = seismicTraceData.getAsFormattedString();
    if (isLocalFile(args_.seismicTraceDataId())) {
        storeLocalRecord(args_.seismicTraceDataId(), stdStyled_string);
    }
    else {
        (void)storageSvcClient_.storeRecord(stdStyled_string);
    }
    setMetadataForGeneratedFile(stdStyled_string);
}

void
ZgyOptionsOsdu::setMetadataForGeneratedFile(const std::string& seismicTraceDataMetadata) const
{
    const auto manager = createSDManager();
    if (ZgyMetaData::ZgyMetaData::setOnGeneratedZgyFile(manager.get(), seismicTraceDataMetadata, targetFile())) {
        SEGYLOG_GSM_INFO("Successfully set ZGY metadata");
    }
    else {
        SEGYLOG_GSM_INFO("setting of ZGY file metadata failed");
    }
}
