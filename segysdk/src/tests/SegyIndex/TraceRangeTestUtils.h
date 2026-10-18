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

#include "../SegyIO/Primitives.h"
#include "../SegyIndex/FileLayoutSpecification.h"
#include "../SegyIndex/IndexBuilderCollatedData.h"
#include "../SegyIndex/IndexBuilderData.h"
#include "../SegyIndex/Utilities/TraceIndexMap.h"
#include "../SegyIndex/Utilities/TraceIndexStorage.h"
#include "../SegyIndex/Utilities/TraceRange.h"
#include "../SegyIndex/Utilities/TraceRangeFactory.h"
#include "../SegyIndex/Utilities/TraceRangeRecord.h"
#include "../SegyIO/SampleFormat.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <vector>

namespace segysdk::segyindex::tests
{
using segysdk::segyindex::trace_range_map::PrimaryIndex;
using segysdk::segyindex::trace_range_map::SecondaryIndex;
using segysdk::segyindex::trace_range_map::TraceIndexMap;
using segysdk::segyindex::trace_range_map::TraceIndexMapKey;
using segysdk::segyindex::trace_range_map::TraceIndexStorage;
using segysdk::segyindex::trace_range_map::TraceRange;
using segysdk::segyindex::trace_range_map::TraceRangeFactory;
using segysdk::segyio::Primitives;
using segysdk::segyio::SampleFormat;

struct TraceRangeTester
{
    static constexpr size_t const primaryDimension_ = 11U;
    static constexpr size_t const secondaryDimension_ = 5U;

    // Note: the tester is designed to support/simulate various index steps
    // The only restriction is to specify the _positive_ 'defaultIndexStep_' value
    static constexpr int const defaultIndexStep_ = 1;

    static constexpr int const primaryIndexPositiveStart_ = 10;
    static constexpr int const primaryIndexNegativeStart_ = -11;
    static constexpr int const secondaryIndexPositiveStart_ = 5;
    static constexpr int const secondaryIndexNegativeStart_ = -5;

    static constexpr size_t const traceMatrixPower_ = primaryDimension_ * secondaryDimension_;

    static constexpr size_t const rangeCountMax_ = 18U;
    static constexpr size_t const defaultFileNo_ = 7U;

    static constexpr SampleFormat const defaultSampleFormat_ = SampleFormat::IeeeFloat4;
    static constexpr size_t const defaultSampleCount_ = 100U;
    static constexpr size_t const firstTraceOffset_ = 100U;
    static constexpr size_t const offsetPerTrace_ = Primitives::getTraceSize(defaultSampleFormat_, defaultSampleCount_);

    static constexpr size_t const sizeOfIbd_One_ = 22U;
    static constexpr size_t const sizeOfIbd_Two_ = 33U;

    static constexpr size_t const sizeOfMissingRanges_One_ = 3U + 6U;
    static constexpr size_t const sizeOfMissingRanges_Two_ = 11U;
    static constexpr size_t const sizeOfMissingRanges_Three_ = 5U + 2U * 6U;
    static constexpr size_t const sizeOfMissingRanges_Four_ = 2U * 5U + 6U;

    static constexpr size_t const numberOfLiveTraces_One_ = 34;
    static constexpr size_t const numberOfLiveTraces_Two_ = traceMatrixPower_ - numberOfLiveTraces_One_;
    static constexpr size_t const numberOfLiveTraces_Three_ = 27;
    static constexpr size_t const numberOfLiveTraces_Four_ = traceMatrixPower_ - numberOfLiveTraces_Three_;

    using TraceAvaliabilityArray = std::array<bool, traceMatrixPower_>;
    using RangeCountArray = std::array<size_t, rangeCountMax_>;
    using PrimaryIndexArray = std::array<PrimaryIndex, primaryDimension_>;
    using SecondaryIndexArray = std::array<SecondaryIndex, secondaryDimension_>;

    // the matrix to the linear index
    static size_t getMI(size_t i, size_t j);

