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

#include <string>

namespace segysdk::segyformat::external
{

class SEGYSDK_DLL_PUBLIC SegyBinaryHeaderStringConsts
{

private:
public:
    inline static const std::string JobId = "JobIdentificationNumber";
    inline static const std::string LineNo = "LineNumber";
    inline static const std::string ReelNo = "ReelNumber";
    inline static const std::string NumberOfAuxTracesPerEnsemble = "NumberOfAuxillaryTracesPerEnsemble";
    inline static const std::string SampleInterval = "SampleInterval";
    inline static const std::string SampleIntervalFieldRecording = "SampleIntervalField";
    inline static const std::string NumberOfDataTracesPerEnsemble = "DataTracesPerEnsemble";
    inline static const std::string SamplesPerTrace = "SamplesPerTrace";
    inline static const std::string SamplesPerTraceFieldRecording = "SamplesPerTraceField";
    inline static const std::string DataSampleFormatCode = "DataSampleFormatCode";
    inline static const std::string EnsembleFold = "EnsembleFold";
    inline static const std::string TraceSortingCode = "TraceSortingCode";
    inline static const std::string VerticalSumCode = "VerticalSumCode";
    inline static const std::string SweepFrequencyAtStart = "SweepFrequencyStart";
    inline static const std::string SweepFrequencyAtEnd = "SweepFrequencyEnd";
    inline static const std::string SweepLength = "SweepLength";
    inline static const std::string SweepTypeCode = "SweepTypeCode";
    inline static const std::string TraceNumberSweepChannel = "TraceNumberSweepChannel";
    inline static const std::string SweepTraceTaperLengthAtStart = "SweepTraceTaperLengthStart";
    inline static const std::string SweepTraceTaperLengthAtEnd = "SweepTraceTaperLengthEnd";
    inline static const std::string TaperType = "TaperType";
    inline static const std::string CorrelatedDataTraces = "CorrelatedDataTraces";
    inline static const std::string BinaryGainRecovered = "BinaryGainRecovered";
    inline static const std::string SweepTraceTaperLengthStart = "SweepTraceTaperLengthStart";
    inline static const std::string AmplitudeRecoveryMethod = "AmplitudeRecoveryMethod";
    inline static const std::string MeasurementSystem = "MeasurementSystem";
    inline static const std::string ImpulseSignalPolarity = "ImpulseSignalPolarity";
    inline static const std::string VibratoryPolarityCode = "VibratorPolarityCode";

    // SEG-Y rev 1 fields
    inline static const std::string SEGYFormatRevisionNumber = "SEGYFormatRevisionNumber";
    inline static const std::string FixedLengthTraceFlag = "FixedLengthTraceFlag";
    inline static const std::string ExtendedHeadersCount = "ExtendedTextHeaderRecords";
};

} // namespace segysdk::segyformat::external
