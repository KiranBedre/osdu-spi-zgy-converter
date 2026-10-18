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

/** @file
 * @brief SegyIndex.h contains the declaration of the SegyIndex class, used to improve the ability to quickly extract data from anywhere in the volume.
 *
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../SegyIO/SegyFile.h"
#include "../SegyIndex/2DIndexing/SeismicLineExtents.h"
#include "../SegyIndex/FileLayoutSpecification.h"
#include "../SegyIndex/Geometry3D.h"
#include "../SegyIndex/IndexField.h"
#include "../SegyIndex/SeismicCubeExtents.h"

#include <memory>
#include <vector>

namespace segysdk::segyindex
{

using SegyFiles = std::vector<std::shared_ptr<segyio::SegyFile>>;
using OptionalString = std::optional<std::string>;

struct SEGYSDK_DLL_PUBLIC Params : public segyio::StorageAccessParams
{
    Params& withSegyFiles(const SegyFiles& value);
    Params& withSegyFileNames(const Strings& value);
    Params& withIndexBaseFileNameOverride(const OptionalString& value);
    Params& withLegalTag(const OptionalString& value);
    SegyFiles segyFiles_{};
    Strings segyFileNames_{};
    OptionalString indexBaseFileNameOverride_{};
    OptionalString legalTag_{};
};

/**
 * @class SegyIndex
 *
 * @brief Used to improve the ability to quickly extract data from anywhere in the volume.
 *
 * @details
 * The index also holds additional information, such as
 * - Whether or not certain values can be cached rather than extracted from the headers every time they are requested,
 * - The data grid calculated from the trace header coordinates
 * - The min/max extents of inline and crossline numbers
 * - The min/max vertical extent
 *
 **/
class SEGYSDK_DLL_PUBLIC SegyIndex
{
public:
    /**
     * @brief Default destructor for the SegyIndex.
     *
     **/
    virtual ~SegyIndex();

    /**
     * @brief Indicates whether or not an index has been generated for the current SEG-Y file(s).
     *
     * @details Returns false if the index file cannot be found in the expected location.
     *      *
     * @return bool value indicating whether an index file was found (true) or not. (false)
     *
     **/
    virtual bool hasIndexFile(bool storageFileCheck = false) const = 0;

    /**
     * @brief Loads the index file
     *
     * @remarks If the available binary index has the previous format
     *          then the implicit conversion to the latest format takes place
     **/
    virtual void Load() = 0;
    virtual void Load(const request::Params& params) = 0;

    /**
     * @brief Saves the index file
     *
     * @remarks If the available binary index has the previous format
     *          then the implicit conversion to the latest format takes place
     **/
    virtual bool Save() = 0;

    /**
     * @brief Saves the index file in JSON format
     *
     * @param outputFileName file path name to save the JSON formatted index to
     **/
    virtual void SaveAsJson(const std::string& outputFileName) const = 0;

    /**
     * @brief Delete existing index files, if any.
     *
     * @details Deletes the main and binary index files, and invalidates the index data.
     */
    virtual void deleteIndex() = 0;

    /**
     * @brief Scans the SEGY file(s) and generates the trace information
     *
     * @param forceGenerateIndex specifies whether to force the trace information generation for the binary index file
     * @param showDebug specifies whether to produce the verbose output to the console
     *
     * @return true if the trace information generation for the binary index is successful and false otherwise
     *
     **/
    virtual bool GenerateIndex(bool forceGenerateIndex) = 0;

    /**
     * @brief Gets the seismic data geometry info.
     *
     * @returns Geometry3D struct holding the seismic data geometry info.
     *
     **/
    virtual Geometry3D getGeometry3D() const = 0;

    /**
     * @brief Gets the seismic data geometry info.
     *
     * @returns FileLayoutSpecification struct holding the seismic data geometry info.
     *
     **/
    virtual FileLayoutSpecification getFileLayout() const = 0;

