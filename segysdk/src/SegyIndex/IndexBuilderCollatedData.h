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

#include "../SegyIndex/IndexBuilderDataManager.h"
#include "../SegyIndex/Range.h"

#include <memory>
#include <string>
#include <vector>

namespace segysdk::segyindex
{

struct SEGYSDK_DLL_PUBLIC IndexBuilderCollatedData
{

    std::shared_ptr<IndexBuilderDataManager> traceDataManager;

    std::string SegyFileName;

    size_t FirstTraceOffset;

    size_t LastTraceNo;

    bool InlineIsPrimary;

    // FileReferenceLookup works with Primary index i.e. either inline or crossline , not necessarily always inline values.
    std::vector<Range<int>> FileReferenceLookup;

    Range<int> InlineNoRange;

    Range<int> CrosslineNoRange;

    Range<int> CMPRange;

    Range<double> TofsRange;

    Range<double> TraceDepthRange;

    Range<uint16_t> TraceLengthRange;

    Range<double> SampleRateRange;

    Range<float> SampleAmplitudeRange;

    Range<int> CoordinateScalarRange;

    Range<int> TraceWeightingFactorRange;
};

} // namespace segysdk::segyindex
