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

#include "../SegyIndex/Geometry3D.h"
#include "../SegyIndex/SegyIndexData.h"
#include "../SegyIndex/IndexField.h"

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>

namespace segysdk::segyindex
{
namespace trace_info
{
using SegyIndexDataPtr = index_data::SegyIndexDataPtr;

/**
 * @brief SegyIndexTraceInfo struct gives the trace information
 **/
struct SegyIndexTraceInfo final
{
    /**
     * @brief Constructs SegyIndexTraceInfo struct instance
     *
     * @param indexData gives the shared pointer to the index base data instance the trace information is obtained from
     *
     **/
    SegyIndexTraceInfo(SegyIndexDataPtr& indexData);

    /**
     * @brief Gets the seismic data geometry info.
     *
     * @returns Geometry3D struct holding the seismic data geometry info.
     *
     **/
    Geometry3D getGeometry3D() const;

    /**
     * @brief Gets the seismic data geometry info.
     *
     * @returns FileLayoutSpecification struct holding the seismic data geometry info.
     *
     **/
    FileLayoutSpecification getFileLayout() const;

    /**
     * @brief Gets the seismic cube extents calculated during the indexing process on Geometry3D info.
     *
     * @returns SeismicCubeExtents struct holding the extents calculated during the indexing process.
     *
     **/
    SeismicCubeExtents getGeometry3DExtents() const;


    /**
     * @brief Gets the seismic cube extents calculated during the indexing process based on FileLayoutSpecification info.
     *
     * @returns SeismicCubeExtents struct holding the extents calculated during the indexing process.
     *
     **/
    SeismicCubeExtents getFileLayoutExtents() const;

    /**
     * @brief Gets the Line Extents calculated during the indexing process.
     *
     * @returns SeismicLineExtents struct holding the extents calculated during the indexing process.
     *
     **/
    index_2d::SeismicLineExtents getLineExtents() const;

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
    bool isRegular() const;

    /**
     * @brief Gets file offset for a given inline, crossline.
     *
     * @param inlineNo gives the primary index value to get the trace trace offset for
     * @param crosslineNo gives the secondary index value to get the trace trace offset for
     *
     * @returns IndexField struct holding the offset for the requested trace.
     *
     **/
    IndexField getIndexAt(int inlineNo, int crosslineNo) const;

     /**
     * @brief Gets file offset for a given inline, crossline. If a DeadEmptyMissing trace scan has been run, this info will be used
     *
     * @param inlineNo gives the primary index value to get the trace trace offset for
     * @param crosslineNo gives the secondary index value to get the trace trace offset for
     *
      * @returns IndexField struct holding the offset for the requested trace.
     *
     **/
    IndexField getLiveIndexAt(int inlineNo, int crosslineNo) const;

    /**
     * @brief Gets file offset for a given Trace Number. Valid only for 2D SEGY
     *
     * @param traceNo gives Trace Number value to get the trace offset for 
     *
     * @returns Offset for the given Trace Number.
     *
     **/
    size_t getOffsetForTraceNo(size_t traceNo) const;

    /**
     * @brief Gets file offset for a given CMP. Valid only for 2D SEGY
     *
     * @param CMP gives CMP value to get the trace offset for 
     *
     * @returns Offset for the given CMP.
     *
     **/
    size_t getOffsetForCMP(int CMP) const;

    /**
     * @brief Gets TraceNo for a given CMP. Valid only for 2D SEGY
     *
     * @param CMP gives CMP value to get the TraceNo for 
     * @param findNextClosest makes function to return closest TraceNo for the next CMP if the requested doesn't exist 
     *
     * @returns Offset for the given CMP.
     *
     **/
    size_t getTraceNoForCMP(int CMP, bool findNextClosest = false) const;

    /**
     * @brief Gets the trace size measured in number of bytes per sample.
     *
     * @returns unsigned long integer with the total number of bytes used to store a single sample
     *
     **/
    size_t getSampleSize() const;

