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

#include "SegyIndexStorage.h"

#include "../FileIO/FileIOException.h"
#include "../FileIO/GenericFileManager.h"
#include "../FileIO/SequentialFileReader.h"
#include "../Logger/Logger.h"
#include "../SegyFormat/Internal/FileHeaderDefinitionStringConstsTraceHeader.h"
#include "../SegyIO/HeaderDefinitionProvider.h"
#include "../SegyIO/Primitives.h"
#include "../SegyIO/SegyIOException.h"
#include "../SegyIO/TraceSequentialScanner.h"
#include "../SegyIndex/CollationOffset.h"
#include "../SegyIndex/GeometryCalculationData.h"
#include "../SegyIndex/JSONIndexStringConsts.h"
#include "../SegyIndex/SegyIndexGenerator.h"
#include "../SegyIndex/SegyIndexImpl.h"
#include "../SegyIndex/Utilities/MissingTraceRangeFactory.h"
#include "../SegyIndex/Utilities/TraceRangeFactory.h"
#include "../Utilities/JsonUtilities.h"
#include "../Utilities/SafeTime.h"
#include "../Utilities/StringUtils.h"

#include <json/json.h>

#include <array>
#include <cmath>
#include <ctime>
#include <functional>
#include <iomanip>
#include <limits>
#include <memory>

namespace segysdk::segyindex
{
namespace index_storage
{

SegyIndexStorage::SegyIndexStorage(fileio::FileReaderOptions fileReaderOptions,
                                   SegyIndexDataPtr& indexData)
  : fileReaderOptions_{std::move(fileReaderOptions)},
    indexData_{indexData}
{
    /* EMPTY */
}

bool
SegyIndexStorage::hasIndexFile() const
{
    return hasIndexFile(request::Params{});
}

bool
SegyIndexStorage::hasIndexFile(const request::Params& params) const
{
    std::string const indexMetaDataFile = getIndexMetaDataFileName(indexData_->indexBaseFileName_);

    fileio::FileReference indexMetafileRef(indexMetaDataFile, fileReaderOptions_);
    bool const indexMetaDataFileExists = fileio::GenericFileManager::exists(indexMetafileRef);

    std::string const indexFile = getIndexBinaryFileName(indexData_->indexBaseFileName_);

    fileio::FileReference indexBinaryfileRef(indexFile, fileReaderOptions_);
    bool const indexFileExists = fileio::GenericFileManager::exists(indexBinaryfileRef);

    if (!indexMetaDataFileExists) {
        return false; // definitely correct
    }

    if (indexFileExists) {
        return true; // definitely correct
    }

    // Read metadata to determine indexCount
    Json::Value const root = readIndexMetaDataFile(params);

    if (bool const isRegular = getIsRegularFromJson(root); isRegular) {
        return true;
    }

    // OK, here we do expect the binary index to exist...

    if (getMajorIndexVersionFromMetaData(root) < index_config::IndexMajorVersion_2021_01_) {
        return true;
    }

    return indexFileExists;
}

void
SegyIndexStorage::Load()
{
    Load(request::Params{});
}

void
SegyIndexStorage::Load(const request::Params& params)
{
    std::vector<std::string> inputFileList;
    for (multi_file::MultiFileStruct const& sg : indexData_->segyFilesIndexed_) {
        inputFileList.push_back(sg.SegyFileName);
    }

    if (inputFileList.empty()) {
        const std::string msg = "SegyIndexStorage::Load: Input SEG-Y file list cannot be empty";
        throw SegyInvalidArgumentException(msg);
    }

    const Json::Value root = readIndexMetaDataFile(params);

    getGeometry3DFromJson(root);
    getFileLayoutFromJson(root);
    getIndexMetadataFromJson(root);
    setLineExtentsFromJson(root);

    if (indexData_->is2D()) {
        loadBinaryIndexData2D(params);
    }
    else {
        loadBinaryIndexData3D(params);
    }

    // Must do this after index has been restored
    verifyInputSegyFilesListIsValid(inputFileList, "SegyIndexStorage::Load");
}

bool
SegyIndexStorage::Save()
{
    bool result = true;

    deleteIndex();

    result = SaveIndexMetaData() and result;
    result = SaveBinaryIndexData() and result;

    return result;
}

void
SegyIndexStorage::SaveAsJson(const std::string& outputFileName) const
{
    Json::Value root = getIndexMetadataAsJson();

    root[JSONIndexStringConsts::SegyIndexDataTag] = getIndexAsJson();

    constexpr bool allowOverwrite = true;

    fileio::FileReference outputFileRef(outputFileName, fileReaderOptions_);
    fileio::GenericFileManager::writeTextFile(outputFileRef, root.toStyledString(), allowOverwrite);
}

void
SegyIndexStorage::deleteIndex()
{
    const std::string indexMetaFilename = getIndexMetaDataFileName(indexData_->indexBaseFileName_);
    fileio::FileReference metaFileRef(indexMetaFilename, fileReaderOptions_);
    fileio::GenericFileManager::remove(metaFileRef);

    const std::string indexFileName = getIndexBinaryFileName(indexData_->indexBaseFileName_);
    fileio::FileReference indexFileRef(indexFileName, fileReaderOptions_);
    fileio::GenericFileManager::remove(indexFileRef);

    const std::string missingIndexFileName = getMissingIndexBinaryFileName(indexData_->indexBaseFileName_);
    fileio::FileReference missingIndexFileRef(missingIndexFileName, fileReaderOptions_);
    fileio::GenericFileManager::remove(missingIndexFileRef);
}

void
SegyIndexStorage::ResetIndexData()
{
    setInitialDefaults();
    setEmptyGeometry3D();
    setEmptyFileLayout();
}

multi_file::FileNameStatus
SegyIndexStorage::validateIndexedFiles(FileList const& inputFileList,
                                       bool logDebugOutput) const
{
    using multi_file::FileNameStatus;

    bool fileNamesOK = filenamesAreUnique(inputFileList);
    bool fileFullPathOK = fullFilenamesAreUnique(inputFileList);

    if (!fileNamesOK && !fileFullPathOK) {
        // input filenames have been duplicated
        return FileNameStatus::NamesNotUnique;
    }

    if (inputFileList.size() != indexData_->segyFilesIndexed_.size()) {
        return FileNameStatus::FileCountDiffers;
    }

    for (auto& sf : indexData_->segyFilesIndexed_) {
        const auto status = IndexedFileFoundInInputContainer(sf.SegyFileName, inputFileList, logDebugOutput);

        fileFullPathOK = fileFullPathOK && status == FileNameStatus::Match;
        if (!fileFullPathOK) {
            fileNamesOK = fileNamesOK && status == FileNameStatus::FileNameMatch;
        }

        if (status == FileNameStatus::NoMatch) {
            break;
        }
    }

    return fileFullPathOK
               ? FileNameStatus::Match
           : fileNamesOK
               ? FileNameStatus::FileNameMatch
               : FileNameStatus::NoMatch;
}

void
SegyIndexStorage::setGeometry3D(Geometry3D const& geometry)
{
    indexData_->geometry3D_ = geometry;
}

void
SegyIndexStorage::reconnectSEGYFilesAfterMove(FileList const& inputFileList)
{
    const std::string callerInfo = "SegyIndexStorage::reconnectSEGYFilesAfterMove";

    if (indexData_->segyFilesIndexed_.empty()) {
        const std::string msg = callerInfo + ": Current list of indexed files cannot be empty";
        throw SegyInvalidArgumentException(msg);
    }

    if (!hasIndexFile()) {
        const std::string msg = callerInfo + ": Invalid SEG-Y file - size < 3840 bytes";
        throw SegyInvalidArgumentException(msg);
    }

    constexpr bool performExactNameCheck = false;
    verifyInputSegyFilesListIsValid(inputFileList, callerInfo, performExactNameCheck);

    // OK, if we are here, our input file list should be good to use.

    auto success = true;
    for (auto& ix : indexData_->segyFilesIndexed_) {
        std::string newFileName = getInputSEGYFileNameFromList(inputFileList, ix.SegyFileName);
        if (newFileName != "InvalidSEGYFileName") {
            ix.SegyFileName = newFileName;
        }
        else {
            success = false;
        }
    }
    if (!success) {
        const std::string msg = callerInfo + ": Didn't manage to reconnect specified SEG-Y files";
        throw segyio::SegyRuntimeException(msg);
    }

    Save();
}

Geometry3D
SegyIndexStorage::GetGeometry3DFromJson(const Json::Value& geometryNode)
{

    auto members = geometryNode.getMemberNames();

    Geometry3D newGeometry;
    if (geometryNode.isMember(JSONIndexStringConsts::JVectorExtent)) {
        newGeometry.JVectorExtent = CopyExtentIntFromJson(geometryNode[JSONIndexStringConsts::JVectorExtent]);
    }

    if (geometryNode.isMember(JSONIndexStringConsts::IVectorExtent)) {
        newGeometry.IVectorExtent = CopyExtentIntFromJson(geometryNode[JSONIndexStringConsts::IVectorExtent]);
    }

    if (geometryNode.isMember(JSONIndexStringConsts::sampleExtent)) {
        newGeometry.sampleExtent = CopyExtentDoubleFromJson(geometryNode[JSONIndexStringConsts::sampleExtent]);
    }

    Json::Value dataGridNode = geometryNode[JSONIndexStringConsts::dataGrid];

    if (dataGridNode.isMember(JSONIndexStringConsts::corner11)) {
        newGeometry.dataGrid.gridCell_11 = CopyCornerFromJson(dataGridNode[JSONIndexStringConsts::corner11]);
    }

    if (dataGridNode.isMember(JSONIndexStringConsts::corner1N)) {
        newGeometry.dataGrid.gridCell_1N = CopyCornerFromJson(dataGridNode[JSONIndexStringConsts::corner1N]);
    }

    if (dataGridNode.isMember(JSONIndexStringConsts::cornerN1)) {
        newGeometry.dataGrid.gridCell_N1 = CopyCornerFromJson(dataGridNode[JSONIndexStringConsts::cornerN1]);
    }

    if (dataGridNode.isMember(JSONIndexStringConsts::cornerNN)) {
        newGeometry.dataGrid.gridCell_NN = CopyCornerFromJson(dataGridNode[JSONIndexStringConsts::cornerNN]);
    }

    if (dataGridNode.isMember(JSONIndexStringConsts::IVectorRotationAngle)) {
        newGeometry.dataGrid.IVectorRotationAngle = dataGridNode[JSONIndexStringConsts::IVectorRotationAngle].asDouble();
    }
    else if (dataGridNode.isMember(JSONIndexStringConsts::InlineRotationAngle)) {
        newGeometry.dataGrid.IVectorRotationAngle = dataGridNode[JSONIndexStringConsts::InlineRotationAngle].asDouble();
    }

    if (dataGridNode.isMember(JSONIndexStringConsts::JVectorUnitDistance)) {
        newGeometry.dataGrid.JVectorUnitDistance = dataGridNode[JSONIndexStringConsts::JVectorUnitDistance].asDouble();
    }
    else if (dataGridNode.isMember(JSONIndexStringConsts::InlineUnitDistance)) {
        newGeometry.dataGrid.JVectorUnitDistance = dataGridNode[JSONIndexStringConsts::InlineUnitDistance].asDouble();
    }

    if (dataGridNode.isMember(JSONIndexStringConsts::JVectorRotationAngle)) {
        newGeometry.dataGrid.JVectorRotationAngle = dataGridNode[JSONIndexStringConsts::JVectorRotationAngle].asDouble();
    }
    else if (dataGridNode.isMember(JSONIndexStringConsts::CrosslineRotationAngle)) {
        newGeometry.dataGrid.JVectorRotationAngle = dataGridNode[JSONIndexStringConsts::CrosslineRotationAngle].asDouble();
    }

    if (dataGridNode.isMember(JSONIndexStringConsts::IVectorUnitDistance)) {
        newGeometry.dataGrid.IVectorUnitDistance = dataGridNode[JSONIndexStringConsts::IVectorUnitDistance].asDouble();
    }
    else if (dataGridNode.isMember(JSONIndexStringConsts::CrosslineUnitDistance)) {
        newGeometry.dataGrid.IVectorUnitDistance = dataGridNode[JSONIndexStringConsts::CrosslineUnitDistance].asDouble();
    }

    newGeometry.isInlineOrdered = geometryNode[JSONIndexStringConsts::IsInlineOrdered].asBool();
    newGeometry.isRegular = geometryNode[JSONIndexStringConsts::IsRegular].asBool();

    return newGeometry;
}

Json::Value
SegyIndexStorage::getGeometry3DAsJson(const Geometry3D& g)
{
    const auto& JVectorExtent = g.JVectorExtent;
    const auto& IVectorExtent = g.IVectorExtent;
    const auto& sampleExtent = g.sampleExtent;
    const auto& dataGrid = g.dataGrid;
    const auto& isRegular = g.isRegular;
    const auto& isInlineOrdered = g.isInlineOrdered;

    Json::Value geometryJson;

    geometryJson[JSONIndexStringConsts::JVectorExtent][JSONIndexStringConsts::first] = JVectorExtent.first();
    geometryJson[JSONIndexStringConsts::JVectorExtent][JSONIndexStringConsts::last] = JVectorExtent.last();
    geometryJson[JSONIndexStringConsts::JVectorExtent][JSONIndexStringConsts::step] = JVectorExtent.step();

    geometryJson[JSONIndexStringConsts::IVectorExtent][JSONIndexStringConsts::first] = IVectorExtent.first();
    geometryJson[JSONIndexStringConsts::IVectorExtent][JSONIndexStringConsts::last] = IVectorExtent.last();
    geometryJson[JSONIndexStringConsts::IVectorExtent][JSONIndexStringConsts::step] = IVectorExtent.step();

    geometryJson[JSONIndexStringConsts::sampleExtent][JSONIndexStringConsts::first] = sampleExtent.first();
    geometryJson[JSONIndexStringConsts::sampleExtent][JSONIndexStringConsts::last] = sampleExtent.last();
    geometryJson[JSONIndexStringConsts::sampleExtent][JSONIndexStringConsts::step] = sampleExtent.step();
    geometryJson[JSONIndexStringConsts::sampleExtent][JSONIndexStringConsts::domain] = sampleExtent.domain() == segyio::VerticalDomain::Time ? "Time" : "Depth";
    geometryJson[JSONIndexStringConsts::sampleExtent][JSONIndexStringConsts::zUoM] = getUoMAsString(sampleExtent.zUoM());

    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::corner11][JSONIndexStringConsts::inlineNo] = dataGrid.gridCell_11.i;
    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::corner11][JSONIndexStringConsts::crosslineNo] = dataGrid.gridCell_11.j;
    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::corner11][JSONIndexStringConsts::coordX] = dataGrid.gridCell_11.location.X;
    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::corner11][JSONIndexStringConsts::coordY] = dataGrid.gridCell_11.location.Y;

    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::corner1N][JSONIndexStringConsts::inlineNo] = dataGrid.gridCell_1N.i;
    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::corner1N][JSONIndexStringConsts::crosslineNo] = dataGrid.gridCell_1N.j;
    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::corner1N][JSONIndexStringConsts::coordX] = dataGrid.gridCell_1N.location.X;
    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::corner1N][JSONIndexStringConsts::coordY] = dataGrid.gridCell_1N.location.Y;

    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::cornerN1][JSONIndexStringConsts::inlineNo] = dataGrid.gridCell_N1.i;
    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::cornerN1][JSONIndexStringConsts::crosslineNo] = dataGrid.gridCell_N1.j;
    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::cornerN1][JSONIndexStringConsts::coordX] = dataGrid.gridCell_N1.location.X;
    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::cornerN1][JSONIndexStringConsts::coordY] = dataGrid.gridCell_N1.location.Y;

    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::cornerNN][JSONIndexStringConsts::inlineNo] = dataGrid.gridCell_NN.i;
    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::cornerNN][JSONIndexStringConsts::crosslineNo] = dataGrid.gridCell_NN.j;
    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::cornerNN][JSONIndexStringConsts::coordX] = dataGrid.gridCell_NN.location.X;
    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::cornerNN][JSONIndexStringConsts::coordY] = dataGrid.gridCell_NN.location.Y;

    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::JVectorUnitDistance] = dataGrid.JVectorUnitDistance;
    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::IVectorUnitDistance] = dataGrid.IVectorUnitDistance;

    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::IVectorRotationAngle] = dataGrid.IVectorRotationAngle;
    geometryJson[JSONIndexStringConsts::dataGrid][JSONIndexStringConsts::JVectorRotationAngle] = dataGrid.JVectorRotationAngle;

    geometryJson[JSONIndexStringConsts::IsRegular] = isRegular;
    geometryJson[JSONIndexStringConsts::IsInlineOrdered] = isInlineOrdered;

    return geometryJson;
}

