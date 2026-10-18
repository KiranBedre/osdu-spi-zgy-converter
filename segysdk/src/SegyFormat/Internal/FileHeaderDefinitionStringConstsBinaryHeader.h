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

#include <string>

namespace segysdk::segyformat::internal
{

class FileHeaderDefinitionStringConstsBinaryHeader
{

private:
public:
    inline static const std::string JobId = "JobId";
    inline static const std::string LineNumber = "LineNumber";
    inline static const std::string ReelNumber = "ReelNumber";
    inline static const std::string TracesPerRecord = "TracesPerRecord";
    inline static const std::string AuxiliaryTracesPerRecord = "AuxiliaryTracesPerRecord";
    inline static const std::string SampleInterval = "SampleInterval";
    inline static const std::string OriginalSampleInterval = "OriginalSampleInterval";
    inline static const std::string TraceLength = "TraceLength";
    inline static const std::string OriginalSamplesPerTrace = "OriginalSamplesPerTrace";
    inline static const std::string SampleDataFormat = "SampleDataFormat";
    inline static const std::string StackFold = "StackFold";
    inline static const std::string TraceSortingCode = "TraceSortingCode";
    inline static const std::string VerticalSumCode = "VerticalSumCode";
    inline static const std::string SweepStart = "SweepStart";
    inline static const std::string SweepEnd = "SweepEnd";
    inline static const std::string SweepLength = "SweepLength";
    inline static const std::string SweepType = "SweepType";
    inline static const std::string SweepTraceNumber = "SweepTraceNumber";
    inline static const std::string SweepTraceTaperLenStart = "SweepTraceTaperLenStart";
    inline static const std::string SweepTraceTaperLenEnd = "SweepTraceTaperLenEnd";
    inline static const std::string TaperType = "TaperType";
    inline static const std::string CorrelatedDataTraces = "CorrelatedDataTraces";
    inline static const std::string BinaryGainRecovered = "BinaryGainRecovered";
    inline static const std::string SweepTraceTaperLengthStart = "SweepTraceTaperLengthStart";
    inline static const std::string AmplitudeRecoveryMethod = "AmplitudeRecoveryMethod";
    inline static const std::string MeasurementSystem = "MeasurementSystem";
    inline static const std::string ImpulseSignalPolarity = "ImpulseSignalPolarity";
    inline static const std::string VibratorPolarityCode = "VibratorPolarityCode";

    // SEG-Y rev 1 fields
    inline static const std::string SegyRevision = "SegyRevision";
    inline static const std::string FixedTraceLengthFlag = "FixedTraceLengthFlag";
    inline static const std::string ExtendedTextualHeaderCount = "ExtendedTextualHeaderCount";
};

} // namespace segysdk::segyformat::internal