    // the trace availability
    static bool getTA_One(size_t i, size_t j);
    static bool getTA_Two(size_t i, size_t j);

    // the ranges/files availability
    static bool getRA_One(size_t i, size_t j);
    static bool getRA_Two(size_t i, size_t j);
    static bool getRA_Three(size_t i, size_t j);
    static bool getRA_Four(size_t i, size_t j);

    // the input file number
    static size_t getFN_One(size_t i, size_t j);
    static size_t getFN_Two(size_t i, size_t j);

    // the trace samples count
    static size_t getSC_One(size_t i, size_t j);
    static size_t getSC_Two(size_t i, size_t j);

    // the trace offset in the file
    static size_t getTO_One(size_t i, size_t j);
    static size_t getTO_Two(size_t i, size_t j);

    // the trace range count
    static size_t getRC(size_t lineNo);

    // the index step
    static int getStepP();
    static int getStepN();

    // the range primary index
    static PrimaryIndex getPriPInc(size_t lineNo);
    static PrimaryIndex getPriNInc(size_t lineNo);
    static PrimaryIndex getPriPDec(size_t lineNo);
    static PrimaryIndex getPriNDec(size_t lineNo);

    // the range secondary index
    static SecondaryIndex getSecPInc(size_t lineNo);
    static SecondaryIndex getSecNInc(size_t lineNo);
    static SecondaryIndex getSecPDec(size_t lineNo);
    static SecondaryIndex getSecNDec(size_t lineNo);

    // a vector of the index builder data
    static std::vector<IndexBuilderData>
    getIBD_One(PrimaryIndex prii0, PrimaryIndex priStep, SecondaryIndex seci0, SecondaryIndex secStep);
    static std::vector<IndexBuilderData>
    getIBD_Two(PrimaryIndex prii0, PrimaryIndex priStep, SecondaryIndex seci0, SecondaryIndex secStep);

    // a Geometry3D
    static FileLayoutSpecification
    getG3D(PrimaryIndex pri0, PrimaryIndex priN, PrimaryIndex priStep,
           SecondaryIndex sec0, SecondaryIndex secN, SecondaryIndex secStep,
           bool isInlineOrdered);

    // the collated data on per file basis
    static std::shared_ptr<IndexBuilderCollatedData>
    getCollatedData_One(PrimaryIndex pri0, PrimaryIndex priN,
                        SecondaryIndex sec0, SecondaryIndex secN,
                        bool isInlineOrdered);
    static std::shared_ptr<IndexBuilderCollatedData>
    getCollatedData_Two(PrimaryIndex pri0, PrimaryIndex priN,
                        SecondaryIndex sec0, SecondaryIndex secN,
                        bool isInlineOrdered);

    // the trace range storage with the predefined contents
    static trace_range_map::TraceIndexStorage const&
    getTisPositiveIncIncIndexes_One();
    static trace_range_map::TraceIndexStorage const&
    getTisPositiveIncIncIndexes_Two();

    static trace_range_map::TraceIndexStorage const&
    getTisPositiveIncDecIndexes_One();
    static trace_range_map::TraceIndexStorage const&
    getTisPositiveIncDecIndexes_Two();

    static trace_range_map::TraceIndexStorage const&
    getTisPositiveDecIncIndexes_One();
    static trace_range_map::TraceIndexStorage const&
    getTisPositiveDecIncIndexes_Two();

    static trace_range_map::TraceIndexStorage const&
    getTisPositiveDecDecIndexes_One();
    static trace_range_map::TraceIndexStorage const&
    getTisPositiveDecDecIndexes_Two();

    static trace_range_map::TraceIndexStorage const&
    getTisNegativeIncIncIndexes_One();
    static trace_range_map::TraceIndexStorage const&
    getTisNegativeIncIncIndexes_Two();

    static trace_range_map::TraceIndexStorage const&
    getTisNegativeIncDecIndexes_One();
    static trace_range_map::TraceIndexStorage const&
    getTisNegativeIncDecIndexes_Two();