index_2d::SeismicLineExtents
SegyIndexStorage::getSeismicLineExtentsFromJson(const Json::Value& lineJson)
{
    HorizontalExtent cmpExtent{0, 0, 0};
    VerticalExtent sampleExtent{0, 0, 0};

    if (lineJson.isMember(JSONIndexStringConsts::CMPExtent)) {
        cmpExtent = CopyExtentIntFromJson(lineJson[JSONIndexStringConsts::CMPExtent]);
    }

    if (lineJson.isMember(JSONIndexStringConsts::sampleExtent)) {
        sampleExtent = CopyExtentDoubleFromJson(lineJson[JSONIndexStringConsts::sampleExtent]);
    }
    return {cmpExtent, sampleExtent};
}

Json::Value
SegyIndexStorage::getSeismicLineExtentsAsJson(const index_2d::SeismicLineExtents& lineExtents)
{
    const auto& cmpExtent = lineExtents.getCMPExtent();
    const auto& sampleExtent = lineExtents.getSampleExtent();
    Json::Value result;

    result[JSONIndexStringConsts::CMPExtent][JSONIndexStringConsts::first] = cmpExtent.first();
    result[JSONIndexStringConsts::CMPExtent][JSONIndexStringConsts::last] = cmpExtent.last();
    result[JSONIndexStringConsts::CMPExtent][JSONIndexStringConsts::step] = cmpExtent.step();

    result[JSONIndexStringConsts::sampleExtent][JSONIndexStringConsts::first] = sampleExtent.first();
    result[JSONIndexStringConsts::sampleExtent][JSONIndexStringConsts::last] = sampleExtent.last();
    result[JSONIndexStringConsts::sampleExtent][JSONIndexStringConsts::step] = sampleExtent.step();
    result[JSONIndexStringConsts::sampleExtent][JSONIndexStringConsts::domain] = sampleExtent.domain() == segyio::VerticalDomain::Time ? "Time" : "Depth";
    result[JSONIndexStringConsts::sampleExtent][JSONIndexStringConsts::zUoM] = getUoMAsString(sampleExtent.zUoM());

    return result;
}

FileLayoutSpecification
SegyIndexStorage::getFileLayoutStructFromJson(Json::Value const& fileLayoutNode)
{
    auto members = fileLayoutNode.getMemberNames();

    FileLayoutSpecification result;
    if (fileLayoutNode.isMember(JSONIndexStringConsts::JVectorExtent)) {
        result.JExtent = CopyExtentIntFromJson(fileLayoutNode[JSONIndexStringConsts::JVectorExtent]);
    }

    if (fileLayoutNode.isMember(JSONIndexStringConsts::IVectorExtent)) {
        result.IExtent = CopyExtentIntFromJson(fileLayoutNode[JSONIndexStringConsts::IVectorExtent]);
    }

    if (fileLayoutNode.isMember(JSONIndexStringConsts::sampleExtent)) {
        result.sampleExtent = CopyExtentDoubleFromJson(fileLayoutNode[JSONIndexStringConsts::sampleExtent]);
    }

    if (fileLayoutNode.isMember(JSONIndexStringConsts::IsInlineOrdered)) {
        result.isInlineOrdered = fileLayoutNode[JSONIndexStringConsts::IsInlineOrdered].asBool();
    }

    if (fileLayoutNode.isMember(JSONIndexStringConsts::IsRegular)) {
        result.isRegular = fileLayoutNode[JSONIndexStringConsts::IsRegular].asBool();
    }

    return result;
}

Json::Value
SegyIndexStorage::getFileLayoutAsJson(const FileLayoutSpecification& f)
{
    const auto& JVectorExtent = f.JExtent;
    const auto& IVectorExtent = f.IExtent;
    const auto& sampleExtent = f.sampleExtent;
    const auto& isRegular = f.isRegular;
    const auto& isInlineOrdered = f.isInlineOrdered;

    Json::Value fileLayoutJson;

    fileLayoutJson[JSONIndexStringConsts::JVectorExtent][JSONIndexStringConsts::first] = JVectorExtent.first();
    fileLayoutJson[JSONIndexStringConsts::JVectorExtent][JSONIndexStringConsts::last] = JVectorExtent.last();
    fileLayoutJson[JSONIndexStringConsts::JVectorExtent][JSONIndexStringConsts::step] = JVectorExtent.step();

    fileLayoutJson[JSONIndexStringConsts::IVectorExtent][JSONIndexStringConsts::first] = IVectorExtent.first();
    fileLayoutJson[JSONIndexStringConsts::IVectorExtent][JSONIndexStringConsts::last] = IVectorExtent.last();
    fileLayoutJson[JSONIndexStringConsts::IVectorExtent][JSONIndexStringConsts::step] = IVectorExtent.step();

    fileLayoutJson[JSONIndexStringConsts::sampleExtent][JSONIndexStringConsts::first] = sampleExtent.first();
    fileLayoutJson[JSONIndexStringConsts::sampleExtent][JSONIndexStringConsts::last] = sampleExtent.last();
    fileLayoutJson[JSONIndexStringConsts::sampleExtent][JSONIndexStringConsts::step] = sampleExtent.step();
    fileLayoutJson[JSONIndexStringConsts::sampleExtent][JSONIndexStringConsts::domain] = sampleExtent.domain() == segyio::VerticalDomain::Time ? "Time" : "Depth";
    fileLayoutJson[JSONIndexStringConsts::sampleExtent][JSONIndexStringConsts::zUoM] = getUoMAsString(sampleExtent.zUoM());

    fileLayoutJson[JSONIndexStringConsts::IsRegular] = isRegular;
    fileLayoutJson[JSONIndexStringConsts::IsInlineOrdered] = isInlineOrdered;

    return fileLayoutJson;
}

uint16_t
SegyIndexStorage::getUShortFromBuffer(const unsigned char* buffer, size_t offset)
{
    const uint16_t value1 = buffer[offset + 1];
    const uint16_t value0 = buffer[offset + 0];

    const uint16_t myValue = static_cast<uint16_t>(value0 | (value1 << 8));

    return myValue;
}

int32_t
SegyIndexStorage::getIntFromBuffer(const unsigned char* buffer, size_t offset)
{
    const int32_t value3 = buffer[offset + 3];
    const int32_t value2 = buffer[offset + 2];
    const int32_t value1 = buffer[offset + 1];
    const int32_t value0 = buffer[offset];

    const int32_t myValue = value0
                            | (value1 << 8)
                            | (value2 << 16)
                            | (value3 << 24);

    return myValue;
}

