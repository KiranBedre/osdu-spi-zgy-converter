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

#include "TraceRangeFactoryBase.h"

#include "../Logger/Logger.h"

#include "../segysdk_details/SegyException.h"

#include <cstring>
#include <utility>

namespace segysdk::segyindex
{
namespace trace_range_map
{

//-----------------------------------------------------------------------------
//        TraceRangeFactoryBase::RangeTrackParameters class
//-----------------------------------------------------------------------------

TraceRangeFactoryBase::PositionTrackParameters::PositionTrackParameters()
{
    clear();
}

void
TraceRangeFactoryBase::PositionTrackParameters::clear()
{
    std::memset(this, 0x00, sizeof(PositionTrackParameters));
}

//-----------------------------------------------------------------------------
//        TraceRangeFactoryBase class
//-----------------------------------------------------------------------------

TraceRangeFactoryBase::TraceRangeFactoryBase(FileLayoutSpecification fileLayout,
                                     TraceIndexStorage& outputStorage)
  : fileLayout_{std::move(fileLayout)},
    outputStorage_{outputStorage},
    primaryStep_{TraceRangeFactoryBase::getPrimaryExtentStep(fileLayout_)},
    secondaryStep_{TraceRangeFactoryBase::getSecondaryExtentStep(fileLayout_)}
{
}

void
TraceRangeFactoryBase::addTrace(PrimaryIndex primaryIndex,
                            SecondaryIndex secondaryIndex,
                            size_t sampleCount,
                            size_t fileNo,
                            size_t fileOffset)
{
    posCurr_.clear();
    posCurr_.prii = primaryIndex;
    posCurr_.seci = secondaryIndex;
    posCurr_.sampleCount = sampleCount;
    posCurr_.fileNo = fileNo;
    posCurr_.fileOffset = fileOffset;

    if (isSameRange()) {
        // increment number of matching the current range traces
        ++rangeCount_;

        enumerateRangeItem();
    }
    else {
        //
        // the current range is to be terminated
        //
        if (isSamePrimaryIndex()) {
            if (isMissingSecondaryIndex()) {

                // first, store the current range record
                storeCurrentRange();

                {
                    // second, store the missing secondary range record
                    size_t const rangeCount = getSecondaryDistance();

                    storeMissingRange(posPrev_.prii,
                                      posPrev_.seci + secondaryStep_ * static_cast<PrimaryIndex>(rangeCount_),
                                      rangeCount);
                }
            }
            else if (not isSameSampleCount()) {
                // store the current range record
                storeCurrentRange();
            }
            else if (not isSameFileNo()) {
                // store the current range record
                storeCurrentRange();
            }
            else {
                // the default action, store the current range record
                storeCurrentRange();
            }
        }
        else if (isNextPrimaryIndex()) {

            // first, store the current range record
            storeCurrentRange();

            // second, store the missing secondary remaining range record
            if (isSecondaryMissingRemainings()) {
                size_t const rangeCount = getSecondaryMissingRemainings();

                storeMissingRange(posPrev_.prii,
                                  posPrev_.seci + secondaryStep_ * static_cast<PrimaryIndex>(rangeCount_),
                                  rangeCount);
            }

            // third, store the missing secondary origin range record
            if (posCurr_.seci != firstSecondaryIndex_) {
                size_t const rangeCount = getSecondaryMissingOrigin();

                storeMissingRange(posCurr_.prii,
                                  firstSecondaryIndex_,
                                  rangeCount);
            }
        }
        else if (isMissingPrimaryIndex()) {
            // first, store the current record
            storeCurrentRange();

            // second, store the missing remaining range record
            if (posPrev_.seci != lastSecondaryIndex_) {
                size_t const rangeCount = getSecondaryMissingRemainings();

                storeMissingRange(posPrev_.prii,
                                  posPrev_.seci + secondaryStep_ * static_cast<PrimaryIndex>(rangeCount_),
                                  rangeCount);
            }

            {
                // third, store the missing primary range record(s)
                size_t const primaryRangeCount = getPrimaryDistance();

                if (primaryRangeCount > 0U) {
                    for (size_t i = 1U; i <= primaryRangeCount; ++i) {
                        storeMissingRange(posPrev_.prii + static_cast<PrimaryIndex>(primaryStep_ * i),
                                          firstSecondaryIndex_,
                                          secondaryDimension_);
                    }
                }
            }

            // third, store the missing secondary origin range record
            if (posCurr_.prii != firstSecondaryIndex_) {
                size_t const rangeCount = getSecondaryMissingOrigin();

                storeMissingRange(posCurr_.prii,
                                  firstSecondaryIndex_,
                                  rangeCount);
            }
        }
        else {
            // the default action, store the current range record
            storeCurrentRange();
        }

        enumerateNextRange();
    }
}

PrimaryIndex
TraceRangeFactoryBase::getPrimaryExtentIndexFirst(FileLayoutSpecification const& fileLayout)
{
    return fileLayout.isInlineOrdered
               ? fileLayout.JExtent.first()
               : fileLayout.IExtent.first();
}

PrimaryIndex
TraceRangeFactoryBase::getPrimaryExtentIndexLast(FileLayoutSpecification const& fileLayout)
{
    return fileLayout.isInlineOrdered
               ? fileLayout.JExtent.last()
               : fileLayout.IExtent.last();
}

SecondaryIndex
TraceRangeFactoryBase::getSecondaryExtentIndexFirst(FileLayoutSpecification const& fileLayout)
{
    return fileLayout.isInlineOrdered
               ? fileLayout.IExtent.first()
               : fileLayout.JExtent.first();
}

SecondaryIndex
TraceRangeFactoryBase::getSecondaryExtentIndexLast(FileLayoutSpecification const& fileLayout)
{
    return fileLayout.isInlineOrdered
               ? fileLayout.IExtent.last()
               : fileLayout.JExtent.last();
}

IndexStep
TraceRangeFactoryBase::getPrimaryExtentStep(FileLayoutSpecification const& fileLayout)
{
    return fileLayout.isInlineOrdered
               ? fileLayout.JExtent.step()
               : fileLayout.IExtent.step();
}

IndexStep
TraceRangeFactoryBase::getSecondaryExtentStep(FileLayoutSpecification const& fileLayout)
{
    return fileLayout.isInlineOrdered
               ? fileLayout.IExtent.step()
               : fileLayout.JExtent.step();
}

void
TraceRangeFactoryBase::checkVariables() const
{
    auto const primaryCollatedStepSign = lastPrimaryIndex_ - firstPrimaryIndex_;

    auto const secondaryCollatedStepSign = lastSecondaryIndex_ - firstSecondaryIndex_;

    if (0 == primaryCollatedStepSign) {
        throw SegyInvalidArgumentException("primaryCollatedStep cannot be zero.");
    }
    if (0 == secondaryCollatedStepSign) {
        throw SegyInvalidArgumentException("secondaryCollatedStep cannot be zero.");
    }

    auto sameSign = [](auto one, auto two) -> bool {
        return (one >= 0 and two >= 0) or (one < 0 and two < 0);
    };

    // Note: it is allowed for the primary and secondary indexes' steps to have either same or opposite signs

    // each input file should have the same primary index's step sign
    if (not sameSign(primaryStep_, primaryCollatedStepSign)) {
        throw SegyInvalidArgumentException("primaryStep and primaryCollatedStep cannot have opposite sign.");
    }

    // each input file should have the same secondary index's step sign
    if (not sameSign(secondaryStep_, secondaryCollatedStepSign)) {
        throw SegyInvalidArgumentException("secondaryStep and secondaryCollatedStep cannot have opposite sign.");
    }
}

bool
TraceRangeFactoryBase::isSameRange() const
{
    return isSamePrimaryIndex()
           && isNextSecondaryIndex()
           && isSameSampleCount()
           && isSameFileNo();
}

bool
TraceRangeFactoryBase::isSamePrimaryIndex() const
{
    return (posPrev_.prii == posCurr_.prii);
}

bool
TraceRangeFactoryBase::isNextPrimaryIndex() const
{
    return (posPrev_.prii + primaryStep_ == posCurr_.prii);
}

bool
TraceRangeFactoryBase::isNextSecondaryIndex() const
{
    return (posPrev_.seci + secondaryStep_ * static_cast<SecondaryIndex>(rangeCount_)
            == posCurr_.seci);
}

bool
TraceRangeFactoryBase::isMissingPrimaryIndex() const
{
    return (primaryStep_ > 0)
               ? (posPrev_.prii + primaryStep_ < posCurr_.prii)
               : (posPrev_.prii + primaryStep_ > posCurr_.prii);
}

bool
TraceRangeFactoryBase::isMissingSecondaryIndex() const
{
    SecondaryIndex const nextSeciOffset = secondaryStep_ * static_cast<SecondaryIndex>(rangeCount_);
    SecondaryIndex const nextSeci = posPrev_.seci + nextSeciOffset;

    return (nextSeci not_eq posCurr_.seci);
}

bool
TraceRangeFactoryBase::isSameSampleCount() const
{
    return (posPrev_.sampleCount == posCurr_.sampleCount);
}

bool
TraceRangeFactoryBase::isSameFileNo() const
{
    return (posPrev_.fileNo == posCurr_.fileNo);
}

bool
TraceRangeFactoryBase::isSecondaryMissingRemainings() const
{
    SecondaryIndex lastSeci = posPrev_.seci;
    if (rangeCount_ > 0U) {
        SecondaryIndex const lastSeciOffset = secondaryStep_ * static_cast<SecondaryIndex>(rangeCount_ - 1U);
        lastSeci = posPrev_.seci + lastSeciOffset;
    }
    return lastSeci != lastSecondaryIndex_;
}

size_t
TraceRangeFactoryBase::getPrimaryDistance() const
{
    return static_cast<size_t>((posCurr_.prii - posPrev_.prii) / primaryStep_ - 1);
}

size_t
TraceRangeFactoryBase::getPrimaryMissingOrigin() const
{
    return static_cast<size_t>((posCurr_.prii - firstPrimaryIndex_) / primaryStep_);
}

size_t
TraceRangeFactoryBase::getPrimaryMissingRemainings() const
{
    return static_cast<size_t>((lastPrimaryIndex_ - posPrev_.prii) / primaryStep_);
}

size_t
TraceRangeFactoryBase::getSecondaryDistance() const
{
    SecondaryIndex lastSeci = posPrev_.seci;
    if (rangeCount_ > 0U) {
        SecondaryIndex const lastSeciOffset = secondaryStep_ * static_cast<SecondaryIndex>(rangeCount_ - 1U);
        lastSeci = posPrev_.seci + lastSeciOffset;
        return static_cast<size_t>((posCurr_.seci - lastSeci) / secondaryStep_ - 1);
    }
    else {
        return static_cast<size_t>((posCurr_.seci - lastSeci) / secondaryStep_);
    }
}

size_t
TraceRangeFactoryBase::getSecondaryMissingOrigin() const
{
    return static_cast<size_t>((posCurr_.seci - firstSecondaryIndex_) / secondaryStep_);
}

size_t
TraceRangeFactoryBase::getSecondaryMissingRemainings() const
{
    SecondaryIndex lastSeci = posPrev_.seci;
    if (rangeCount_ > 0U) {
        SecondaryIndex const lastSeciOffset = secondaryStep_ * static_cast<SecondaryIndex>(rangeCount_ - 1);
        lastSeci += lastSeciOffset;
    }
    return static_cast<size_t>((lastSecondaryIndex_ - lastSeci) / secondaryStep_);
}

void
TraceRangeFactoryBase::storeCurrentRange()
{
    if (rangeCount_ > 0U) {
        TraceRange currentTraceRange(/*exists*/ true,
                                     posPrev_.prii,
                                     posPrev_.seci,
                                     posPrev_.fileNo,
                                     posPrev_.sampleCount,
                                     rangeCount_,
                                     posPrev_.fileOffset);

        outputStorage_.addRange(std::move(currentTraceRange));
    }
}

void
TraceRangeFactoryBase::storeMissingRange(PrimaryIndex prii,
                                     SecondaryIndex seci,
                                     size_t rangeCount)
{
    if (rangeCount > 0U) {
        TraceRange missingTraceRange(/*exists*/ false,
                                     prii,
                                     seci,
                                     TraceRange::missingFileNo_,
                                     /*samplesCount*/ 0U,
                                     rangeCount,
                                     TraceRange::missingFileOffset_);

        outputStorage_.addRange(std::move(missingTraceRange));
    }
}

void
TraceRangeFactoryBase::finishLastPrimaryIndex()
{
    // first, store the current range record
    storeCurrentRange();

    // second, store the missing secondary remaining range record
    if (posPrev_.seci != lastSecondaryIndex_) {
        size_t const rangeCount = getSecondaryMissingRemainings();

        storeMissingRange(posPrev_.prii,
                          posPrev_.seci + secondaryStep_ * static_cast<PrimaryIndex>(rangeCount_),
                          rangeCount);
    }

    // second, store the missing primary remaining range record(s)
    if (posPrev_.prii != lastPrimaryIndex_) {
        size_t const rangeCount = getPrimaryMissingRemainings();

        for (size_t i = 1U; i <= rangeCount; ++i) {
            storeMissingRange(posPrev_.prii + static_cast<PrimaryIndex>(primaryStep_ * i),
                              firstSecondaryIndex_,
                              secondaryDimension_);
        }
    }
}

void
TraceRangeFactoryBase::enumerateRangeItem()
{
    // prevent any changes of the next items:
    PrimaryIndex const prii = posPrev_.prii;
    SecondaryIndex const seci = posPrev_.seci;
    size_t const offset = posPrev_.fileOffset; // save the file offset

    posPrev_ = posCurr_;

    // restore the saved items
    posPrev_.prii = prii;
    posPrev_.seci = seci;
    posPrev_.fileOffset = offset;
}

void
TraceRangeFactoryBase::enumerateNextRange()
{
    // current params set will be the initial for the ongoing range
    posPrev_ = posCurr_;

    // set to 1 to state explicitly that there is already one item of the next ongoing range
    rangeCount_ = 1U;
}

} // namespace trace_range_map
} // namespace segysdk::segyindex
