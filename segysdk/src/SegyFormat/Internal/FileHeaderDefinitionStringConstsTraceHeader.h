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

namespace segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader
{

constexpr auto TraceSequenceLine = "TraceSequenceLine";
constexpr auto TraceSequenceFile = "TraceSequenceFile";
constexpr auto FieldFileNumber = "FieldFileNumber";
constexpr auto FieldTraceNumber = "FieldTraceNumber";
constexpr auto EnergySourcePointNumber = "EnergySourcePointNumber";
constexpr auto CMP = "CMP";
constexpr auto CDPTraceNumber = "CDPTraceNumber";
constexpr auto TraceType = "TraceType";
constexpr auto NumberVertTraces = "NumberVertTraces";
constexpr auto NumberHorizTraces = "NumberHorizTraces";
constexpr auto DataUse = "DataUse";
constexpr auto DistanceSourceReceiver = "DistanceSourceReceiver";
constexpr auto ReceiverElevation = "ReceiverElevation";
constexpr auto SourceElevation = "SourceElevation";
constexpr auto SourceDepth = "SourceDepth";
constexpr auto ReceiverElevationDatum = "ReceiverElevationDatum";
constexpr auto SourceElevationDatum = "SourceElevationDatum";
constexpr auto SourceWaterDepth = "SourceWaterDepth";
constexpr auto ReceiverWaterDepth = "ReceiverWaterDepth";
constexpr auto ScalarOnElevationsAndDepths = "ScalarOnElevationsAndDepths";
constexpr auto ScalarOnCoordinates = "ScalarOnCoordinates";
constexpr auto SourceXCoordinate = "SourceXCoordinate";
constexpr auto SourceYCoordinate = "SourceYCoordinate";
constexpr auto ReceiverXCoordinate = "ReceiverXCoordinate";
constexpr auto ReceiverYCoordinate = "ReceiverYCoordinate";
constexpr auto CoordinateUnits = "CoordinateUnits";
constexpr auto WeatheringVelocity = "WeatheringVelocity";
constexpr auto SubweatheringVelocity = "SubweatheringVelocity";
constexpr auto SourceUpholeTime = "SourceUpholeTime";
constexpr auto ReceiverUpholeTime = "ReceiverUpholeTime";
constexpr auto SourceStaticCorrection = "SourceStaticCorrection";
constexpr auto ReceiverStaticCorrection = "ReceiverStaticCorrection";
constexpr auto TotalStaticApplied = "TotalStaticApplied";
constexpr auto HeaderToTimeBreakLagTime = "HeaderToTimeBreakLagTime";
constexpr auto TimeBreakToShotLagTime = "TimeBreakToShotLagTime";
constexpr auto ShotToRecordingLagTime = "ShotToRecordingLagTime";
constexpr auto MuteStart = "MuteStart";
constexpr auto MuteEnd = "MuteEnd";
constexpr auto TraceLength = "TraceLength";
constexpr auto SampleRate = "SampleRate";
constexpr auto InstrumentGainType = "InstrumentGainType";
constexpr auto InstrumentGainConstant = "InstrumentGainConstant";
constexpr auto InstrumentInitialGain = "InstrumentInitialGain";
constexpr auto Correlated = "Correlated";
constexpr auto SweepStart = "SweepStart";
constexpr auto SweepEnd = "SweepEnd";
constexpr auto SweepLength = "SweepLength";
constexpr auto SweepType = "SweepType";
constexpr auto SweepTraceTaperLenStart = "SweepTraceTaperLenStart";
constexpr auto SweepTraceTaperLenEnd = "SweepTraceTaperLenEnd";
constexpr auto TaperType = "TaperType";
constexpr auto AliasFilterFrequency = "AliasFilterFrequency";
constexpr auto AliasFilterSlope = "AliasFilterSlope";
constexpr auto NotchFilterFrequency = "NotchFilterFrequency";
constexpr auto NotchFilterSlope = "NotchFilterSlope";
constexpr auto LowCutFrequency = "LowCutFrequency";
constexpr auto HighCutFrequency = "HighCutFrequency";
constexpr auto LowCutSlope = "LowCutSlope";
constexpr auto HighCutSlope = "HighCutSlope";
constexpr auto Year = "Year";
constexpr auto DayOfYear = "DayOfYear";
constexpr auto Hour = "Hour";
constexpr auto Minute = "Minute";
constexpr auto Second = "Second";
constexpr auto TimeBasis = "TimeBasis";
constexpr auto TraceWeightFactor = "TraceWeightFactor";
constexpr auto GeophoneGroupNumberOfRollSwitchP1 = "GeophoneGroupNumberOfRollSwitchP1";
constexpr auto GeophoneGroupNumberOfFirstTrace = "GeophoneGroupNumberOfFirstTrace";
constexpr auto GeophoneGroupNumberOfLastTrace = "GeophoneGroupNumberOfLastTrace";
constexpr auto GapSize = "GapSize";
constexpr auto Overtravel = "Overtravel";
constexpr auto CoordinateX = "CoordinateX";
constexpr auto CoordinateY = "CoordinateY";
constexpr auto Inline = "Inline";
constexpr auto Crossline = "Crossline";
constexpr auto ShotNumber = "ShotNumber";
constexpr auto ShotpointScalar = "ShotpointScalar";
constexpr auto TraceUnit = "TraceUnit";
constexpr auto TransductionConstantMantisa = "TransductionConstantMantisa";
constexpr auto TransductionConstantExponent = "TransductionConstantExponent";
constexpr auto TransductionUnits = "TransductionUnits";
constexpr auto DeviceTraceId = "DeviceTraceId";
constexpr auto TimeScalar = "TimeScalar";
constexpr auto SourceType = "SourceType";
constexpr auto SourceEnergyDirection = "SourceEnergyDirection";
constexpr auto SourceEnergyDirectionExponent = "SourceEnergyDirectionExponent";
constexpr auto SourceEnergyDirectionPositive = "SourceEnergyDirectionPositive";
constexpr auto SourceMeasurementMantisa = "SourceMeasurementMantisa";
constexpr auto SourceMeasurementExponent = "SourceMeasurementExponent";
constexpr auto SourceMeasurementUnit = "SourceMeasurementUnit";
constexpr auto Angle = "Angle";
constexpr auto Azimuth = "Azimuth";
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

} // namespace segysdk::segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader
