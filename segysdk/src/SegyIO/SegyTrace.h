/*
Copyright 2021 Schlumberger

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

#include <cmath>
#include <cstddef>
#include <cstdint>
#include <limits>

namespace segysdk::segyio
{

class SEGYSDK_DLL_PUBLIC SegyTrace
{
private:
    uint64_t traceFileOffset_;
    uint64_t samplesFileOffset_;
    uint64_t samplesCount_;
    size_t bytesPerSample_;
    uint64_t seq_;
    double firstSampleAt_{std::numeric_limits<double>::quiet_NaN()};

public:
    SegyTrace(uint64_t seq, uint64_t traceFileOffset, uint64_t samplesFileOffset, uint64_t samplesCount, size_t bytesPerSample, double firstSampleAt);

    SegyTrace(uint64_t seq, uint64_t traceFileOffset, uint64_t samplesFileOffset, uint64_t samplesCount, size_t bytesPerSample);

    SegyTrace & operator=(const SegyTrace&) = delete;

    virtual ~SegyTrace();

    void setSamplesCount(uint64_t samplesCount);
    void setBytesPerSample(size_t bytesPerSample);

    uint64_t getBytesPerSample() const { return this->bytesPerSample_; }

    uint64_t getSamplesCount() const { return this->samplesCount_; }

    uint64_t getSamplesByteCount() const;

    uint64_t getSamplesFileOffset() const { return this->samplesFileOffset_; }

    uint64_t getTraceFileOffset() const { return this->traceFileOffset_; }

    uint64_t getSeq() const { return this->seq_; }

    double getFirstSampleAt() const { return this->firstSampleAt_; }

    void setFirstSampleAt(double value);

    bool operator==(const SegyTrace& other) const
    {
        return !(*this != other);
    }

    bool operator!=(const SegyTrace& other) const
    {
        if (this->traceFileOffset_ != other.traceFileOffset_)
            return true;

        if (this->samplesFileOffset_ != other.samplesFileOffset_)
            return true;

        if (this->samplesCount_ != other.samplesCount_)
            return true;

        if (this->bytesPerSample_ != other.bytesPerSample_)
            return true;

        if (this->seq_ != other.seq_)
            return true;

        const auto firstSampleIsNaN = std::isnan(this->firstSampleAt_);
        const auto otherFirstSampleIsNaN = std::isnan(other.firstSampleAt_);

        if (firstSampleIsNaN && !otherFirstSampleIsNaN) {
            return true;
        }
        else if (!firstSampleIsNaN && otherFirstSampleIsNaN) {
            return true;
        }
        // no-one is NaN
        else if (!firstSampleIsNaN && !otherFirstSampleIsNaN && this->firstSampleAt_ != other.firstSampleAt_) {
            return true;
        }

        return false;
    }
};

} // namespace segysdk::segyio
