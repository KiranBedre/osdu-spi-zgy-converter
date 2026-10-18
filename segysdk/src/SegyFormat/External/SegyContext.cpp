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

#include "SegyContext.h"

#include "SegyBinaryHeaderStringConsts.h"
#include "SegyTraceHeaderStringConsts.h"
#include "../segysdk_details/SegyException.h"

#include <filesystem>
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>

using std::exception;

namespace segysdk::segyformat::external
{

SegyContext::SegyContext()
{
}

SegyContext::SegyContext(const std::vector<std::string>& segyFileNames, int segyVersion)
  : segyFileNames_(segyFileNames), segyVersion_(segyVersion)
{
    binaryHeaderDefinitions_ = getSegyStandardBinaryHeaderDefinitions();
    traceHeaderDefinitions_ = getDefaultTraceHdrLocations();

    ValidateInputParameters(segyFileNames, binaryHeaderDefinitions_, traceHeaderDefinitions_);
}

std::vector<std::shared_ptr<SegyHeaderField>>
SegyContext::getTraceHeaderDefinitions() const
{
    if (!traceHeaderDefinitions_.empty()) {
        return traceHeaderDefinitions_;
    }

    return getDefaultTraceHdrLocations();
}

void
SegyContext::setTraceHeaderDefinitions(std::vector<std::shared_ptr<SegyHeaderField>>& traceHeaders)
{
    traceHeaderDefinitions_ = traceHeaders;
}

std::vector<std::shared_ptr<SegyHeaderField>>
SegyContext::getBinaryHeaderDefinitions() const
{
    if (!binaryHeaderDefinitions_.empty()) {
        return binaryHeaderDefinitions_;
    }

    return getSegyStandardBinaryHeaderDefinitions();
}

void
SegyContext::setBinaryHeaderDefinitions(std::vector<std::shared_ptr<SegyHeaderField>>& binaryHeaders)
{
    binaryHeaderDefinitions_ = binaryHeaders;
}

std::vector<std::shared_ptr<SegyOverrideField>>
SegyContext::getTraceHeaderOverrideDefinitions() const
{
    return traceHeaderOverrides_;
}

void
SegyContext::setTraceHeaderOverrideDefinitions(std::vector<std::shared_ptr<SegyOverrideField>>& traceHeaderOverridess)
{
    traceHeaderOverrides_ = traceHeaderOverridess;
}

std::vector<std::shared_ptr<SegyOverrideField>>
SegyContext::getBinaryHeaderOverrideDefinitions() const
{
    return binaryHeaderOverrides_;
}

void
SegyContext::setBinaryHeaderOverrideDefinitions(std::vector<std::shared_ptr<SegyOverrideField>>& binaryHeaderOverridess)
{
    binaryHeaderOverrides_ = binaryHeaderOverridess;
}

void
SegyContext::ValidateInputParameters(std::vector<std::string> const& segyFileNames, std::vector<std::shared_ptr<SegyHeaderField>> binaryHeaders, std::vector<std::shared_ptr<SegyHeaderField>> traceHeaders) const
{
    const bool isBinHdrsUnique = CheckForOverlaps(std::move(binaryHeaders));
    if (isBinHdrsUnique) {
        throw SegyInvalidArgumentException("SegyContext::ValidateInputParameters: Binary header offset and length overlaps! ");
    }
    const bool isTraceHdrsUnique = CheckForOverlaps(std::move(traceHeaders));
    if (isTraceHdrsUnique) {
        throw SegyInvalidArgumentException("SegyContext::ValidateInputParameters: Trace header offset and length overlaps! ");
    }

    if (segyFileNames.empty()) {
        throw SegyInvalidArgumentException("SegyContext::ValidateInputParameters: Input list of SEG-Y files cannot be empty! ");
    }
}

std::string
SegyContext::getFilePath(std::string const& fileName)
{
    const size_t found = fileName.find_last_of("/\\");
    if (found == std::string::npos) {
        return fileName;
    }

    return fileName.substr(0, found);
}

void
SegyContext::setSegyFileNames(std::vector<std::string> const& segyFileNames)
{
    segyFileNames_ = segyFileNames;
}

std::vector<std::string> const&
SegyContext::getSegyFileNames()
{
    return segyFileNames_;
}

void
SegyContext::setHeaderValues(const std::vector<SegyHeaderField>& binaryHeaders, const std::vector<SegyHeaderField>&
                                                                                    traceHeaders)
{
    for (const SegyHeaderField& hdr : binaryHeaders) {
        bool found = false;
        for (const std::shared_ptr<SegyHeaderField>& bhPtr : binaryHeaderDefinitions_) {
            if (hdr.getKey() == bhPtr->getKey()) {
                bhPtr->updateDefinitions(hdr);
                found = true;
                break;
            }
        }
        if (!found) {
            binaryHeaderDefinitions_.push_back(std::make_shared<SegyHeaderField>(hdr));
            const bool overlaps = CheckForOverlaps(binaryHeaderDefinitions_, hdr.getOffset(), hdr.getLength());

            if (overlaps) {
                throw SegyInvalidArgumentException(std::string("Overlapping field: " + hdr.getKey()));
            }
        }
    }

    for (const SegyHeaderField& hdr : traceHeaders) {
        bool found = false;
        for (const std::shared_ptr<SegyHeaderField>& thPtr : traceHeaderDefinitions_) {
            if (hdr.getKey() == thPtr->getKey()) {
                thPtr->updateDefinitions(hdr);
                found = true;
                break;
            }
        }
        if (!found) {
            traceHeaderDefinitions_.push_back(std::make_shared<SegyHeaderField>(hdr));
            // bool overlaps = CheckForOverlaps(_binaryHeaderDefinitions, hdr.getKey(), hdr.getOffset(), hdr.getLength());
        }
    }
}

bool
SegyContext::CheckForOverlaps(std::vector<std::shared_ptr<SegyHeaderField>> headers)
{

    int headerSize = 240;
    if (headers[0]->IsBinaryHeader()) {
        headerSize = 400;
    }

    std::vector<unsigned char> hdrMapping;
    for (int i = 0; i < headerSize; i++)
        hdrMapping.push_back(0);

    for (const std::shared_ptr<SegyHeaderField>& hPtr : headers) {
        int offset = hPtr->getOffset() - 1;
        for (int i = 0; i < hPtr->getLength(); i++) {
            hdrMapping[offset] += 1;
            offset++;
        }
    }

    for (const char c : hdrMapping) {
        if (c > 1)
            return true;
    }
    return false;
}

bool
SegyContext::CheckForOverlaps(const std::vector<std::shared_ptr<SegyHeaderField>>& headers, const int offset, const unsigned short length)
{
    char hdrMapping[240] = {0};

    for (const std::shared_ptr<SegyHeaderField>& hPtr : headers) {
        int myOffset = hPtr->getOffset() - 1;
        for (int i = 0; i < hPtr->getLength(); i++) {
            hdrMapping[myOffset] += 1;
            myOffset++;
        }
    }

    for (int i = 0; i < length; i++) {
        hdrMapping[offset + i]++;
    }

    for (const char c : hdrMapping) {
        if (c > 1)
            return true;
    }
    return false;
}

std::vector<std::shared_ptr<SegyHeaderField>>
SegyContext::getSegyStandardBinaryHeaderDefinitions()
{
    std::vector<std::shared_ptr<SegyHeaderField>> binaryHeaders;

    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::JobId, DataFormat::Integer, 4, 1));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::LineNo, DataFormat::Integer, 4, 5));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::ReelNo, DataFormat::Integer, 4, 9));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::NumberOfDataTracesPerEnsemble, DataFormat::Integer, 2, 13));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::NumberOfAuxTracesPerEnsemble, DataFormat::Integer, 2, 15));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::SampleInterval, DataFormat::UnsignedInt, 2, 17));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::SampleIntervalFieldRecording, DataFormat::UnsignedInt, 2, 19));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::SamplesPerTrace, DataFormat::UnsignedInt, 2, 21));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::SampleIntervalFieldRecording, DataFormat::UnsignedInt, 2, 23));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::DataSampleFormatCode, DataFormat::Integer, 2, 25));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::EnsembleFold, DataFormat::UnsignedInt, 2, 27));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::TraceSortingCode, DataFormat::Integer, 2, 29));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::VerticalSumCode, DataFormat::Integer, 2, 31));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::SweepFrequencyAtStart, DataFormat::Integer, 2, 33));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::SweepFrequencyAtEnd, DataFormat::Integer, 2, 35));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::SweepLength, DataFormat::Integer, 2, 37));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::SweepTypeCode, DataFormat::Integer, 2, 39));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::TraceNumberSweepChannel, DataFormat::Integer, 2, 41));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::SweepTraceTaperLengthAtStart, DataFormat::Integer, 2, 43));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::SweepTraceTaperLengthAtEnd, DataFormat::Integer, 2, 45));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::TaperType, DataFormat::Integer, 2, 47));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::CorrelatedDataTraces, DataFormat::Integer, 2, 49));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::BinaryGainRecovered, DataFormat::Integer, 2, 51));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::AmplitudeRecoveryMethod, DataFormat::Integer, 2, 53));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::MeasurementSystem, DataFormat::Integer, 2, 55));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::ImpulseSignalPolarity, DataFormat::Integer, 2, 57));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::VibratoryPolarityCode, DataFormat::Integer, 2, 59));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::SEGYFormatRevisionNumber, DataFormat::Integer, 2, 301));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::FixedLengthTraceFlag, DataFormat::Integer, 2, 303));
    binaryHeaders.push_back(std::make_shared<BinaryHeaderField>(SegyBinaryHeaderStringConsts::ExtendedHeadersCount, DataFormat::Integer, 2, 305));

    return binaryHeaders;
}