    /**
     * @brief Gets the total size measured in number of bytes of each trace.
     *
     * @returns unsigned long integer with the total number of bytes used to store a complete trace, including the trace header in the SEG-Y file. In cases where this value is not constant, the returned value is true for the first trace in the volume.
     *
     **/
    size_t getTraceSize() const;

    /**
     * @brief Gets minimum trace amplitude value for the volume or file.
     *
     * @returns float with the minimum amplitude value.
     *
     **/
    float getMinimumAmplitude() const;

    /**
     * @brief Gets maximum trace amplitude value for the volume or file.
     *
     * @returns float with the maximum amplitude value.
     *
     **/
    float getMaximumAmplitude() const;

    /**
     * @brief Gets the offset in number of bytes before the start of the first trace of the file.
     *
     * @param fileId identifies the SEGY input file to get the first trace offset for
     *
     * @returns unsigned long integer with the total number of bytes used to store the Textual header, binary header and any extended textual headers.
     *
     **/
    size_t getFirstTraceOffset(uint16_t fileId) const;

    /**
     * @brief This method determines whether a SEG-Y volume is organized with inlines or crosslines as the primary index.
     *
     *  @returns bool  A value of true signifies that the SEG-Y volume is organized with Inlines as the primary index, a value of false means the volume is organized by Crosslines.
     *
     **/
    bool isInlineOrdered() const;

    /**
     * @brief Is the coordinate scalar constant for every trace.
     *
     * @returns bool value true indicates same scalar used for every trace.
     *
     **/
    bool hasConstantCoordinateScalar() const;

    /**
     * @brief Does every trace have the same number of samples.
     *
     * @returns bool value true indicates same sample count for every trace.
     *
     **/
    bool hasConstantTraceLength() const;

    /**
     * @brief Does every trace have the same trace weighting factor.
     *
     * @returns bool value true indicates same the same trace weighting factor is used for every trace.
     *
     **/
    bool hasConstantTraceWeightingFactor() const;

    /**
     * @brief Gets the constant coordinate scaling factor.
     *
     * @returns int The constant coordinate scaling factor. A value of 1 is returned if the SEG-Y volume does NOT have a constant coordinate scalar.
     *
     **/
    int getConstantCoordinateScalar() const;

    /**
     * @brief Gets the constant number of samples per trace.
     *
     * @returns size_t The constant number of samples per trace. A value of 0 is returned if the SEG-Y volume does NOT have a constant number of samples per trace.
     *
     **/
    size_t getConstantTraceLength() const;

    /**
     * @brief Gets the constant trace weighting factor value.
     *
     * @returns unsigned short The constant trace weighting factor. A value of 1 is returned if the SEG-Y volume does NOT have a constant trace weighting factor.
     *
     **/
    unsigned short getConstantTraceWeightingFactor() const;

    /**
     * @brief Gets the total number of live traces in the SEG-Y volume.
     *
     * @returns unsigned long integer with the total number of traces stored in the SEG-Y file. If the SEG-Y volume is complete, this is number of inlines x number of crosslines.
     *
     **/
    size_t getIndexedTraceCount() const;

private:
    IndexField getCalculatedIndexAt(int inlineNo, int crosslineNo) const;
    uint16_t getIndexFileNo(int primaryIndex) const;
    size_t calculateTraceOffsetCount(uint16_t fileNo, int inlineNo, int crosslineNo) const;
    size_t getInlinesToRequestedInLine(uint16_t fileNo, int inlineNo) const;
    size_t getCrosslinesToRequestedCrossline(uint16_t fileNo, int crosslineNo) const;

private:
    SegyIndexTraceInfo(SegyIndexTraceInfo const&) = delete;
    SegyIndexTraceInfo& operator=(SegyIndexTraceInfo const&) = delete;
    SegyIndexTraceInfo(SegyIndexTraceInfo&&) = delete;
    SegyIndexTraceInfo& operator=(SegyIndexTraceInfo&&) = delete;

private:
    SegyIndexDataPtr indexData_{nullptr};
};

} // namespace trace_info
} // namespace segysdk::segyindex