    /**
     * @brief Gets the seismic cube extents calculated during the indexing process based on Geometry3D info.
     *
     * @returns SeismicCubeExtents struct holding the extents calculated during the indexing process.
     *
     **/
    virtual SeismicCubeExtents getGeometry3DExtents() const = 0;

    /**
     * @brief Gets the seismic cube extents calculated during the indexing process based on FileLayoutSpecification info.
     *
     * @returns SeismicCubeExtents struct holding the extents calculated during the indexing process.
     *
     **/
    virtual SeismicCubeExtents getFileLayoutExtents() const = 0;

    /**
     * @brief Gets the Line Extents calculated during the indexing process.
     *
     * @returns SeismicLineExtents struct holding the extents calculated during the indexing process.
     *
     **/
    virtual index_2d::SeismicLineExtents getLineExtents() const = 0;

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
    virtual bool isRegular() const = 0;

    /**
     * @brief This method verifies whether the SEG-Y volume is 3D.
     *
     * @details
     * Returns true if there is an inline/crossline progression.
     *
     *  @returns bool value indicating whether a SEG-Y volume is 3D.
     **/
    virtual bool isSegy3D() const = 0;

    /**
     * @brief Specify whether SEGY data is 2D or not.
     *
     * Sets the dimensionality flag which tells how SEGY data should be processed.
     **/
    virtual void setSegy2DFlag(bool isSegy2D) = 0;

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
    virtual bool isValid() const = 0;

    /**

     * @brief Gets file offset for a given inline, crossline.
     *
     * @param inlineNo gives the primary index value to get the trace trace offset for
     * @param crosslineNo gives the secondary index value to get the trace trace offset for
     *
     * @returns SeismicCubeExtents struct holding the extents calculated during the indexing process.
     *
     **/
    virtual IndexField getIndexAt(int inlineNo, int crosslineNo) const = 0;

    /**
     * @brief Gets file offset for a given inline, crossline. If hasMissingTraceIndex(), this info will be used
     *
     * @param inlineNo gives the primary index value to get the trace trace offset for
     * @param crosslineNo gives the secondary index value to get the trace trace offset for
     *
     * @returns SeismicCubeExtents struct holding the extents calculated during the indexing process.
     *
     **/
    virtual IndexField getLiveIndexAt(int inlineNo, int crosslineNo) const = 0;

    /**
     * @brief Indicates whether or not a missing traces index has been generated for the index file).
     *
     * @details Returns false if the missing traces index file cannot be found in the expected location.
     *
     * @return bool value indicating whether an index file was found (true) or not. (false)
     *
     **/
    virtual bool hasMissingTraceIndex() const = 0;

    /**
     * @brief if hasMissingTraceIndex() this index will be loaded.
     *
     * @details Returns false if the missing traces index file cannot be loaded.
     *
     * @return bool value indicating whether an index file was successfully loaded (true) or not. (false)
     *
     **/
    virtual void loadMissingTraceIndex(const request::Params& params) = 0;

    /**
     * @brief Indicates whether or not a live traces index has been generated and explicitly loaded for the index file).
     *
     * @details Returns false if the missing traces index file cannot be found in the expected location.
     *
     * @return bool value indicating whether an index file was found (true) or not. (false)
     *
     **/
    virtual bool IsLiveTracesIndexLoaded() const = 0;

    /**
     * @brief Gets file offset for a given Trace Number. Valid only for 2D SEGY
     *
     * @param traceNo gives Trace Number value to get the trace offset for
     *
     * @returns Offset for the given Trace Number.
     *
     **/
    virtual size_t getOffsetForTraceNo(size_t traceNo) const = 0;

    /**
     * @brief Gets file offset for a given CMP. Valid only for 2D SEGY
     *
     * @param CMP gives CMP value to get the trace offset for
     *
     * @returns Offset for the given CMP.
     *
     **/
    virtual size_t getOffsetForCMP(int CMP) const = 0;

    /**
     * @brief Gets TraceNo for a given CMP. Valid only for 2D SEGY
     *
     * @param CMP gives CMP value to get the TraceNo for
     * @param findNextClosest makes function to return closest TraceNo for the next CMP if the requested doesn't exist
     *
     * @returns Offset for the given CMP.
     *
     **/
    virtual size_t getTraceNoForCMP(int CMP, bool findNextClosest = false) const = 0;

