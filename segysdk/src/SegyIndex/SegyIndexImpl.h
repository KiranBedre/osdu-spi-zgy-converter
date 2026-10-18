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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../BulkAccess/TraceHeaderResult.h"
#include "../FileIO/FileReaderOptions.h"
#include "../SegyIO/SegyFile.h"
#include "../SegyIndex/SegyIndex.h"
#include "../SegyIndex/SegyIndexConfig.h"
#include "../SegyIndex/SegyIndexData.h"
#include "../SegyIndex/SegyIndexGenerator.h"
#include "../SegyIndex/SegyIndexStorage.h"
#include "../SegyIndex/SegyIndexTraceInfo.h"

#include <json/json.h>

#include <memory>
#include <optional>
#include <string>
#include <vector>

namespace segysdk::segyindex
{

struct SEGYSDK_DLL_PUBLIC SegyIndexImpl final : public SegyIndex
{
    using FileList = std::vector<std::string>;

    /**
     * @brief Creates SegyIndexImpl struct instance
     *
     * @param segyFilesVector gives the SEGY file(s) to build the trace information by
     * @param segyFileNames  vector of SEGY file path name(s) to operate with the index for.
     * @param fileReaderOptions  parameters to get access to the SEGY file(s).
     * @param indexBaseFileNameOverride  file path name to be used instead of the default one (the first file in the segyFileNames parameter)
     *
     **/
    SegyIndexImpl(
        std::vector<std::shared_ptr<segyio::SegyFile>> const& segyFilesVector,
        FileList const& segyFileNames,
        const fileio::FileReaderOptions& fileReaderOptions,
        std::optional<std::string> const& indexBaseFileNameOverride = std::nullopt);

    explicit SegyIndexImpl(const segyindex::Params& params);

    virtual ~SegyIndexImpl() override;

    /**
     * @brief Indicates whether or not an index has been generated for the current SEG-Y file(s).
     *
     * @details Returns false if the index file cannot be found in the expected location.
     *
     * @param storageFileCheck indicates whether check for file existance in storage should be performed or cached value should be used
     *
     * @return bool value indicating whether an index file was found (true) or not fount. (false)
     *
     **/
    virtual bool hasIndexFile(bool storageFileCheck = false) const override;

    /**
     * @brief Loads the index file
     *
     * @remarks If the available binary index has the previous format
     *          then the implicit conversion to the latest format takes place
     **/
    virtual void Load() override;
    void Load(const request::Params& params) override;

    /**
     * @brief Saves the index file
     *
     * @remarks If the available binary index has the previous format
     *          then the implicit conversion to the latest format takes place
     **/
    virtual bool Save() override;

    /**
     * @brief Saves the index file in JSON format
     *
     * @param outputFileName file path name to save the JSON formatted index to
     **/
    virtual void SaveAsJson(const std::string& outputFileName) const override;

    /**
     * @brief Delete existing index files, if any.
     *
     * @details Deletes the main and binary index files, and invalidates the index data.
     */
    virtual void deleteIndex() override;

    /**
     * @brief Scans the SEGY file(s) and generates the trace information
     *
     * @param forceGenerateIndex specifies whether to force the trace information generation for the binary index file
     *
     * @return true if the trace information generation for the binary index is successful and false otherwise
     *
     **/
    virtual bool GenerateIndex(bool forceGenerateIndex) override;

    /**
     * @brief Gets the seismic data geometry info.
     *
     * @returns Geometry3D struct holding the seismic data geometry info.
     *
     **/
    virtual Geometry3D getGeometry3D() const override;

    /**
     * @brief Gets the seismic data geometry info.
     *
     * @returns FileLayoutSpecification struct holding the seismic data geometry info.
     *
     **/
    virtual FileLayoutSpecification getFileLayout() const override;

    /**
     * @brief Gets the seismic cube extents calculated during the indexing process based on Geometry3D info.
     *
     * @returns SeismicCubeExtents struct holding the extents calculated during the indexing process.
     *
     **/
    virtual SeismicCubeExtents getGeometry3DExtents() const override;

