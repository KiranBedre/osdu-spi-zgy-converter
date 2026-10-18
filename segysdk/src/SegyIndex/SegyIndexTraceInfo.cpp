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

#include "SegyIndexTraceInfo.h"

#include "../segysdk_details/SegyException.h"

#include <deque>
#include <limits.h>

namespace segysdk::segyindex
{
namespace trace_info
{

SegyIndexTraceInfo::SegyIndexTraceInfo(SegyIndexDataPtr& indexData)
  : indexData_{indexData}
{
    /* EMPTY */
}

Geometry3D
SegyIndexTraceInfo::getGeometry3D() const
{
    return indexData_->geometry3D_;
}

FileLayoutSpecification
SegyIndexTraceInfo::getFileLayout() const
{
    return indexData_->fileLayout_;
}

SeismicCubeExtents
SegyIndexTraceInfo::getGeometry3DExtents() const
{
    return SeismicCubeExtents(
        indexData_->geometry3D_.IVectorExtent,
        indexData_->geometry3D_.JVectorExtent,
        indexData_->geometry3D_.sampleExtent);
}

SeismicCubeExtents
SegyIndexTraceInfo::getFileLayoutExtents() const
{
    return SeismicCubeExtents(
        indexData_->fileLayout_.IExtent,
        indexData_->fileLayout_.JExtent,
        indexData_->fileLayout_.sampleExtent);
}

index_2d::SeismicLineExtents
SegyIndexTraceInfo::getLineExtents() const
{
    return indexData_->indexing2dInfo_.getLineExtents();
}

bool
SegyIndexTraceInfo::isRegular() const
{
    return indexData_->isRegular();
}

IndexField
SegyIndexTraceInfo::getIndexAt(int inlineNo, int crosslineNo) const
{
    if (indexData_->fileLayout_.isRegular) {
        return getCalculatedIndexAt(inlineNo, crosslineNo);
    }

    trace_range_map::PrimaryIndex const primaryIndex = static_cast<trace_range_map::PrimaryIndex>(indexData_->fileLayout_.isInlineOrdered
                                                                                                      ? inlineNo
                                                                                                      : crosslineNo);
    trace_range_map::SecondaryIndex const secondaryIndex = static_cast<trace_range_map::SecondaryIndex>(indexData_->fileLayout_.isInlineOrdered
                                                                                                            ? crosslineNo
                                                                                                            : inlineNo);
    trace_range_map::IndexStep const secondaryIndexStep = static_cast<trace_range_map::IndexStep>(indexData_->fileLayout_.isInlineOrdered
                                                                                                      ? indexData_->fileLayout_.IExtent.step()
                                                                                                      : indexData_->fileLayout_.JExtent.step());

    auto const itr = indexData_->traceIndexMap_.getRange(primaryIndex, secondaryIndex);
    if (itr.has_value()) {
        if (auto const& range = itr.value()->second.get(); range.isLiveTrace(primaryIndex, secondaryIndex, secondaryIndexStep)) {

            IndexField const indexField(range.getFileNo(),
                                        static_cast<int>(primaryIndex),
                                        static_cast<int>(secondaryIndex),
                                        range.getFileOffset(primaryIndex,
                                                            secondaryIndex,
                                                            secondaryIndexStep,
                                                            indexData_->sampleSize_));
            return indexField;
        }
    }

    return IndexField(/*segyFileNo*/ USHRT_MAX, /*inlineNo*/ 0, /*crosslineNo*/ 0, /*offset*/ 0);
}

IndexField
SegyIndexTraceInfo::getLiveIndexAt(int inlineNo, int crosslineNo) const
{
    if (indexData_->missingTraceIndexMap_.isEmpty()) {
        return getIndexAt(inlineNo, crosslineNo);
    }

    trace_range_map::PrimaryIndex const primaryIndex = static_cast<trace_range_map::PrimaryIndex>(indexData_->fileLayout_.isInlineOrdered
                                                                                                      ? inlineNo
                                                                                                      : crosslineNo);
    trace_range_map::SecondaryIndex const secondaryIndex = static_cast<trace_range_map::SecondaryIndex>(indexData_->fileLayout_.isInlineOrdered
                                                                                                            ? crosslineNo
                                                                                                            : inlineNo);
    trace_range_map::IndexStep const secondaryIndexStep = static_cast<trace_range_map::IndexStep>(indexData_->fileLayout_.isInlineOrdered
                                                                                                      ? indexData_->fileLayout_.IExtent.step()
                                                                                                      : indexData_->fileLayout_.JExtent.step());

    auto const itr = indexData_->missingTraceIndexMap_.getRange(primaryIndex, secondaryIndex);
    if (itr.has_value()) {
        if (auto const& range = itr.value()->second.get(); range.isLiveTrace(primaryIndex, secondaryIndex, secondaryIndexStep)) {

            IndexField const indexField(range.getFileNo(),
                                        static_cast<int>(primaryIndex),
                                        static_cast<int>(secondaryIndex),
                                        range.getFileOffset(primaryIndex,
                                                            secondaryIndex,
                                                            secondaryIndexStep,
                                                            indexData_->sampleSize_));
            return indexField;
        }
    }

    return IndexField(/*segyFileNo*/ USHRT_MAX, /*inlineNo*/ 0, /*crosslineNo*/ 0, /*offset*/ 0);
}

size_t
SegyIndexTraceInfo::getOffsetForTraceNo(size_t traceNo) const
{
    if (indexData_->is2D()) {
        return indexData_->indexing2dInfo_.getFileOffsetForTrace(traceNo);
    }
    return 0;
}

size_t
SegyIndexTraceInfo::getOffsetForCMP(int CMP) const
{
    if (indexData_->is2D()) {
        return indexData_->indexing2dInfo_.getFileOffsetForCMP(CMP);
    }
    return 0;
}

size_t
SegyIndexTraceInfo::getTraceNoForCMP(int CMP, bool findNextClosest) const
{
    if (indexData_->is2D()) {
        return indexData_->indexing2dInfo_.getTraceNoForCMP(CMP, findNextClosest);
    }
    return 0;
}

size_t
SegyIndexTraceInfo::getSampleSize() const
{

    return indexData_->sampleSize_;
}

size_t
SegyIndexTraceInfo::getTraceSize() const
{
    const size_t traceDataSize = static_cast<size_t>(indexData_->sampleSize_) * indexData_->traceLength_;
    return indexData_->traceHeaderSize_ + traceDataSize;
}

float
SegyIndexTraceInfo::getMinimumAmplitude() const
{
    return indexData_->amplitudeMinimum_;
}

float
SegyIndexTraceInfo::getMaximumAmplitude() const
{
    return indexData_->amplitudeMaximum_;
}

size_t
SegyIndexTraceInfo::getFirstTraceOffset(uint16_t fileId) const
{
    if (!indexData_->segyFilesIndexed_.empty() && fileId < indexData_->segyFilesIndexed_.size()) {
        return indexData_->segyFilesIndexed_[fileId].FirstTraceOffset;
    }

    return 0;
}

bool
SegyIndexTraceInfo::isInlineOrdered() const
{
    return indexData_->fileLayout_.isInlineOrdered;
}

bool
SegyIndexTraceInfo::hasConstantCoordinateScalar() const
{
    return indexData_->hasConstantCoordinateScalar_;
}

bool
SegyIndexTraceInfo::hasConstantTraceLength() const
{
    return indexData_->hasConstantTraceLength_;
}

bool
SegyIndexTraceInfo::hasConstantTraceWeightingFactor() const
{
    return indexData_->hasConstantTraceWeightingFactor_;
}

int
SegyIndexTraceInfo::getConstantCoordinateScalar() const
{
    return indexData_->hasConstantCoordinateScalar_ ? indexData_->constantCoordinateScalar_ : 1;
}

size_t
SegyIndexTraceInfo::getConstantTraceLength() const
{
    return indexData_->hasConstantTraceLength_ ? indexData_->constantTraceLength_ : 0U;
}

unsigned short
SegyIndexTraceInfo::getConstantTraceWeightingFactor() const
{
    return indexData_->hasConstantTraceWeightingFactor_ ? indexData_->constantTraceWeightingFactor_ : static_cast<unsigned short>(1);
}

size_t
SegyIndexTraceInfo::getIndexedTraceCount() const
{
    return indexData_->indexedTraceCount_;
}

IndexField
SegyIndexTraceInfo::getCalculatedIndexAt(int inlineNo, int crosslineNo) const
{
    // EDahl2 ToDo:
    // Should we be able to handle old Charisma cubes where there are EBCDIC/Binary header between inlines?

    const int primary = indexData_->fileLayout_.isInlineOrdered ? inlineNo : crosslineNo;
    const int secondary = indexData_->fileLayout_.isInlineOrdered ? crosslineNo : inlineNo;
    const uint16_t fileNo = getIndexFileNo(primary);

    const size_t traceOffsetCount = calculateTraceOffsetCount(fileNo, primary, secondary);
    const size_t traceSize = getTraceSize();

    const size_t offset = indexData_->offsetFirtstSEGYTrace_ + (traceOffsetCount * traceSize);

    return IndexField(fileNo, inlineNo, crosslineNo, offset);
}

uint16_t
SegyIndexTraceInfo::getIndexFileNo(int primaryIndex) const
{
    if (indexData_->segyFilesIndexed_.empty()) {
        return 0;
    }

    const auto lookup = indexData_->multiFileMap_.find(primaryIndex);
    if (lookup != indexData_->multiFileMap_.end()) {
        return lookup->second;
    }

    return 0;
}

size_t
SegyIndexTraceInfo::calculateTraceOffsetCount(uint16_t fileNo, int inlineNo, int crosslineNo) const
{
    size_t const inlineCount = getInlinesToRequestedInLine(fileNo, inlineNo);
    size_t const crosslineCount = getCrosslinesToRequestedCrossline(fileNo, crosslineNo);

    if (indexData_->fileLayout_.isInlineOrdered) {
        size_t const tracesPerLine
            = static_cast<size_t>((indexData_->fileLayout_.IExtent.last() - indexData_->fileLayout_.IExtent.first()) / indexData_->fileLayout_.IExtent.step() + 1);
        return inlineCount * tracesPerLine + crosslineCount;
    }
    else {
        size_t const tracesPerCrossline
            = static_cast<size_t>((indexData_->fileLayout_.JExtent.last() - indexData_->fileLayout_.JExtent.first()) / indexData_->fileLayout_.JExtent.step() + 1);
        return crosslineCount * tracesPerCrossline + inlineCount;
    }
}

size_t
SegyIndexTraceInfo::getInlinesToRequestedInLine(uint16_t fileNo, int inlineNo) const
{

    if (indexData_->fileLayout_.JExtent.step() == 0) {
        throw SegyInvalidArgumentException("getInlinesToRequestedInLine: Inline increment cannot be 0!");
    }

    if (fileNo == 0 || !indexData_->fileLayout_.isInlineOrdered) {
        return static_cast<size_t>((inlineNo - indexData_->fileLayout_.JExtent.first()) / indexData_->fileLayout_.JExtent.step());
    }

    return static_cast<size_t>((indexData_->fileLayout_.JExtent.step() > 0)
                                   ? (inlineNo - indexData_->segyFilesIndexed_[fileNo].PrimaryKeyRange.Min) / indexData_->fileLayout_.JExtent.step()
                                   : (inlineNo - indexData_->segyFilesIndexed_[fileNo].PrimaryKeyRange.Max) / indexData_->fileLayout_.JExtent.step());
}

size_t
SegyIndexTraceInfo::getCrosslinesToRequestedCrossline(uint16_t fileNo, int crosslineNo) const
{

    if (indexData_->fileLayout_.IExtent.step() == 0) {
        throw SegyInvalidArgumentException("getCrosslinesToRequestedCrossline: Crossline increment cannot be 0!");
    }

    if (fileNo == 0 || indexData_->fileLayout_.isInlineOrdered) {
        return static_cast<size_t>((crosslineNo - indexData_->fileLayout_.IExtent.first()) / indexData_->fileLayout_.IExtent.step());
    }

    return static_cast<size_t>((indexData_->fileLayout_.IExtent.step() > 0)
                                   ? (crosslineNo - indexData_->segyFilesIndexed_[fileNo].PrimaryKeyRange.Min) / indexData_->fileLayout_.IExtent.step()
                                   : (crosslineNo - indexData_->segyFilesIndexed_[fileNo].PrimaryKeyRange.Max) / indexData_->fileLayout_.IExtent.step());
}

} // namespace trace_info
} // namespace segysdk::segyindex