size_t
SegyIndexStorage::getSizeFromBuffer(const unsigned char* buffer, size_t offset)
{
    // This value has been stored as an uint64_t so use fixed 8 bytes word length

    const size_t value7 = buffer[offset + 7];
    const size_t value6 = buffer[offset + 6];
    const size_t value5 = buffer[offset + 5];
    const size_t value4 = buffer[offset + 4];
    const size_t value3 = buffer[offset + 3];
    const size_t value2 = buffer[offset + 2];
    const size_t value1 = buffer[offset + 1];
    const size_t value0 = buffer[offset];

    const size_t myValue = value0
                           | (value1 << 8)
                           | (value2 << 16)
                           | (value3 << 24)
                           | (value4 << 32)
                           | (value5 << 40)
                           | (value6 << 48)
                           | (value7 << 56);

    return myValue;
}

std::string
SegyIndexStorage::getIndexBinaryFileName(const std::string& segyFile)
{
    std::string const indexFile = segyFile + index_config::indexFileExtension_;
    return indexFile;
}

std::string
SegyIndexStorage::getIndexMetaDataFileName(const std::string& segyFile)
{
    std::string const indexFile = segyFile + index_config::indexMetaFileExtension_;
    return indexFile;
}

std::string
SegyIndexStorage::getMissingIndexBinaryFileName(const std::string& segyFile)
{
    std::string const indexFile = segyFile + index_config::indexMissingIndexExtension_;
    return indexFile;
}

Json::Value
SegyIndexStorage::readIndexMetaDataFile(const request::Params& params) const
{
    const std::string indexMetadataFileName = getIndexMetaDataFileName(indexData_->indexBaseFileName_);
    const auto managerParams = fileio::FileManagerParams{
        fileio::FileAccessParams{params}.withFileName(indexMetadataFileName)}
                                   .withReaderOptions(fileReaderOptions_);
    const size_t metadataSize = getMetadataSizeFromIndexFile(managerParams);

    std::string metadata;
    if (metadataSize == 0) {
        // New style metadata file
        auto [success, text] = fileio::GenericFileManager::readTextFile(managerParams);
        if (!success) {
            throw segyio::SegyRuntimeException("Unable to read saved index metadata file : " + indexMetadataFileName);
        }
        metadata = text;
    }
    else if (metadataSize < oldStyleMetadataSize_) {
        // Old style metafile
        std::vector<char> buffer(metadataSize + 1, '\0');
        auto reader = fileio::GenericFileManager::createReader(managerParams);
        metadata = stringFromBuffer(reader->read(fileio::FileReadParams{params}.withOffset(indexAsciiHeaderSize_).withSize(metadataSize)));
    }

    Json::Value root = utilities::JsonUtilities::getJsonFromString(metadata);
    if (root.isNull()) {
        throw segyio::SegyRuntimeException("Unable to parse index metadata - probable data corruption.");
    }

    return root;
}

bool
SegyIndexStorage::getIsRegularFromJson(Json::Value node)
{
    if (!node.isMember(JSONIndexStringConsts::Geometry3DTag)) {
        throw SegyInvalidArgumentException("getIsRegularFromJson: json object does not contain Geometry3D tag.");
    }

    Json::Value geometryNode = node[JSONIndexStringConsts::Geometry3DTag];

    if (!geometryNode.isMember(JSONIndexStringConsts::IsRegular)) {
        throw SegyInvalidArgumentException("getIsRegularFromJson: json object does not contain IsRegular parameter.");
    }

    const bool isRegular = geometryNode[JSONIndexStringConsts::IsRegular].asBool();
    return isRegular;
}

size_t
SegyIndexStorage::getMajorIndexVersionFromMetaData(Json::Value node)
{
    if (!node.isMember(JSONIndexStringConsts::metaTag)) {
        throw SegyInvalidArgumentException("getMajorIndexVersionFromMetaData: json object does not contain Metadata tag.");
    }

    Json::Value metaNode = node[JSONIndexStringConsts::metaTag];
    if (!metaNode.isMember(JSONIndexStringConsts::versionTag)) {
        throw SegyInvalidArgumentException("getMajorIndexVersionFromMetaData: Metadata json object does not contain Version tag.");
    }

    Json::Value versionNode = metaNode[JSONIndexStringConsts::versionTag];
    if (!versionNode.isMember(JSONIndexStringConsts::Major)) {
        throw SegyInvalidArgumentException("getMajorIndexVersionFromMetaData: Version json object does not contain Major property.");
    }

    const size_t major = versionNode[JSONIndexStringConsts::Major].asLargestUInt();

    return major;
}

size_t
SegyIndexStorage::getMetadataSizeFromIndexFile(const fileio::FileManagerParams& params)
{
    auto reader = fileio::GenericFileManager::createReader(params);
    const auto metadata = stringFromBuffer(reader->read(fileio::FileReadParams{request::Params{params}}.withOffset(0).withSize(indexAsciiHeaderSize_)));
    const std::size_t found = metadata.find(JSONIndexStringConsts::IndexAsciiHeaderSize);
    if (found == std::string::npos) {
        return 0;
    }

    try {
        Json::Value root = utilities::JsonUtilities::getJsonFromString(metadata);
        if (!root.isNull() && root.isMember(JSONIndexStringConsts::IndexAsciiHeaderSize)) {
            return static_cast<size_t>(root[JSONIndexStringConsts::IndexAsciiHeaderSize].asInt());
        }
    }
    catch (...) {
        SEGYLOG_REPORT_ERROR("segyIndexStorage; Error reading IndexFile size");
    }
    return oldStyleMetadataSize_;
}

void
SegyIndexStorage::getGeometry3DFromJson(Json::Value json)
{
    // Sanity check input json to ensure we work on correct node ...
    if (json.isMember(JSONIndexStringConsts::Geometry3DTag)) {
        indexData_->geometry3D_ = GetGeometry3DFromJson(json[JSONIndexStringConsts::Geometry3DTag]);
    }
}

void
SegyIndexStorage::getFileLayoutFromJson(Json::Value const& json)
{
    // Sanity check input json to ensure we work on correct node ...
    if (json.isMember(JSONIndexStringConsts::FileLayoutSpecificationTag)) {
        indexData_->fileLayout_ = getFileLayoutStructFromJson(json[JSONIndexStringConsts::FileLayoutSpecificationTag]);
    }
}

void
SegyIndexStorage::setLineExtentsFromJson(Json::Value json)
{
    // Sanity check input json to ensure we work on correct node ...
    if (json.isMember(JSONIndexStringConsts::LineExtentTag)) {
        indexData_->indexing2dInfo_.setSeismicLineExtents(getSeismicLineExtentsFromJson(json[JSONIndexStringConsts::LineExtentTag]));
    }
}

void
SegyIndexStorage::getIndexMetadataFromJson(Json::Value json)
{
    Json::Value VersionNode = json[JSONIndexStringConsts::metaTag][JSONIndexStringConsts::versionTag];
    indexData_->indexVersionMajor_ = VersionNode[JSONIndexStringConsts::Major].asInt();
    indexData_->indexVersionMinor_ = VersionNode[JSONIndexStringConsts::Minor].asInt();

    Json::Value infoNode = json[JSONIndexStringConsts::segyInfoTag];
    indexData_->sampleSize_ = static_cast<size_t>(infoNode[JSONIndexStringConsts::SampleSize].asInt());

    indexData_->hasConstantCoordinateScalar_ = infoNode[JSONIndexStringConsts::ConstantCoordinateScalar].asBool();
    indexData_->hasConstantTraceLength_ = infoNode[JSONIndexStringConsts::ConstantTraceLength].asBool();
    indexData_->hasConstantTraceWeightingFactor_ = infoNode[JSONIndexStringConsts::ConstantTraceWeighting].asBool();

    indexData_->constantCoordinateScalar_ = (infoNode.isMember(JSONIndexStringConsts::ConstantCoordinateScalarValue)) ? infoNode[JSONIndexStringConsts::ConstantCoordinateScalarValue].asInt() : 0;
    indexData_->constantTraceLength_ = (infoNode.isMember(JSONIndexStringConsts::ConstantTraceLengthValue)) ? static_cast<size_t>(infoNode[JSONIndexStringConsts::ConstantTraceLengthValue].asInt()) : 0U;
    indexData_->constantTraceWeightingFactor_ = (infoNode.isMember(JSONIndexStringConsts::ConstantTraceWeightingValue)) ? static_cast<unsigned short>(infoNode[JSONIndexStringConsts::ConstantTraceWeightingValue].asInt()) : 1U;

    indexData_->offsetFirtstSEGYTrace_ = static_cast<size_t>(infoNode[JSONIndexStringConsts::OffsetFirtstSEGYTrace].asInt64());
    indexData_->traceHeaderSize_ = static_cast<size_t>(infoNode[JSONIndexStringConsts::TraceHeaderSize].asInt());

    indexData_->traceLength_ = indexData_->hasConstantTraceLength_ ? indexData_->constantTraceLength_ : 0U;

    if (infoNode.isMember(JSONIndexStringConsts::AmplitudeMin) and infoNode.isMember(JSONIndexStringConsts::AmplitudeMax)) {
        indexData_->amplitudeMinimum_ = static_cast<float>(infoNode[JSONIndexStringConsts::AmplitudeMin].asDouble());
        indexData_->amplitudeMaximum_ = static_cast<float>(infoNode[JSONIndexStringConsts::AmplitudeMax].asDouble());
    }

    bool is2D = infoNode.isMember(JSONIndexStringConsts::SegyDimension) ? infoNode[JSONIndexStringConsts::SegyDimension].asString() == JSONIndexStringConsts::dimension2D : false;
    indexData_->set2DFlag(is2D);

    getSegyFileNamesFromJson(infoNode);
    getFileMapFromJson(infoNode);
}

bool
SegyIndexStorage::isRegular() const
{
    return indexData_->isRegular();
}

void
SegyIndexStorage::migrateJsonConfigFrom_2022_01()
{
    //
    // Migrating the JSON config to the latest format
    //

    initFileLayout_2022_06();

    //
    // Set the latest version as the current
    //
    indexData_->indexVersionMajor_ = index_config::IndexMajorVersionCurrent_;
    indexData_->indexVersionMinor_ = index_config::IndexMinorVersionCurrent_;

    //
    // Convert any of the *previous* version index to the latest format
    //
    if (not Save()) {
        SEGYLOG_REPORT_ERROR("segyIndexStorage; Failed to convert '2022_01' version binary index to the latest format.");
    }
}

void
SegyIndexStorage::populateIndexItemsFromFile_2024_01(const request::Params& params)
{
    // This will read '2024_01' version binary index file for index values

    readRangeMapFromFile_2022_01(params);
}

void
SegyIndexStorage::populateMissingTracesIndexItemsFromFile_2024_01(const request::Params& params)
{
    // This will read '2024_01' version binary index file for index values

    readMissingRangeMapFromFile_2024_01(params);
}

void
SegyIndexStorage::populateIndexItemsFromFile_2022_06(const request::Params& params)
{
    // This will read '2022_06' version binary index file for index values

    readRangeMapFromFile_2022_01(params);
}

void
SegyIndexStorage::populateIndexItemsFromFile_2022_01(const request::Params& params)
{
    // This will read '2022_01' version binary index file for index values

    initFileLayout_2022_06();
    readRangeMapFromFile_2022_01(params);

    //
    // Set the latest version as the current
    //
    indexData_->indexVersionMajor_ = index_config::IndexMajorVersionCurrent_;
    indexData_->indexVersionMinor_ = index_config::IndexMinorVersionCurrent_;

    //
    // Convert the '2022_01' version binary index to the latest format
    //
    if (not Save()) {
        SEGYLOG_REPORT_ERROR("segyIndexStorage; Failed to convert '2022_01' version binary index to the latest format.");
    }
}

