
/*
Copyright 2022 Schlumberger

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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "SegyIndexConfig.h"

#include "../BulkAccess/TraceHeaderResult.h"
#include "../FileIO/FileReaderOptions.h"
#include "../FileIO/FileReference.h"
#include "../SegyIndex/Geometry3D.h"
#include "../SegyIndex/GridCell.h"
#include "../SegyIndex/SegyIndexData.h"
#include "../SegyIndex/SegyIndexTraceInfo.h"
#include "../SegyIndex/SegyIndexMultiFileStruct.h"
#include "../SegyIndex/HorizontalExtent.h"
#include "../SegyIndex/IndexField.h"

#include "../SegyIO/VerticalDomain.h"
#include "../SegyIO/VerticalUOM.h"
#include "../SegyIndex/2DIndexing/SeismicLineExtents.h"
#include "../SegyIndex/Geometry3D.h"
#include "../SegyIndex/IndexField.h"
#include "../SegyIndex/SegyIndexData.h"

#include <json/json.h>

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace segysdk::fileio
{

struct FileManagerParams;

} // namespace segysdk::fileio

namespace segysdk::segyindex
{
namespace index_storage
{
using FileList = std::vector<std::string>;
using LegacyIndexMap = std::map<size_t, IndexField>;
using SegyIndexDataPtr = index_data::SegyIndexDataPtr;

/**
 * @brief SegyIndexStorage struct loads/saves the trace information from/to the storage
 **/
struct SEGYSDK_DLL_PUBLIC SegyIndexStorage final
{
    /**
     * @brief Constructs SegyIndexStorage struct instance
     *
     * @param fileReaderOptions specifies the credentials and cache sizes to access the index files
     * @param indexData gives the shared pointer to the index base data instance the trace information is obtained from
     *
     **/
    SegyIndexStorage(fileio::FileReaderOptions fileReaderOptions,
                     SegyIndexDataPtr& indexData);

    /**
     * @brief Indicates whether or not an index has been generated for the current SEG-Y file(s).
     *
     * @details Returns false if the index file cannot be found in the expected location.
     *
     * @return bool value indicating whether an index file was found (true) or not fount. (false)
     *
     **/
    bool hasIndexFile() const;
    bool hasIndexFile(const request::Params& params) const;

    /**
     * @brief Loads the index file
     *
     * @remarks If the available binary index has the previous format
     *          then the implicit conversion to the latest format takes place
     **/
    void Load();
    void Load(const request::Params& params);

    /**
     * @brief Saves the index file using the latest format
     **/
    bool Save();

    /**
     * @brief Saves the index file using the JSON format
     *
     * @param outputFileName file path name to save the JSON formatted index to
     **/
    void SaveAsJson(const std::string& outputFileName) const;

    /**
     * @brief Delete existing index files, if any.
     *
     * @details Deletes the main and binary index files, and invalidates the index data.
     */
    void deleteIndex();

    /**
     * @brief Initializes data to generate index from the scratch
     *
     **/
    void ResetIndexData();

    /**
     * @brief Validates input file path names
     *
     * @param inputFileList input file path names to verify
     * @param logDebugOutput if true then produces the verbose output
     *
     * @return the FileNameStatus enum value showing the match status
     **/
    multi_file::FileNameStatus validateIndexedFiles(FileList const& inputFileList, bool logDebugOutput = false) const;

    /**
     * @brief Overrides the Geometry3D data member
     *
     * @param geometry new value for the Geometry3D data member
     **/
    void setGeometry3D(Geometry3D const& geometry);

    /**
     * @brief Reuses the existing files where it is possible based on new input files list
     *
     * @param inputFileList new input files list
     **/
    void reconnectSEGYFilesAfterMove(FileList const& inputFileList);

    /**
     * @brief Returns the Geometry3D instance from the JSON value
     *
     * @param json JSON value to retrieve the Geometry3D instance from
     *
     * @return the Geometry3D instance
     **/
    static Geometry3D GetGeometry3DFromJson(const Json::Value& json);

    /**
     * @brief Returns the JSON value from the Geometry3D instance
     *
     * @param g the Geometry3D instance to retrieve the JSON value from
     *
     * @return the JSON Geometry3D instance
     **/
    static Json::Value getGeometry3DAsJson(const Geometry3D& g);

