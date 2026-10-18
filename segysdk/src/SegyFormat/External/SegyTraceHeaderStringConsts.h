/*
Copyright 2021-2025 Schlumberger

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

namespace segysdk::segyformat::external::SegyTraceHeaderStringConsts
{

constexpr auto TraceSequenceNumberLine = "TraceSequenceLine";
constexpr auto TraceSequenceNumberFile = "TraceSequenceFile";
constexpr auto OriginalFieldRecordNumber = "FieldRecordNumber";
constexpr auto TraceNumberWithinOriginalFieldRecord = "TraceNumberField";
constexpr auto EnergySourcePointNumber = "EnergySourcePointNumber";
constexpr auto EnsembleNumber = "EnsembleNumber";
constexpr auto TraceNumberWithinEnsemble = "TraceNumberEnsemble";
constexpr auto TraceIdentificationCode = "TraceIdentificationCode";
constexpr auto NumberOfVerticallySummedTraces = "NumberVerticallySummedTraces";
constexpr auto NumberOfHorizontallyStackedTraces = "NumberHorizontallyStackedTraces";
constexpr auto DataUse = "DataUse";
constexpr auto DistanceSourcePointToReceiver = "Offset";
constexpr auto ReceiverGroupElevation = "ReceiverGroupElevation";
constexpr auto SurfaceElevationAtSource = "SurfaceElevationAtSource";
constexpr auto SourceDepthBelowSurface = "SourceDepthBelowSurface";
constexpr auto DatumElevationAtReceiverGroup = "DatumElevationAtReceiverGroup";
constexpr auto DatumElevationAtSource = "DatumElevationAtSource";
constexpr auto WaterDepthAtSource = "WaterDepthAtSource";
constexpr auto WaterDepthAtReceiverGroup = "WaterDepthArtReceiverGroup";
constexpr auto ScalarForElevations = "ScalarForElevations";
constexpr auto ScalarForCoordinates = "ScalarForCoordinates";
constexpr auto SourceCoordinateX = "SourceCoordinateX";
constexpr auto SourceCoordinateY = "SourceCoordinateY";
constexpr auto ReceiverGroupCoordinateX = "ReceiverCoordinateX";
constexpr auto ReceiverGroupCoordinateY = "ReceiverCoordinateY";
constexpr auto CoordinateUnits = "CoordinateUnits";
constexpr auto WeatheringVelocity = "WeatheringVelocity";
constexpr auto SubWeatheringVelocity = "SubweatheringVelocity";
constexpr auto UpholeTimeAtSource = "UpholeTimeSource";
constexpr auto UpholeTimeAtReceiverGroup = "UpholeTimeReceiverGroup";
constexpr auto SourceStaticCorrection = "StaticCorrectionSource";
constexpr auto ReceiverGroupStaticCorrection = "StaticCorrectionReceiverGroup";
constexpr auto TotalStaticApplied = "TotalStaticApplied";
constexpr auto LagTimeA = "HeaderToTimeBreakLagTime";
constexpr auto LagTimeB = "TimeBreakToShotLagTime";
constexpr auto DelayRecordingTime = "ShotToRecordingLagTime";
constexpr auto MuteTimeStart = "MuteTimeStart";
constexpr auto MuteTimeEnd = "MuteTimeEnd";
constexpr auto NumberOfSamplesIntrace = "SamplesPerTrace";
constexpr auto SampleInterval = "SampleInterval";
constexpr auto GainTypeFieldInstruments = "GainTypeInstrument";
constexpr auto InstrumentGainConstant = "GainConstantInstrument";
constexpr auto InstrumentInitialGain = "GainInstrumentInitial";
constexpr auto Correlated = "Correlated";
constexpr auto SweepFrequencyStart = "SweepFrequencyStart";
constexpr auto SweepFrequencyEnd = "SweepFrequencyEnd";
constexpr auto SweepLength = "SweepLength";
constexpr auto SweepType = "SweepType";
constexpr auto SweepTraceTaperLengthStart = "SweepTraceTaperLengthStart";
constexpr auto SweepTraceTaperLengthEnd = "SweepTraceTaperLengthEnd";
constexpr auto TaperType = "TaperType";
constexpr auto AliasFilterFrequency = "AliasFilterFrequency";
constexpr auto AliasFilterSlope = "AliasFilterSlope";
constexpr auto NotchFilterFrequency = "NotchFilterFrequency";
constexpr auto NotchFilterSlope = "NotchFilterSlope";
constexpr auto LowCutFrequency = "LowCutFrequency";
constexpr auto LowCutSlope = "LowCutSlope";
constexpr auto HighCutFrequency = "HighCutFrequency";
constexpr auto HighCutSlope = "HighCutSlope";
constexpr auto YearDataRecorded = "Year";
constexpr auto DayOfYear = "DayOfYear";
constexpr auto HourOfDay = "Hour";
constexpr auto MinuteOfHour = "Minute";
constexpr auto SecondOfMinute = "Second";
constexpr auto TimeBasisCode = "TimeBasisCode";
constexpr auto TraceWeightingFactor = "TraceWeightingFactor";
constexpr auto GeophoneGroupNumberRollSwitchOne = "GeophoneGroupNumberOfRollSwitchP1";
constexpr auto GeophoneGroupNumberTraceNumberOne = "GeophoneGroupNumberOfFirstTrace";
constexpr auto GeophoneGroupNumberLastTrace = "GeophoneGroupNumberOfLastTrace";
constexpr auto GapSize = "GapSize";
constexpr auto OverTravelTaper = "Overtravel";
constexpr auto XCoordinateEnsemble = "XCoordinateEnsemble";
constexpr auto YCoordinateEnsemble = "YCoordinateEnsemble";
constexpr auto InlineNumber = "InlineNumber";
constexpr auto CrosslineNumber = "CrosslineNumber";
constexpr auto ShotpointNumber = "ShotpointNumber";
constexpr auto ScalarForShotpoint = "ScalarForShotpoint";
constexpr auto TraceValueMeasurementUnit = "TraceUnitOfMeasure";
constexpr auto TransductionConstantMantissa = "TransductionConstantMantisa";
constexpr auto TransductionConstantExponent = "TransductionConstantExponent";
constexpr auto TransductionUnit = "TransductionUnits";
constexpr auto DeviceTraceidentifier = "DeviceTraceId";
constexpr auto ScalarForTimeValues = "TimeScalar";
constexpr auto SourceTypeOrientation = "SourceType";
constexpr auto SourceEnergyDirectionMantissa = "SourceEnergyDirectionMantissa";
constexpr auto SourceEnergyDirectionExponent = "SourceEnergyDirectionExponent";
constexpr auto SourceMeasurementMantissa = "SourceMeasurementMantissa";
constexpr auto SourceMeasurementExponent = "SourceMeasurementExponent";
constexpr auto SourceMeasurementUnit = "SourceMeasurementUnit";
constexpr auto Angle = "Angle";
constexpr auto Azimuth = "Azimuth";
constexpr auto CMP = "CMP";
constexpr auto SP = "SP";
constexpr auto SPSCALED = "SPSCALED";
constexpr auto CMPX = "CMPX";
constexpr auto CMPY = "CMPY";
constexpr auto STARTTIME = "STARTTIME";
constexpr auto SPSCALAR = "SPSCALAR";
constexpr auto XYSCALAR = "XYSCALAR";
constexpr auto ZSCALAR = "ZSCALAR";
constexpr auto Unassigned01 = "Unassigned01";
constexpr auto Unassigned02 = "Unassigned02";
constexpr auto Unassigned03 = "Unassigned03";
constexpr auto Unassigned04 = "Unassigned04";
constexpr auto Unassigned05 = "Unassigned05";
constexpr auto Unassigned06 = "Unassigned06";
constexpr auto Unassigned07 = "Unassigned07";
constexpr auto Unassigned08 = "Unassigned08";
constexpr auto Unassigned09 = "Unassigned09";
constexpr auto Unassigned10 = "Unassigned10";
constexpr auto Unassigned11 = "Unassigned11";
constexpr auto Unassigned12 = "Unassigned12";
constexpr auto Unassigned13 = "Unassigned13";
constexpr auto Unassigned14 = "Unassigned14";
constexpr auto Unassigned15 = "Unassigned15";
constexpr auto Unassigned16 = "Unassigned16";
constexpr auto Unassigned17 = "Unassigned17";
constexpr auto Unassigned18 = "Unassigned18";

} // namespace segysdk::segyformat::external::SegyTraceHeaderStringConsts