void
SegyIndexStorage::populateIndexItemsFromFile_2021_01(const request::Params& params)
{
    // This will read '2021_01' version binary index file for index values
    // with the following conversion to the latest format

    if (not indexData_->traceIndexMap_.isEmpty()) {
        indexData_->traceIndexMap_.clearRanges();
        indexData_->traceIndexStorage_.clearStorage();
    }

    LegacyIndexMap legacyIndex;

    //
    // Import binary index file data to the legacy map
    //
    {
        const std::string indexFilename = getIndexBinaryFileName(indexData_->indexBaseFileName_);
        const auto managerParams = fileio::FileManagerParams{
            fileio::FileAccessParams{params}.withFileName(indexFilename)}
                                       .withReaderOptions(fileReaderOptions_);
        std::vector<decltype(fileio::GenericFileManager::createReader(managerParams))> readers(index_config::binaryIndexReadersCount_, nullptr);
        std::generate(
            readers.begin(), readers.end(), [&managerParams]() -> auto {
                return fileio::GenericFileManager::createReader(managerParams);
            });

        std::shared_ptr<fileio::BufferedFileReaderBase> reader = std::make_unique<fileio::SequentialFileReader>(
            fileio::PipelineFileReaderParams{params}.withFileReaders(readers));
        reader->setBufferSize(readersLegacyIndexBufferSize_);

        size_t indexRecordsRead = 0;
        const size_t indexCount = fileio::GenericFileManager::getSize({indexFilename, fileReaderOptions_}) / indexRecordSize_;

        for (/* EMPTY */; indexRecordsRead < indexCount; ++indexRecordsRead) {
            size_t offset = indexRecordsRead * indexRecordSize_;

            auto const* const indexRecordBuff = reader->readAligned(offset, indexRecordSize_, /*alignment*/ 0U);
            if (nullptr == indexRecordBuff) {
                throw segyio::SegyFileIOException("SegyIndexStorage::populateIndexItemsFromFile_2021_01: cannot read binary index file.");
            }

            size_t bufferOffset = 0U;
            const uint16_t fileNo = getUShortFromBuffer(indexRecordBuff, bufferOffset);
            bufferOffset += sizeof(uint16_t);
            const int inlineNo = getIntFromBuffer(indexRecordBuff, bufferOffset);
            bufferOffset += sizeof(int32_t);
            const int crosslineNo = getIntFromBuffer(indexRecordBuff, bufferOffset);
            bufferOffset += sizeof(int32_t);
            const size_t myOffset = getSizeFromBuffer(indexRecordBuff, bufferOffset);

            legacyIndex.try_emplace(getKeyFromGrid(inlineNo, crosslineNo), IndexField(fileNo, inlineNo, crosslineNo, myOffset));
        }
    }

    //
    // Import the legacy map to the latest format
    //
    initFileLayout_2022_06();
    importLegacyIndex_2022_01(legacyIndex);

    //
    // Convert the '2021_01' version binary index to the latest format
    //
    if (not Save()) {
        SEGYLOG_REPORT_ERROR("segyIndexStorage; Failed to convert '2021_01' version binary index to the latest format.");
    }
}

void
SegyIndexStorage::populateIndexItemsFromFile_2020_01(const request::Params& params)
{
    // This will read '2020_01' version binary index file for index values
    // with the following conversion to the latest format

    if (not indexData_->traceIndexMap_.isEmpty()) {
        indexData_->traceIndexMap_.clearRanges();
        indexData_->traceIndexStorage_.clearStorage();
    }

    LegacyIndexMap legacyIndex;

    //
    // Import binary index file data to the legacy map
    //
    {
        const std::string indexFilename = getIndexMetaDataFileName(indexData_->indexBaseFileName_);
        const auto managerParams = fileio::FileManagerParams{
            fileio::FileAccessParams{params}.withFileName(indexFilename)}
                                       .withReaderOptions(fileReaderOptions_);
        std::vector<decltype(fileio::GenericFileManager::createReader(managerParams))> readers(index_config::binaryIndexReadersCount_, nullptr);
        std::generate(
            readers.begin(), readers.end(), [&managerParams]() -> auto {
                return fileio::GenericFileManager::createReader(managerParams);
            });

        std::shared_ptr<fileio::BufferedFileReaderBase> reader = std::make_unique<fileio::SequentialFileReader>(
            fileio::PipelineFileReaderParams{params}.withFileReaders(readers));
        reader->setBufferSize(readersLegacyIndexBufferSize_);

        size_t indexRecordsRead = 0;
        const size_t readOffset = indexAsciiHeaderSize_ + getMetadataSizeFromIndexFile(managerParams);

        const size_t indexCount = (fileio::GenericFileManager::getSize({indexFilename, fileReaderOptions_}) - readOffset) / indexRecordSize_;

        for (/* EMPTY */; indexRecordsRead < indexCount; ++indexRecordsRead) {
            size_t offset = indexRecordsRead * indexRecordSize_ + readOffset;

            auto const* const indexRecordBuff = reader->readAligned(offset, indexRecordSize_, /*alignment*/ 0U);
            if (nullptr == indexRecordBuff) {
                throw segyio::SegyFileIOException("SegyIndexStorage::populateIndexItemsFromFile_2021_01: cannot read binary index file.");
            }

            size_t bufferOffset = 0U;
            const uint16_t fileNo = getUShortFromBuffer(indexRecordBuff, bufferOffset);
            bufferOffset += sizeof(uint16_t);
            const int inlineNo = getIntFromBuffer(indexRecordBuff, bufferOffset);
            bufferOffset += sizeof(int32_t);
            const int crosslineNo = getIntFromBuffer(indexRecordBuff, bufferOffset);
            bufferOffset += sizeof(int32_t);
            const size_t myOffset = getSizeFromBuffer(indexRecordBuff, bufferOffset);

            legacyIndex.try_emplace(getKeyFromGrid(inlineNo, crosslineNo), IndexField(fileNo, inlineNo, crosslineNo, myOffset));
        }
    }

    //
    // Import the legacy map to the latest format
    //
    initFileLayout_2022_06();
    importLegacyIndex_2022_01(legacyIndex);

    //
    // Convert the '2020_01' version binary index to the latest format
    //
    if (not Save()) {
        SEGYLOG_REPORT_ERROR("segyIndexStorage; Failed to convert '2020_01' version binary index to the latest format.");
    }
}

void
SegyIndexStorage::loadBinaryIndexData3D(const request::Params& params)
{
    if (isRegular()) {
        if ((indexData_->indexVersionMajor_ == index_config::IndexMajorVersion_2022_01_
             and indexData_->indexVersionMinor_ == index_config::IndexMinorVersion_2022_01_)
            or (indexData_->indexVersionMajor_ == index_config::IndexMajorVersion_2021_01_
                and indexData_->indexVersionMinor_ == index_config::IndexMinorVersion_2021_01_)
            or (indexData_->indexVersionMajor_ == index_config::IndexMajorVersion_2020_01_
                and indexData_->indexVersionMinor_ == index_config::IndexMinorVersion_2020_01_)) {
            migrateJsonConfigFrom_2022_01();
        }
    }
    else {
        if ((indexData_->indexVersionMajor_ == index_config::IndexMajorVersion_2024_11_
             and indexData_->indexVersionMinor_ == index_config::IndexMinorVersion_2024_11_)
            or (indexData_->indexVersionMajor_ == index_config::IndexMajorVersion_2024_01_
                and indexData_->indexVersionMinor_ == index_config::IndexMinorVersion_2024_01_)) {
            populateIndexItemsFromFile_2024_01(params);
        }
        else if (indexData_->indexVersionMajor_ == index_config::IndexMajorVersion_2022_06_
                 and indexData_->indexVersionMinor_ == index_config::IndexMinorVersion_2022_06_) {
            populateIndexItemsFromFile_2022_06(params);
        }
        else if (indexData_->indexVersionMajor_ == index_config::IndexMajorVersion_2022_01_
                 and indexData_->indexVersionMinor_ == index_config::IndexMinorVersion_2022_01_) {
            populateIndexItemsFromFile_2022_01(params);
        }
        else if (indexData_->indexVersionMajor_ == index_config::IndexMajorVersion_2021_01_
                 and indexData_->indexVersionMinor_ == index_config::IndexMinorVersion_2021_01_) {
            populateIndexItemsFromFile_2021_01(params);
        }
        else if (indexData_->indexVersionMajor_ == index_config::IndexMajorVersion_2020_01_
                 and indexData_->indexVersionMinor_ == index_config::IndexMinorVersion_2020_01_) {
            populateIndexItemsFromFile_2020_01(params);
        }
        else {
            const std::string msg = "SegyIndexStorage::Load: Input SEG-Y file index format is unsupported: "
                                    "Major("
                                    + std::to_string(indexData_->indexVersionMajor_)
                                    + ") Minor("
                                    + std::to_string(indexData_->indexVersionMinor_)
                                    + "). Drop the unsupported index file and re-launch";
            throw SegyInvalidArgumentException(msg);
        }
    }
}

void
SegyIndexStorage::loadBinaryIndexData2D(const request::Params& params)
{
    const std::string indexFilename = getIndexBinaryFileName(indexData_->indexBaseFileName_);
    const auto managerParams = fileio::FileManagerParams{
        fileio::FileAccessParams{params}.withFileName(indexFilename)}
                                   .withReaderOptions(fileReaderOptions_);
    std::vector<decltype(fileio::GenericFileManager::createReader(managerParams))> readers(index_config::binaryIndexReadersCount_, nullptr);
    std::generate(
        readers.begin(), readers.end(), [&managerParams]() -> auto {
            return fileio::GenericFileManager::createReader(managerParams);
        });

    std::shared_ptr<fileio::BufferedFileReaderBase> reader = std::make_unique<fileio::SequentialFileReader>(
        fileio::PipelineFileReaderParams{params}.withFileReaders(readers));
    reader->setBufferSize(readersTraceRecordBufferSize_);

    indexData_->indexing2dInfo_.loadIndex(*reader, {index_config::IndexMajorVersionCurrent_, index_config::IndexMinorVersion_2024_01_});
    indexData_->indexing2dInfo_.setSampleSize(indexData_->sampleSize_);
    indexData_->indexing2dInfo_.setTraceHeaderSize(indexData_->traceHeaderSize_);

    indexData_->indexedTraceCount_ = indexData_->indexing2dInfo_.getIndexedTraceCount();
}

void
SegyIndexStorage::verifyInputSegyFilesListIsValid(FileList const& inputFileList,
                                                  std::string callerInfo,
                                                  bool performExactFileNameCheck) const
{
    using multi_file::FileNameStatus;

    std::string msg = std::move(callerInfo);

    if (inputFileList.empty()) {
        msg += ": Current list of indexed files cannot be empty";
        throw SegyInvalidArgumentException(msg);
    }

    const auto status = validateIndexedFiles(inputFileList);
    switch (status) {
    case FileNameStatus::Match:
        return;

    case FileNameStatus::FileNameMatch:
        if (!performExactFileNameCheck) {
            return;
        }

        msg += ": InputFileList: Input file names have different file paths to the list of indexed.";
        break;

    case FileNameStatus::FileCountDiffers:
        msg += ": Input file count does not match list of indexed SEG-Y files!";
        break;

    case FileNameStatus::NamesNotUnique:
        msg += ": Input file names have to be unique!";
        break;

    case FileNameStatus::NoMatch:
        msg += ": Input file list does not match indexed SEG-Y file names!";
        throw segyio::SegyRuntimeException(msg);

    default:
        msg += ": Input file names does not match list of indexed SEG-Y files!";
        break;
    }

    throw SegyInvalidArgumentException(msg);
}