std::vector<std::shared_ptr<SegyHeaderField>>
SegyContext::getDefaultTraceHdrLocations(int segyVersion)
{
    if (segyVersion == 0) {
        return getSegyRev0Locations();
    }
    if (segyVersion == 1) {
        return getSegyRev1TraceHdrLocations();
    }

    const std::string message = "Segy version as specified in the binary header is currently not supported! [" + std::to_string(segyVersion) + ']';
    throw SegyInvalidArgumentException(message);
}

std::vector<std::shared_ptr<SegyHeaderField>>
SegyContext::getDefaultTraceHdrLocations() const
{
    if (segyVersion_ == 0) {
        return getSegyRev0Locations();
    }
    if (segyVersion_ == 1) {
        return getSegyRev1TraceHdrLocations();
    }

    const std::string message = "Segy version as specified in the binary header is currently not supported! [" + std::to_string(segyVersion_) + ']';
    throw SegyInvalidArgumentException(message);
}

std::vector<std::shared_ptr<SegyHeaderField>>
SegyContext::getSegyRev1TraceHdrLocations()
{

    std::vector<std::shared_ptr<SegyHeaderField>> traceHeaders;

    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TraceSequenceNumberLine, "", DataFormat::Integer, 4, 1));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TraceSequenceNumberFile, "", DataFormat::Integer, 4, 5));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::OriginalFieldRecordNumber, "", DataFormat::Integer, 4, 9));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TraceNumberWithinOriginalFieldRecord, "", DataFormat::Integer, 4, 13));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::EnergySourcePointNumber, "", DataFormat::Integer, 4, 17));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::EnsembleNumber, "", DataFormat::Integer, 4, 21));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TraceNumberWithinEnsemble, "", DataFormat::Integer, 4, 25));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TraceIdentificationCode, "", DataFormat::Integer, 2, 29));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::NumberOfVerticallySummedTraces, "", DataFormat::UnsignedInt, 2, 31));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::NumberOfHorizontallyStackedTraces, "", DataFormat::UnsignedInt, 2, 33));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::DataUse, "", DataFormat::Integer, 2, 35));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::DistanceSourcePointToReceiver, "", DataFormat::Integer, 4, 37, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::ReceiverGroupElevation, "", DataFormat::Integer, 4, 41, ScalarIndicator::STANDARD ,SegyTraceHeaderStringConsts::ScalarForElevations, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SurfaceElevationAtSource, "", DataFormat::Integer, 4, 45, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForElevations, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SourceDepthBelowSurface, "", DataFormat::Integer, 4, 49, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForElevations, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::DatumElevationAtReceiverGroup, "", DataFormat::Integer, 4, 53, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForElevations, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::DatumElevationAtSource, "", DataFormat::Integer, 4, 57, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForElevations, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::WaterDepthAtSource, "", DataFormat::Integer, 4, 61, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForElevations, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::WaterDepthAtReceiverGroup, "", DataFormat::Integer, 4, 65, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForElevations, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::ScalarForElevations, "", DataFormat::Integer, 2, 69));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::ScalarForCoordinates, "", DataFormat::Integer, 2, 71));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SourceCoordinateX, "", DataFormat::Integer, 4, 73, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForCoordinates, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SourceCoordinateY, "", DataFormat::Integer, 4, 77, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForCoordinates, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::ReceiverGroupCoordinateX, "", DataFormat::Integer, 4, 81, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForCoordinates, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::ReceiverGroupCoordinateY, "", DataFormat::Integer, 4, 85, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForCoordinates, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::CoordinateUnits, "", DataFormat::Integer, 2, 89));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::WeatheringVelocity, "", DataFormat::Integer, 2, 91, UoMStringConsts::mps));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SubWeatheringVelocity, "", DataFormat::Integer, 2, 93, UoMStringConsts::mps));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::UpholeTimeAtSource, "", DataFormat::Integer, 2, 95, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::us));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::UpholeTimeAtReceiverGroup, "", DataFormat::Integer, 2, 97, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::us));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SourceStaticCorrection, "", DataFormat::Integer, 2, 99, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::ReceiverGroupStaticCorrection, "", DataFormat::Integer, 2, 101, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TotalStaticApplied, "", DataFormat::Integer, 2, 103, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::LagTimeA, "", DataFormat::Integer, 2, 105, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::LagTimeB, "", DataFormat::Integer, 2, 107, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::DelayRecordingTime, "", DataFormat::Integer, 2, 109, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::MuteTimeStart, "", DataFormat::Integer, 2, 111, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::MuteTimeEnd, "", DataFormat::Integer, 2, 113, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::NumberOfSamplesIntrace, "", DataFormat::UnsignedInt, 2, 115));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SampleInterval, "", DataFormat::UnsignedInt, 2, 117, UoMStringConsts::us));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::GainTypeFieldInstruments, "", DataFormat::Integer, 2, 119));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::InstrumentGainConstant, "", DataFormat::Integer, 2, 121));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::InstrumentInitialGain, "", DataFormat::Integer, 2, 123));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::Correlated, "", DataFormat::Integer, 2, 125));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SweepFrequencyStart, "", DataFormat::Integer, 2, 127, UoMStringConsts::Hz));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SweepFrequencyEnd, "", DataFormat::Integer, 2, 129, UoMStringConsts::Hz));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SweepLength, "", DataFormat::Integer, 2, 131, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SweepType, "", DataFormat::Integer, 2, 133));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SweepTraceTaperLengthStart, "", DataFormat::Integer, 2, 135, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SweepTraceTaperLengthEnd, "", DataFormat::Integer, 2, 137, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TaperType, "", DataFormat::Integer, 2, 139));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::AliasFilterFrequency, "", DataFormat::Integer, 2, 141, UoMStringConsts::Hz));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::AliasFilterSlope, "", DataFormat::Integer, 2, 143, UoMStringConsts::Hz));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::NotchFilterFrequency, "", DataFormat::Integer, 2, 145, UoMStringConsts::Hz));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::NotchFilterSlope, "", DataFormat::Integer, 2, 147, UoMStringConsts::dBpOctave));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::LowCutFrequency, "", DataFormat::Integer, 2, 149, UoMStringConsts::Hz));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::HighCutFrequency, "", DataFormat::Integer, 2, 151, UoMStringConsts::Hz));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::LowCutSlope, "", DataFormat::Integer, 2, 153, UoMStringConsts::dBpOctave));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::HighCutSlope, "", DataFormat::Integer, 2, 155, UoMStringConsts::dBpOctave));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::YearDataRecorded, "", DataFormat::Integer, 2, 157));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::DayOfYear, "", DataFormat::Integer, 2, 159));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::HourOfDay, "", DataFormat::Integer, 2, 161));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::MinuteOfHour, "", DataFormat::Integer, 2, 163));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SecondOfMinute, "", DataFormat::Integer, 2, 165));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TimeBasisCode, "", DataFormat::Integer, 2, 167));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TraceWeightingFactor, "", DataFormat::Integer, 2, 169, UoMStringConsts::Volts));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::GeophoneGroupNumberRollSwitchOne, "", DataFormat::Integer, 2, 171));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::GeophoneGroupNumberTraceNumberOne, "", DataFormat::Integer, 2, 173));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::GeophoneGroupNumberLastTrace, "", DataFormat::Integer, 2, 175));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::GapSize, "", DataFormat::Integer, 2, 177));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::OverTravelTaper, "", DataFormat::Integer, 2, 179));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::XCoordinateEnsemble, "", DataFormat::Integer, 4, 181, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForCoordinates, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::YCoordinateEnsemble, "", DataFormat::Integer, 4, 185, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForCoordinates, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::InlineNumber, "", DataFormat::Integer, 4, 189));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::CrosslineNumber, "", DataFormat::Integer, 4, 193));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::ShotpointNumber, "", DataFormat::Integer, 4, 197, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForShotpoint, UoMStringConsts::Unitless));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::ScalarForShotpoint, "", DataFormat::Integer, 2, 201));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TraceValueMeasurementUnit, "", DataFormat::Integer, 2, 203));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TransductionConstantMantissa, "", DataFormat::Integer, 4, 205));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TransductionConstantExponent, "", DataFormat::Integer, 2, 209));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TransductionUnit, "", DataFormat::Integer, 2, 211));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::DeviceTraceidentifier, "", DataFormat::Integer, 2, 213));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::ScalarForTimeValues, "", DataFormat::Integer, 2, 215));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SourceTypeOrientation, "", DataFormat::Integer, 2, 217));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SourceEnergyDirectionMantissa, "", DataFormat::Integer, 4, 219));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SourceEnergyDirectionExponent, "", DataFormat::Integer, 2, 223));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SourceMeasurementMantissa, "", DataFormat::Integer, 4, 225));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SourceMeasurementExponent, "", DataFormat::Integer, 2, 229));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SourceMeasurementUnit, "", DataFormat::Integer, 2, 231));

    return traceHeaders;
}

std::vector<std::shared_ptr<SegyHeaderField>>
SegyContext::getSegyRev0Locations()
{
    std::vector<std::shared_ptr<SegyHeaderField>> traceHeaders;

    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TraceSequenceNumberLine, "", DataFormat::Integer, 4, 1));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TraceSequenceNumberFile, "", DataFormat::Integer, 4, 5));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::OriginalFieldRecordNumber, "", DataFormat::Integer, 4, 9));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TraceNumberWithinOriginalFieldRecord, "", DataFormat::Integer, 4, 13));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::EnergySourcePointNumber, "", DataFormat::Integer, 4, 17));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::EnsembleNumber, "", DataFormat::Integer, 4, 21));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TraceNumberWithinEnsemble, "", DataFormat::Integer, 4, 25));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TraceIdentificationCode, "", DataFormat::Integer, 2, 29));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::NumberOfVerticallySummedTraces, "", DataFormat::UnsignedInt, 2, 31));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::NumberOfHorizontallyStackedTraces, "", DataFormat::UnsignedInt, 2, 33));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::DataUse, "", DataFormat::Integer, 2, 35));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::DistanceSourcePointToReceiver, "", DataFormat::Integer, 4, 37, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::ReceiverGroupElevation, "", DataFormat::Integer, 4, 41, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForElevations, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SurfaceElevationAtSource, "", DataFormat::Integer, 4, 45, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForElevations, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SourceDepthBelowSurface, "", DataFormat::Integer, 4, 49, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForElevations, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::DatumElevationAtReceiverGroup, "", DataFormat::Integer, 4, 53, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForElevations, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::DatumElevationAtSource, "", DataFormat::Integer, 4, 57, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForElevations, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::WaterDepthAtSource, "", DataFormat::Integer, 4, 61, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForElevations, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::WaterDepthAtReceiverGroup, "", DataFormat::Integer, 4, 65, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForElevations, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::ScalarForElevations, "", DataFormat::Integer, 2, 69));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::ScalarForCoordinates, "", DataFormat::Integer, 2, 71));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SourceCoordinateX, "", DataFormat::Integer, 4, 73, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForCoordinates, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SourceCoordinateY, "", DataFormat::Integer, 4, 77, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForCoordinates, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::ReceiverGroupCoordinateX, "", DataFormat::Integer, 4, 81, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForCoordinates, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::ReceiverGroupCoordinateY, "", DataFormat::Integer, 4, 85, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForCoordinates, UoMStringConsts::m));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::CoordinateUnits, "", DataFormat::Integer, 2, 89));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::WeatheringVelocity, "", DataFormat::Integer, 2, 91, UoMStringConsts::mps));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SubWeatheringVelocity, "", DataFormat::Integer, 2, 93, UoMStringConsts::mps));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::UpholeTimeAtSource, "", DataFormat::Integer, 2, 95, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::us));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::UpholeTimeAtReceiverGroup, "", DataFormat::Integer, 2, 97, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::us));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SourceStaticCorrection, "", DataFormat::Integer, 2, 99, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::ReceiverGroupStaticCorrection, "", DataFormat::Integer, 2, 101, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TotalStaticApplied, "", DataFormat::Integer, 2, 103, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::LagTimeA, "", DataFormat::Integer, 2, 105, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::LagTimeB, "", DataFormat::Integer, 2, 107, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::DelayRecordingTime, "", DataFormat::Integer, 2, 109, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::MuteTimeStart, "", DataFormat::Integer, 2, 111, ScalarIndicator::STANDARD, SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::MuteTimeEnd, "", DataFormat::Integer, 2, 113, ScalarIndicator::STANDARD,  SegyTraceHeaderStringConsts::ScalarForTimeValues, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::NumberOfSamplesIntrace, "", DataFormat::UnsignedInt, 2, 115));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SampleInterval, "", DataFormat::UnsignedInt, 2, 117, UoMStringConsts::us));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::GainTypeFieldInstruments, "", DataFormat::Integer, 2, 119));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::InstrumentGainConstant, "", DataFormat::Integer, 2, 121));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::InstrumentInitialGain, "", DataFormat::Integer, 2, 123));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::Correlated, "", DataFormat::Integer, 2, 125));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SweepFrequencyStart, "", DataFormat::Integer, 2, 127, UoMStringConsts::Hz));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SweepFrequencyEnd, "", DataFormat::Integer, 2, 129, UoMStringConsts::Hz));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SweepLength, "", DataFormat::Integer, 2, 131, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SweepType, "", DataFormat::Integer, 2, 133));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SweepTraceTaperLengthStart, "", DataFormat::Integer, 2, 135, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SweepTraceTaperLengthEnd, "", DataFormat::Integer, 2, 137, UoMStringConsts::ms));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TaperType, "", DataFormat::Integer, 2, 139));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::AliasFilterFrequency, "", DataFormat::Integer, 2, 141, UoMStringConsts::Hz));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::AliasFilterSlope, "", DataFormat::Integer, 2, 143, UoMStringConsts::Hz));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::NotchFilterFrequency, "", DataFormat::Integer, 2, 145, UoMStringConsts::Hz));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::NotchFilterSlope, "", DataFormat::Integer, 2, 147, UoMStringConsts::dBpOctave));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::LowCutFrequency, "", DataFormat::Integer, 2, 149, UoMStringConsts::Hz));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::HighCutFrequency, "", DataFormat::Integer, 2, 151, UoMStringConsts::Hz));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::LowCutSlope, "", DataFormat::Integer, 2, 153, UoMStringConsts::dBpOctave));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::HighCutSlope, "", DataFormat::Integer, 2, 155, UoMStringConsts::dBpOctave));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::YearDataRecorded, "", DataFormat::Integer, 2, 157));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::DayOfYear, "", DataFormat::Integer, 2, 159));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::HourOfDay, "", DataFormat::Integer, 2, 161));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::MinuteOfHour, "", DataFormat::Integer, 2, 163));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::SecondOfMinute, "", DataFormat::Integer, 2, 165));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TimeBasisCode, "", DataFormat::Integer, 2, 167));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::TraceWeightingFactor, "", DataFormat::Integer, 2, 169, UoMStringConsts::Volts));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::GeophoneGroupNumberRollSwitchOne, "", DataFormat::Integer, 2, 171));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::GeophoneGroupNumberTraceNumberOne, "", DataFormat::Integer, 2, 173));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::GeophoneGroupNumberLastTrace, "", DataFormat::Integer, 2, 175));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::GapSize, "", DataFormat::Integer, 2, 177));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::OverTravelTaper, "", DataFormat::Integer, 2, 179));

    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::InlineNumber, "", DataFormat::Integer, 4, 189));
    traceHeaders.push_back(std::make_shared<TraceHeaderField>(SegyTraceHeaderStringConsts::CrosslineNumber, "", DataFormat::Integer, 4, 193));

    return traceHeaders;
}

} // namespace segysdk::segyformat::external