    static trace_range_map::TraceIndexStorage const&
    getTisNegativeDecIncIndexes_One();
    static trace_range_map::TraceIndexStorage const&
    getTisNegativeDecIncIndexes_Two();

    static trace_range_map::TraceIndexStorage const&
    getTisNegativeDecDecIndexes_One();
    static trace_range_map::TraceIndexStorage const&
    getTisNegativeDecDecIndexes_Two();

    static trace_range_map::TraceIndexStorage const&
    getTisRangesAvailability_One();
    static trace_range_map::TraceIndexStorage const&
    getTisRangesAbsence_One();
    static trace_range_map::TraceIndexStorage const&
    getTisRangesAvailability_Two();
    static trace_range_map::TraceIndexStorage const&
    getTisRangesAbsence_Two();
    static trace_range_map::TraceIndexStorage const&
    getTisRangesAvailability_Three();
    static trace_range_map::TraceIndexStorage const&
    getTisRangesAbsence_Three();
    static trace_range_map::TraceIndexStorage const&
    getTisRangesAvailability_Four();
    static trace_range_map::TraceIndexStorage const&
    getTisRangesAbsence_Four();

    // the unit test for the TraceIndexMap class
    static void testFillAndReadIndexMap(TraceIndexStorage const& tis_const,
                                        TraceIndexMapKey::Order orderPri,
                                        TraceIndexMapKey::Order orderSeci,
                                        std::function<bool(size_t, size_t)> getTA,
                                        std::function<PrimaryIndex(size_t)> getPriIndex,
                                        std::function<SecondaryIndex(size_t)> getSecIndex,
                                        std::function<size_t(size_t, size_t)> getTO,
                                        int stepPri, int stepSec,
                                        SampleFormat sampleFormat);

    // the unit test for the TraceIndexStorage class
    static void testFillTraceStorage(TraceIndexStorage const& tis_const,
                                     std::function<std::shared_ptr<IndexBuilderCollatedData>(PrimaryIndex, PrimaryIndex, SecondaryIndex, SecondaryIndex, bool)> getCollatedData,
                                     std::function<std::vector<IndexBuilderData>(PrimaryIndex, PrimaryIndex, SecondaryIndex, SecondaryIndex)> getIBD,
                                     std::function<PrimaryIndex(size_t)> getPriIndex,
                                     std::function<SecondaryIndex(size_t)> getSecIndex,
                                     int stepPri, int stepSec, size_t sizeOfIbd);

    // the unit test simulates loading of the trace information from the binary index file
    static void testDetectTraceRangeMapGaps(TraceIndexStorage const& tis_const,
                                            TraceIndexStorage const& tis_missing_const,
                                            std::function<bool(size_t, size_t)> getRA,
                                            std::function<PrimaryIndex(size_t)> getPriIndex,
                                            std::function<SecondaryIndex(size_t)> getSecIndex,
                                            int step, size_t sizeOfMissingRanges,
                                            size_t numberOfLiveTraces);

private:
    static PrimaryIndexArray
    getPrimaryIndexArray(int pri0, int step);
    static SecondaryIndexArray
    getSecondaryIndexArray(int sec0, int step);
    static trace_range_map::TraceIndexStorage
    getTisTA(std::function<bool(size_t, size_t)> getTA,
             std::function<PrimaryIndex(size_t)> getPri,
             std::function<PrimaryIndex(size_t)> getSec,
             std::function<size_t(size_t, size_t)> getFN,
             std::function<size_t(size_t, size_t)> getSC,
             std::function<size_t(size_t)> getRC,
             std::function<size_t(size_t, size_t)> getTO);

private:
    //
    // Traces availability #1
    //
    // 0 1 1 1 0
    // 1 0 0 0 0
    // 1 0 0 0 1
    // 0 0 1 1 1
    // 1 1 1 0 0
    // 0 0 0 0 0
    // 0 0 0 0 0
    // 0 0 0 0 0
    // 1 1 1 1 1
    // 1 1 1 1 1
    // 0 0 0 0 0
    //