    /**
     * @brief Returns the SeismicLineExtents instance from the JSON value
     *
     * @param json JSON value to retrieve the SeismicLineExtents instance from
     *
     * @return the SeismicLineExtents instance
     **/
    static index_2d::SeismicLineExtents getSeismicLineExtentsFromJson(const Json::Value& json);

    /**
     * @brief Returns the JSON value from the SeismicLineExtents instance
     *
     * @param lineExtents the SeismicLineExtents instance to retrieve the JSON value from
     *
     * @return the JSON SeismicLineExtents instance
     **/
    static Json::Value getSeismicLineExtentsAsJson(const index_2d::SeismicLineExtents& lineExtents);

    /**
     * @brief Returns the FileLayoutSpecification instance from the JSON value
     *
     * @param json JSON value to retrieve the FileLayoutSpecification instance from
     *
     * @return the FileLayoutSpecification instance
     **/
    static FileLayoutSpecification getFileLayoutStructFromJson(const Json::Value& json);

    /**
     * @brief Returns the JSON value from the FileLayoutSpecification instance
     *
     * @param f the FileLayoutSpecification instance to retrieve the JSON value from
     *
     * @return the JSON FileLayoutSpecification instance
     **/
    static Json::Value getFileLayoutAsJson(const FileLayoutSpecification& f);

    /**
     * @brief Appends value to std::vector based buffer instance
     *
     * @param buffer the buffer to append the value to
     * @param value the value to append the buffer
     **/
    template<typename T>
    static std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, void>
    append_to_buffer(std::vector<unsigned char>& buffer, T value);

    /**
     * @brief Retrieves uint16_t value from the buffer at the specified offset
     *
     * @param buffer the buffer to retrieve the value from
     * @param offset the buffer offset in bytes to retrieve the value starting from
     *
     * @return the uint16_t value
     **/
    static uint16_t getUShortFromBuffer(const unsigned char* buffer, size_t offset);

    /**
     * @brief Retrieves int32_t value from the buffer at the specified offset
     *
     * @param buffer the buffer to retrieve the value from
     * @param offset the buffer offset in bytes to retrieve the value starting from
     *
     * @return the int32_t value
     **/
    static int32_t getIntFromBuffer(const unsigned char* buffer, size_t offset);

    /**
     * @brief Retrieves size_t value from the buffer at the specified offset
     *
     * @param buffer the buffer to retrieve the value from
     * @param offset the buffer offset in bytes to retrieve the value starting from
     *
     * @return the size_t value
     **/
    static size_t getSizeFromBuffer(const unsigned char* buffer, size_t offset);

    /**
    * @brief Check if dead, empty or missing trace index exists
    * 
    * @return true if file exist, false otherwise
    * 
    **/
    bool hasMissingTraceIndex() const;

    bool saveBinaryMissingIndexData3D(const segyindex::trace_info::SegyIndexTraceInfo& indexTraceInfo, std::vector<segysdk::bulkaccess::TraceHeaderResult> missingTraces);

    void loadBinaryMissingIndexData3D(const request::Params& params);

    void loadBinaryIndexData3D(const request::Params& params);
    void loadBinaryIndexData2D(const request::Params& params);

    bool isLiveTracesIndexLoaded() const { return missingEmptyDeadTracesIndexLoaded_; }

 private:
    static std::string getIndexBinaryFileName(const std::string& segyFile);
    static std::string getIndexMetaDataFileName(const std::string& segyFile);
    Json::Value readIndexMetaDataFile(const request::Params& params) const;
    static std::string getMissingIndexBinaryFileName(const std::string& segyFile);

    fileio::FileReaderOptions getIndexFileReaderOptions() const { return fileReaderOptions_; }

    Json::Value readIndexMetaDataFile() const;

    static bool getIsRegularFromJson(Json::Value node);
    static size_t getMajorIndexVersionFromMetaData(Json::Value node);
    static size_t getMetadataSizeFromIndexFile(const fileio::FileManagerParams& params);
    void getGeometry3DFromJson(Json::Value json);
    void getFileLayoutFromJson(Json::Value const& json);
    void setLineExtentsFromJson(Json::Value json);
    void getIndexMetadataFromJson(Json::Value json);
    bool isRegular() const;
    void migrateJsonConfigFrom_2022_01();
    void populateIndexItemsFromFile_2024_01(const request::Params& params);
    void populateIndexItemsFromFile_2022_06(const request::Params& params);
    void populateIndexItemsFromFile_2022_01(const request::Params& params);
    void populateIndexItemsFromFile_2021_01(const request::Params& params);
    void populateIndexItemsFromFile_2020_01(const request::Params& params);
    void populateMissingTracesIndexItemsFromFile_2024_01(const request::Params& params);

