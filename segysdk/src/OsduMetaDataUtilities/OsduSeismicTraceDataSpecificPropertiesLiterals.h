/*
Copyright 2023 Schlumberger

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

namespace segysdk::metadata::osdu
{

class SEGYSDK_DLL_PUBLIC OsduSeismicTraceDataSpecificPropertiesLiterals
{
public:
    // osdu:SeismicTraceData private data properties
    inline static constexpr const char* const LiveTraceOutline{"LiveTraceOutline"};
    inline static constexpr const char* const BinGridID{"BinGridID"};
    inline static constexpr const char* const HorizontalCRSID{"HorizontalCRSID"};
    inline static constexpr const char* const SeismicLineGeometryID{"SeismicLineGeometryID"};
    inline static constexpr const char* const TraceRelationFileID{"TraceRelationFileID"};
    inline static constexpr const char* const PrincipalAcquisitionProjectID{"PrincipalAcquisitionProjectID"};

    inline static constexpr const char* const ProcessingProjectID{"ProcessingProjectID"};
    inline static constexpr const char* const Preferred3DInterpretationSetID{"Preferred3DInterpretationSetID"};
    inline static constexpr const char* const Preferred2DInterpretationSetID{"Preferred2DInterpretationSetID"};
    inline static constexpr const char* const SeismicTraceDataDimensionalityTypeID{"SeismicTraceDataDimensionalityTypeID"};
    inline static constexpr const char* const SeismicDomainTypeID{"SeismicDomainTypeID"};

    inline static constexpr const char* const Seismic2DName{"Seismic2DName"};
    inline static constexpr const char* const SeismicMigrationTypeID{"SeismicMigrationTypeID"};
    inline static constexpr const char* const SeismicStackingTypeID{"SeismicStackingTypeID"};
    inline static constexpr const char* const SeismicAttributeTypeID{"SeismicAttributeTypeID"};
    inline static constexpr const char* const SeismicFilteringTypeID{"SeismicFilteringTypeID"};

    inline static constexpr const char* const SeismicPhaseID{"SeismicPhaseID"};
    inline static constexpr const char* const SeismicPolarityID{"SeismicPolarityID"};
    inline static constexpr const char* const SeismicProcessingStageTypeID{"SeismicProcessingStageTypeID"};
    inline static constexpr const char* const SampleInterval{"SampleInterval"};
    inline static constexpr const char* const SampleCount{"SampleCount"};

    inline static constexpr const char* const Difference{"Difference"};
    inline static constexpr const char* const StartTime{"StartTime"};
    inline static constexpr const char* const EndTime{"EndTime"};
    inline static constexpr const char* const StartDepth{"StartDepth"};
    inline static constexpr const char* const EndDepth{"EndDepth"};

    inline static constexpr const char* const TraceCount{"TraceCount"};
    inline static constexpr const char* const TraceLength{"TraceLength"};
    inline static constexpr const char* const TraceDomainUOM{"TraceDomainUOM"};
    inline static constexpr const char* const FirstShotPoint{"FirstShotPoint"};
    inline static constexpr const char* const LastShotPoint{"LastShotPoint"};

    inline static constexpr const char* const FirstCMP{"FirstCMP"};
    inline static constexpr const char* const LastCMP{"LastCMP"};
    inline static constexpr const char* const InlineMin{"InlineMin"};
    inline static constexpr const char* const InlineMax{"InlineMax"};
    inline static constexpr const char* const InlineIncrement{"InlineIncrement"};

    inline static constexpr const char* const CrosslineMin{"CrosslineMin"};
    inline static constexpr const char* const CrosslineMax{"CrosslineMax"};
    inline static constexpr const char* const CrosslineIncrement{"CrosslineIncrement"};
    inline static constexpr const char* const VerticalDatumOffset{"VerticalDatumOffset"};
    inline static constexpr const char* const VerticalMeasurementTypeID{"VerticalMeasurementTypeID"};

    inline static constexpr const char* const ReplacementVelocity{"ReplacementVelocity"};
    inline static constexpr const char* const ShiftApplied{"ShiftApplied"};
    inline static constexpr const char* const Precision{"Precision"};
    inline static constexpr const char* const ProcessingParameters{"ProcessingParameters"};
    inline static constexpr const char* const CoveragePercent{"CoveragePercent"};

    inline static constexpr const char* const TextualFileHeader{"TextualFileHeader"};

    inline static constexpr const char* const RangeAmplitudeMax{"RangeAmplitudeMax"};
    inline static constexpr const char* const RangeAmplitudeMin{"RangeAmplitudeMin"};

    inline static constexpr const char* const StackAngleRangeMax{"StackAngleRangeMax"};
    inline static constexpr const char* const StackAngleRangeMin{"StackAngleRangeMin"};

    inline static constexpr const char* const StackAzimuthRangeMax{"StackAzimuthRangeMax"};
    inline static constexpr const char* const StackAzimuthRangeMin{"StackAzimuthRangeMin"};

    inline static constexpr const char* const StackOffsetRangeMax{"StackOffsetRangeMax"};
    inline static constexpr const char* const StackOffsetRangeMin{"StackOffsetRangeMin"};

    inline static constexpr const char* const SeismicWaveTypeID{"SeismicWaveTypeID"};

    inline static constexpr const char* const StackAngleRanges{"StackAngleRanges"};
    inline static constexpr const char* const StackAzimuthRanges{"StackAzimuthRanges"};
    inline static constexpr const char* const StackOffsetRanges{"StackOffsetRanges"};

    inline static constexpr const char* const SortOrderID{"SortOrderID"};
    inline static constexpr const char* const GatherTypeID{"GatherTypeID"};
    inline static constexpr const char* const TimeLapse{"TimeLapse"};
};

} // namespace segysdk::metadata::osdu
