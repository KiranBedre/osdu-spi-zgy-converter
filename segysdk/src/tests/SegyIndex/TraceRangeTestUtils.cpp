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

#include "TraceRangeTestUtils.h"

#include <cassert>
#include <gtest/gtest.h>

namespace segysdk::segyindex::tests
{

size_t
TraceRangeTester::getMI(size_t i, size_t j)
{
    return i * secondaryDimension_ + j;
}

bool
TraceRangeTester::getTA_One(size_t i, size_t j)
{
    return traceAvailability_One_[getMI(i, j)];
}

bool
TraceRangeTester::getTA_Two(size_t i, size_t j)
{
    return traceAvailability_Two_[getMI(i, j)];
}

bool
TraceRangeTester::getRA_One(size_t i, size_t j)
{
    return rangeAvailability_One_[getMI(i, j)];
}

bool
TraceRangeTester::getRA_Two(size_t i, size_t j)
{
    return rangeAvailability_Two_[getMI(i, j)];
}

bool
TraceRangeTester::getRA_Three(size_t i, size_t j)
{
    return rangeAvailability_Three_[getMI(i, j)];
}

bool
TraceRangeTester::getRA_Four(size_t i, size_t j)
{
    return rangeAvailability_Four_[getMI(i, j)];
}

size_t
TraceRangeTester::getFN_One(size_t i, size_t j)
{
    if (getTA_One(i, j)) {
        return defaultFileNo_;
    }
    else {
        return trace_range_map::TraceRange::missingFileNo_;
    }
}

size_t
TraceRangeTester::getFN_Two(size_t i, size_t j)
{
    if (getTA_Two(i, j)) {
        return defaultFileNo_;
    }
    else {
        return trace_range_map::TraceRange::missingFileNo_;
    }
}

size_t
TraceRangeTester::getSC_One(size_t i, size_t j)
{
    if (getTA_One(i, j)) {
        return defaultSampleCount_;
    }
    else {
        return 0U;
    }
}

size_t
TraceRangeTester::getSC_Two(size_t i, size_t j)
{
    if (getTA_Two(i, j)) {
        return defaultSampleCount_;
    }
    else {
        return 0U;
    }
}

size_t
TraceRangeTester::getTO_One(size_t i, size_t j)
{
    if (getTA_One(i, j)) {
        return firstTraceOffset_ + getMI(i, j) * offsetPerTrace_;
    }
    else {
        return trace_range_map::TraceRange::missingFileOffset_;
    }
}

size_t
TraceRangeTester::getTO_Two(size_t i, size_t j)
{
    if (getTA_Two(i, j)) {
        return firstTraceOffset_ + getMI(i, j) * offsetPerTrace_;
    }
    else {
        return trace_range_map::TraceRange::missingFileOffset_;
    }
}

size_t
TraceRangeTester::getRC(size_t lineNo)
{
    return rangeCount_[lineNo];
}

int
TraceRangeTester::getStepP()
{
    return defaultIndexStep_;
}

int
TraceRangeTester::getStepN()
{
    return -defaultIndexStep_;
}

PrimaryIndex
TraceRangeTester::getPriPInc(size_t lineNo)
{
    return primaryIndexPositiveIncreasing_[lineNo];
}

PrimaryIndex
TraceRangeTester::getPriNInc(size_t lineNo)
{
    return primaryIndexNegativeIncreasing_[lineNo];
}

PrimaryIndex
TraceRangeTester::getPriPDec(size_t lineNo)
{
    return primaryIndexPositiveDecreasing_[lineNo];
}

PrimaryIndex
TraceRangeTester::getPriNDec(size_t lineNo)
{
    return primaryIndexNegativeDecreasing_[lineNo];
}

SecondaryIndex
TraceRangeTester::getSecPInc(size_t lineNo)
{
    return secondaryIndexPositiveIncreasing_[lineNo];
}

SecondaryIndex
TraceRangeTester::getSecNInc(size_t lineNo)
{
    return secondaryIndexNegativeIncreasing_[lineNo];
}

SecondaryIndex
TraceRangeTester::getSecPDec(size_t lineNo)
{
    return secondaryIndexPositiveDecreasing_[lineNo];
}

SecondaryIndex
TraceRangeTester::getSecNDec(size_t lineNo)
{
    return secondaryIndexNegativeDecreasing_[lineNo];
}

std::vector<IndexBuilderData>
TraceRangeTester::getIBD_One(PrimaryIndex prii0, PrimaryIndex priStep, SecondaryIndex seci0, SecondaryIndex secStep)
{
    std::vector<IndexBuilderData> result;
    result.reserve(traceMatrixPower_);

    for (size_t i = 0U; i < primaryDimension_; ++i) {
        for (size_t j = 0U; j < secondaryDimension_; ++j) {
            if (not getTA_One(i, j)) {
                continue; // skip the missing record
            }

            int const prii = static_cast<int>(prii0) + static_cast<int>(i) * static_cast<int>(priStep);
            int const seci = static_cast<int>(seci0) + static_cast<int>(j) * static_cast<int>(secStep);

            IndexBuilderData ibd{
                /*FileNo*/ getFN_One(i, j),
                /*Offset*/ getTO_One(i, j),
                /*InlineNo*/ prii,
                /*CrosslineNo*/ seci,
                /*CMP*/0,
                /*TraceNo*/0,
                /*X*/ 0.0,
                /*Y*/ 0.0,
                /*SampleCount*/ getSC_One(i, j),
                /*MinimumAmplitude*/ std::numeric_limits<float>::max(),
                /*MaximumAmplitude*/ -std::numeric_limits<float>::max(),
            };

            result.emplace_back(std::move(ibd));
        }
    }
    return result;
}

std::vector<IndexBuilderData>
TraceRangeTester::getIBD_Two(PrimaryIndex prii0, PrimaryIndex priStep, SecondaryIndex seci0, SecondaryIndex secStep)
{
    std::vector<IndexBuilderData> result;
    result.reserve(traceMatrixPower_);

    for (size_t i = 0U; i < primaryDimension_; ++i) {
        for (size_t j = 0U; j < secondaryDimension_; ++j) {
            if (not getTA_Two(i, j)) {
                continue; // skip the missing record
            }

            int const prii = static_cast<int>(prii0) + static_cast<int>(i) * static_cast<int>(priStep);
            int const seci = static_cast<int>(seci0) + static_cast<int>(j) * static_cast<int>(secStep);

            IndexBuilderData ibd{
                /*FileNo*/ getFN_Two(i, j),
                /*Offset*/ getTO_Two(i, j),
                /*InlineNo*/ prii,
                /*CrosslineNo*/ seci,
                /*CMP*/0,
                /*TraceNo*/0,
                /*X*/ 0.0,
                /*Y*/ 0.0,
                /*SampleCount*/ getSC_Two(i, j),
                /*MinimumAmplitude*/ std::numeric_limits<float>::max(),
                /*MaximumAmplitude*/ -std::numeric_limits<float>::max(),
            };

            result.emplace_back(std::move(ibd));
        }
    }
    return result;
}

FileLayoutSpecification
TraceRangeTester::getG3D(PrimaryIndex pri0, PrimaryIndex priN, PrimaryIndex priStep,
                         SecondaryIndex sec0, SecondaryIndex secN, SecondaryIndex secStep,
                         bool isInlineOrdered)
{
    FileLayoutSpecification result;

    result.isInlineOrdered = isInlineOrdered;
    result.JExtent = HorizontalExtent{/*first*/ static_cast<int>(pri0),
                                      /*last*/ static_cast<int>(priN),
                                      /*step*/ static_cast<int>(priStep)};
    result.IExtent = HorizontalExtent{/*first*/ static_cast<int>(sec0),
                                      /*last*/ static_cast<int>(secN),
                                      /*step*/ static_cast<int>(secStep)};
    result.sampleExtent = VerticalExtent{/*first*/ 1.0,
                                         /*last*/ static_cast<double>(defaultSampleCount_) + 1,
                                         /*step*/ 1.0};

    return result;
}

std::shared_ptr<IndexBuilderCollatedData>
TraceRangeTester::getCollatedData_One(PrimaryIndex pri0, PrimaryIndex priN,
                                      SecondaryIndex sec0, SecondaryIndex secN,
                                      bool isInlineOrdered)
{
    std::shared_ptr<IndexBuilderCollatedData> result = std::make_shared<IndexBuilderCollatedData>();

    result->InlineIsPrimary = isInlineOrdered;

    result->FirstTraceOffset = []() -> size_t {
        for (size_t i = 0U; i < primaryDimension_; ++i) {
            for (size_t j = 0U; j < secondaryDimension_; ++j) {
                if (getTA_One(i, j)) {
                    return getTO_One(i, j);
                }
            }
        }
        return trace_range_map::TraceRange::missingFileOffset_;
    }();

    if (result->InlineIsPrimary) {
        result->InlineNoRange.UpdateMinMax(static_cast<int>(pri0));
        result->InlineNoRange.UpdateMinMax(static_cast<int>(priN));
        result->CrosslineNoRange.UpdateMinMax(static_cast<int>(sec0));
        result->CrosslineNoRange.UpdateMinMax(static_cast<int>(secN));
    }
    else {
        result->CrosslineNoRange.UpdateMinMax(static_cast<int>(pri0));
        result->CrosslineNoRange.UpdateMinMax(static_cast<int>(priN));
        result->InlineNoRange.UpdateMinMax(static_cast<int>(sec0));
        result->InlineNoRange.UpdateMinMax(static_cast<int>(secN));
    }

    return result;
}

std::shared_ptr<IndexBuilderCollatedData>
TraceRangeTester::getCollatedData_Two(PrimaryIndex pri0, PrimaryIndex priN,
                                      SecondaryIndex sec0, SecondaryIndex secN,
                                      bool isInlineOrdered)
{
    std::shared_ptr<IndexBuilderCollatedData> result = std::make_shared<IndexBuilderCollatedData>();

    result->InlineIsPrimary = isInlineOrdered;

    result->FirstTraceOffset = []() -> size_t {
        for (size_t i = 0U; i < primaryDimension_; ++i) {
            for (size_t j = 0U; j < secondaryDimension_; ++j) {
                if (getTA_Two(i, j)) {
                    return getTO_One(i, j);
                }
            }
        }
        return trace_range_map::TraceRange::missingFileOffset_;
    }();

    if (result->InlineIsPrimary) {
        result->InlineNoRange.UpdateMinMax(static_cast<int>(pri0));
        result->InlineNoRange.UpdateMinMax(static_cast<int>(priN));
        result->CrosslineNoRange.UpdateMinMax(static_cast<int>(sec0));
        result->CrosslineNoRange.UpdateMinMax(static_cast<int>(secN));
    }
    else {
        result->CrosslineNoRange.UpdateMinMax(static_cast<int>(pri0));
        result->CrosslineNoRange.UpdateMinMax(static_cast<int>(priN));
        result->InlineNoRange.UpdateMinMax(static_cast<int>(sec0));
        result->InlineNoRange.UpdateMinMax(static_cast<int>(secN));
    }

    return result;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisPositiveIncIncIndexes_One()
{
    return tis_positive_inc_inc_indexes_One_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisPositiveIncIncIndexes_Two()
{
    return tis_positive_inc_inc_indexes_Two_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisPositiveIncDecIndexes_One()
{
    return tis_positive_inc_dec_indexes_One_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisPositiveIncDecIndexes_Two()
{
    return tis_positive_inc_dec_indexes_Two_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisPositiveDecIncIndexes_One()
{
    return tis_positive_dec_inc_indexes_One_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisPositiveDecIncIndexes_Two()
{
    return tis_positive_dec_inc_indexes_Two_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisPositiveDecDecIndexes_One()
{
    return tis_positive_dec_dec_indexes_One_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisPositiveDecDecIndexes_Two()
{
    return tis_positive_dec_dec_indexes_Two_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisNegativeIncIncIndexes_One()
{
    return tis_negative_inc_inc_indexes_One_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisNegativeIncIncIndexes_Two()
{
    return tis_negative_inc_inc_indexes_Two_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisNegativeIncDecIndexes_One()
{
    return tis_negative_inc_dec_indexes_One_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisNegativeIncDecIndexes_Two()
{
    return tis_negative_inc_dec_indexes_Two_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisNegativeDecIncIndexes_One()
{
    return tis_negative_dec_inc_indexes_One_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisNegativeDecIncIndexes_Two()
{
    return tis_negative_dec_inc_indexes_Two_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisNegativeDecDecIndexes_One()
{
    return tis_negative_dec_dec_indexes_One_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisNegativeDecDecIndexes_Two()
{
    return tis_negative_dec_dec_indexes_Two_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisRangesAvailability_One()
{
    return tis_ranges_availability_One_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisRangesAbsence_One()
{
    return tis_ranges_absence_One_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisRangesAvailability_Two()
{
    return tis_ranges_availability_Two_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisRangesAbsence_Two()
{
    return tis_ranges_absence_Two_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisRangesAvailability_Three()
{
    return tis_ranges_availability_Three_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisRangesAbsence_Three()
{
    return tis_ranges_absence_Three_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisRangesAvailability_Four()
{
    return tis_ranges_availability_Four_;
}

trace_range_map::TraceIndexStorage const&
TraceRangeTester::getTisRangesAbsence_Four()
{
    return tis_ranges_absence_Four_;
}

void
TraceRangeTester::testFillAndReadIndexMap(TraceIndexStorage const& tis_const,
                                          TraceIndexMapKey::Order orderPri,
                                          TraceIndexMapKey::Order orderSeci,
                                          std::function<bool(size_t, size_t)> getTA,
                                          std::function<PrimaryIndex(size_t)> getPriIndex,
                                          std::function<SecondaryIndex(size_t)> getSecIndex,
                                          std::function<size_t(size_t, size_t)> getTO,
                                          int stepPri, int stepSec,
                                          SampleFormat sampleFormat)
{
    // unused parameter but the one is kept as a parameter
    // to simplify the understanding of the signature
    (void)stepPri;

    TraceIndexMap tim;
    tim.setOrderMode(orderPri, orderSeci);

    TraceIndexStorage tis;

    for (auto const& range : tis_const) {
        TraceRange r(range);
        tis.addRange(std::move(r));
    }

    tim.addRanges(tis);

    for (size_t i = 0U; i < primaryDimension_; ++i) {
        for (size_t j = 0U; j < secondaryDimension_; ++j) {

            PrimaryIndex const prii = getPriIndex(i);
            SecondaryIndex const seci = getSecIndex(j);
            auto const range = tim.getRange(prii, seci);

            ASSERT_TRUE(range.has_value());

            auto const& holderRange = range.value();

            ASSERT_TRUE(holderRange->second.get().isHit(prii, seci, stepSec));
            ASSERT_EQ(getTA(i, j), holderRange->second.get().isLiveTrace(prii, seci, stepSec));
            ASSERT_EQ(getTO(i, j), holderRange->second.get().getFileOffset(prii, seci, stepSec, sampleFormat));
        }
    }
}

void
TraceRangeTester::testFillTraceStorage(TraceIndexStorage const& tis_const,
                                       std::function<std::shared_ptr<IndexBuilderCollatedData>(PrimaryIndex, PrimaryIndex, SecondaryIndex, SecondaryIndex, bool)> getCollatedData,
                                       std::function<std::vector<IndexBuilderData>(PrimaryIndex, PrimaryIndex, SecondaryIndex, SecondaryIndex)> getIBD,
                                       std::function<PrimaryIndex(size_t)> getPriIndex,
                                       std::function<SecondaryIndex(size_t)> getSecIndex,
                                       int stepPri, int stepSec, size_t sizeOfIbd)
{
    auto const builderData = getIBD(/*pri0*/ getPriIndex(0), /*priStep*/ stepPri,
                                    /*sec0*/ getSecIndex(0), /*secStep*/ stepSec);
    ASSERT_EQ(sizeOfIbd, builderData.size());

    auto const fileLayout = getG3D(/*pri0*/ getPriIndex(0), /*priN*/ getPriIndex(primaryDimension_ - 1), /*priStep*/ stepPri,
                                   /*sec0*/ getSecIndex(0), /*secN*/ getSecIndex(secondaryDimension_ - 1), /*secStep*/ stepSec,
                                   /*isInlineOrdered*/ true);
    auto const collatedData = getCollatedData(/*pri0*/ getPriIndex(0), /*priN*/ getPriIndex(primaryDimension_ - 1),
                                              /*sec0*/ getSecIndex(0), /*secN*/ getSecIndex(secondaryDimension_ - 1),
                                              /*isInlineOrdered*/ true);

    TraceIndexStorage tis;
    {
        TraceRangeFactory trf(fileLayout, collatedData, tis);

        for (auto const& ibd : builderData) {
            trf.addTrace(ibd.InlineNo,
                         ibd.CrosslineNo,
                         ibd.SampleCount,
                         ibd.FileNo,
                         ibd.Offset);
        }
    }

    ASSERT_EQ(tis.size(), tis_const.size());

    bool const tis_are_equal = std::equal(tis.cbegin(), tis.cend(), tis_const.cbegin());

    ASSERT_TRUE(tis_are_equal);
}

void
TraceRangeTester::testDetectTraceRangeMapGaps(TraceIndexStorage const& tis_const,
                                              TraceIndexStorage const& tis_missing_const,
                                              std::function<bool(size_t, size_t)> getRA,
                                              std::function<PrimaryIndex(size_t)> getPriIndex,
                                              std::function<SecondaryIndex(size_t)> getSecIndex,
                                              int step, size_t sizeOfMissingRanges,
                                              size_t numberOfLiveTraces)
{
    PrimaryIndex const priExtentFirst = getPriIndex(0U);
    PrimaryIndex const priExtentLast = getPriIndex(primaryDimension_ - 1);
    PrimaryIndex const priExtentStep = step;

    SecondaryIndex const secExtentFirst = getSecIndex(0U);
    SecondaryIndex const secExtentLast = getSecIndex(secondaryDimension_ - 1);
    SecondaryIndex const secExtentStep = step;

    TraceIndexMap tim;
    tim.setOrderMode(/*pri*/ TraceIndexMapKey::Order::Asc,
                     /*seci*/ TraceIndexMapKey::Order::Asc);

    TraceIndexStorage tis;

    for (auto const& range : tis_const) {
        TraceRange r(range);
        tis.addRange(std::move(r));
    }

    tim.addRanges(tis);

    for (PrimaryIndex prii = priExtentFirst; prii <= priExtentLast; prii += priExtentStep) {
        for (SecondaryIndex seci = secExtentFirst; seci <= secExtentLast; seci += secExtentStep) {
            auto const range = tim.getRange(prii, seci);

            bool const isHit = range.has_value()
                               && range.value()->second.get().isHit(prii,
                                                                    seci,
                                                                    secExtentStep);
            bool const isLive = range.has_value()
                                && range.value()->second.get().isLiveTrace(prii,
                                                                           seci,
                                                                           secExtentStep);
            size_t const i = static_cast<size_t>((prii - priExtentFirst) / priExtentStep);
            size_t const j = static_cast<size_t>((seci - secExtentFirst) / secExtentStep);
            ASSERT_EQ(getRA(i, j), isHit);
            ASSERT_EQ(getRA(i, j), isLive);
        }
    }

    TraceIndexStorage tis_missing_ranges;

    auto firstAddedItem = tim.enumerateMissingRanges(priExtentFirst, priExtentLast, priExtentStep,
                                                     secExtentFirst, secExtentLast, secExtentStep,
                                                     tis_missing_ranges);

    ASSERT_EQ(firstAddedItem, tis_missing_ranges.cbegin());
    ASSERT_EQ(sizeOfMissingRanges, tis_missing_ranges.size());

    ASSERT_EQ(sizeOfMissingRanges, tis_missing_const.size());

    bool const is_tis_same = std::equal(tis_missing_ranges.cbegin(), tis_missing_ranges.cend(), tis_missing_const.cbegin());

    ASSERT_TRUE(is_tis_same);

    tim.addRanges(firstAddedItem, tis_missing_ranges.cend());

    for (PrimaryIndex prii = priExtentFirst; prii <= priExtentLast; prii += priExtentStep) {
        for (SecondaryIndex seci = secExtentFirst; seci <= secExtentLast; seci += secExtentStep) {
            auto const range = tim.getRange(prii, seci);

            bool const isHit = range.has_value()
                               && range.value()->second.get().isHit(prii,
                                                                    seci,
                                                                    secExtentStep);
            bool const isLive = range.has_value()
                                && range.value()->second.get().isLiveTrace(prii,
                                                                           seci,
                                                                           secExtentStep);
            ASSERT_TRUE(isHit);

            size_t const i = static_cast<size_t>((prii - priExtentFirst) / priExtentStep);
            size_t const j = static_cast<size_t>((seci - secExtentFirst) / secExtentStep);
            ASSERT_EQ(getRA(i, j), isLive);
        }
    }

    auto const indexedTraceCount = tim.enumerateLiveTraceCount(priExtentFirst, priExtentLast, priExtentStep,
                                                               secExtentFirst, secExtentLast, secExtentStep);

    ASSERT_EQ(numberOfLiveTraces, indexedTraceCount);
}

TraceRangeTester::PrimaryIndexArray
TraceRangeTester::getPrimaryIndexArray(int pri0, int step)
{
    PrimaryIndexArray result{};
    for (size_t i = 0U; i < result.size(); ++i) {
        result[i] = pri0 + step * static_cast<int>(i);
    }
    return result;
}

TraceRangeTester::SecondaryIndexArray
TraceRangeTester::getSecondaryIndexArray(int sec0, int step)
{
    SecondaryIndexArray result{};
    for (size_t i = 0U; i < result.size(); ++i) {
        result[i] = sec0 + step * static_cast<int>(i);
    }
    return result;
}

trace_range_map::TraceIndexStorage
TraceRangeTester::getTisTA(std::function<bool(size_t, size_t)> getTA,
                           std::function<PrimaryIndex(size_t)> getPri,
                           std::function<PrimaryIndex(size_t)> getSec,
                           std::function<size_t(size_t, size_t)> getFN,
                           std::function<size_t(size_t, size_t)> getSC,
                           std::function<size_t(size_t)> getRC,
                           std::function<size_t(size_t, size_t)> getTO)
{
    return trace_range_map::TraceIndexStorage{
        trace_range_map::TraceRange{/*exists*/ getTA(0, 0), /*primaryNo*/ getPri(0), /*secondaryNo*/ getSec(0), /*fileNo*/ getFN(0, 0), /*samplesCount*/ getSC(0, 0), /*rangeCount*/ getRC(0), /*fileOffset*/ getTO(0, 0)},
        trace_range_map::TraceRange{/*exists*/ getTA(0, 1), /*primaryNo*/ getPri(0), /*secondaryNo*/ getSec(1), /*fileNo*/ getFN(0, 1), /*samplesCount*/ getSC(0, 1), /*rangeCount*/ getRC(1), /*fileOffset*/ getTO(0, 1)},
        trace_range_map::TraceRange{/*exists*/ getTA(0, 4), /*primaryNo*/ getPri(0), /*secondaryNo*/ getSec(4), /*fileNo*/ getFN(0, 4), /*samplesCount*/ getSC(0, 4), /*rangeCount*/ getRC(2), /*fileOffset*/ getTO(0, 4)},

        trace_range_map::TraceRange{/*exists*/ getTA(1, 0), /*primaryNo*/ getPri(1), /*secondaryNo*/ getSec(0), /*fileNo*/ getFN(1, 0), /*samplesCount*/ getSC(1, 0), /*rangeCount*/ getRC(3), /*fileOffset*/ getTO(1, 0)},
        trace_range_map::TraceRange{/*exists*/ getTA(1, 1), /*primaryNo*/ getPri(1), /*secondaryNo*/ getSec(1), /*fileNo*/ getFN(1, 1), /*samplesCount*/ getSC(1, 1), /*rangeCount*/ getRC(4), /*fileOffset*/ getTO(1, 1)},

        trace_range_map::TraceRange{/*exists*/ getTA(2, 0), /*primaryNo*/ getPri(2), /*secondaryNo*/ getSec(0), /*fileNo*/ getFN(2, 0), /*samplesCount*/ getSC(2, 0), /*rangeCount*/ getRC(5), /*fileOffset*/ getTO(2, 0)},
        trace_range_map::TraceRange{/*exists*/ getTA(2, 1), /*primaryNo*/ getPri(2), /*secondaryNo*/ getSec(1), /*fileNo*/ getFN(2, 1), /*samplesCount*/ getSC(2, 1), /*rangeCount*/ getRC(6), /*fileOffset*/ getTO(2, 1)},
        trace_range_map::TraceRange{/*exists*/ getTA(2, 4), /*primaryNo*/ getPri(2), /*secondaryNo*/ getSec(4), /*fileNo*/ getFN(2, 4), /*samplesCount*/ getSC(2, 4), /*rangeCount*/ getRC(7), /*fileOffset*/ getTO(2, 4)},

        trace_range_map::TraceRange{/*exists*/ getTA(3, 0), /*primaryNo*/ getPri(3), /*secondaryNo*/ getSec(0), /*fileNo*/ getFN(3, 0), /*samplesCount*/ getSC(3, 0), /*rangeCount*/ getRC(8), /*fileOffset*/ getTO(3, 0)},
        trace_range_map::TraceRange{/*exists*/ getTA(3, 2), /*primaryNo*/ getPri(3), /*secondaryNo*/ getSec(2), /*fileNo*/ getFN(3, 2), /*samplesCount*/ getSC(3, 2), /*rangeCount*/ getRC(9), /*fileOffset*/ getTO(3, 2)},

        trace_range_map::TraceRange{/*exists*/ getTA(4, 0), /*primaryNo*/ getPri(4), /*secondaryNo*/ getSec(0), /*fileNo*/ getFN(4, 0), /*samplesCount*/ getSC(4, 0), /*rangeCount*/ getRC(10), /*fileOffset*/ getTO(4, 0)},
        trace_range_map::TraceRange{/*exists*/ getTA(4, 3), /*primaryNo*/ getPri(4), /*secondaryNo*/ getSec(3), /*fileNo*/ getFN(4, 3), /*samplesCount*/ getSC(4, 3), /*rangeCount*/ getRC(11), /*fileOffset*/ getTO(4, 3)},

        trace_range_map::TraceRange{/*exists*/ getTA(5, 0), /*primaryNo*/ getPri(5), /*secondaryNo*/ getSec(0), /*fileNo*/ getFN(5, 0), /*samplesCount*/ getSC(5, 0), /*rangeCount*/ getRC(12), /*fileOffset*/ getTO(5, 0)},

        trace_range_map::TraceRange{/*exists*/ getTA(6, 0), /*primaryNo*/ getPri(6), /*secondaryNo*/ getSec(0), /*fileNo*/ getFN(6, 0), /*samplesCount*/ getSC(6, 0), /*rangeCount*/ getRC(13), /*fileOffset*/ getTO(6, 0)},

        trace_range_map::TraceRange{/*exists*/ getTA(7, 0), /*primaryNo*/ getPri(7), /*secondaryNo*/ getSec(0), /*fileNo*/ getFN(7, 0), /*samplesCount*/ getSC(7, 0), /*rangeCount*/ getRC(14), /*fileOffset*/ getTO(7, 0)},

        trace_range_map::TraceRange{/*exists*/ getTA(8, 0), /*primaryNo*/ getPri(8), /*secondaryNo*/ getSec(0), /*fileNo*/ getFN(8, 0), /*samplesCount*/ getSC(8, 0), /*rangeCount*/ getRC(15), /*fileOffset*/ getTO(8, 0)},

        trace_range_map::TraceRange{/*exists*/ getTA(9, 0), /*primaryNo*/ getPri(9), /*secondaryNo*/ getSec(0), /*fileNo*/ getFN(9, 0), /*samplesCount*/ getSC(9, 0), /*rangeCount*/ getRC(16), /*fileOffset*/ getTO(9, 0)},

        trace_range_map::TraceRange{/*exists*/ getTA(10, 0), /*primaryNo*/ getPri(10), /*secondaryNo*/ getSec(0), /*fileNo*/ getFN(10, 0), /*samplesCount*/ getSC(10, 0), /*rangeCount*/ getRC(17), /*fileOffset*/ getTO(10, 0)}};
}

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

TraceRangeTester::TraceAvaliabilityArray const
    TraceRangeTester::traceAvailability_One_{
        false, true, true, true, false,
        true, false, false, false, false,
        true, false, false, false, true,
        false, false, true, true, true,
        true, true, true, false, false,
        false, false, false, false, false,
        false, false, false, false, false,
        false, false, false, false, false,
        true, true, true, true, true,
        true, true, true, true, true,
        false, false, false, false, false};

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

TraceRangeTester::TraceAvaliabilityArray const
    TraceRangeTester::traceAvailability_Two_{
        true, false, false, false, true,
        false, true, true, true, true,
        false, true, true, true, false,
        true, true, false, false, false,
        false, false, false, true, true,
        true, true, true, true, true,
        true, true, true, true, true,
        true, true, true, true, true,
        false, false, false, false, false,
        false, false, false, false, false,
        true, true, true, true, true};

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

TraceRangeTester::RangeCountArray const
    TraceRangeTester::rangeCount_{
        1U, 3U, 1U,
        1U, 4U,
        1U, 3U, 1U,
        2U, 3U,
        3U, 2U,
        5U,
        5U,
        5U,
        5U,
        5U,
        5U};

//
// Primary index
//
// 10 .. 20
//
TraceRangeTester::PrimaryIndexArray const
    TraceRangeTester::primaryIndexPositiveIncreasing_
    = getPrimaryIndexArray(TraceRangeTester::primaryIndexPositiveStart_,
                           TraceRangeTester::getStepP());

//
// Primary index
//
// -11 .. -1
//
TraceRangeTester::PrimaryIndexArray const
    TraceRangeTester::primaryIndexNegativeIncreasing_
    = getPrimaryIndexArray(TraceRangeTester::primaryIndexNegativeStart_,
                           TraceRangeTester::getStepP());

//
// Primary index
//
// 10 .. 0
//
TraceRangeTester::PrimaryIndexArray const
    TraceRangeTester::primaryIndexPositiveDecreasing_
    = getPrimaryIndexArray(TraceRangeTester::primaryIndexPositiveStart_,
                           TraceRangeTester::getStepN());

//
// Primary index
//
// -11 .. -21
//
TraceRangeTester::PrimaryIndexArray const
    TraceRangeTester::primaryIndexNegativeDecreasing_
    = getPrimaryIndexArray(TraceRangeTester::primaryIndexNegativeStart_,
                           TraceRangeTester::getStepN());

//
// Secondary index
//
// 5 .. 9
//
TraceRangeTester::SecondaryIndexArray const
    TraceRangeTester::secondaryIndexPositiveIncreasing_
    = getSecondaryIndexArray(TraceRangeTester::secondaryIndexPositiveStart_,
                             TraceRangeTester::getStepP());

//
// Secondary index
//
// -5 .. -1
//
TraceRangeTester::SecondaryIndexArray const
    TraceRangeTester::secondaryIndexNegativeIncreasing_
    = getSecondaryIndexArray(TraceRangeTester::secondaryIndexNegativeStart_,
                             TraceRangeTester::getStepP());

//
// Secondary index
//
// 5 .. 1
//
TraceRangeTester::SecondaryIndexArray const
    TraceRangeTester::secondaryIndexPositiveDecreasing_
    = getSecondaryIndexArray(TraceRangeTester::secondaryIndexPositiveStart_,
                             TraceRangeTester::getStepN());

//
// Secondary index
//
// -5 .. -9
//
TraceRangeTester::SecondaryIndexArray const
    TraceRangeTester::secondaryIndexNegativeDecreasing_
    = getSecondaryIndexArray(TraceRangeTester::secondaryIndexNegativeStart_,
                             TraceRangeTester::getStepN());

//
// Trace indexes matrix #1
// Positive Increasing/Increasing
//
// (10,5)  (10,6)  ..  (10,9)
// (11,5)  (11,6)  ..  (11,9)
// ..      ..      ..  ..
// (20,5)  (20,6)  ..  (20,9)
//

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_positive_inc_inc_indexes_One_
    = getTisTA(getTA_One,
               getPriPInc,
               getSecPInc,
               getFN_One,
               getSC_One,
               getRC,
               getTO_One);

//
// Trace indexes matrix #2
// Positive Increasing/Increasing
//
// (10,5)  (10,6)  ..  (10,9)
// (11,5)  (11,6)  ..  (11,9)
// ..      ..      ..  ..
// (20,5)  (20,6)  ..  (20,9)
//

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_positive_inc_inc_indexes_Two_
    = getTisTA(getTA_Two,
               getPriPInc,
               getSecPInc,
               getFN_Two,
               getSC_Two,
               getRC,
               getTO_Two);

//
// Trace indexes matrix #1
// Positive Increasing/Decreasing
//
// (10,5)  (10,4)  ..  (10,1)
// (11,5)  (11,4)  ..  (11,1)
// ..      ..      ..  ..
// (20,5)  (20,4)  ..  (20,1)
//

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_positive_inc_dec_indexes_One_
    = getTisTA(getTA_One,
               getPriPInc,
               getSecPDec,
               getFN_One,
               getSC_One,
               getRC,
               getTO_One);

//
// Trace indexes matrix #2
// Positive Increasing/Decreasing
//
// (10,5)  (10,4)  ..  (10,1)
// (11,5)  (11,4)  ..  (11,1)
// ..      ..      ..  ..
// (20,5)  (20,4)  ..  (20,1)
//

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_positive_inc_dec_indexes_Two_
    = getTisTA(getTA_Two,
               getPriPInc,
               getSecPDec,
               getFN_Two,
               getSC_Two,
               getRC,
               getTO_Two);

//
// Trace indexes matrix #1
// Positive Decreasing/Increasing
//
// (10,5)  (10,6)  ..  (10,9)
// ( 9,5)  ( 9,6)  ..  ( 9,9)
// ..      ..      ..  ..
// ( 0,5)  ( 0,6)  ..  ( 0,9)
//

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_positive_dec_inc_indexes_One_
    = getTisTA(getTA_One,
               getPriPDec,
               getSecPInc,
               getFN_One,
               getSC_One,
               getRC,
               getTO_One);

//
// Trace indexes matrix #2
// Positive Decreasing/Increasing
//
// (10,5)  (10,6)  ..  (10,9)
// ( 9,5)  ( 9,6)  ..  ( 9,9)
// ..      ..      ..  ..
// ( 0,5)  ( 0,6)  ..  ( 0,9)
//

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_positive_dec_inc_indexes_Two_
    = getTisTA(getTA_Two,
               getPriPDec,
               getSecPInc,
               getFN_Two,
               getSC_Two,
               getRC,
               getTO_Two);

//
// Trace indexes matrix #1
// Positive Decreasing/Decreasing
//
// (10,5)  (10,4)  ..  (10,1)
// ( 9,5)  ( 9,4)  ..  ( 9,1)
// ..      ..      ..  ..
// ( 0,5)  ( 0,4)  ..  ( 0,1)
//

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_positive_dec_dec_indexes_One_
    = getTisTA(getTA_One,
               getPriPDec,
               getSecPDec,
               getFN_One,
               getSC_One,
               getRC,
               getTO_One);

//
// Trace indexes matrix #2
// Positive Decreasing/Decreasing
//
// (10,5)  (10,4)  ..  (10,1)
// ( 9,5)  ( 9,4)  ..  ( 9,1)
// ..      ..      ..  ..
// ( 0,5)  ( 0,4)  ..  ( 0,1)
//

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_positive_dec_dec_indexes_Two_
    = getTisTA(getTA_Two,
               getPriPDec,
               getSecPDec,
               getFN_Two,
               getSC_Two,
               getRC,
               getTO_Two);

//
// Trace indexes matrix #1
// Negative Increasing/Increasing
//
// (-11, -5)  (-11, -4) .. (-11, -1)
// (-10, -5)  (-10, -4) .. (-10, -1)
// ..         ..        .. ..
// (-1,  -5)  (-1,  -4) .. (-1,  -1)
//

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_negative_inc_inc_indexes_One_
    = getTisTA(getTA_One,
               getPriNInc,
               getSecNInc,
               getFN_One,
               getSC_One,
               getRC,
               getTO_One);

//
// Trace indexes matrix #2
// Negative Increasing/Increasing
//
// (-11, -5)  (-11, -4) .. (-11, -1)
// (-10, -5)  (-10, -4) .. (-10, -1)
// ..         ..        .. ..
// (-1,  -5)  (-1,  -4) .. (-1,  -1)
//

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_negative_inc_inc_indexes_Two_
    = getTisTA(getTA_Two,
               getPriNInc,
               getSecNInc,
               getFN_Two,
               getSC_Two,
               getRC,
               getTO_Two);

//
// Trace indexes matrix #1
// Negative Increasing/Decreasing
//
// (-11, -5)  (-11, -6) .. (-11, -9)
// (-10, -5)  (-10, -6) .. (-10, -9)
// ..         ..        .. ..
// (-1,  -5)  (-1,  -6) .. (-1,  -9)
//

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_negative_inc_dec_indexes_One_
    = getTisTA(getTA_One,
               getPriNInc,
               getSecNDec,
               getFN_One,
               getSC_One,
               getRC,
               getTO_One);

//
// Trace indexes matrix #2
// Negative Increasing/Decreasing
//
// (-11, -5)  (-11, -6) .. (-11, -9)
// (-10, -5)  (-10, -6) .. (-10, -9)
// ..         ..        .. ..
// (-1,  -5)  (-1,  -6) .. (-1,  -9)
//

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_negative_inc_dec_indexes_Two_
    = getTisTA(getTA_Two,
               getPriNInc,
               getSecNDec,
               getFN_Two,
               getSC_Two,
               getRC,
               getTO_Two);

//
// Trace indexes matrix #1
// Negative Decreasing/Increasing
//
// (-11, -5)  (-11, -4) .. (-11, -1)
// (-12, -5)  (-12, -4) .. (-12, -1)
// ..         ..        .. ..
// (-20, -5)  (-20, -4) .. (-20, -1)
//

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_negative_dec_inc_indexes_One_
    = getTisTA(getTA_One,
               getPriNDec,
               getSecNInc,
               getFN_One,
               getSC_One,
               getRC,
               getTO_One);

//
// Trace indexes matrix #2
// Negative Decreasing/Increasing
//
// (-11, -5)  (-11, -4) .. (-11, -1)
// (-12, -5)  (-12, -4) .. (-12, -1)
// ..         ..        .. ..
// (-20, -5)  (-20, -4) .. (-20, -1)
//

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_negative_dec_inc_indexes_Two_
    = getTisTA(getTA_Two,
               getPriNDec,
               getSecNInc,
               getFN_Two,
               getSC_Two,
               getRC,
               getTO_Two);

//
// Trace indexes matrix #1
// Negative Decreasing/Decreasing
//
// (-11, -5)  (-11, -6) .. (-11, -9)
// (-12, -5)  (-12, -6) .. (-12, -9)
// ..         ..        .. ..
// (-20, -5)  (-20, -6) .. (-20, -9)
//

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_negative_dec_dec_indexes_One_
    = getTisTA(getTA_One,
               getPriNDec,
               getSecNDec,
               getFN_One,
               getSC_One,
               getRC,
               getTO_One);

//
// Trace indexes matrix #2
// Negative Decreasing/Decreasing
//
// (-11, -5)  (-11, -6) .. (-11, -9)
// (-12, -5)  (-12, -6) .. (-12, -9)
// ..         ..        .. ..
// (-20, -5)  (-20, -6) .. (-20, -9)
//

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_negative_dec_dec_indexes_Two_
    = getTisTA(getTA_Two,
               getPriNDec,
               getSecNDec,
               getFN_Two,
               getSC_Two,
               getRC,
               getTO_Two);

//
// Ranges/files availability #1
//
//     5 6 7 8 9
//     - - - - -
// 10: 1 1 x x x
// 11: 1 1 x x x
// 12: 1 1 x x x
// 13: 1 1 1 1 1
// 14: 1 1 1 1 1
// 15: x x 1 1 1
// 16: x x 1 1 1
// 17: x x 1 1 1
// 18: x x 1 1 1
// 19: x x 1 1 1
// 20: x x 1 1 1
//

TraceRangeTester::TraceAvaliabilityArray const
    TraceRangeTester::rangeAvailability_One_{
        true, true, false, false, false,
        true, true, false, false, false,
        true, true, false, false, false,
        true, true, true, true, true,
        true, true, true, true, true,
        false, false, true, true, true,
        false, false, true, true, true,
        false, false, true, true, true,
        false, false, true, true, true,
        false, false, true, true, true,
        false, false, true, true, true};

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_ranges_availability_One_{
        trace_range_map::TraceRange{/*exists*/ getRA_One(0, 0), /*primaryNo*/ getPriPInc(0), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_One(1, 0), /*primaryNo*/ getPriPInc(1), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_One(2, 0), /*primaryNo*/ getPriPInc(2), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_One(3, 0), /*primaryNo*/ getPriPInc(3), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_One(4, 0), /*primaryNo*/ getPriPInc(4), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},

        trace_range_map::TraceRange{/*exists*/ getRA_One(3, 2), /*primaryNo*/ getPriPInc(3), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 3U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_One(4, 2), /*primaryNo*/ getPriPInc(4), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 3U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_One(5, 2), /*primaryNo*/ getPriPInc(5), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 3U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_One(6, 2), /*primaryNo*/ getPriPInc(6), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 3U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_One(7, 2), /*primaryNo*/ getPriPInc(7), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 3U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_One(8, 2), /*primaryNo*/ getPriPInc(8), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 3U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_One(9, 2), /*primaryNo*/ getPriPInc(9), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 3U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_One(10, 2), /*primaryNo*/ getPriPInc(10), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 3U, /*fileOffset*/ 0U}};

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_ranges_absence_One_{
        trace_range_map::TraceRange{/*exists*/ getRA_One(0, 2), /*primaryNo*/ getPriPInc(0), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 3U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_One(1, 2), /*primaryNo*/ getPriPInc(1), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 3U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_One(2, 2), /*primaryNo*/ getPriPInc(2), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 3U, /*fileOffset*/ TraceRange::missingFileOffset_},

        trace_range_map::TraceRange{/*exists*/ getRA_One(5, 0), /*primaryNo*/ getPriPInc(5), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_One(6, 0), /*primaryNo*/ getPriPInc(6), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_One(7, 0), /*primaryNo*/ getPriPInc(7), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_One(8, 0), /*primaryNo*/ getPriPInc(8), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_One(9, 0), /*primaryNo*/ getPriPInc(9), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_One(10, 0), /*primaryNo*/ getPriPInc(10), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_}};

//
// Ranges/files availability #2
//
//     5 6 7 8 9
//     - - - - -
// 10: x x 1 1 1
// 11: x x 1 1 1
// 12: x x 1 1 1
// 13: x x x x x
// 14: x x x x x
// 15: 1 1 x x x
// 16: 1 1 x x x
// 17: 1 1 x x x
// 18: 1 1 x x x
// 19: 1 1 x x x
// 20: 1 1 x x x
//

TraceRangeTester::TraceAvaliabilityArray const
    TraceRangeTester::rangeAvailability_Two_{
        false, false, true, true, true,
        false, false, true, true, true,
        false, false, true, true, true,
        false, false, false, false, false,
        false, false, false, false, false,
        true, true, false, false, false,
        true, true, false, false, false,
        true, true, false, false, false,
        true, true, false, false, false,
        true, true, false, false, false,
        true, true, false, false, false};

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_ranges_availability_Two_{
        trace_range_map::TraceRange{/*exists*/ getRA_Two(0, 2), /*primaryNo*/ getPriPInc(0), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 3U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Two(1, 2), /*primaryNo*/ getPriPInc(1), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 3U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Two(2, 2), /*primaryNo*/ getPriPInc(2), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 3U, /*fileOffset*/ 0U},

        trace_range_map::TraceRange{/*exists*/ getRA_Two(5, 0), /*primaryNo*/ getPriPInc(5), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Two(6, 0), /*primaryNo*/ getPriPInc(6), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Two(7, 0), /*primaryNo*/ getPriPInc(7), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Two(8, 0), /*primaryNo*/ getPriPInc(8), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Two(9, 0), /*primaryNo*/ getPriPInc(9), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Two(10, 0), /*primaryNo*/ getPriPInc(10), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U}};

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_ranges_absence_Two_{
        trace_range_map::TraceRange{/*exists*/ getRA_Two(0, 0), /*primaryNo*/ getPriPInc(0), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Two(1, 0), /*primaryNo*/ getPriPInc(1), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Two(2, 0), /*primaryNo*/ getPriPInc(2), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},

        trace_range_map::TraceRange{/*exists*/ getRA_Two(3, 0), /*primaryNo*/ getPriPInc(3), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 5U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Two(4, 0), /*primaryNo*/ getPriPInc(4), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 5U, /*fileOffset*/ TraceRange::missingFileOffset_},

        trace_range_map::TraceRange{/*exists*/ getRA_Two(5, 2), /*primaryNo*/ getPriPInc(5), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 3U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Two(6, 2), /*primaryNo*/ getPriPInc(6), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 3U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Two(7, 2), /*primaryNo*/ getPriPInc(7), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 3U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Two(8, 2), /*primaryNo*/ getPriPInc(8), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 3U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Two(9, 2), /*primaryNo*/ getPriPInc(9), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 3U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Two(10, 2), /*primaryNo*/ getPriPInc(10), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 3U, /*fileOffset*/ TraceRange::missingFileOffset_}};

//
// Ranges/files availability #3
//
//     5 6 7 8 9
//     - - - - -
// 10: 1 1 x x 1
// 11: 1 1 x x 1
// 12: 1 1 x x 1
// 13: 1 1 x x 1
// 14: 1 1 x x 1
// 15: x x 1 1 x
// 16: x x 1 1 x
// 17: x x 1 1 x
// 18: x x 1 1 x
// 19: x x 1 1 x
// 20: x x 1 1 x
//

TraceRangeTester::TraceAvaliabilityArray const
    TraceRangeTester::rangeAvailability_Three_{
        true, true, false, false, true,
        true, true, false, false, true,
        true, true, false, false, true,
        true, true, false, false, true,
        true, true, false, false, true,
        false, false, true, true, false,
        false, false, true, true, false,
        false, false, true, true, false,
        false, false, true, true, false,
        false, false, true, true, false,
        false, false, true, true, false};

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_ranges_availability_Three_{
        trace_range_map::TraceRange{/*exists*/ getRA_Three(0, 0), /*primaryNo*/ getPriPInc(0), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(1, 0), /*primaryNo*/ getPriPInc(1), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(2, 0), /*primaryNo*/ getPriPInc(2), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(3, 0), /*primaryNo*/ getPriPInc(3), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(4, 0), /*primaryNo*/ getPriPInc(4), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},

        trace_range_map::TraceRange{/*exists*/ getRA_Three(0, 4), /*primaryNo*/ getPriPInc(0), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 1U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(1, 4), /*primaryNo*/ getPriPInc(1), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 1U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(2, 4), /*primaryNo*/ getPriPInc(2), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 1U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(3, 4), /*primaryNo*/ getPriPInc(3), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 1U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(4, 4), /*primaryNo*/ getPriPInc(4), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 1U, /*fileOffset*/ 0U},

        trace_range_map::TraceRange{/*exists*/ getRA_Three(5, 2), /*primaryNo*/ getPriPInc(5), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 2U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(6, 2), /*primaryNo*/ getPriPInc(6), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 2U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(7, 2), /*primaryNo*/ getPriPInc(7), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 2U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(8, 2), /*primaryNo*/ getPriPInc(8), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 2U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(9, 2), /*primaryNo*/ getPriPInc(9), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 2U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(10, 2), /*primaryNo*/ getPriPInc(10), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 2U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U}};

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_ranges_absence_Three_{
        trace_range_map::TraceRange{/*exists*/ getRA_Three(0, 2), /*primaryNo*/ getPriPInc(0), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(1, 2), /*primaryNo*/ getPriPInc(1), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(2, 2), /*primaryNo*/ getPriPInc(2), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(3, 2), /*primaryNo*/ getPriPInc(3), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(4, 2), /*primaryNo*/ getPriPInc(4), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},

        trace_range_map::TraceRange{/*exists*/ getRA_Three(5, 0), /*primaryNo*/ getPriPInc(5), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(5, 4), /*primaryNo*/ getPriPInc(5), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 1U, /*fileOffset*/ TraceRange::missingFileOffset_},

        trace_range_map::TraceRange{/*exists*/ getRA_Three(6, 0), /*primaryNo*/ getPriPInc(6), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(6, 4), /*primaryNo*/ getPriPInc(6), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 1U, /*fileOffset*/ TraceRange::missingFileOffset_},

        trace_range_map::TraceRange{/*exists*/ getRA_Three(7, 0), /*primaryNo*/ getPriPInc(7), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(7, 4), /*primaryNo*/ getPriPInc(7), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 1U, /*fileOffset*/ TraceRange::missingFileOffset_},

        trace_range_map::TraceRange{/*exists*/ getRA_Three(8, 0), /*primaryNo*/ getPriPInc(8), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(8, 4), /*primaryNo*/ getPriPInc(8), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 1U, /*fileOffset*/ TraceRange::missingFileOffset_},

        trace_range_map::TraceRange{/*exists*/ getRA_Three(9, 0), /*primaryNo*/ getPriPInc(9), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(9, 4), /*primaryNo*/ getPriPInc(9), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 1U, /*fileOffset*/ TraceRange::missingFileOffset_},

        trace_range_map::TraceRange{/*exists*/ getRA_Three(10, 0), /*primaryNo*/ getPriPInc(10), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Three(10, 4), /*primaryNo*/ getPriPInc(10), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 1U, /*fileOffset*/ TraceRange::missingFileOffset_}};

//
// Ranges/files availability #4
//
//     5 6 7 8 9
//     - - - - -
// 10: x x 1 1 x
// 11: x x 1 1 x
// 12: x x 1 1 x
// 13: x x 1 1 x
// 14: x x 1 1 x
// 15: 1 1 x x 1
// 16: 1 1 x x 1
// 17: 1 1 x x 1
// 18: 1 1 x x 1
// 19: 1 1 x x 1
// 20: 1 1 x x 1
//

TraceRangeTester::TraceAvaliabilityArray const
    TraceRangeTester::rangeAvailability_Four_{
        false, false, true, true, false,
        false, false, true, true, false,
        false, false, true, true, false,
        false, false, true, true, false,
        false, false, true, true, false,
        true, true, false, false, true,
        true, true, false, false, true,
        true, true, false, false, true,
        true, true, false, false, true,
        true, true, false, false, true,
        true, true, false, false, true};

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_ranges_availability_Four_{
        trace_range_map::TraceRange{/*exists*/ getRA_Four(0, 2), /*primaryNo*/ getPriPInc(0), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(1, 2), /*primaryNo*/ getPriPInc(1), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(2, 2), /*primaryNo*/ getPriPInc(2), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(3, 2), /*primaryNo*/ getPriPInc(3), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(4, 2), /*primaryNo*/ getPriPInc(4), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ 0U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},

        trace_range_map::TraceRange{/*exists*/ getRA_Four(5, 0), /*primaryNo*/ getPriPInc(5), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(6, 0), /*primaryNo*/ getPriPInc(6), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(7, 0), /*primaryNo*/ getPriPInc(7), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(8, 0), /*primaryNo*/ getPriPInc(8), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(9, 0), /*primaryNo*/ getPriPInc(9), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(10, 0), /*primaryNo*/ getPriPInc(10), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ 1U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 2U, /*fileOffset*/ 0U},

        trace_range_map::TraceRange{/*exists*/ getRA_Four(5, 4), /*primaryNo*/ getPriPInc(5), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ 2U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 1U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(6, 4), /*primaryNo*/ getPriPInc(6), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ 2U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 1U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(7, 4), /*primaryNo*/ getPriPInc(7), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ 2U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 1U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(8, 4), /*primaryNo*/ getPriPInc(8), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ 2U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 1U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(9, 4), /*primaryNo*/ getPriPInc(9), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ 2U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 1U, /*fileOffset*/ 0U},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(10, 4), /*primaryNo*/ getPriPInc(10), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ 2U, /*samplesCount*/ defaultSampleCount_, /*rangeCount*/ 1U, /*fileOffset*/ 0U}};

trace_range_map::TraceIndexStorage const
    TraceRangeTester::tis_ranges_absence_Four_{
        trace_range_map::TraceRange{/*exists*/ getRA_Four(0, 0), /*primaryNo*/ getPriPInc(0), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(0, 4), /*primaryNo*/ getPriPInc(0), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 1U, /*fileOffset*/ TraceRange::missingFileOffset_},

        trace_range_map::TraceRange{/*exists*/ getRA_Four(1, 0), /*primaryNo*/ getPriPInc(1), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(1, 4), /*primaryNo*/ getPriPInc(1), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 1U, /*fileOffset*/ TraceRange::missingFileOffset_},

        trace_range_map::TraceRange{/*exists*/ getRA_Four(2, 0), /*primaryNo*/ getPriPInc(2), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(2, 4), /*primaryNo*/ getPriPInc(2), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 1U, /*fileOffset*/ TraceRange::missingFileOffset_},

        trace_range_map::TraceRange{/*exists*/ getRA_Four(3, 0), /*primaryNo*/ getPriPInc(3), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(3, 4), /*primaryNo*/ getPriPInc(3), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 1U, /*fileOffset*/ TraceRange::missingFileOffset_},

        trace_range_map::TraceRange{/*exists*/ getRA_Four(4, 0), /*primaryNo*/ getPriPInc(4), /*secondaryNo*/ getSecPInc(0), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(4, 4), /*primaryNo*/ getPriPInc(4), /*secondaryNo*/ getSecPInc(4), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 1U, /*fileOffset*/ TraceRange::missingFileOffset_},

        trace_range_map::TraceRange{/*exists*/ getRA_Four(5, 2), /*primaryNo*/ getPriPInc(5), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(6, 2), /*primaryNo*/ getPriPInc(6), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(7, 2), /*primaryNo*/ getPriPInc(7), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(8, 2), /*primaryNo*/ getPriPInc(8), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(9, 2), /*primaryNo*/ getPriPInc(9), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_},
        trace_range_map::TraceRange{/*exists*/ getRA_Four(10, 2), /*primaryNo*/ getPriPInc(10), /*secondaryNo*/ getSecPInc(2), /*fileNo*/ TraceRange::missingFileNo_, /*samplesCount*/ 0U, /*rangeCount*/ 2U, /*fileOffset*/ TraceRange::missingFileOffset_}};

} // namespace segysdk::segyindex::tests