    static TraceAvaliabilityArray const traceAvailability_One_;

    //
    // Traces availability #2
    //
    // 1 0 0 0 1
    // 0 1 1 1 1
    // 0 1 1 1 0
    // 1 1 0 0 0
    // 0 0 0 1 1
    // 1 1 1 1 1
    // 1 1 1 1 1
    // 1 1 1 1 1
    // 0 0 0 0 0
    // 0 0 0 0 0
    // 1 1 1 1 1
    //

    static TraceAvaliabilityArray const traceAvailability_Two_;

    //
    // Traces range count
    //
    // 1 3 1
    // 1 4
    // 1 3 1
    // 2 3
    // 3 2
    // 4
    // 4
    // 4
    // 4
    // 4
    // 4
    //

    static RangeCountArray const rangeCount_;

    //
    // Primary index
    //
    // 10 .. 20
    //
    static PrimaryIndexArray const primaryIndexPositiveIncreasing_;

    //
    // Primary index
    //
    // -11 .. -1
    //
    static PrimaryIndexArray const primaryIndexNegativeIncreasing_;

    //
    // Primary index
    //
    // 10 .. 0
    //
    static PrimaryIndexArray const primaryIndexPositiveDecreasing_;

    //
    // Primary index
    //
    // -11 .. -21
    //
    static PrimaryIndexArray const primaryIndexNegativeDecreasing_;

    //
    // Secondary index
    //
    // 5 .. 9
    //
    static SecondaryIndexArray const secondaryIndexPositiveIncreasing_;

    //
    // Secondary index
    //
    // -5 .. -1
    //
    static SecondaryIndexArray const secondaryIndexNegativeIncreasing_;

    //
    // Secondary index
    //
    // 5 .. 1
    //
    static SecondaryIndexArray const secondaryIndexPositiveDecreasing_;

    //
    // Secondary index
    //
    // -5 .. -9
    //
    static SecondaryIndexArray const secondaryIndexNegativeDecreasing_;

    //
    // Trace indexes matrix #1
    // Positive Increasing/Increasing
    //
    // (10,5)  (10,6)  ..  (10,9)
    // (11,5)  (11,6)  ..  (11,9)
    // ..      ..      ..  ..
    // (20,5)  (20,6)  ..  (20,9)
    //

    static trace_range_map::TraceIndexStorage const tis_positive_inc_inc_indexes_One_;

    //
    // Trace indexes matrix #2
    // Positive Increasing/Increasing
    //
    // (10,5)  (10,6)  ..  (10,9)
    // (11,5)  (11,6)  ..  (11,9)
    // ..      ..      ..  ..
    // (20,5)  (20,6)  ..  (20,9)
    //

    static trace_range_map::TraceIndexStorage const tis_positive_inc_inc_indexes_Two_;

    //
    // Trace indexes matrix #1
    // Positive Increasing/Decreasing
    //
    // (10,5)  (10,4)  ..  (10,1)
    // (11,5)  (11,4)  ..  (11,1)
    // ..      ..      ..  ..
    // (20,5)  (20,4)  ..  (20,1)
    //

    static trace_range_map::TraceIndexStorage const tis_positive_inc_dec_indexes_One_;

    //
    // Trace indexes matrix #2
    // Positive Increasing/Decreasing
    //
    // (10,5)  (10,4)  ..  (10,1)
    // (11,5)  (11,4)  ..  (11,1)
    // ..      ..      ..  ..
    // (20,5)  (20,4)  ..  (20,1)
    //

    static trace_range_map::TraceIndexStorage const tis_positive_inc_dec_indexes_Two_;

    //
    // Trace indexes matrix #1
    // Positive Decreasing/Increasing
    //
    // (10,5)  (10,6)  ..  (10,9)
    // ( 9,5)  ( 9,6)  ..  ( 9,9)
    // ..      ..      ..  ..
    // ( 0,5)  ( 0,6)  ..  ( 0,9)
    //