void
SegyIndexStorage::getSegyFileNamesFromJson(Json::Value node)
{
    indexData_->segyFilesIndexed_.clear();

    auto infoNodeMembers = node.getMemberNames();

    if (node.isMember(JSONIndexStringConsts::SegyFileList)) {
        const Json::Value& fileList = node[JSONIndexStringConsts::SegyFileList];
        for (const auto& mfile : fileList) {
            if (mfile.empty()) {
                continue;
            }

            try {
                if (mfile.isMember(JSONIndexStringConsts::FileNo) && mfile.isMember(JSONIndexStringConsts::SegyFile)) {
                    multi_file::MultiFileStruct fs;
                    fs.FileNo = static_cast<uint16_t>(mfile[JSONIndexStringConsts::FileNo].asUInt());
                    fs.SegyFileName = mfile[JSONIndexStringConsts::SegyFile].asString();

                    fs.FirstTraceOffset = mfile.isMember(JSONIndexStringConsts::FileOffset) ? mfile[JSONIndexStringConsts::FileOffset].asUInt() : 0;
                    fs.PrimaryKeyRange.Min = mfile.isMember(JSONIndexStringConsts::PrimaryMin) ? mfile[JSONIndexStringConsts::PrimaryMin].asInt() : 0;
                    fs.PrimaryKeyRange.Max = mfile.isMember(JSONIndexStringConsts::PrimaryMax) ? mfile[JSONIndexStringConsts::PrimaryMax].asInt() : 0;
                    indexData_->segyFilesIndexed_.push_back(fs);
                }
            }
            catch (...) {
                // old style
                multi_file::MultiFileStruct fs(mfile.asString());
                indexData_->segyFilesIndexed_.push_back(fs);
            }
        }
    }
    else if (node.isMember(JSONIndexStringConsts::SegyFile)) {
        multi_file::MultiFileStruct mfs;
        mfs.FileNo = 0;
        mfs.SegyFileName = node[JSONIndexStringConsts::SegyFile].asString();
        mfs.FirstTraceOffset = segyio::Primitives::getTextualHeaderSize()
                               + segyio::Primitives::getBinaryHeaderSize(); // 3600;
        mfs.PrimaryKeyRange.Min = 0;
        mfs.PrimaryKeyRange.Max = 0;

        indexData_->segyFilesIndexed_.push_back(mfs);
    }
}

void
SegyIndexStorage::getFileMapFromJson(Json::Value node)
{
    indexData_->multiFileMap_.clear();
    if (node.isMember(JSONIndexStringConsts::SegyFileMap)) {
        const Json::Value& multiFileMap = node[JSONIndexStringConsts::SegyFileMap];
        for (const auto& element : multiFileMap) {
            if (element.isMember(JSONIndexStringConsts::FileNo) && element.isMember(JSONIndexStringConsts::Primary)) {
                const auto fileNo = static_cast<uint16_t>(element[JSONIndexStringConsts::FileNo].asUInt());
                int primary = element[JSONIndexStringConsts::Primary].asInt();
                indexData_->multiFileMap_[primary] = fileNo;
            }
        }
    }
}

size_t
SegyIndexStorage::getKeyFromGrid(int inlineNo, int crosslineNo)
{
    size_t value = inlineNo * 4294967296UL; // 2 ^ 32 is 4294967296
    value += crosslineNo;

    return value;
}

void
SegyIndexStorage::importLegacyIndex_2022_01(LegacyIndexMap const& legacyIndex)
{
    //
    // Init a fake collated data object
    // to simulate the index generation happening for the first time
    //
    std::shared_ptr<IndexBuilderCollatedData> collatedData = std::make_shared<IndexBuilderCollatedData>();
    collatedData->InlineIsPrimary = indexData_->fileLayout_.isInlineOrdered;
    collatedData->CrosslineNoRange.UpdateMinMax(indexData_->fileLayout_.IExtent.first());
    collatedData->CrosslineNoRange.UpdateMinMax(indexData_->fileLayout_.IExtent.last());
    collatedData->InlineNoRange.UpdateMinMax(indexData_->fileLayout_.JExtent.first());
    collatedData->InlineNoRange.UpdateMinMax(indexData_->fileLayout_.JExtent.last());
    collatedData->FirstTraceOffset = indexData_->offsetFirtstSEGYTrace_;

    //
    // Import the legacy map data to the currently supported one
    //
    trace_range_map::PrimaryIndex const prii0 = trace_range_map::TraceRangeFactory::getPrimaryExtentIndexFirst(indexData_->fileLayout_);
    trace_range_map::PrimaryIndex const priiN = trace_range_map::TraceRangeFactory::getPrimaryExtentIndexLast(indexData_->fileLayout_);
    trace_range_map::PrimaryIndex const priStep = trace_range_map::TraceRangeFactory::getPrimaryExtentStep(indexData_->fileLayout_);
    trace_range_map::SecondaryIndex const seci0 = trace_range_map::TraceRangeFactory::getSecondaryExtentIndexFirst(indexData_->fileLayout_);
    trace_range_map::SecondaryIndex const seciN = trace_range_map::TraceRangeFactory::getSecondaryExtentIndexLast(indexData_->fileLayout_);
    trace_range_map::SecondaryIndex const seciStep = trace_range_map::TraceRangeFactory::getSecondaryExtentStep(indexData_->fileLayout_);

    {
        trace_range_map::TraceRangeFactory traceRangeFactory(indexData_->fileLayout_,
                                                             collatedData,
                                                             indexData_->traceIndexStorage_);

        for (trace_range_map::PrimaryIndex prii = prii0; prii <= priiN; prii += priStep) {
            for (trace_range_map::SecondaryIndex seci = seci0; seci <= seciN; seci += seciStep) {

                size_t const key = (indexData_->fileLayout_.isInlineOrdered)
                                       ? getKeyFromGrid(static_cast<int>(prii), static_cast<int>(seci))
                                       : getKeyFromGrid(static_cast<int>(seci), static_cast<int>(prii));
                auto const itr = legacyIndex.find(key);
                if (itr == legacyIndex.end()) {
                    continue;
                }

                IndexField const& indexField = itr->second;

                if (indexData_->fileLayout_.isInlineOrdered) {
                    traceRangeFactory.addTrace(indexField.getInline(),
                                               indexField.getCrossline(),
                                               static_cast<size_t>(indexData_->fileLayout_.sampleExtent.count()),
                                               indexField.getSegyFileNumber(),
                                               indexField.getOffset());
                }
                else {
                    traceRangeFactory.addTrace(indexField.getCrossline(),
                                               indexField.getInline(),
                                               static_cast<size_t>(indexData_->fileLayout_.sampleExtent.count()),
                                               indexField.getSegyFileNumber(),
                                               indexField.getOffset());
                }
            }
        }
    }

    auto const priOrder = priStep > 0
                              ? trace_range_map::TraceIndexMapKey::Order::Asc
                              : trace_range_map::TraceIndexMapKey::Order::Desc;
    auto const seciOrder = seciStep > 0
                               ? trace_range_map::TraceIndexMapKey::Order::Asc
                               : trace_range_map::TraceIndexMapKey::Order::Desc;
    indexData_->traceIndexMap_.setOrderMode(priOrder, seciOrder);

    indexData_->traceIndexMap_.addRanges(indexData_->traceIndexStorage_);

    auto firstAddedRange = indexData_->traceIndexMap_.enumerateMissingRanges(prii0, priiN, priStep,
                                                                             seci0, seciN, seciStep,
                                                                             indexData_->traceIndexStorage_);
    indexData_->traceIndexMap_.addRanges(firstAddedRange, indexData_->traceIndexStorage_.cend());

    indexData_->indexedTraceCount_ = indexData_->traceIndexMap_.enumerateLiveTraceCount(prii0, priiN, priStep,
                                                                                        seci0, seciN, seciStep);

    indexData_->indexVersionMajor_ = index_config::IndexMajorVersionCurrent_;
    indexData_->indexVersionMinor_ = index_config::IndexMinorVersionCurrent_;
}

void
SegyIndexStorage::readRangeMapFromFile_2022_01(const request::Params& params)
{
    // This will read '2022_01' version range map as the part of binary index

    if (not indexData_->traceIndexMap_.isEmpty()) {
        indexData_->traceIndexMap_.clearRanges();
        indexData_->traceIndexStorage_.clearStorage();
    }

    const std::string indexFilename = getIndexBinaryFileName(indexData_->indexBaseFileName_);
    const auto managerParams = fileio::FileManagerParams{
        fileio::FileAccessParams{params}.withFileName(indexFilename)}
                                   .withReaderOptions(fileReaderOptions_);
    std::vector<decltype(fileio::GenericFileManager::createReader(managerParams))> readers(index_config::binaryIndexReadersCount_, nullptr);
    std::generate(
        readers.begin(), readers.end(), [&managerParams]() -> auto {
            return fileio::GenericFileManager::createReader(managerParams);
        });

    std::shared_ptr<fileio::BufferedFileReaderBase> reader = std::make_unique<fileio::SequentialFileReader>(
        fileio::PipelineFileReaderParams{params}.withFileReaders(readers));
    reader->setBufferSize(readersTraceRecordBufferSize_);

    size_t indexRecordsRead = 0;
    const size_t indexCount = fileio::GenericFileManager::getSize({indexFilename, fileReaderOptions_}) / traceRecordSize_;

    for (/* EMPTY */; indexRecordsRead < indexCount; ++indexRecordsRead) {
        size_t offset = indexRecordsRead * traceRecordSize_;

        auto const* const indexRecordBuff = reader->readAligned(offset, traceRecordSize_, /*alignment*/ 0U);
        if (nullptr == indexRecordBuff) {
            throw segyio::SegyFileIOException("SegyIndexStorage::populateIndexItemsFromFile_2022_01: cannot read binary index file.");
        }

        auto const* recordPtr = reinterpret_cast<trace_range_map::TraceRangeRecord const*>(indexRecordBuff);

        trace_range_map::TraceRange range(*recordPtr);

        indexData_->traceIndexStorage_.addRange(std::move(range));
    }

    trace_range_map::PrimaryIndex const prii0 = trace_range_map::TraceRangeFactory::getPrimaryExtentIndexFirst(indexData_->fileLayout_);
    trace_range_map::PrimaryIndex const priiN = trace_range_map::TraceRangeFactory::getPrimaryExtentIndexLast(indexData_->fileLayout_);
    trace_range_map::PrimaryIndex const priStep = trace_range_map::TraceRangeFactory::getPrimaryExtentStep(indexData_->fileLayout_);
    trace_range_map::SecondaryIndex const seci0 = trace_range_map::TraceRangeFactory::getSecondaryExtentIndexFirst(indexData_->fileLayout_);
    trace_range_map::SecondaryIndex const seciN = trace_range_map::TraceRangeFactory::getSecondaryExtentIndexLast(indexData_->fileLayout_);
    trace_range_map::SecondaryIndex const seciStep = trace_range_map::TraceRangeFactory::getSecondaryExtentStep(indexData_->fileLayout_);

    auto const priOrder = priStep > 0
                              ? trace_range_map::TraceIndexMapKey::Order::Asc
                              : trace_range_map::TraceIndexMapKey::Order::Desc;
    auto const seciOrder = seciStep > 0
                               ? trace_range_map::TraceIndexMapKey::Order::Asc
                               : trace_range_map::TraceIndexMapKey::Order::Desc;
    indexData_->traceIndexMap_.setOrderMode(priOrder, seciOrder);

    indexData_->traceIndexMap_.addRanges(indexData_->traceIndexStorage_);

    indexData_->indexedTraceCount_ = indexData_->traceIndexMap_.enumerateLiveTraceCount(prii0, priiN, priStep,
                                                                                        seci0, seciN, seciStep);
}

