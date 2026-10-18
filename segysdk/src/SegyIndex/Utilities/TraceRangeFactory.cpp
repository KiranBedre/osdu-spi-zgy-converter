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

#include "TraceRangeFactory.h"

#include "../Logger/Logger.h"

#include "../segysdk_details/SegyException.h"

#include <cstring>
#include <utility>

namespace segysdk::segyindex
{
namespace trace_range_map
{

TraceRangeFactory::TraceRangeFactory(FileLayoutSpecification fileLayout,
                                     std::shared_ptr<IndexBuilderCollatedData> const& collatedData,
                                     TraceIndexStorage& outputStorage)
  : TraceRangeFactoryBase(fileLayout, outputStorage),
    collatedData_{collatedData}
{
    checkParameters();

    init();

    checkVariables();
}

TraceRangeFactory::~TraceRangeFactory()
{
    finishLastPrimaryIndex();
}


void
TraceRangeFactory::checkParameters() const
{
    SEGYLOG_DEBUG("checkParameters:");
    
    if (not collatedData_) {
        throw SegyInvalidArgumentException("IndexBuilderCollatedData pointer is invalid.");
    }
    if (0 == primaryStep_) {
        throw SegyInvalidArgumentException("primaryStep cannot be zero.");
    }
    if (0 == secondaryStep_) {
        throw SegyInvalidArgumentException("secondaryStep cannot be zero.");
    }
    
    if (fileLayout_.isInlineOrdered not_eq collatedData_->InlineIsPrimary) {
        throw SegyInvalidArgumentException("fileLayout and collatedData have different primary index.");
    }

    SEGYLOG_DEBUG("checkParameters OK ");
}

void
TraceRangeFactory::init()
{
    firstPrimaryIndex_ = collatedData_->InlineIsPrimary
                             ? (primaryStep_ > 0 ? collatedData_->InlineNoRange.Min : collatedData_->InlineNoRange.Max)
                             : (primaryStep_ > 0 ? collatedData_->CrosslineNoRange.Min : collatedData_->CrosslineNoRange.Max);
    lastPrimaryIndex_ = collatedData_->InlineIsPrimary
                            ? (primaryStep_ > 0 ? collatedData_->InlineNoRange.Max : collatedData_->InlineNoRange.Min)
                            : (primaryStep_ > 0 ? collatedData_->CrosslineNoRange.Max : collatedData_->CrosslineNoRange.Min);
    firstSecondaryIndex_ = collatedData_->InlineIsPrimary
                               ? (secondaryStep_ > 0 ? collatedData_->CrosslineNoRange.Min : collatedData_->CrosslineNoRange.Max)
                               : (secondaryStep_ > 0 ? collatedData_->InlineNoRange.Min : collatedData_->InlineNoRange.Max);
    lastSecondaryIndex_ = collatedData_->InlineIsPrimary
                              ? (secondaryStep_ > 0 ? collatedData_->CrosslineNoRange.Max : collatedData_->CrosslineNoRange.Min)
                              : (secondaryStep_ > 0 ? collatedData_->InlineNoRange.Max : collatedData_->InlineNoRange.Min);
    primaryDimension_ = static_cast<size_t>((lastPrimaryIndex_ - firstPrimaryIndex_) / primaryStep_ + 1);
    secondaryDimension_ = static_cast<size_t>((lastSecondaryIndex_ - firstSecondaryIndex_) / secondaryStep_ + 1);

    posPrev_.clear();

    posPrev_.prii = firstPrimaryIndex_;
    posPrev_.seci = firstSecondaryIndex_;
    posPrev_.sampleCount = static_cast<size_t>(fileLayout_.sampleExtent.count());
    posPrev_.fileNo = TraceRange::missingFileNo_;
    posPrev_.fileOffset = collatedData_->FirstTraceOffset;

    posCurr_ = posPrev_;

    rangeCount_ = 0U;
}

} // namespace trace_range_map
} // namespace segysdk::segyindex