    static trace_range_map::TraceIndexStorage const tis_positive_dec_inc_indexes_One_;

    //
    // Trace indexes matrix #2
    // Positive Decreasing/Increasing
    //
    // (10,5)  (10,6)  ..  (10,9)
    // ( 9,5)  ( 9,6)  ..  ( 9,9)
    // ..      ..      ..  ..
    // ( 0,5)  ( 0,6)  ..  ( 0,9)
    //

    static trace_range_map::TraceIndexStorage const tis_positive_dec_inc_indexes_Two_;

    //
    // Trace indexes matrix #1
    // Positive Decreasing/Decreasing
    //
    // (10,5)  (10,4)  ..  (10,1)
    // ( 9,5)  ( 9,4)  ..  ( 9,1)
    // ..      ..      ..  ..
    // ( 0,5)  ( 0,4)  ..  ( 0,1)
    //

    static trace_range_map::TraceIndexStorage const tis_positive_dec_dec_indexes_One_;

    //
    // Trace indexes matrix #2
    // Positive Decreasing/Decreasing
    //
    // (10,5)  (10,4)  ..  (10,1)
    // ( 9,5)  ( 9,4)  ..  ( 9,1)
    // ..      ..      ..  ..
    // ( 0,5)  ( 0,4)  ..  ( 0,1)
    //

    static trace_range_map::TraceIndexStorage const tis_positive_dec_dec_indexes_Two_;

    //
    // Trace indexes matrix #1
    // Negative Increasing/Increasing
    //
    // (-11, -5)  (-11, -4) .. (-11, -1)
    // (-10, -5)  (-10, -4) .. (-10, -1)
    // ..         ..        .. ..
    // (-1,  -5)  (-1,  -4) .. (-1,  -1)
    //

    static trace_range_map::TraceIndexStorage const tis_negative_inc_inc_indexes_One_;

    //
    // Trace indexes matrix #2
    // Negative Increasing/Increasing
    //
    // (-11, -5)  (-11, -4) .. (-11, -1)
    // (-10, -5)  (-10, -4) .. (-10, -1)
    // ..         ..        .. ..
    // (-1,  -5)  (-1,  -4) .. (-1,  -1)
    //

    static trace_range_map::TraceIndexStorage const tis_negative_inc_inc_indexes_Two_;

    //
    // Trace indexes matrix #1
    // Negative Increasing/Decreasing
    //
    // (-11, -5)  (-11, -6) .. (-11, -9)
    // (-10, -5)  (-10, -6) .. (-10, -9)
    // ..         ..        .. ..
    // (-1,  -5)  (-1,  -6) .. (-1,  -9)
    //

    static trace_range_map::TraceIndexStorage const tis_negative_inc_dec_indexes_One_;

    //
    // Trace indexes matrix #2
    // Negative Increasing/Decreasing
    //
    // (-11, -5)  (-11, -6) .. (-11, -9)
    // (-10, -5)  (-10, -6) .. (-10, -9)
    // ..         ..        .. ..
    // (-1,  -5)  (-1,  -6) .. (-1,  -9)
    //

    static trace_range_map::TraceIndexStorage const tis_negative_inc_dec_indexes_Two_;

    //
    // Trace indexes matrix #1
    // Negative Decreasing/Increasing
    //
    // (-11, -5)  (-11, -4) .. (-11, -1)
    // (-12, -5)  (-12, -4) .. (-12, -1)
    // ..         ..        .. ..
    // (-20, -5)  (-20, -4) .. (-20, -1)
    //

    static trace_range_map::TraceIndexStorage const tis_negative_dec_inc_indexes_One_;

    //
    // Trace indexes matrix #2
    // Negative Decreasing/Increasing
    //
    // (-11, -5)  (-11, -4) .. (-11, -1)
    // (-12, -5)  (-12, -4) .. (-12, -1)
    // ..         ..        .. ..
    // (-20, -5)  (-20, -4) .. (-20, -1)
    //

