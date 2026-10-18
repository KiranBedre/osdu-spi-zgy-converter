/*
Copyright 2021-2022 Schlumberger

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

#include "IndexBuilderCollatedData.h"
#include "IndexBuilderData.h"
#include "Range.h"
#include <memory>

namespace segysdk::segyindex
{

class GeometryCalculationData
{

private:
    Range<int> inlineRange_ = Range(std::numeric_limits<int>::max(), std::numeric_limits<int>::min());

    Range<int> inlineStepRange_ = Range(std::numeric_limits<int>::max(), std::numeric_limits<int>::min());

    int tracesPerInline_ = 1;
    Range<int> inlineTracesRange_ = Range(std::numeric_limits<int>::max(), std::numeric_limits<int>::min());

    Range<int> crosslineRange_ = Range(std::numeric_limits<int>::max(), std::numeric_limits<int>::min());
    Range<int> crosslineStepRange_ = Range(std::numeric_limits<int>::max(), std::numeric_limits<int>::min());

    int tracesPerCrossline_ = 1;
    Range<int> crosslineTracesRange_ = Range(std::numeric_limits<int>::max(), std::numeric_limits<int>::min());

    Range<int> sampleCountRange_ = Range(std::numeric_limits<int>::max(), std::numeric_limits<int>::min());
    Range<int> twfRange_ = Range(std::numeric_limits<int>::max(), std::numeric_limits<int>::min());

    Range<double> tofsRange_ = Range(std::numeric_limits<double>::max(), std::numeric_limits<double>::min());
    Range<double> traceDepthRange_ = Range(std::numeric_limits<double>::max(), std::numeric_limits<double>::min());
    Range<double> sampleRateRange_ = Range(std::numeric_limits<double>::max(), std::numeric_limits<double>::min());

    bool file_is_inline_sorted_{true};

public:
    Range<int> getInlineRange() const { return inlineRange_; }
    Range<int> getInlineStepRange() const { return inlineStepRange_; }
    Range<int> getInlineTracesRange() const { return inlineTracesRange_; }

    Range<int> getCrosslineRange() const { return crosslineRange_; }
    Range<int> getCrosslineStepRange() const { return crosslineStepRange_; }
    Range<int> getCrosslineTracesRange() const { return crosslineTracesRange_; }

    Range<int> getSampleCountRange() const { return sampleCountRange_; }
    Range<int> getTwfRange() const { return twfRange_; }

public:
    Range<double> getTofsRange() const { return tofsRange_; }
    Range<double> getTraceDepthRange() const { return traceDepthRange_; }
    Range<double> getSampleRateRange() const { return sampleRateRange_; }

public:
    bool IsRegular() const;
    bool IsInlineOrdered() const;

public:
    // Public methods
    void updateCalculatedRangeValues(const std::shared_ptr<IndexBuilderCollatedData>& collated);
    void updateCalculationData(IndexBuilderData const& traceData, bool onFirstTrace, int lastInline, int lastCrossline, bool isInlineSorted);
    void finalizeCalculationData();
};

} // namespace segysdk::segyindex
