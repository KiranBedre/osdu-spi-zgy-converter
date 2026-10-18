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

#include "GeometryCalculationData.h"

namespace segysdk::segyindex
{

void
GeometryCalculationData::updateCalculatedRangeValues(const std::shared_ptr<IndexBuilderCollatedData>& collated)
{

    inlineRange_.UpdateMinMax(collated->InlineNoRange.Min);
    inlineRange_.UpdateMinMax(collated->InlineNoRange.Max);

    crosslineRange_.UpdateMinMax(collated->CrosslineNoRange.Min);
    crosslineRange_.UpdateMinMax(collated->CrosslineNoRange.Max);

    sampleCountRange_.UpdateMinMax(collated->TraceLengthRange.Min);
    sampleCountRange_.UpdateMinMax(collated->TraceLengthRange.Max);

    twfRange_.UpdateMinMax(collated->TraceWeightingFactorRange.Min);
    twfRange_.UpdateMinMax(collated->TraceWeightingFactorRange.Max);

    tofsRange_.UpdateMinMax(collated->TofsRange.Min);
    tofsRange_.UpdateMinMax(collated->TofsRange.Max);

    traceDepthRange_.UpdateMinMax(collated->TraceDepthRange.Min);
    traceDepthRange_.UpdateMinMax(collated->TraceDepthRange.Max);

    sampleRateRange_.UpdateMinMax(collated->SampleRateRange.Min);
    sampleRateRange_.UpdateMinMax(collated->SampleRateRange.Max);
}

void
GeometryCalculationData::updateCalculationData(IndexBuilderData const& traceData, bool onFirstTrace, int lastInline, int lastCrossline, bool isInlineSorted)
{

    const int currentInline = traceData.InlineNo;
    const int currentCrossline = traceData.CrosslineNo;

    if (onFirstTrace) {
        inlineRange_.UpdateMinMax(currentInline);
        crosslineRange_.UpdateMinMax(currentCrossline);
        tracesPerInline_ = 1;
        tracesPerCrossline_ = 1;
        file_is_inline_sorted_ = isInlineSorted;
        return;
    }

    if (isInlineSorted) {
        // Traces per inline should be updated at the end of each inline
        // Traces per crossline needs to scan the entire file to find first and last inline value
        if (currentCrossline != lastCrossline) {

            crosslineRange_.UpdateMinMax(currentCrossline);

            if (currentInline == lastInline) {
                const int crosslineStep = currentCrossline - lastCrossline;
                crosslineStepRange_.UpdateMinMax(crosslineStep);

                tracesPerInline_++;
            }

            crosslineTracesRange_.UpdateMinMax(tracesPerCrossline_);
        }
        else {
            tracesPerCrossline_++;
        }

        if (currentInline != lastInline) {

            inlineRange_.UpdateMinMax(currentInline);

            const int inlineStep = currentInline - lastInline;
            inlineStepRange_.UpdateMinMax(inlineStep);

            inlineTracesRange_.UpdateMinMax(tracesPerInline_);

            tracesPerInline_ = 1;
        }
    }
    else {
        // Crossline sorted
        // Traces per crossline should be updated at the end of each crossline
        // Traces per inline needs to scan the entire file to find first and last crossline value
        if (currentInline != lastInline) {

            inlineRange_.UpdateMinMax(currentInline);

            if (currentCrossline == lastCrossline) {
                const int inlineStep = currentInline - lastInline;
                inlineStepRange_.UpdateMinMax(inlineStep);

                tracesPerCrossline_++;
            }

            inlineTracesRange_.UpdateMinMax(tracesPerInline_);
        }
        else {
            tracesPerInline_++;
        }

        if (currentCrossline != lastCrossline) {

            crosslineRange_.UpdateMinMax(currentCrossline);

            const int crosslineStep = currentCrossline - lastCrossline;
            crosslineStepRange_.UpdateMinMax(crosslineStep);

            crosslineTracesRange_.UpdateMinMax(tracesPerCrossline_);

            tracesPerCrossline_ = 1;
        }
    }
}

void
GeometryCalculationData::finalizeCalculationData()
{
    if (tracesPerInline_ > 1) {
        inlineTracesRange_.UpdateMinMax(tracesPerInline_);
        tracesPerInline_ = 1;
    }
    if (tracesPerCrossline_ > 1) {
        crosslineTracesRange_.UpdateMinMax(tracesPerCrossline_);
        tracesPerCrossline_ = 1;
    }
}

bool
GeometryCalculationData::IsRegular() const
{
    return inlineStepRange_.Delta() == 0 && crosslineStepRange_.Delta() == 0 && inlineRange_.Delta() >= 2 && crosslineRange_.Delta() >= 2 && inlineTracesRange_.Delta() == 0 && crosslineTracesRange_.Delta() == 0 && sampleCountRange_.Delta() == 0;
}

bool
GeometryCalculationData::IsInlineOrdered() const
{
    return file_is_inline_sorted_;
}
} // namespace segysdk::segyindex