void
SegyIndexStorage::readMissingRangeMapFromFile_2024_01(const request::Params& params)
{
    // This will read '2022_01' version range map as the part of binary index

    if (not indexData_->missingTraceIndexMap_.isEmpty()) {
        indexData_->missingTraceIndexMap_.clearRanges();
    }

    const std::string indexFilename = getMissingIndexBinaryFileName(indexData_->indexBaseFileName_);
    const auto managerParams = fileio::FileManagerParams{
        fileio::FileAccessParams{params}.withFileName(indexFilename)}
                                   .withReaderOptions(fileReaderOptions_);
    std::vector<decltype(fileio::GenericFileManager::createReader(managerParams))> readers(index_config::binaryIndexReadersCount_, nullptr);

    std::generate(
        readers.begin(), readers.end(), [&managerParams]() -> auto {
            return fileio::GenericFileManager::createReader(managerParams);
        });

    std::shared_ptr<fileio::BufferedFileReaderBase> reader = std::make_unique<fileio::SequentialFileReader>(
        fileio::PipelineFileReaderParams{params}.withFileReaders(readers));
    reader->setBufferSize(readersTraceRecordBufferSize_);

    size_t indexRecordsRead = 0;
    const size_t indexCount = fileio::GenericFileManager::getSize({indexFilename, fileReaderOptions_}) / traceRecordSize_;

    for (/* EMPTY */; indexRecordsRead < indexCount; ++indexRecordsRead) {
        size_t offset = indexRecordsRead * traceRecordSize_;

        auto const* const indexRecordBuff = reader->readAligned(offset, traceRecordSize_, /*alignment*/ 0U);
        if (nullptr == indexRecordBuff) {
            throw segyio::SegyFileIOException("SegyIndexStorage::populateIndexItemsFromFile_2022_01: cannot read binary MissingTraces index file.");
        }

        auto const* recordPtr = reinterpret_cast<trace_range_map::TraceRangeRecord const*>(indexRecordBuff);

        trace_range_map::TraceRange range(*recordPtr);

        indexData_->missingTraceIndexStorage_.addRange(std::move(range));
    }

    trace_range_map::PrimaryIndex const prii0 = trace_range_map::TraceRangeFactory::getPrimaryExtentIndexFirst(indexData_->fileLayout_);
    trace_range_map::PrimaryIndex const priiN = trace_range_map::TraceRangeFactory::getPrimaryExtentIndexLast(indexData_->fileLayout_);
    trace_range_map::PrimaryIndex const priStep = trace_range_map::TraceRangeFactory::getPrimaryExtentStep(indexData_->fileLayout_);
    trace_range_map::SecondaryIndex const seci0 = trace_range_map::TraceRangeFactory::getSecondaryExtentIndexFirst(indexData_->fileLayout_);
    trace_range_map::SecondaryIndex const seciN = trace_range_map::TraceRangeFactory::getSecondaryExtentIndexLast(indexData_->fileLayout_);
    trace_range_map::SecondaryIndex const seciStep = trace_range_map::TraceRangeFactory::getSecondaryExtentStep(indexData_->fileLayout_);

    auto const priOrder = priStep > 0
                              ? trace_range_map::TraceIndexMapKey::Order::Asc
                              : trace_range_map::TraceIndexMapKey::Order::Desc;
    auto const seciOrder = seciStep > 0
                               ? trace_range_map::TraceIndexMapKey::Order::Asc
                               : trace_range_map::TraceIndexMapKey::Order::Desc;
    indexData_->missingTraceIndexMap_.setOrderMode(priOrder, seciOrder);

    indexData_->missingTraceIndexMap_.addRanges(indexData_->missingTraceIndexStorage_);

    indexData_->indexedLiveTraceCount_ = indexData_->missingTraceIndexMap_.enumerateLiveTraceCount(prii0, priiN, priStep,
                                                                                                   seci0, seciN, seciStep);
}

void
SegyIndexStorage::initFileLayout_2022_06()
{
    //
    // Declaration of auxiliary functions
    //

    auto updateCrosslineInfo = [this](int prevCrosslineNo, IndexBuilderData const& ibd) {
        auto const minCrossline = indexData_->geometry3D_.IVectorExtent.first();
        auto const maxCrossline = indexData_->geometry3D_.IVectorExtent.last();
        auto const stepCrossline = indexData_->geometry3D_.IVectorExtent.step();

        if (prevCrosslineNo <= ibd.CrosslineNo) {
            HorizontalExtent const iVectorExtent(/*first*/ minCrossline,
                                                 /*last*/ maxCrossline,
                                                 std::abs(stepCrossline));
            indexData_->fileLayout_.IExtent = iVectorExtent;
        }
        else {
            HorizontalExtent const iVectorExtent(/*first*/ maxCrossline,
                                                 /*last*/ minCrossline,
                                                 -std::abs(stepCrossline));
            indexData_->fileLayout_.IExtent = iVectorExtent;
        }
    };

    auto updateInlineInfo = [this](int prevInlineNo, IndexBuilderData const& ibd) {
        auto const minInline = indexData_->geometry3D_.JVectorExtent.first();
        auto const maxInline = indexData_->geometry3D_.JVectorExtent.last();
        auto const stepInline = indexData_->geometry3D_.JVectorExtent.step();

        if (prevInlineNo <= ibd.InlineNo) {
            HorizontalExtent const jVectorExtent(/*first*/ minInline,
                                                 /*last*/ maxInline,
                                                 std::abs(stepInline));
            indexData_->fileLayout_.JExtent = jVectorExtent;
        }
        else {
            HorizontalExtent const jVectorExtent(/*first*/ maxInline,
                                                 /*last*/ minInline,
                                                 -std::abs(stepInline));
            indexData_->fileLayout_.JExtent = jVectorExtent;
        }
    };

    auto updateSampleInfo = [this](GeometryCalculationData const& geometryCalcData) {
        auto const minSample = indexData_->geometry3D_.sampleExtent.first();
        auto const maxSample = indexData_->geometry3D_.sampleExtent.last();
        auto const stepSample = indexData_->geometry3D_.sampleExtent.step();

        if (geometryCalcData.getSampleRateRange().Min >= 0.0) {
            VerticalExtent const sampleExtent(/*first*/ minSample,
                                              /*last*/ maxSample,
                                              std::fabs(stepSample));
            indexData_->fileLayout_.sampleExtent = sampleExtent;
        }
        else {
            VerticalExtent const sampleExtent(/*first*/ maxSample,
                                              /*last*/ minSample,
                                              -std::fabs(stepSample));
            indexData_->fileLayout_.sampleExtent = sampleExtent;
        }
    };

    //
    // Doing initial assignment
    //
    indexData_->fileLayout_.JExtent
        = indexData_->geometry3D_.JVectorExtent;
    indexData_->fileLayout_.IExtent
        = indexData_->geometry3D_.IVectorExtent;
    indexData_->fileLayout_.sampleExtent
        = indexData_->geometry3D_.sampleExtent;
    indexData_->fileLayout_.isRegular
        = indexData_->geometry3D_.isRegular;
    indexData_->fileLayout_.isInlineOrdered
        = indexData_->geometry3D_.isInlineOrdered;

    //
    // Getting the index increment/decrement information
    //

    // Selecting any of SEGY files that we are processing at the moment
    auto const fileNo = indexData_->segyFilesIndexed_[0].FileNo;
    auto const segyFilePtr = indexData_->segyFilesVector_[0];

    // Obtaining the SegyFile object clones for the multithread reading
    std::vector<std::shared_ptr<fileio::FileReader>> files(index_config::numberOfTraceScannerPrefetchReaders_, nullptr);
    std::generate(
        files.begin(), files.end(), [&segyFilePtr]() -> auto {
            return segyFilePtr->getCloneFile();
        });

    // Obtaining the trace header definitions
    constexpr size_t const traceHeaderIDsCount{4U};
    static const auto traceHeaderIDs = std::array<std::string, traceHeaderIDsCount>{
        segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Inline,
        segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Crossline,
        segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SampleRate,
        segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::TraceLength};

    using TraceHeaderDefinitions_t = std::vector<std::shared_ptr<segyio::HeaderDefinition>>;
    TraceHeaderDefinitions_t traceHeaderDefinitions;

    std::transform(traceHeaderIDs.cbegin(), traceHeaderIDs.cend(), std::back_insert_iterator<TraceHeaderDefinitions_t>(traceHeaderDefinitions),
                   [&segyFilePtr](std::string const& id) {
                       return segyFilePtr->getTraceHeaderDefinition(id);
                   });

    // Obtaining trace scanner object
    constexpr bool raw = false;
    segyio::HeaderDefinitionProvider hdp(traceHeaderDefinitions, raw);
    segyio::TraceSequentialScanner scanner(std::move(files), segyFilePtr->getDerivedHeader(), hdp, hdp.getHeaderNames());

    if (segyFilePtr->getFileName().rfind("sd:", 0) != 0) {
        // Temporary performance work: This should probably only be done for local files ...
        size_t bufferSize
            = index_generator::SegyIndexGenerator::getIndexReadBufferSize();

        if (bufferSize > segyFilePtr->getFileSize()) {
            bufferSize = segyFilePtr->getFileSize();
        }

        scanner.setReaderBufferSize(bufferSize); // Set buffer size 25MB
    }

    // Initialize scanner
    constexpr long const startTrace{0L};
    scanner.reset(request::TraceParams{startTrace});

    CollationOffset const collatedOffsets
        = index_generator::SegyIndexGenerator::prepareCollatedOffsets(traceHeaderDefinitions);

    // Go trace by trace...
    bool isInlineInfoReady{};
    bool isCrosslineInfoReady{};

    bool isFirstTrace{true};
    int prevInlineNo{std::numeric_limits<int>::min()};
    int prevCrosslineNo{std::numeric_limits<int>::min()};

    std::shared_ptr<IndexBuilderCollatedData> collatedDataPtr = std::make_shared<IndexBuilderCollatedData>();

    GeometryCalculationData geometryCalcData;

    while (scanner.moveNext({})) {

        if (scanner.getHeaderValueCount() != traceHeaderDefinitions.size()) {
            throw SegyInvalidArgumentException("Returned trace header values do not match requested index fields!");
        }

        uint64_t const offset = scanner.getTraceInfo().offset_;
        const double* headerValues = scanner.getHeaderValues();

        IndexBuilderData const ibd
            = index_generator::SegyIndexGenerator::extractIndexBuilderDataFromTraceHeaders(collatedOffsets,
                                                                                           headerValues,
                                                                                           fileNo,
                                                                                           offset);
        index_generator::SegyIndexGenerator::updateCollatedDataRanges(collatedDataPtr,
                                                                      ibd,
                                                                      collatedOffsets,
                                                                      headerValues);

        geometryCalcData.updateCalculatedRangeValues(collatedDataPtr);
        geometryCalcData.updateCalculationData(ibd,
                                               isFirstTrace,
                                               prevInlineNo,
                                               prevCrosslineNo,
                                               indexData_->geometry3D_.isInlineOrdered);

        if (isFirstTrace) {
            isFirstTrace = false;

            prevInlineNo = ibd.InlineNo;
            prevCrosslineNo = ibd.CrosslineNo;

            // the goal to get some amount of sequential traces
            // so skip till the next trace...
            continue;
        }

        //
        // Obtain inline and crossline increment/decrement info
        //

        if (indexData_->geometry3D_.isInlineOrdered) {
            if (prevInlineNo == ibd.InlineNo) {
                if (not isCrosslineInfoReady) {
                    isCrosslineInfoReady = true;
                    updateCrosslineInfo(prevCrosslineNo, ibd);
                }
            }
            else {
                if (not isInlineInfoReady) {
                    isInlineInfoReady = true;
                    updateInlineInfo(prevInlineNo, ibd);
                }
            }
        }
        else {
            if (prevCrosslineNo == ibd.CrosslineNo) {
                if (not isInlineInfoReady) {
                    isInlineInfoReady = true;
                    updateInlineInfo(prevInlineNo, ibd);
                }
            }
            else {
                if (not isCrosslineInfoReady) {
                    isCrosslineInfoReady = true;
                    updateCrosslineInfo(prevCrosslineNo, ibd);
                }
            }
        }

        if (isInlineInfoReady and isCrosslineInfoReady) {
            // the inline and crossline increment/decrement info
            // has been obtained so break the loop...
            break;
        }

        prevInlineNo = ibd.InlineNo;
        prevCrosslineNo = ibd.CrosslineNo;

    } // while (scanner.moveNext())

    if (not isInlineInfoReady or not isCrosslineInfoReady) {
        const std::string msg = "SegyIndexStorage::initFileLayoutSpecification_2022_06: Input SEG-Y file does not have sequential traces enough to obtain inline and crossline increment/decrement info";
        throw SegyInvalidArgumentException(msg);
    }

    geometryCalcData.finalizeCalculationData();

    updateSampleInfo(geometryCalcData);
}

