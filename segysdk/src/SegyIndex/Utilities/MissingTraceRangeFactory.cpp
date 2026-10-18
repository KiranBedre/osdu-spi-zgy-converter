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

#include "MissingTraceRangeFactory.h"

#include "../Logger/Logger.h"

#include "../segysdk_details/SegyException.h"

#include <cstring>
#include <utility>

namespace segysdk::segyindex
{
namespace trace_range_map
{


//-----------------------------------------------------------------------------
//        MissingTraceRangeFactory class
//-----------------------------------------------------------------------------

MissingTraceRangeFactory::MissingTraceRangeFactory(FileLayoutSpecification fileLayout,
                                                   segyindex::trace_range_map::TraceIndexStorage& outputStorage)
  : TraceRangeFactoryBase(fileLayout, outputStorage)
{
    checkParameters();

    init();

    checkVariables();
}

MissingTraceRangeFactory::~MissingTraceRangeFactory()
{
    finishLastPrimaryIndex();
}

void
MissingTraceRangeFactory::checkParameters() const
{
    SEGYLOG_DEBUG("checkParameters:");

    if (0 == primaryStep_) {
        throw SegyInvalidArgumentException("primaryStep cannot be zero.");
    }
    if (0 == secondaryStep_) {
        throw SegyInvalidArgumentException("secondaryStep cannot be zero.");
    }

    SEGYLOG_DEBUG("checkParameters OK ");
}

void
MissingTraceRangeFactory::init()
{
    // fileLayout is using JExtent to hold the inline range. IExtent holds the crossline range!!
    // fileLayout is organized like the file, so IExtent.first() and JExtent.first() will hold the
    // correct value of inline- and crossline index
    firstPrimaryIndex_ = fileLayout_.isInlineOrdered
                             ? fileLayout_.JExtent.first()
                             : fileLayout_.IExtent.first();
    lastPrimaryIndex_ = fileLayout_.isInlineOrdered
                            ? fileLayout_.JExtent.last()
                            : fileLayout_.IExtent.last();
    firstSecondaryIndex_ = fileLayout_.isInlineOrdered
                               ? fileLayout_.IExtent.first()
                               : fileLayout_.JExtent.first();
    lastSecondaryIndex_ = fileLayout_.isInlineOrdered
                              ? fileLayout_.IExtent.last()
                              : fileLayout_.JExtent.last();
    primaryDimension_ = static_cast<size_t>((lastPrimaryIndex_ - firstPrimaryIndex_) / primaryStep_ + 1);
    secondaryDimension_ = static_cast<size_t>((lastSecondaryIndex_ - firstSecondaryIndex_) / secondaryStep_ + 1);

    posPrev_.clear();

    posPrev_.prii = firstPrimaryIndex_;
    posPrev_.seci = firstSecondaryIndex_;
    posPrev_.sampleCount = static_cast<size_t>(fileLayout_.sampleExtent.count());
    posPrev_.fileNo = segysdk::segyindex::trace_range_map::TraceRange::missingFileNo_;
    posPrev_.fileOffset = 0;

    posCurr_ = posPrev_;

    rangeCount_ = 0U;
}

} // namespace missing_trace_range_map
} // namespace segysdk::segyindex
