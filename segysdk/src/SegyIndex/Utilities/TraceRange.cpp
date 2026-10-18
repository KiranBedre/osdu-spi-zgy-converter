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

#include "TraceRange.h"

#include "../SegyIO/Primitives.h"

#include <cstring>
#include <json/json.h>

namespace segysdk::segyindex
{
namespace trace_range_map
{

TraceRange::TraceRange() noexcept
{
    std::memset(this, 0x00, sizeof(TraceRange));
}

TraceRange::TraceRange(TraceRangeRecord const& record) noexcept
  : TraceRange(static_cast<bool>(record.exists_),
               record.primaryNo_,
               record.secondaryNo_,
               record.fileNo_,
               record.samplesCount_,
               record.rangeCount_,
               record.fileOffset_)
{
    /* EMPTY */
}

TraceRange::TraceRange(bool exists,
                       PrimaryIndex primaryNo,
                       SecondaryIndex secondaryNo,
                       size_t fileNo,
                       size_t samplesCount,
                       size_t rangeCount,
                       size_t fileOffset) noexcept
{
    exists_ = static_cast<uint8_t>(exists);
    primaryNo_ = primaryNo;
    secondaryNo_ = secondaryNo;
    fileNo_ = fileNo;
    samplesCount_ = samplesCount;
    rangeCount_ = rangeCount;
    fileOffset_ = fileOffset;
}

TraceRange::TraceRange(TraceRange const& other) noexcept
{
    *this = other;
}

TraceRange&
TraceRange::operator=(TraceRange const& other) noexcept
{
    std::memcpy(this, &other, sizeof(TraceRange));
    return *this;
}

TraceRange::TraceRange(TraceRange&& other) noexcept
{
    *this = other;
}

TraceRange&
TraceRange::operator=(TraceRange&& other) noexcept
{
    std::memcpy(this, &other, sizeof(TraceRange));
    return *this;
}

bool
TraceRange::operator==(TraceRange const& other) const noexcept
{
    return (exists_ == other.exists_
            && primaryNo_ == other.primaryNo_
            && secondaryNo_ == other.secondaryNo_
            && fileNo_ == other.fileNo_
            && samplesCount_ == other.samplesCount_
            && rangeCount_ == other.rangeCount_
            && fileOffset_ == other.fileOffset_);
}

bool
TraceRange::operator!=(TraceRange const& other) const noexcept
{
    return not(*this == other);
}

bool
TraceRange::isExist() const noexcept
{
    return static_cast<bool>(exists_);
}

PrimaryIndex
TraceRange::getPrimaryNo() const noexcept
{
    return primaryNo_;
}

SecondaryIndex
TraceRange::getSecondaryNo() const noexcept
{
    return secondaryNo_;
}

size_t
TraceRange::getFileNo() const noexcept
{
    return fileNo_;
}

size_t
TraceRange::getSamplesCount() const noexcept
{
    return samplesCount_;
}

size_t
TraceRange::getRangeCount() const noexcept
{
    return rangeCount_;
}

size_t
TraceRange::getFileOffset() const noexcept
{
    return fileOffset_;
}

TraceRangeRecord const&
TraceRange::getRecord() const
{
    return *static_cast<TraceRangeRecord const*>(this);
}

Json::Value
TraceRange::toJsonValue() const
{
    Json::Value root;

    // Json::Value doesn't directly support size_t ...
    root[fieldExists_] = static_cast<uint16_t>(exists_);
    root[fieldPrimaryNo_] = static_cast<Json::Int64>(primaryNo_);
    root[fieldSecondaryNo_] = static_cast<Json::Int64>(secondaryNo_);
    root[fieldFileNo_] = static_cast<Json::UInt64>(fileNo_);
    root[fieldSamplesCount_] = static_cast<Json::UInt64>(samplesCount_);
    root[fieldRangeCount_] = static_cast<Json::UInt64>(rangeCount_);
    root[fieldFileOffset_] = static_cast<Json::UInt64>(fileOffset_);

    return root;
}

std::string
TraceRange::toJsonString() const
{
    Json::Value const root = toJsonValue();

    return root.toStyledString().c_str();
}

bool
TraceRange::isHit(PrimaryIndex primaryNo, SecondaryIndex secondaryNo, IndexStep secStep) const noexcept
{
    if (primaryNo not_eq primaryNo_) {
        return false;
    }
    else {
        SecondaryIndex const rangePoint = secondaryNo_;
        SecondaryIndex const calcPoint = secondaryNo_ + secStep * static_cast<SecondaryIndex>(rangeCount_);
        if (secStep >= 0) {
            return (rangePoint <= secondaryNo && secondaryNo < calcPoint);
        }
        else {
            return (calcPoint < secondaryNo && secondaryNo <= rangePoint);
        }
    }
}

bool
TraceRange::isLiveTrace(PrimaryIndex primaryNo, SecondaryIndex secondaryNo, IndexStep secStep) const noexcept
{
    return exists_ && isHit(primaryNo, secondaryNo, secStep);
}

size_t
TraceRange::getFileOffset(PrimaryIndex primaryNo,
                          SecondaryIndex secondaryNo,
                          IndexStep secStep,
                          segysdk::segyio::SampleFormat sampleFormat) const noexcept
{
    return getFileOffset(primaryNo,
                         secondaryNo,
                         secStep,
                         segysdk::segyio::Primitives::getSampleSize(sampleFormat));
}

size_t
TraceRange::getFileOffset(PrimaryIndex primaryNo,
                          SecondaryIndex secondaryNo,
                          IndexStep secStep,
                          size_t sampleSize) const noexcept
{
    if (isLiveTrace(primaryNo, secondaryNo, secStep)) {
        size_t const traceSize = segysdk::segyio::Primitives::getTraceSize(sampleSize, samplesCount_);
        size_t const tracesCount = static_cast<size_t>((secondaryNo - secondaryNo_) / secStep);

        size_t const fileOffset = traceSize * tracesCount + fileOffset_;

        return fileOffset;
    }

    return missingFileOffset_;
}

} // namespace trace_range_map
} // namespace segysdk::segyindex