HorizontalExtent
SegyIndexStorage::CopyExtentIntFromJson(Json::Value node)
{
    return {
        node[JSONIndexStringConsts::first].asInt(),
        node[JSONIndexStringConsts::last].asInt(),
        node[JSONIndexStringConsts::step].asInt()};
}

VerticalExtent
SegyIndexStorage::CopyExtentDoubleFromJson(Json::Value node)
{
    return {
        node[JSONIndexStringConsts::first].asDouble(),
        node[JSONIndexStringConsts::last].asDouble(),
        node[JSONIndexStringConsts::step].asDouble(),
        node.isMember(JSONIndexStringConsts::domain) ? getDomainFromString(node[JSONIndexStringConsts::domain].asString()) : segyio::VerticalDomain::Time,
        node.isMember(JSONIndexStringConsts::zUoM) ? getVerticalUoMFromString(node[JSONIndexStringConsts::zUoM].asString()) : segyio::VerticalUOM::seconds};
}

GridCell
SegyIndexStorage::CopyCornerFromJson(Json::Value node)
{
    GridCell myCorner;

    myCorner.i = node[JSONIndexStringConsts::inlineNo].asInt();
    myCorner.j = node[JSONIndexStringConsts::crosslineNo].asInt();
    myCorner.location = {node[JSONIndexStringConsts::coordX].asDouble(), node[JSONIndexStringConsts::coordY].asDouble()};

    return myCorner;
}

/// <summary>
/// Checks if every filename is unique irrespective of its path
/// </summary>
/// <param name="inputFileNames"></param>
/// <returns>bool true if every filename is uniqye, false if filenames have been duplicated, possibly across multiple folders</returns>
bool
SegyIndexStorage::filenamesAreUnique(FileList const& inputFileNames) const
{

    size_t ix = 1;
    for (const std::string& ifname : inputFileNames) {
        auto ifn = getFileName(ifname);

        if (ix >= inputFileNames.size()) {
            return true;
        }

        for (auto mIter = std::next(inputFileNames.begin(), static_cast<std::iterator_traits<FileList::iterator>::difference_type>(ix));
             mIter != inputFileNames.end();
             ++mIter) {

            auto tfn = getFileName(*mIter);
            if (ifn == tfn) {
                return false;
            }
        }

        ix++;
    }

    return true;
}

/// <summary>
/// Checks if every fully qualified file name is unique
/// </summary>
/// <param name="inputFileNames"></param>
/// <returns>
/// bool true if every fq filename is unique, otherwise false</returns>
bool
SegyIndexStorage::fullFilenamesAreUnique(FileList const& inputFileNames)
{

    size_t ix = 1;
    for (const auto& ifname : inputFileNames) {
        if (ix >= inputFileNames.size()) {
            return true;
        }

        for (auto mIter = std::next(inputFileNames.begin(), static_cast<std::iterator_traits<FileList::iterator>::difference_type>(ix));
             mIter != inputFileNames.end();
             ++mIter) {

            if (ifname == *mIter) {
                return false;
            }
        }

        ix++;
    }

    return true;
}

multi_file::FileNameStatus
SegyIndexStorage::IndexedFileFoundInInputContainer(const std::string& indexedFileName,
                                                   FileList const& inputFileList,
                                                   bool logDebugOutput) const
{
    (void)logDebugOutput;
    using multi_file::FileNameStatus;

    const std::string lhsPath = getFilePath(indexedFileName);
    const std::string lhsName = getFileName(indexedFileName);

    bool fnameOK = false;
    for (const auto& ifn : inputFileList) {

        std::string rhsName = getFileName(ifn);
        std::string rhsPath = getFilePath(ifn);

        SEGYLOG_DEBUG("IndexedFileFoundInInputContainer::FileNameMatch: Compare " + lhsName + " size=" + std::to_string(lhsName.size()) + " to " + rhsName + " size=" + std::to_string(rhsName.size()));

        if ((rhsName.size() == lhsName.size() && rhsPath.size() == lhsPath.size())
            && (rhsName == lhsName && rhsPath == lhsPath)) {
            return FileNameStatus::Match;
        }

        if (rhsName.size() != lhsName.size()) {
            continue;
        }

        const auto matchResult = rhsName.compare(lhsName);

        SEGYLOG_DEBUG("IndexedFileFoundInInputContainer::FileNameMatch: compare(" + std::to_string(matchResult) + ") for file names " + lhsName + " and " + rhsName);

        if (matchResult == 0) {

            SEGYLOG_DEBUG("IndexedFileFoundInInputContainer::FileNameMatch: " + lhsName + " and " + rhsName);

            fnameOK = true;
        }
    }

    if (fnameOK) {
        return FileNameStatus::FileNameMatch;
    }

    return FileNameStatus::NoMatch;
}

segyio::VerticalDomain
SegyIndexStorage::getDomainFromString(const std::string& domain)
{
    if (domain == "Depth") {
        return segyio::VerticalDomain::Depth;
    }

    return segyio::VerticalDomain::Time;
}

segyio::VerticalUOM
SegyIndexStorage::getVerticalUoMFromString(const std::string& zUoM)
{
    if (zUoM == "meter") {
        return segyio::VerticalUOM::meter;
    }

    if (zUoM == "feet") {
        return segyio::VerticalUOM::feet;
    }

    if (zUoM == "seconds") {
        return segyio::VerticalUOM::seconds;
    }

    const std::string msg = "Illegal string value for vertical unit of measurement. " + zUoM + " is not recognized.";
    throw SegyInvalidArgumentException(msg);
}

constexpr auto pathDelimiter = '/';

std::string
SegyIndexStorage::getFilePath(const std::string& filename)
{
    std::string tmp = filename;
    utilities::StringUtils::ReplaceAll(tmp, '\\', pathDelimiter);

    const auto pos = getLastFolderSeparatorPosition(tmp);
    if (pos > 0) {
        return tmp.substr(0, pos);
    }

    return "";
}

std::string
SegyIndexStorage::getFileName(const std::string& filename)
{
    const auto pos = getLastFolderSeparatorPosition(filename);

    if (pos > 0) {
        return filename.substr(pos + 1);
    }

    return filename;
}

size_t
SegyIndexStorage::getLastFolderSeparatorPosition(const std::string& filename)
{
    std::string tmp = filename;
    utilities::StringUtils::ReplaceAll(tmp, '\\', pathDelimiter);

    const size_t pos = tmp.rfind(pathDelimiter);

    return pos;
}

bool
SegyIndexStorage::SaveIndexMetaData() const
{
    bool result = false;

    do {
        const std::string metaDataFileName = getIndexMetaDataFileName(indexData_->indexBaseFileName_);
        fileio::FileReference metaFileRef(metaDataFileName, fileReaderOptions_);

        auto writer = fileio::GenericFileManager::createWriter(metaFileRef, fileio::WriteMode::Truncate);
        if (writer == nullptr) {
            break;
        }

        const Json::Value root = getIndexMetadataAsJson();
        const std::string headerData = root.toStyledString();

        // Write meta data section.
        writer->write(reinterpret_cast<const uint8_t*>(headerData.c_str()), headerData.size());
        writer->close();

        result = true;

    } while (false);

    return result;
}

bool
SegyIndexStorage::SaveBinaryIndexData() const
{
    if (indexData_->is2D()) {
        return SaveBinaryIndexData2D();
    }
    return SaveBinaryIndexData3D();
}

bool
SegyIndexStorage::SaveBinaryIndexData2D() const
{
    bool result = false;

    const std::string indexFilename = getIndexBinaryFileName(indexData_->indexBaseFileName_);
    fileio::FileReference indexFileRef(indexFilename, fileReaderOptions_);

    auto indexWriter = fileio::GenericFileManager::createWriter(indexFileRef, fileio::WriteMode::Truncate);
    if (indexWriter == nullptr) {
        return result;
    }

    result = indexData_->indexing2dInfo_.saveIndex(indexWriter);

    return result;
}

bool
SegyIndexStorage::SaveBinaryIndexData3D() const
{
    const std::string indexFilename = getIndexBinaryFileName(indexData_->indexBaseFileName_);
    fileio::FileReference indexFileRef(indexFilename, fileReaderOptions_);

    auto indexWriter = fileio::GenericFileManager::createWriter(indexFileRef, fileio::WriteMode::Truncate);
    if (indexWriter == nullptr) {
        return false;
    }

    if (indexData_->traceIndexMap_.isEmpty()) {
        return true;
    }

    std::vector<trace_range_map::TraceRangeRecord> indexVector;
    indexVector.reserve(indexData_->traceIndexMap_.size());

    for (const auto& [id, indexRecord] : indexData_->traceIndexMap_) {
        trace_range_map::TraceRangeRecord record = indexRecord.get().getRecord();
        indexVector.emplace_back(std::move(record));
    }

    indexWriter->write(reinterpret_cast<const uint8_t*>(indexVector.data()),
                       indexVector.size() * sizeof(trace_range_map::TraceRangeRecord));

    const std::string finish = "{END INDEX}";
    indexWriter->write(reinterpret_cast<const uint8_t*>(finish.c_str()), finish.size());

    indexWriter->close();

    return true;
}

bool
SegyIndexStorage::hasMissingTraceIndex() const
{
    const std::string indexFilename = getMissingIndexBinaryFileName(indexData_->indexBaseFileName_);
    fileio::FileReference indexFileRef(indexFilename, fileReaderOptions_);

    return fileio::GenericFileManager::exists(indexFileRef);
}