    /**
     * @brief Gets the seismic cube extents calculated during the indexing process based on FileLayoutSpecification info.
     *
     * @returns SeismicCubeExtents struct holding the extents calculated during the indexing process.
     *
     **/
    virtual SeismicCubeExtents getFileLayoutExtents() const override;

    /**
     * @brief Gets the Line Extents calculated during the indexing process.
     *
     * @returns SeismicLineExtents struct holding the extents calculated during the indexing process.
     *
     **/
    virtual index_2d::SeismicLineExtents getLineExtents() const override;

    /**
     * @brief This method verifies whether the SEG-Y volume is complete or not.
     *
     * @details
     * Returns true if all inlines in the volume are complete and have traces of equal number of samples.
     * If this is the case, a trace's location in the SEG-Y file can easily be calculated by working out the number of traces into the volume you would have to be to find a desired trace.
     * If this is false, the file offset to reach a certain trace will have to be read from a generated index.
     *
     *  @returns bool value indicating whether a SEG-Y volume is complete, i.e. having all traces and each trace is the same length.
     **/
    virtual bool isRegular() const override;

    /**
     * @brief This method verifies whether the SEG-Y volume is 3D.
     *
     * @details
     * Returns true if there is an inline/crossline progression. Works properly only after index is loaded
     *
     *  @returns bool value indicating whether a SEG-Y volume is 3D.
     **/
    virtual bool isSegy3D() const override;

    /**
     * @brief Specify whether SEGY data is 2D or not.
     *
     * Sets the dimensionality flag which tells how SEGY data should be processed.
     **/
    virtual void setSegy2DFlag(bool isSegy2D) override;

    /**
     * @brief Test to verify if the current index representation is valid.
     *
     * @details
     * This returns true if the following conditions are all true:
     * - Inline and Crossline extents are both more than a single trace, and the increment step is greater than 0
     * - There are more than a single sample per trace, and the sample increment is not 0
     * - IsRegular is true OR number of elements in the Index is greater than 0
     *
     **/
    virtual bool isValid() const override;

    /**
     * @brief Gets file offset for a given inline, crossline.
     *
     * @param inlineNo gives the primary index value to get the trace trace offset for
     * @param crosslineNo gives the secondary index value to get the trace trace offset for
     *
     * @returns SeismicCubeExtents struct holding the extents calculated during the indexing process.
     *
     **/
    virtual IndexField getIndexAt(int inlineNo, int crosslineNo) const override;

    /**
     * @brief Gets file offset for a given inline, crossline. If a DeadEmptyMissing trace scan has been run, this info will be used
     *
     * @param inlineNo gives the primary index value to get the trace trace offset for
     * @param crosslineNo gives the secondary index value to get the trace trace offset for
     *
     * @returns SeismicCubeExtents struct holding the extents calculated during the indexing process.
     *
     **/
    virtual IndexField getLiveIndexAt(int inlineNo, int crosslineNo) const override;

    /**
     * @brief Gets file offset for a given Trace Number. Valid only for 2D SEGY
     *
     * @param traceNo gives Trace Number value to get the trace offset for
     *
     * @returns Offset for the given Trace Number.
     *
     **/
    virtual size_t getOffsetForTraceNo(size_t traceNo) const override;

    /**
     * @brief Gets file offset for a given CMP. Valid only for 2D SEGY
     *
     * @param CMP gives CMP value to get the trace offset for
     *
     * @returns Offset for the given CMP.
     *
     **/
    virtual size_t getOffsetForCMP(int CMP) const override;

    /**
     * @brief Gets TraceNo for a given CMP. Valid only for 2D SEGY
     *
     * @param CMP gives CMP value to get the TraceNo for
     * @param findNextClosest makes function to return closest TraceNo for the next CMP if the requested doesn't exist
     *
     * @returns Offset for the given CMP.
     *
     **/
    virtual size_t getTraceNoForCMP(int CMP, bool findNextClosest = false) const override;

    /**
     * @brief Gets the sample size measured in number of bytes of each trace sample.
     *
     * @returns unsigned long integer with the total number of bytes used to store a single sample
     *
     **/
    virtual size_t getSampleSize() const override;

    /**
     * @brief Gets the total size measured in number of bytes of each trace.
     *
     * @returns unsigned long integer with the total number of bytes used to store a complete trace, including the trace header in the SEG-Y file. In cases where this value is not constant, the returned value is true for the first trace in the volume.
     *
     **/
    virtual size_t getTraceSize() const override;

