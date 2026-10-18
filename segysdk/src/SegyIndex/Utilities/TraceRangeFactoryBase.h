/*
Copyright 2024 Schlumberger

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

#include "../SegyIndex/Utilities/TraceIndexStorage.h"
#include "../SegyIndex/Utilities/TraceRange.h"
#include "../SegyIndex/Utilities/TraceRangeRecord.h"
#include "../SegyIndex/FileLayoutSpecification.h"
#include "../SegyIndex/IndexBuilderCollatedData.h"

#include <cstdint>
#include <memory>

namespace segysdk::segyindex
{
namespace trace_range_map
{

/**
 * @brief struct TraceRangeFactory Implements the trace range items generator that stores the created ranges to the TraceIndexStorage instance
 **/
struct SEGYSDK_DLL_PUBLIC TraceRangeFactoryBase 
{
    /**
     * @brief Constructs the TraceRangeFactory instance
     *
     * @param fileLayout - the geometry information for the entire SEGY data set
     * @param collatedData - the geometry information for an individual input SEGY file which is being processed by the current TraceRangeFactory instance
     * @param outputStorage - the created ranges to be stored to this TraceIndexStorage instance
     **/
    TraceRangeFactoryBase(FileLayoutSpecification fileLayout,
                      TraceIndexStorage& outputStorage);
    virtual ~TraceRangeFactoryBase() = default;

    /**
     * @brief Adds/enumerates the present trace by the key parameters
     *
     * @param primaryIndex - the present trace's primary index
     * @param secondaryIndex - the present trace's secondary index
     * @param sampleCount - the present trace's number of samples
     * @param fileNo - the present trace's input SEGY file ID number
     * @param fileOffset - the present trace's file offset
     **/
    void addTrace(PrimaryIndex primaryIndex,
                  SecondaryIndex secondaryIndex,
                  size_t sampleCount,
                  size_t fileNo,
                  size_t fileOffset);

    // the functions below are to reduce the code duplication for the basic operations
    static PrimaryIndex getPrimaryExtentIndexFirst(FileLayoutSpecification const& fileLayout);
    static PrimaryIndex getPrimaryExtentIndexLast(FileLayoutSpecification const& fileLayout);
    static SecondaryIndex getSecondaryExtentIndexFirst(FileLayoutSpecification const& fileLayout);
    static SecondaryIndex getSecondaryExtentIndexLast(FileLayoutSpecification const& fileLayout);
    static IndexStep getPrimaryExtentStep(FileLayoutSpecification const& fileLayout);
    static IndexStep getSecondaryExtentStep(FileLayoutSpecification const& fileLayout);

private:
    struct PositionTrackParameters
    {
        PrimaryIndex prii;
        SecondaryIndex seci;
        size_t sampleCount;
        size_t fileNo;
        size_t fileOffset;

        PositionTrackParameters();
        void clear();
    };

protected:
    virtual void checkParameters() const = 0;
    virtual void init() = 0;
    void checkVariables() const;
    bool isSameRange() const;
    bool isSamePrimaryIndex() const;
    bool isNextPrimaryIndex() const;
    bool isNextSecondaryIndex() const;
    bool isMissingPrimaryIndex() const;
    bool isMissingSecondaryIndex() const;
    bool isSameSampleCount() const;
    bool isSameFileNo() const;
    bool isSecondaryMissingRemainings() const;

    size_t getPrimaryDistance() const;
    size_t getPrimaryMissingOrigin() const;
    size_t getPrimaryMissingRemainings() const;
    size_t getSecondaryDistance() const;
    size_t getSecondaryMissingOrigin() const;
    size_t getSecondaryMissingRemainings() const;

    void storeCurrentRange();
    void storeMissingRange(PrimaryIndex prii,
                           SecondaryIndex seci,
                           size_t rangeCount);

    void finishLastPrimaryIndex();

    void enumerateRangeItem();
    void enumerateNextRange();

private:
    TraceRangeFactoryBase(TraceRangeFactoryBase const&);
    TraceRangeFactoryBase& operator=(TraceRangeFactoryBase const&);
    TraceRangeFactoryBase(TraceRangeFactoryBase&&);
    TraceRangeFactoryBase& operator=(TraceRangeFactoryBase&&);

protected:
    FileLayoutSpecification fileLayout_;
    TraceIndexStorage& outputStorage_;

    IndexStep const primaryStep_;
    IndexStep const secondaryStep_;

    PrimaryIndex firstPrimaryIndex_;
    PrimaryIndex lastPrimaryIndex_;
    SecondaryIndex firstSecondaryIndex_;
    SecondaryIndex lastSecondaryIndex_;

    size_t primaryDimension_;
    size_t secondaryDimension_;

    PositionTrackParameters posCurr_;
    PositionTrackParameters posPrev_;

    size_t rangeCount_;
};

} // namespace trace_range_map
} // namespace segysdk::segyindex