    static trace_range_map::TraceIndexStorage const tis_negative_dec_inc_indexes_Two_;

    //
    // Trace indexes matrix #1
    // Negative Decreasing/Decreasing
    //
    // (-11, -5)  (-11, -6) .. (-11, -9)
    // (-12, -5)  (-12, -6) .. (-12, -9)
    // ..         ..        .. ..
    // (-20, -5)  (-20, -6) .. (-20, -9)
    //

    static trace_range_map::TraceIndexStorage const tis_negative_dec_dec_indexes_One_;

    //
    // Trace indexes matrix #2
    // Negative Decreasing/Decreasing
    //
    // (-11, -5)  (-11, -6) .. (-11, -9)
    // (-12, -5)  (-12, -6) .. (-12, -9)
    // ..         ..        .. ..
    // (-20, -5)  (-20, -6) .. (-20, -9)
    //

    static trace_range_map::TraceIndexStorage const tis_negative_dec_dec_indexes_Two_;

    //
    // Ranges/files availability #1
    //
    //    0 1 2 3 4
    //    - - - - -
    // 0: 1 1 x x x
    // 1: 1 1 x x x
    // 2: 1 1 x x x
    // 3: 1 1 1 1 1
    // 4: 1 1 1 1 1
    // 5: x x 1 1 1
    // 6: x x 1 1 1
    // 7: x x 1 1 1
    // 8: x x 1 1 1
    // 9: x x 1 1 1
    // 10:x x 1 1 1
    //

    static TraceAvaliabilityArray const rangeAvailability_One_;
    static trace_range_map::TraceIndexStorage const tis_ranges_availability_One_;
    static trace_range_map::TraceIndexStorage const tis_ranges_absence_One_;

    //
    // Ranges/files availability #2
    //
    //    0 1 2 3 4
    //    - - - - -
    // 0: x x 1 1 1
    // 1: x x 1 1 1
    // 2: x x 1 1 1
    // 3: x x x x x
    // 4: x x x x x
    // 5: 1 1 x x x
    // 6: 1 1 x x x
    // 7: 1 1 x x x
    // 8: 1 1 x x x
    // 9: 1 1 x x x
    // 10:1 1 x x x
    //

    static TraceAvaliabilityArray const rangeAvailability_Two_;
    static trace_range_map::TraceIndexStorage const tis_ranges_availability_Two_;
    static trace_range_map::TraceIndexStorage const tis_ranges_absence_Two_;

    //
    // Ranges/files availability #3
    //
    //    0 1 2 3 4
    //    - - - - -
    // 0: 1 1 x x 1
    // 1: 1 1 x x 1
    // 2: 1 1 x x 1
    // 3: 1 1 x x 1
    // 4: 1 1 x x 1
    // 5: x x 1 1 x
    // 6: x x 1 1 x
    // 7: x x 1 1 x
    // 8: x x 1 1 x
    // 9: x x 1 1 x
    // 10:x x 1 1 x
    //

    static TraceAvaliabilityArray const rangeAvailability_Three_;
    static trace_range_map::TraceIndexStorage const tis_ranges_availability_Three_;
    static trace_range_map::TraceIndexStorage const tis_ranges_absence_Three_;

    //
    // Ranges/files availability #4
    //
    //    0 1 2 3 4
    //    - - - - -
    // 0: x x 1 1 x
    // 1: x x 1 1 x
    // 2: x x 1 1 x
    // 3: x x 1 1 x
    // 4: x x 1 1 x
    // 5: 1 1 x x 1
    // 6: 1 1 x x 1
    // 7: 1 1 x x 1
    // 8: 1 1 x x 1
    // 9: 1 1 x x 1
    // 10:1 1 x x 1
    //

    static TraceAvaliabilityArray const rangeAvailability_Four_;
    static trace_range_map::TraceIndexStorage const tis_ranges_availability_Four_;
    static trace_range_map::TraceIndexStorage const tis_ranges_absence_Four_;
};

} // namespace segysdk::segyindex::tests