    /**
     * @brief Gets the offset in number of bytes before the start of the first trace of the file.
     *
     * @param fileId identifies the SEGY input file to get the first trace offset for
     *
     * @returns unsigned long integer with the total number of bytes used to store the Textual header, binary header and any extended textual headers.
     *
     **/
    virtual size_t getFirstTraceOffset(uint16_t fileId) const override;

    /**
     * @brief This method determines whether a SEG-Y volume is organized with inlines or crosslines as the primary index.
     *
     *  @returns bool  A value of true signifies that the SEG-Y volume is organized with Inlines as the primary index, a value of false means the volume is organized by Crosslines.
     *
     **/
    virtual bool isInlineOrdered() const override;

    /**
     * @brief Is the coordinate scalar constant for every trace.
     *
     * @returns bool value true indicates same scalar used for every trace.
     *
     **/
    virtual bool hasConstantCoordinateScalar() const override;

    /**
     * @brief Does every trace have the same number of samples.
     *
     * @returns bool value true indicates same sample count for every trace.
     *
     **/
    virtual bool hasConstantTraceLength() const override;

    /**
     * @brief Does every trace have the same trace weighting factor.
     *
     * @returns bool value true indicates same the same trace weighting factor is used for every trace.
     *
     **/
    virtual bool hasConstantTraceWeightingFactor() const override;

    /**
     * @brief Gets the constant coordinate scaling factor.
     *
     * @returns int The constant coordinate scaling factor. A value of 1 is returned if the SEG-Y volume does NOT have a constant coordinate scalar.
     *
     **/
    virtual int getConstantCoordinateScalar() const override;

    /**
     * @brief Gets the constant number of samples per trace.
     *
     * @returns size_t The constant number of samples per trace. A value of 0 is returned if the SEG-Y volume does NOT have a constant number of samples per trace.
     *
     **/
    virtual size_t getConstantTraceLength() const override;

    /**
     * @brief Gets the constant trace weighting factor value.
     *
     * @returns unsigned short The constant trace weighting factor. A value of 1 is returned if the SEG-Y volume does NOT have a constant trace weighting factor.
     *
     **/
    virtual unsigned short getConstantTraceWeightingFactor() const override;

    /**
     * @brief Gets the total number of live traces in the SEG-Y volume.
     *
     * @returns unsigned long integer with the total number of traces stored in the SEG-Y file. If the SEG-Y volume is complete, this is number of inlines x number of crosslines.
     *
     **/
    virtual size_t getIndexedTraceCount() const override;

    /**
     * @brief Is the seismic Time indexed?  
     *
     * @returns bool value true indicates the seismic is TimeIndexed, false means the seismic is Depth indexed.
     *
     **/
    virtual bool isSeismicTimeIndexed() const override;
    
    /**
     * @brief Gets Minimum (or largest negative value) Sample amplitude value
     *
     * @returns float value representing the smallest sample value.
     *
     **/
    virtual float getMinimumAmplitudeValue() const override;

    /**
     * @brief Gets Maximum (or largest positive value) Sample amplitude value
     *
     * @returns float value representing the smallest sample value.
     *
     **/
    virtual float getMaximumAmplitudeValue() const override;

    //
    // below are member functions used for the unit tests back-door
    //

    /**
     * @brief Returns base part for the binary index name
     **/
    std::string getIndexBaseFileName() const;

    /**
     * @brief Overrides the Geometry3D data member
     *
     * @param geometry new value for the Geometry3D data member
     **/
    void setGeometry3D(Geometry3D const& geometry);

    /**
     * @brief Validates input file path names
     *
     * @param inputFileList input file path names to verify
     * @param logDebugOutput if true then produces the verbose output
     *
     * @return the FileNameStatus enum value showing the match status
     **/
    multi_file::FileNameStatus validateIndexedFiles(FileList const& inputFileList, bool logDebugOutput) const;

    /**
     * @brief Reuses the existing files where it is possible based on new input files list
     *
     * @param inputFileList new input files list
     **/
    void reconnectSEGYFilesAfterMove(const std::vector<std::string>& inputFileList);