    /**
     * @brief Gets the sample size measured in number of bytes of each trace sample.
     *
     * @returns unsigned long integer with the total number of bytes used to store a single sample
     *
     **/
    virtual size_t getSampleSize() const = 0;

    /**
     * @brief Gets the total size measured in number of bytes of each trace.
     *
     * @returns unsigned long integer with the total number of bytes used to store a complete trace, including the trace header in the SEG-Y file. In cases where this value is not constant, the returned value is true for the first trace in the volume.
     *
     **/
    virtual size_t getTraceSize() const = 0;

    /**
     * @brief Gets the offset in number of bytes before the start of the first trace of the file.
     *
     * @param fileId identifies the SEGY input file to get the first trace offset for
     *
     * @returns unsigned long integer with the total number of bytes used to store the Textual header, binary header and any extended textual headers.
     *
     **/
    virtual size_t getFirstTraceOffset(uint16_t fileId) const = 0;

    /**
     * @brief This method determines whether a SEG-Y volume is organized with inlines or crosslines as the primary index.
     *
     *  @returns bool  A value of true signifies that the SEG-Y volume is organized with Inlines as the primary index, a value of false means the volume is organized by Crosslines.
     *
     **/
    virtual bool isInlineOrdered() const = 0;

    /**
     * @brief Is the coordinate scalar constant for every trace.
     *
     * @returns bool value true indicates same scalar used for every trace.
     *
     **/
    virtual bool hasConstantCoordinateScalar() const = 0;

    /**
     * @brief Does every trace have the same number of samples.
     *
     * @returns bool value true indicates same sample count for every trace.
     *
     **/
    virtual bool hasConstantTraceLength() const = 0;

    /**
     * @brief Does every trace have the same trace weighting factor.
     *
     * @returns bool value true indicates same the same trace weighting factor is used for every trace.
     *
     **/
    virtual bool hasConstantTraceWeightingFactor() const = 0;

    /**
     * @brief Gets the constant coordinate scaling factor.
     *
     * @returns int The constant coordinate scaling factor. A value of 1 is returned if the SEG-Y volume does NOT have a constant coordinate scalar.
     *
     **/
    virtual int getConstantCoordinateScalar() const = 0;

    /**
     * @brief Gets the constant number of samples per trace.
     *
     * @returns size_t The constant number of samples per trace. A value of 0 is returned if the SEG-Y volume does NOT have a constant number of samples per trace.
     *
     **/
    virtual size_t getConstantTraceLength() const = 0;

    /**
     * @brief Gets the constant trace weighting factor value.
     *
     * @returns unsigned short The constant trace weighting factor. A value of 1 is returned if the SEG-Y volume does NOT have a constant trace weighting factor.
     *
     **/
    virtual unsigned short getConstantTraceWeightingFactor() const = 0;

    /**
     * @brief Gets Minimum (or largest negative value) Sample amplitude value
     *
     * @returns float value representing the smallest sample value.
     *
     **/
    virtual float getMinimumAmplitudeValue() const = 0;

    /**
     * @brief Gets Maximum (or largest positive value) Sample amplitude value
     *
     * @returns float value representing the smallest sample value.
     *
     **/
    virtual float getMaximumAmplitudeValue() const = 0;
    /**
     * @brief Gets the total number of live traces in the SEG-Y volume.
     *
     * @returns unsigned long integer with the total number of traces stored in the SEG-Y file. If the SEG-Y volume is complete, this is number of inlines x number of crosslines.
     *
     **/
    virtual size_t getIndexedTraceCount() const = 0;

    /**
     * @brief gets if seismic data is time indexed or depth indexed.
     *
     * @returns if seismic data is time indexed (true) or depth indexed(false).
     *
     **/
    virtual bool isSeismicTimeIndexed() const = 0;
};

} // namespace segysdk::segyindex
