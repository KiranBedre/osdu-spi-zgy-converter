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

#include "SegyTrace.h"

#pragma warning(push)
#pragma warning(disable : 4995) // 'function': name was marked as #pragma deprecated
#include <limits>
#pragma warning(pop)

namespace segysdk::segyio
{

SegyTrace::SegyTrace(uint64_t seq,
                     uint64_t traceFileOffset,
                     uint64_t samplesFileOffset,
                     uint64_t samplesCount,
                     size_t bytesPerSample,
                     double firstSampleAt)
  : traceFileOffset_(traceFileOffset),
    samplesFileOffset_(samplesFileOffset),
    samplesCount_(samplesCount),
    bytesPerSample_(bytesPerSample),
    seq_(seq),
    firstSampleAt_(firstSampleAt)
{
}

SegyTrace::SegyTrace(uint64_t seq,
                     uint64_t traceFileOffset,
                     uint64_t samplesFileOffset,
                     uint64_t samplesCount,
                     size_t bytesPerSample)
  : traceFileOffset_(traceFileOffset),
    samplesFileOffset_(samplesFileOffset),
    samplesCount_(samplesCount),
    bytesPerSample_(bytesPerSample),
    seq_(seq)
{
}

SegyTrace::~SegyTrace()
{}


uint64_t
SegyTrace::getSamplesByteCount() const
{
    return this->samplesCount_ * this->bytesPerSample_;
}

void
SegyTrace::setSamplesCount(uint64_t samplesCount)
{
    this->samplesCount_ = samplesCount;
}

void
SegyTrace::setFirstSampleAt(double value)
{
    this->firstSampleAt_ = value;
}

void
SegyTrace::setBytesPerSample(size_t bytesPerSample)
{
    this->bytesPerSample_ = bytesPerSample;
}

} // namespace segysdk::segyio