    /**
     * @brief Processes the collated data to generate binary index data
     *
     * @param indexCollatedDataVector contains the collated data itself
     * @param domain specifies the domain of a SEG-Y volume
     * @param zUoM specifies the Unit of measurement for the sample interval in a SEG-Y volume
     * @param sampleSize specifies the trace sample size in bytes
     * @param forceGenerateIndex if true then the index generation will be forced
     *
     * @return true on success that means the following saving will produce the correct binary index contents
     **/
    bool ProcessIndexBuildingData(std::vector<std::shared_ptr<IndexBuilderCollatedData>> const& indexCollatedDataVector,
                                  segyio::VerticalDomain domain,
                                  segyio::VerticalUOM zUoM,
                                  size_t sampleSize,
                                  bool forceGenerateIndex = false);

    /**
     * @brief Returns the Geometry3D instance from the JSON value
     *
     * @param json JSON value to retrieve the Geometry3D instance from
     *
     * @return the Geometry3D instance
     **/
    static Geometry3D getGeometry3DStructFromJson(Json::Value const& json);

    /**
     * @brief Returns the JSON value from the Geometry3D instance
     *
     * @param g the Geometry3D instance to retrieve the JSON value from
     *
     * @return the Geometry3D instance
     **/
    static Json::Value getGeometry3DAsJson(Geometry3D const& g);

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
     * @return the FileLayoutSpecification instance
     **/
    static Json::Value getFileLayoutAsJson(const FileLayoutSpecification& f);

    /**
     * @brief Indicates whether or not a missing traces index has been generated for the index file).
     *
     * @details Returns false if the missing traces index file cannot be found in the expected location.
     *
     * @return bool value indicating whether an index file was found (true) or not. (false)
     *
     **/
    virtual bool hasMissingTraceIndex() const override;

    /**
     * @brief Indicates whether or not the live trace index has been generated and loaded for the index file).
     *
     * @details Returns false if the missing traces index file cannot be found in the expected location.
     *
     * @return bool value indicating whether an index file was found (true) or not. (false)
     *
     **/
    virtual bool IsLiveTracesIndexLoaded() const override { return isLiveTraceIndexIndexLoaded_; }

    /**
     * @brief Stores dead, empty or missing traces 
     *
     * @param missingTraces Traces missing from input file
     * @param emptyTraces   Traces having zero value for every sample
     * @param deadTraces    Traces having trace header ise-code = dead or use-code = dummy
     *
     * @return code true = successfully saved, false means operation failed
     **/
    bool storeEmptyDeadMissingTraces(std::vector<segysdk::bulkaccess::TraceHeaderResult> missingTraces, 
        const std::vector<segysdk::bulkaccess::TraceHeaderResult>& emptyTraces, 
        const std::vector<segysdk::bulkaccess::TraceHeaderResult>& deadTraces);

    /**
     * @brief if hasMissingTraceIndex() this index will be loaded.
     *
     * @details Returns false if the missing traces index file cannot be loaded.
     *
     * @return bool value indicating whether an index file was successfully loaded (true) or not. (false)
     *
     **/
    virtual void loadMissingTraceIndex(const request::Params& params) override;

    bool traceComparer(const segysdk::bulkaccess::TraceHeaderResult& t1, const segysdk::bulkaccess::TraceHeaderResult& t2);


private:
    void ResetIndexData();

 private:
    SegyIndexImpl(SegyIndexImpl const&) = delete;
    SegyIndexImpl& operator=(SegyIndexImpl const&) = delete;
    SegyIndexImpl(SegyIndexImpl&&) = delete;
    SegyIndexImpl& operator=(SegyIndexImpl&&) = delete;

private:
    index_data::SegyIndexDataPtr indexData_{nullptr};
    index_generator::SegyIndexGenerator indexGenerator_;
    index_storage::SegyIndexStorage indexStorage_;
    trace_info::SegyIndexTraceInfo indexTraceInfo_;
    mutable bool hasIndexFile_ = false;
    mutable bool isLiveTraceIndexIndexLoaded_ = false;
    mutable bool isLoaded_ = false;
};

} // namespace segysdk::segyindex