    void verifyInputSegyFilesListIsValid(FileList const& inputFileList,
                                         std::string callerInfo,
                                         bool performExactFileNameCheck = true) const;
    void getSegyFileNamesFromJson(Json::Value node);
    void getFileMapFromJson(Json::Value node);
    static size_t getKeyFromGrid(int inlineNo, int crosslineNo);
    void importLegacyIndex_2022_01(LegacyIndexMap const& legacyIndex);
    void readRangeMapFromFile_2022_01(const request::Params& params);
    void initFileLayout_2022_06();

    void readMissingRangeMapFromFile_2024_01(const request::Params& params);

    static HorizontalExtent CopyExtentIntFromJson(Json::Value node);
    static VerticalExtent CopyExtentDoubleFromJson(Json::Value node);
    static GridCell CopyCornerFromJson(Json::Value node);
    bool filenamesAreUnique(FileList const& inputFileNames) const;
    static bool fullFilenamesAreUnique(FileList const& inputFileNames);
    multi_file::FileNameStatus IndexedFileFoundInInputContainer(const std::string& indexedFileName,
                                                                FileList const& inputFileList,
                                                                bool logDebugOutput = false) const;
    static segyio::VerticalDomain getDomainFromString(const std::string& domain);
    static segyio::VerticalUOM getVerticalUoMFromString(const std::string& zUoM);
    static std::string getFilePath(const std::string& filename);
    static std::string getFileName(const std::string& filename);
    static size_t getLastFolderSeparatorPosition(const std::string& filename);
    bool SaveIndexMetaData() const;
    bool SaveBinaryIndexData() const;
    bool SaveBinaryIndexData2D() const;
    bool SaveBinaryIndexData3D() const;
    Json::Value getIndexMetadataAsJson() const;
    Json::Value getIndexAsJson() const;
    static std::string getTimeDateString();
    Json::Value getSegyFilesAsJson() const;
    Json::Value getSegyFileMapAsJson() const;
    static std::string getUoMAsString(segyio::VerticalUOM zUoM);
    void setInitialDefaults();
    void setEmptyGeometry3D();
    void setEmptyFileLayout();
    std::string getInputSEGYFileNameFromList(FileList const& inputFileContainer,
                                             const std::string& indexFileName) const;

    Json::Value getSegyDimensionAsJson() const;

private:
    SegyIndexStorage(SegyIndexStorage const&) = delete;
    SegyIndexStorage& operator=(SegyIndexStorage const&) = delete;
    SegyIndexStorage(SegyIndexStorage&&) = delete;
    SegyIndexStorage& operator=(SegyIndexStorage&&) = delete;

private:
    static constexpr size_t const oldStyleMetadataSize_ = 2000U;
    static constexpr size_t const indexAsciiHeaderSize_ = 50U;
    static constexpr size_t const traceRecordSize_ = sizeof(trace_range_map::TraceRangeRecord);
    static constexpr size_t const readersTraceRecordBufferSize_ = 1024U * traceRecordSize_ * index_config::binaryIndexReadersCount_;
    static constexpr size_t const indexRecordSize_ = sizeof(uint16_t) + sizeof(int32_t) + sizeof(int32_t) + sizeof(uint64_t);
    static constexpr size_t const readersLegacyIndexBufferSize_ = 1024U * indexRecordSize_ * index_config::binaryIndexReadersCount_;

private:
    fileio::FileReaderOptions fileReaderOptions_{};
    SegyIndexDataPtr indexData_{nullptr};

    bool missingEmptyDeadTracesIndexLoaded_{false};

};

} // namespace index_storage
} // namespace segysdk::segyindex

//-----------------------------------------------------------------------------
//    SegyIndexStorage class
//-----------------------------------------------------------------------------

namespace segysdk::segyindex
{
namespace index_storage
{

template<typename T>
inline std::enable_if_t<std::is_integral_v<T> || std::is_floating_point_v<T>, void>
SegyIndexStorage::append_to_buffer(std::vector<unsigned char>& buffer, T value)
{
    const auto* bytes = reinterpret_cast<const unsigned char*>(&value);
    for (size_t i = 0; i < sizeof(T); ++i) {
        buffer.push_back(bytes[i]);
    }
}

} // namespace index_storage
} // namespace segysdk::segyindex