bool
SegyIndexStorage::saveBinaryMissingIndexData3D(const segyindex::trace_info::SegyIndexTraceInfo& indexTraceInfo, std::vector<segysdk::bulkaccess::TraceHeaderResult> missingTraces)
{
    const std::string indexFilename = getMissingIndexBinaryFileName(indexData_->indexBaseFileName_);
    fileio::FileReference indexFileRef(indexFilename, fileReaderOptions_);

    auto indexWriter = fileio::GenericFileManager::createWriter(indexFileRef, fileio::WriteMode::Truncate);
    if (indexWriter == nullptr) {
        return false;
    }
    if (missingTraces.empty()) {
        return true;
    }

    segysdk::segyindex::index_data::TraceIndexStorage traceIndexStorage;
    trace_range_map::MissingTraceRangeFactory factory(indexData_->fileLayout_, traceIndexStorage);
    auto primaryFirst = indexData_->fileLayout_.isInlineOrdered ? static_cast<int64_t>(indexData_->fileLayout_.JExtent.first()) : static_cast<int64_t>(indexData_->fileLayout_.IExtent.first());
    auto primaryLast = indexData_->fileLayout_.isInlineOrdered ? static_cast<int64_t>(indexData_->fileLayout_.JExtent.last()) : static_cast<int64_t>(indexData_->fileLayout_.IExtent.last());
    auto primaryStep = indexData_->fileLayout_.isInlineOrdered ? static_cast<int64_t>(indexData_->fileLayout_.JExtent.step()) : static_cast<int64_t>(indexData_->fileLayout_.IExtent.step());

    auto secondaryFirst = indexData_->fileLayout_.isInlineOrdered ? static_cast<int64_t>(indexData_->fileLayout_.IExtent.first()) : static_cast<int64_t>(indexData_->fileLayout_.JExtent.first());
    auto secondaryLast = indexData_->fileLayout_.isInlineOrdered ? static_cast<int64_t>(indexData_->fileLayout_.IExtent.last()) : static_cast<int64_t>(indexData_->fileLayout_.JExtent.last());
    auto secondaryStep = indexData_->fileLayout_.isInlineOrdered ? static_cast<int64_t>(indexData_->fileLayout_.IExtent.step()) : static_cast<int64_t>(indexData_->fileLayout_.JExtent.step());

    auto traceIterator = missingTraces.begin();
    const auto sampleCount = indexData_->fileLayout_.sampleExtent.count();

    for (auto priIx = primaryFirst; priIx <= primaryLast; priIx += primaryStep) {
        for (auto secIx = secondaryFirst; secIx <= secondaryLast; secIx += secondaryStep) {
            auto currIL = indexData_->fileLayout_.isInlineOrdered ? priIx : secIx;
            auto currXL = indexData_->fileLayout_.isInlineOrdered ? secIx : priIx;
            bool isMissing = traceIterator != missingTraces.end() and (traceIterator->getInline() == currIL) and (traceIterator->getCrossline() == currXL);

            if (not isMissing) {
                // indexTrace should be found!
                const auto indexTrace = indexTraceInfo.getIndexAt(currIL, currXL);
                const auto offset = indexTrace.getOffset();
                const auto fileNo = indexTrace.getSegyFileNumber();
                factory.addTrace(priIx, secIx, sampleCount, fileNo, offset);
            }
            else {
                // Simulate missing trace ...
                const auto prevIL = traceIterator->getInline();
                const auto prevXL = traceIterator->getCrossline();

                // Filter out possible duplicate missing traces
                while (traceIterator != missingTraces.end() and traceIterator->getInline() == prevIL and traceIterator->getCrossline() == prevXL) {
                    traceIterator++;
                }
            }
        }
    }

    indexData_->missingTraceIndexMap_.addRanges(traceIndexStorage);

    std::vector<trace_range_map::TraceRangeRecord> indexVector;
    indexVector.reserve(indexData_->missingTraceIndexMap_.size());

    for (const auto& [id, indexRecord] : indexData_->missingTraceIndexMap_) {
        trace_range_map::TraceRangeRecord record = indexRecord.get().getRecord();
        indexVector.emplace_back(std::move(record));
    }

    indexWriter->write(reinterpret_cast<const uint8_t*>(indexVector.data()),
                       indexVector.size() * sizeof(trace_range_map::TraceRangeRecord));

    const std::string finish = "{END INDEX}";
    indexWriter->write(reinterpret_cast<const uint8_t*>(finish.c_str()), finish.size());

    indexWriter->close();

    return true;
}

void
SegyIndexStorage::loadBinaryMissingIndexData3D(const request::Params& params)
{
    if (!hasMissingTraceIndex()) {
        return;
    }

    if (indexData_->indexVersionMajor_ == index_config::IndexMajorVersion_2024_01_
        and indexData_->indexVersionMinor_ == index_config::IndexMinorVersion_2024_01_) {
        populateMissingTracesIndexItemsFromFile_2024_01(params);
        missingEmptyDeadTracesIndexLoaded_ = not indexData_->missingTraceIndexMap_.isEmpty();
    }
}

Json::Value
SegyIndexStorage::getIndexMetadataAsJson() const
{
    Json::Value root;
    Json::Value metadataNode;

    Json::Value versionNode;
    versionNode[JSONIndexStringConsts::Major] = indexData_->indexVersionMajor_;
    versionNode[JSONIndexStringConsts::Minor] = indexData_->indexVersionMinor_;

    metadataNode[JSONIndexStringConsts::GeneratedDate] = getTimeDateString();

    metadataNode[JSONIndexStringConsts::versionTag] = versionNode;

    Json::Value infoNode;
    //  Json::Value cannot be directly assigned a size_t value
    infoNode[JSONIndexStringConsts::SampleSize] = static_cast<Json::UInt64>(indexData_->sampleSize_);

    infoNode[JSONIndexStringConsts::ConstantCoordinateScalar] = indexData_->hasConstantCoordinateScalar_;
    if (indexData_->hasConstantCoordinateScalar_) {
        infoNode[JSONIndexStringConsts::ConstantCoordinateScalarValue] = indexData_->constantCoordinateScalar_;
    }
    infoNode[JSONIndexStringConsts::ConstantTraceLength] = indexData_->hasConstantTraceLength_;
    if (indexData_->hasConstantTraceLength_) {
        infoNode[JSONIndexStringConsts::ConstantTraceLengthValue] = static_cast<Json::UInt64>(indexData_->constantTraceLength_);
    }
    if (indexData_->amplitudeMaximum_ > indexData_->amplitudeMinimum_) {
        infoNode[JSONIndexStringConsts::AmplitudeMin] = static_cast<double>(indexData_->amplitudeMinimum_);
        infoNode[JSONIndexStringConsts::AmplitudeMax] = static_cast<double>(indexData_->amplitudeMaximum_);
    }
    infoNode[JSONIndexStringConsts::ConstantTraceWeighting] = indexData_->hasConstantTraceWeightingFactor_;
    if (indexData_->hasConstantTraceWeightingFactor_) {
        infoNode[JSONIndexStringConsts::ConstantTraceWeightingValue] = static_cast<Json::UInt64>(indexData_->constantTraceWeightingFactor_);
    }

    infoNode[JSONIndexStringConsts::OffsetFirtstSEGYTrace] = static_cast<Json::UInt64>(indexData_->offsetFirtstSEGYTrace_);
    infoNode[JSONIndexStringConsts::TraceHeaderSize] = static_cast<Json::UInt64>(indexData_->traceHeaderSize_);

    infoNode[JSONIndexStringConsts::SegyDimension] = getSegyDimensionAsJson();

    infoNode[JSONIndexStringConsts::SegyFileList] = getSegyFilesAsJson();
    infoNode[JSONIndexStringConsts::SegyFileMap] = getSegyFileMapAsJson();
    root[JSONIndexStringConsts::metaTag] = metadataNode;
    root[JSONIndexStringConsts::segyInfoTag] = infoNode;

    if (indexData_->is2D()) {
        root[JSONIndexStringConsts::LineExtentTag] = getSeismicLineExtentsAsJson(indexData_->indexing2dInfo_.getLineExtents());
    }
    else {
        root[JSONIndexStringConsts::Geometry3DTag] = getGeometry3DAsJson(indexData_->geometry3D_);
        root[JSONIndexStringConsts::FileLayoutSpecificationTag] = getFileLayoutAsJson(indexData_->fileLayout_);
    }

    root[JSONIndexStringConsts::IndexCount] = static_cast<Json::UInt64>(indexData_->indexedTraceCount_);

    return root;
}

Json::Value
SegyIndexStorage::getIndexAsJson() const
{
    if (indexData_->is2D()) {
        return indexData_->indexing2dInfo_.toJsonValue();
    }

    Json::Value indexJson;

    for (const auto& [_, indexRecord] : indexData_->traceIndexMap_) {
        indexJson.append(indexRecord.get().toJsonValue());
    }

    return indexJson;
}

std::string
SegyIndexStorage::getTimeDateString()
{
    std::time_t time = std::time(nullptr);
    std::tm tm{};
    utilities::safe_gmtime(time, tm);
    std::stringstream stream;
    stream << std::put_time(&tm, "%Y-%m-%dT%H:%M:%SZ");

    return stream.str();
}

Json::Value
SegyIndexStorage::getSegyFilesAsJson() const
{
    Json::Value node;
    // Json::Value cannot be directly assigned size_t values without static cast...
    for (const multi_file::MultiFileStruct& fs : indexData_->segyFilesIndexed_) {
        Json::Value fnode;
        fnode[JSONIndexStringConsts::FileNo] = static_cast<Json::UInt64>(fs.FileNo);
        fnode[JSONIndexStringConsts::FileOffset] = static_cast<Json::UInt64>(fs.FirstTraceOffset);
        fnode[JSONIndexStringConsts::SegyFile] = fs.SegyFileName;
        fnode[JSONIndexStringConsts::PrimaryMin] = fs.PrimaryKeyRange.Min;
        fnode[JSONIndexStringConsts::PrimaryMax] = fs.PrimaryKeyRange.Max;
        node.append(fnode);
    }

    return node;
}

Json::Value
SegyIndexStorage::getSegyFileMapAsJson() const
{
    Json::Value node;
    for (auto it = indexData_->multiFileMap_.begin(); it != indexData_->multiFileMap_.end(); ++it) {
        Json::Value element;
        element[JSONIndexStringConsts::Primary] = it->first;
        element[JSONIndexStringConsts::FileNo] = it->second;
        node.append(element);
    }

    return node;
}

std::string
SegyIndexStorage::getUoMAsString(segyio::VerticalUOM zUoM)
{
    switch (zUoM) {
    case segyio::VerticalUOM::seconds:
        return JSONIndexStringConsts::seconds;
    case segyio::VerticalUOM::meter:
        return JSONIndexStringConsts::meter;
    case segyio::VerticalUOM::feet:
        return JSONIndexStringConsts::feet;
    default:
        throw SegyInvalidArgumentException("getUoMAsString: Illegal string value for vertical unit of measurement. ");
    }
}

void
SegyIndexStorage::setInitialDefaults()
{
    indexData_->setInitialDefaults();
}

void
SegyIndexStorage::setEmptyGeometry3D()
{
    indexData_->setEmptyGeometry3D();
}

void
SegyIndexStorage::setEmptyFileLayout()
{
    indexData_->setEmptyFileLayout();
}

std::string
SegyIndexStorage::getInputSEGYFileNameFromList(FileList const& inputFileContainer,
                                               const std::string& indexFileName) const
{
    const std::string newfn = getFileName(indexFileName);

    for (const auto& sf : inputFileContainer) {
        std::string ifn = getFileName(sf);

        if (newfn.size() == ifn.size() && newfn == ifn) {
            return sf;
        }
    }
    return "InvalidSEGYFileName"; // This really should not happen!
}

Json::Value
SegyIndexStorage::getSegyDimensionAsJson() const
{
    return indexData_->is2D() ? Json::Value(JSONIndexStringConsts::dimension2D) : Json::Value(JSONIndexStringConsts::dimension3D);
}

} // namespace index_storage
} // namespace segysdk::segyindex
