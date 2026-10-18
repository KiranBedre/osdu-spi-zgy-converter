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

#include "FileHeaderDefinitionFactory.h"

#include "FileHeaderDescription.h"
#include "HeaderDefinition.h"

#include "../SegyFormat/Internal/FileHeaderDefinitionStringConstsBinaryHeader.h"
#include "../SegyFormat/Internal/FileHeaderDefinitionStringConstsTraceHeader.h"
#include "../SegyFormat/External/SegyBinaryHeaderStringConsts.h"
#include "../SegyFormat/External/SegyTraceHeaderStringConsts.h"
#include "../SegyIO/HeaderOverride.h"
#include "../SegyIO/SegyIOException.h"

#include <algorithm>
#include <stdexcept>
#include <utility>

using namespace segysdk;
using namespace segyformat::external;
using namespace segyformat::internal;

namespace segysdk::segyio
{

FileHeaderDescriptionFactory::FileHeaderDescriptionFactory(std::shared_ptr<SegyContext> context)
  : context_(std::move(context))
{}

std::shared_ptr<FileHeaderDescription>
FileHeaderDescriptionFactory::GetFileHeaderDesctription() const
{
    auto binaryHeaders = Generate_binaryHeaders();
    auto traceHeaders = Generate_traceHeaders();
    const auto traceHeaderOverrides = GetTraceHeaderOverrides();
    const auto binaryHeaderOverrides = GetBinaryHeaderOverrides();
    auto fileHdrDesc = std::make_shared<FileHeaderDescription>(binaryHeaders, traceHeaders);
    if (binaryHeaderOverrides.size() > 0)
        fileHdrDesc->setBinaryHeaderOverrides(binaryHeaderOverrides);
    if (traceHeaderOverrides.size() > 0)
        fileHdrDesc->setTraceHeaderOverrides(traceHeaderOverrides);
    return fileHdrDesc;
}

void
FileHeaderDescriptionFactory::InitFileHeaderDescription() const
{
    const auto binaryHeaders = Generate_binaryHeaders();
    const auto traceHeaders = Generate_traceHeaders();
    FileHeaderDescription filedescr(binaryHeaders, traceHeaders);
}

std::vector<std::shared_ptr<HeaderDefinition>>
FileHeaderDescriptionFactory::Generate_binaryHeaders() const
{
    const auto binaryHdrMap = getBinaryHeaderMap();
    auto binaryHdrsInternal = std::vector<std::shared_ptr<HeaderDefinition>>();

    const auto binaryHdrs = context_->getBinaryHeaderDefinitions();
    for (const auto& hdr : binaryHdrs) {
        auto it = binaryHdrMap->lower_bound(hdr->getKey());
        std::string key;

        if (it != binaryHdrMap->end())
            key = it->second;
        else
            key = hdr->getKey();

        const HeaderDataType dataType = getDataType(hdr->getNumericFormat());

        if (hdr->IsBinaryHeader()) {
            auto fld = std::make_shared<HeaderDefinition>(key, key, hdr->getOffset() - 1 + 3200, hdr->getLength());
            fld->setExternalHeaderId(hdr->getKey());
            fld->setHeaderDataType(dataType);
            binaryHdrsInternal.push_back(fld);
        }
    }
    return binaryHdrsInternal;
}

HeaderDataType
FileHeaderDescriptionFactory::getDataType(const DataFormat& fmt) const
{
    // Integer, FloatIBM, FloatIEEE
    switch (fmt) {
    case DataFormat::Integer:
        return HeaderDataType::Integer;
    case DataFormat::UnsignedInt:
        return HeaderDataType::UnsignedInteger;
    case DataFormat::FloatIBM:
        return HeaderDataType::IBMFloat;
    case DataFormat::FloatIEEE:
        return HeaderDataType::IEEEFloat;
    default:
        throw SegyInvalidArgumentException("Invalid header data type specified.");
    }
}

// No binary header fields have been renamed between rev 0 and rev 1
std::shared_ptr<std::map<std::string, std::string>>
FileHeaderDescriptionFactory::getBinaryHeaderMap() const
{

    auto binaryHdrMapPtr = std::make_shared<std::map<std::string, std::string>>();
    const auto binaryHdrs = context_->getBinaryHeaderDefinitions();

    for (const auto& hdr : binaryHdrs) {

        std::string key = hdr->getKey();

        if (key == SegyBinaryHeaderStringConsts::JobId) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::JobId, FileHeaderDefinitionStringConstsBinaryHeader::JobId));
        }
        else if (key == SegyBinaryHeaderStringConsts::LineNo) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::LineNo, FileHeaderDefinitionStringConstsBinaryHeader::LineNumber));
        }
        else if (key == SegyBinaryHeaderStringConsts::ReelNo) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::ReelNo, FileHeaderDefinitionStringConstsBinaryHeader::ReelNumber));
        }
        else if (key == SegyBinaryHeaderStringConsts::NumberOfDataTracesPerEnsemble) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::NumberOfDataTracesPerEnsemble, FileHeaderDefinitionStringConstsBinaryHeader::TracesPerRecord));
        }
        else if (key == SegyBinaryHeaderStringConsts::NumberOfAuxTracesPerEnsemble) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::NumberOfAuxTracesPerEnsemble, FileHeaderDefinitionStringConstsBinaryHeader::AuxiliaryTracesPerRecord));
        }
        else if (key == SegyBinaryHeaderStringConsts::SampleInterval) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::SampleInterval, FileHeaderDefinitionStringConstsBinaryHeader::SampleInterval));
        }
        else if (key == SegyBinaryHeaderStringConsts::SampleIntervalFieldRecording) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::SampleIntervalFieldRecording, FileHeaderDefinitionStringConstsBinaryHeader::OriginalSampleInterval));
        }
        else if (key == SegyBinaryHeaderStringConsts::SamplesPerTrace) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::SamplesPerTrace, FileHeaderDefinitionStringConstsBinaryHeader::TraceLength));
        }
        else if (key == SegyBinaryHeaderStringConsts::SamplesPerTraceFieldRecording) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::SampleIntervalFieldRecording, FileHeaderDefinitionStringConstsBinaryHeader::OriginalSamplesPerTrace));
        }
        else if (key == SegyBinaryHeaderStringConsts::DataSampleFormatCode) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::DataSampleFormatCode, FileHeaderDefinitionStringConstsBinaryHeader::SampleDataFormat));
        }
        else if (key == SegyBinaryHeaderStringConsts::EnsembleFold) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::EnsembleFold, FileHeaderDefinitionStringConstsBinaryHeader::StackFold));
        }
        else if (key == SegyBinaryHeaderStringConsts::TraceSortingCode) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::TraceSortingCode, FileHeaderDefinitionStringConstsBinaryHeader::TraceSortingCode));
        }
        else if (key == SegyBinaryHeaderStringConsts::VerticalSumCode) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::VerticalSumCode, FileHeaderDefinitionStringConstsBinaryHeader::VerticalSumCode));
        }
        else if (key == SegyBinaryHeaderStringConsts::SweepFrequencyAtStart) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::SweepFrequencyAtStart, FileHeaderDefinitionStringConstsBinaryHeader::SweepStart));
        }
        else if (key == SegyBinaryHeaderStringConsts::SweepFrequencyAtEnd) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::SweepFrequencyAtEnd, FileHeaderDefinitionStringConstsBinaryHeader::SweepEnd));
        }
        else if (key == SegyBinaryHeaderStringConsts::SweepLength) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::SweepLength, FileHeaderDefinitionStringConstsBinaryHeader::SweepLength));
        }
        else if (key == SegyBinaryHeaderStringConsts::SweepTypeCode) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::SweepTypeCode, FileHeaderDefinitionStringConstsBinaryHeader::SweepType));
        }
        else if (key == SegyBinaryHeaderStringConsts::TraceNumberSweepChannel) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::TraceNumberSweepChannel, FileHeaderDefinitionStringConstsBinaryHeader::SweepTraceNumber));
        }
        else if (key == SegyBinaryHeaderStringConsts::SweepTraceTaperLengthAtStart) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::SweepTraceTaperLengthAtStart, FileHeaderDefinitionStringConstsBinaryHeader::SweepTraceTaperLenStart));
        }
        else if (key == SegyBinaryHeaderStringConsts::SweepTraceTaperLengthAtEnd) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::SweepTraceTaperLengthAtEnd, FileHeaderDefinitionStringConstsBinaryHeader::SweepTraceTaperLenEnd));
        }
        else if (key == SegyBinaryHeaderStringConsts::TaperType) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::TaperType, FileHeaderDefinitionStringConstsBinaryHeader::TaperType));
        }
        else if (key == SegyBinaryHeaderStringConsts::CorrelatedDataTraces) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::CorrelatedDataTraces, FileHeaderDefinitionStringConstsBinaryHeader::CorrelatedDataTraces));
        }
        else if (key == SegyBinaryHeaderStringConsts::BinaryGainRecovered) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::BinaryGainRecovered, FileHeaderDefinitionStringConstsBinaryHeader::BinaryGainRecovered));
        }
        else if (key == SegyBinaryHeaderStringConsts::AmplitudeRecoveryMethod) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::AmplitudeRecoveryMethod, FileHeaderDefinitionStringConstsBinaryHeader::AmplitudeRecoveryMethod));
        }
        else if (key == SegyBinaryHeaderStringConsts::MeasurementSystem) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::MeasurementSystem, FileHeaderDefinitionStringConstsBinaryHeader::MeasurementSystem));
        }
        else if (key == SegyBinaryHeaderStringConsts::ImpulseSignalPolarity) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::ImpulseSignalPolarity, FileHeaderDefinitionStringConstsBinaryHeader::ImpulseSignalPolarity));
        }
        else if (key == SegyBinaryHeaderStringConsts::VibratoryPolarityCode) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::VibratoryPolarityCode, FileHeaderDefinitionStringConstsBinaryHeader::VibratorPolarityCode));
        }
        // SEG-Y rev 1 fields
        else if (key == SegyBinaryHeaderStringConsts::SEGYFormatRevisionNumber) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::SEGYFormatRevisionNumber, FileHeaderDefinitionStringConstsBinaryHeader::SegyRevision));
        }
        else if (key == SegyBinaryHeaderStringConsts::FixedLengthTraceFlag) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::FixedLengthTraceFlag, FileHeaderDefinitionStringConstsBinaryHeader::FixedTraceLengthFlag));
        }
        else if (key == SegyBinaryHeaderStringConsts::ExtendedHeadersCount) {
            binaryHdrMapPtr->insert(std::pair(SegyBinaryHeaderStringConsts::ExtendedHeadersCount, FileHeaderDefinitionStringConstsBinaryHeader::ExtendedTextualHeaderCount));
        }
    }

    return binaryHdrMapPtr;
}

std::shared_ptr<segysdk::segyio::HeaderDefinition> FileHeaderDescriptionFactory::GetHeaderDefinition(
                    const std::shared_ptr<segysdk::segyformat::external::SegyHeaderField>& hdr) const
{
    const auto traceHdrMap = getTraceHeaderMap();
    std::string key = getNativeKeyFromUserKey(traceHdrMap, hdr->getKey());
    const HeaderDataType dataType = getDataType(hdr->getNumericFormat());
    auto traceHdr = std::dynamic_pointer_cast<TraceHeaderField>(hdr);
    auto fld = std::make_shared<HeaderDefinition>(key, key, traceHdr->getOffset() - 1, traceHdr->getLength());
    if (traceHdr != nullptr) {
        fld->setIsWellKnown(isWellKnownKey(traceHdrMap, key));
        if (traceHdr->getScalarIndicatorValue() == ScalarIndicator::OVERRIDE) {
            fld->setScalarHeaderId(""); // Set normal scaling attribute blank as a non-zero scalar override trumps scaling attribute
            fld->setScalarOverride(traceHdr->getScalarOverrideValue());
            fld->setScalarUsageStrategy(ScalarUsageStrategy::Override);
        }
        else {
            fld->setScalarOverride(0);
            if (traceHdr->getScalarAttribute().empty())
                fld->setScalarUsageStrategy(ScalarUsageStrategy::NoScaling);
            else {
                std::string scalarKey = getNativeKeyFromUserKey(traceHdrMap, traceHdr->getScalarAttribute());
                fld->setScalarHeaderId(scalarKey);
                fld->setScalarUsageStrategy(ScalarUsageStrategy::Default);
            }
        }
        if (traceHdr->getUoM() != "")
            fld->setUoM(traceHdr->getUoM());
    }
    fld->setExternalHeaderId(hdr->getOriginalKey());
    fld->setHeaderDataType(dataType);
    return fld;
}

std::vector<std::shared_ptr<HeaderDefinition>>
FileHeaderDescriptionFactory::Generate_traceHeaders() const
{
    auto traceHdrsInternal = std::vector<std::shared_ptr<HeaderDefinition>>();
    const auto traceHdrs = context_->getTraceHeaderDefinitions();

    for (const auto& hdr : traceHdrs) {
        if (!hdr->IsTraceHeader()) {
            continue;
        }
        auto fld = GetHeaderDefinition(hdr);
        clearAnyOverlaps(traceHdrsInternal, fld);
        traceHdrsInternal.push_back(fld);
    }
    return traceHdrsInternal;
}

void
FileHeaderDescriptionFactory::clearAnyOverlaps(std::vector<std::shared_ptr<HeaderDefinition>> headers, const std::shared_ptr<HeaderDefinition>& fld) const
{
    for (const auto& h : headers) {
        if (h->getOffset() < fld->getOffset() + fld->getByteCount() && fld->getOffset() < h->getOffset() + h->getByteCount()) {
            headers.erase(std::remove(headers.begin(), headers.end(), h), headers.end());
        }
    }
}

bool
FileHeaderDescriptionFactory::isWellKnownKey(const std::shared_ptr<std::map<std::string, std::string>>& traceHdrMap, const std::string& myKey) const
{
    const auto it = traceHdrMap->find(myKey);

    return it != traceHdrMap->end();
}

std::string
FileHeaderDescriptionFactory::getNativeKeyFromUserKey(const std::shared_ptr<std::map<std::string, std::string>>& traceHdrMap, const std::string& myKey) const
{
    const auto it = traceHdrMap->find(myKey);
    std::string key = it != traceHdrMap->end()
                          ? it->second
                          : myKey;
    return key;
}

// No trace header fields have been renamed between rev 0 and rev 1
std::shared_ptr<std::map<std::string, std::string>>
FileHeaderDescriptionFactory::getTraceHeaderMap() const
{

    auto traceHdrMapPtr = std::make_shared<std::map<std::string, std::string>>();

    const auto traceHdrs = context_->getTraceHeaderDefinitions();

    for (const auto& hdr : traceHdrs) {

        std::string key = hdr->getKey();

        if (key == SegyTraceHeaderStringConsts::TraceSequenceNumberLine) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::TraceSequenceNumberLine, FileHeaderDefinitionStringConstsTraceHeader::TraceSequenceLine));
        }
        else if (key == SegyTraceHeaderStringConsts::TraceSequenceNumberFile) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::TraceSequenceNumberFile, FileHeaderDefinitionStringConstsTraceHeader::TraceSequenceFile));
        }
        else if (key == SegyTraceHeaderStringConsts::OriginalFieldRecordNumber) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::OriginalFieldRecordNumber, FileHeaderDefinitionStringConstsTraceHeader::FieldFileNumber));
        }
        else if (key == SegyTraceHeaderStringConsts::TraceNumberWithinOriginalFieldRecord) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::TraceNumberWithinOriginalFieldRecord, FileHeaderDefinitionStringConstsTraceHeader::FieldTraceNumber));
        }
        else if (key == SegyTraceHeaderStringConsts::EnergySourcePointNumber) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::EnergySourcePointNumber, FileHeaderDefinitionStringConstsTraceHeader::EnergySourcePointNumber));
        }
        else if (key == SegyTraceHeaderStringConsts::EnsembleNumber) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::EnsembleNumber, FileHeaderDefinitionStringConstsTraceHeader::CMP));
        }
        else if (key == SegyTraceHeaderStringConsts::TraceNumberWithinEnsemble) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::TraceNumberWithinEnsemble, FileHeaderDefinitionStringConstsTraceHeader::CDPTraceNumber));
        }
        else if (key == SegyTraceHeaderStringConsts::TraceIdentificationCode) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::TraceIdentificationCode, FileHeaderDefinitionStringConstsTraceHeader::TraceType));
        }
        else if (key == SegyTraceHeaderStringConsts::NumberOfVerticallySummedTraces) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::NumberOfVerticallySummedTraces, FileHeaderDefinitionStringConstsTraceHeader::NumberVertTraces));
        }
        else if (key == SegyTraceHeaderStringConsts::NumberOfHorizontallyStackedTraces) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::NumberOfHorizontallyStackedTraces, FileHeaderDefinitionStringConstsTraceHeader::NumberHorizTraces));
        }
        else if (key == SegyTraceHeaderStringConsts::DataUse) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::DataUse, FileHeaderDefinitionStringConstsTraceHeader::DataUse));
        }
        else if (key == SegyTraceHeaderStringConsts::DistanceSourcePointToReceiver) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::DistanceSourcePointToReceiver, FileHeaderDefinitionStringConstsTraceHeader::DistanceSourceReceiver));
        }
        else if (key == SegyTraceHeaderStringConsts::ReceiverGroupElevation) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::ReceiverGroupElevation, FileHeaderDefinitionStringConstsTraceHeader::ReceiverElevation));
        }
        else if (key == SegyTraceHeaderStringConsts::SurfaceElevationAtSource) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SurfaceElevationAtSource, FileHeaderDefinitionStringConstsTraceHeader::SourceElevation));
        }
        else if (key == SegyTraceHeaderStringConsts::SourceDepthBelowSurface) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SourceDepthBelowSurface, FileHeaderDefinitionStringConstsTraceHeader::SourceDepth));
        }
        else if (key == SegyTraceHeaderStringConsts::DatumElevationAtReceiverGroup) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::DatumElevationAtReceiverGroup, FileHeaderDefinitionStringConstsTraceHeader::ReceiverElevationDatum));
        }
        else if (key == SegyTraceHeaderStringConsts::DatumElevationAtSource) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::DatumElevationAtSource, FileHeaderDefinitionStringConstsTraceHeader::SourceElevationDatum));
        }
        else if (key == SegyTraceHeaderStringConsts::WaterDepthAtSource) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::WaterDepthAtSource, FileHeaderDefinitionStringConstsTraceHeader::SourceWaterDepth));
        }
        else if (key == SegyTraceHeaderStringConsts::WaterDepthAtReceiverGroup) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::WaterDepthAtReceiverGroup, FileHeaderDefinitionStringConstsTraceHeader::ReceiverWaterDepth));
        }
        else if (key == SegyTraceHeaderStringConsts::ScalarForElevations) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::ScalarForElevations, FileHeaderDefinitionStringConstsTraceHeader::ScalarOnElevationsAndDepths));
        }
        else if (key == SegyTraceHeaderStringConsts::ScalarForCoordinates) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::ScalarForCoordinates, FileHeaderDefinitionStringConstsTraceHeader::ScalarOnCoordinates));
        }
        else if (key == SegyTraceHeaderStringConsts::SourceCoordinateX) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SourceCoordinateX, FileHeaderDefinitionStringConstsTraceHeader::SourceXCoordinate));
        }
        else if (key == SegyTraceHeaderStringConsts::SourceCoordinateY) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SourceCoordinateY, FileHeaderDefinitionStringConstsTraceHeader::SourceYCoordinate));
        }
        else if (key == SegyTraceHeaderStringConsts::ReceiverGroupCoordinateX) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::ReceiverGroupCoordinateX, FileHeaderDefinitionStringConstsTraceHeader::ReceiverXCoordinate));
        }
        else if (key == SegyTraceHeaderStringConsts::ReceiverGroupCoordinateY) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::ReceiverGroupCoordinateY, FileHeaderDefinitionStringConstsTraceHeader::ReceiverYCoordinate));
        }
        else if (key == SegyTraceHeaderStringConsts::CoordinateUnits) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::CoordinateUnits, FileHeaderDefinitionStringConstsTraceHeader::CoordinateUnits));
        }
        else if (key == SegyTraceHeaderStringConsts::WeatheringVelocity) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::WeatheringVelocity, FileHeaderDefinitionStringConstsTraceHeader::WeatheringVelocity));
        }
        else if (key == SegyTraceHeaderStringConsts::SubWeatheringVelocity) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SubWeatheringVelocity, FileHeaderDefinitionStringConstsTraceHeader::SubweatheringVelocity));
        }
        else if (key == SegyTraceHeaderStringConsts::UpholeTimeAtSource) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::UpholeTimeAtSource, FileHeaderDefinitionStringConstsTraceHeader::SourceUpholeTime));
        }
        else if (key == SegyTraceHeaderStringConsts::UpholeTimeAtReceiverGroup) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::UpholeTimeAtReceiverGroup, FileHeaderDefinitionStringConstsTraceHeader::ReceiverUpholeTime));
        }
        else if (key == SegyTraceHeaderStringConsts::SourceStaticCorrection) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SourceStaticCorrection, FileHeaderDefinitionStringConstsTraceHeader::SourceStaticCorrection));
        }
        else if (key == SegyTraceHeaderStringConsts::ReceiverGroupStaticCorrection) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::ReceiverGroupStaticCorrection, FileHeaderDefinitionStringConstsTraceHeader::ReceiverStaticCorrection));
        }
        else if (key == SegyTraceHeaderStringConsts::TotalStaticApplied) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::TotalStaticApplied, FileHeaderDefinitionStringConstsTraceHeader::TotalStaticApplied));
        }
        else if (key == SegyTraceHeaderStringConsts::LagTimeA) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::LagTimeA, FileHeaderDefinitionStringConstsTraceHeader::HeaderToTimeBreakLagTime));
        }
        else if (key == SegyTraceHeaderStringConsts::LagTimeB) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::LagTimeB, FileHeaderDefinitionStringConstsTraceHeader::TimeBreakToShotLagTime));
        }
        else if (key == SegyTraceHeaderStringConsts::DelayRecordingTime) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::DelayRecordingTime, FileHeaderDefinitionStringConstsTraceHeader::ShotToRecordingLagTime));
        }
        else if (key == SegyTraceHeaderStringConsts::MuteTimeStart) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::MuteTimeStart, FileHeaderDefinitionStringConstsTraceHeader::MuteStart));
        }
        else if (key == SegyTraceHeaderStringConsts::MuteTimeEnd) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::MuteTimeEnd, FileHeaderDefinitionStringConstsTraceHeader::MuteEnd));
        }
        else if (key == SegyTraceHeaderStringConsts::NumberOfSamplesIntrace) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::NumberOfSamplesIntrace, FileHeaderDefinitionStringConstsTraceHeader::TraceLength));
        }
        else if (key == SegyTraceHeaderStringConsts::SampleInterval) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SampleInterval, FileHeaderDefinitionStringConstsTraceHeader::SampleRate));
        }
        else if (key == SegyTraceHeaderStringConsts::GainTypeFieldInstruments) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::GainTypeFieldInstruments, FileHeaderDefinitionStringConstsTraceHeader::InstrumentGainType));
        }
        else if (key == SegyTraceHeaderStringConsts::InstrumentGainConstant) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::InstrumentGainConstant, FileHeaderDefinitionStringConstsTraceHeader::InstrumentGainConstant));
        }
        else if (key == SegyTraceHeaderStringConsts::InstrumentInitialGain) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::InstrumentInitialGain, FileHeaderDefinitionStringConstsTraceHeader::InstrumentInitialGain));
        }
        else if (key == SegyTraceHeaderStringConsts::Correlated) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::Correlated, FileHeaderDefinitionStringConstsTraceHeader::Correlated));
        }
        else if (key == SegyTraceHeaderStringConsts::SweepFrequencyStart) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SweepFrequencyStart, FileHeaderDefinitionStringConstsTraceHeader::SweepStart));
        }
        else if (key == SegyTraceHeaderStringConsts::SweepFrequencyEnd) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SweepFrequencyEnd, FileHeaderDefinitionStringConstsTraceHeader::SweepEnd));
        }
        else if (key == SegyTraceHeaderStringConsts::SweepLength) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SweepLength, FileHeaderDefinitionStringConstsTraceHeader::SweepLength));
        }
        else if (key == SegyTraceHeaderStringConsts::SweepType) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SweepType, FileHeaderDefinitionStringConstsTraceHeader::SweepType));
        }
        else if (key == SegyTraceHeaderStringConsts::SweepTraceTaperLengthStart) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SweepTraceTaperLengthStart, FileHeaderDefinitionStringConstsTraceHeader::SweepTraceTaperLenStart));
        }
        else if (key == SegyTraceHeaderStringConsts::SweepTraceTaperLengthEnd) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SweepTraceTaperLengthEnd, FileHeaderDefinitionStringConstsTraceHeader::SweepTraceTaperLenEnd));
        }
        else if (key == SegyTraceHeaderStringConsts::TaperType) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::TaperType, FileHeaderDefinitionStringConstsTraceHeader::TaperType));
        }
        else if (key == SegyTraceHeaderStringConsts::AliasFilterFrequency) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::AliasFilterFrequency, FileHeaderDefinitionStringConstsTraceHeader::AliasFilterFrequency));
        }
        else if (key == SegyTraceHeaderStringConsts::AliasFilterSlope) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::AliasFilterSlope, FileHeaderDefinitionStringConstsTraceHeader::AliasFilterSlope));
        }
        else if (key == SegyTraceHeaderStringConsts::NotchFilterFrequency) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::NotchFilterFrequency, FileHeaderDefinitionStringConstsTraceHeader::NotchFilterFrequency));
        }
        else if (key == SegyTraceHeaderStringConsts::NotchFilterSlope) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::NotchFilterSlope, FileHeaderDefinitionStringConstsTraceHeader::NotchFilterSlope));
        }
        else if (key == SegyTraceHeaderStringConsts::LowCutFrequency) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::LowCutFrequency, FileHeaderDefinitionStringConstsTraceHeader::LowCutFrequency));
        }
        else if (key == SegyTraceHeaderStringConsts::HighCutFrequency) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::HighCutFrequency, FileHeaderDefinitionStringConstsTraceHeader::HighCutFrequency));
        }
        else if (key == SegyTraceHeaderStringConsts::LowCutSlope) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::LowCutSlope, FileHeaderDefinitionStringConstsTraceHeader::LowCutSlope));
        }
        else if (key == SegyTraceHeaderStringConsts::HighCutSlope) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::HighCutSlope, FileHeaderDefinitionStringConstsTraceHeader::HighCutSlope));
        }
        else if (key == SegyTraceHeaderStringConsts::YearDataRecorded) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::YearDataRecorded, FileHeaderDefinitionStringConstsTraceHeader::Year));
        }
        else if (key == SegyTraceHeaderStringConsts::DayOfYear) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::DayOfYear, FileHeaderDefinitionStringConstsTraceHeader::DayOfYear));
        }
        else if (key == SegyTraceHeaderStringConsts::HourOfDay) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::HourOfDay, FileHeaderDefinitionStringConstsTraceHeader::Hour));
        }
        else if (key == SegyTraceHeaderStringConsts::MinuteOfHour) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::MinuteOfHour, FileHeaderDefinitionStringConstsTraceHeader::Minute));
        }
        else if (key == SegyTraceHeaderStringConsts::SecondOfMinute) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SecondOfMinute, FileHeaderDefinitionStringConstsTraceHeader::Second));
        }
        else if (key == SegyTraceHeaderStringConsts::TimeBasisCode) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::TimeBasisCode, FileHeaderDefinitionStringConstsTraceHeader::TimeBasis));
        }
        else if (key == SegyTraceHeaderStringConsts::TraceWeightingFactor) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::TraceWeightingFactor, FileHeaderDefinitionStringConstsTraceHeader::TraceWeightFactor));
        }
        else if (key == SegyTraceHeaderStringConsts::GeophoneGroupNumberRollSwitchOne) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::GeophoneGroupNumberRollSwitchOne, FileHeaderDefinitionStringConstsTraceHeader::GeophoneGroupNumberOfRollSwitchP1));
        }
        else if (key == SegyTraceHeaderStringConsts::GeophoneGroupNumberTraceNumberOne) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::GeophoneGroupNumberTraceNumberOne, FileHeaderDefinitionStringConstsTraceHeader::GeophoneGroupNumberOfFirstTrace));
        }
        else if (key == SegyTraceHeaderStringConsts::GeophoneGroupNumberLastTrace) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::GeophoneGroupNumberLastTrace, FileHeaderDefinitionStringConstsTraceHeader::GeophoneGroupNumberOfLastTrace));
        }
        else if (key == SegyTraceHeaderStringConsts::GapSize) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::GapSize, FileHeaderDefinitionStringConstsTraceHeader::GapSize));
        }
        else if (key == SegyTraceHeaderStringConsts::OverTravelTaper) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::OverTravelTaper, FileHeaderDefinitionStringConstsTraceHeader::Overtravel));
        }
        else if (key == SegyTraceHeaderStringConsts::XCoordinateEnsemble) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::XCoordinateEnsemble, FileHeaderDefinitionStringConstsTraceHeader::CoordinateX));
        }
        else if (key == SegyTraceHeaderStringConsts::YCoordinateEnsemble) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::YCoordinateEnsemble, FileHeaderDefinitionStringConstsTraceHeader::CoordinateY));
        }
        else if (key == SegyTraceHeaderStringConsts::InlineNumber) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::InlineNumber, FileHeaderDefinitionStringConstsTraceHeader::Inline));
        }
        else if (key == SegyTraceHeaderStringConsts::CrosslineNumber) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::CrosslineNumber, FileHeaderDefinitionStringConstsTraceHeader::Crossline));
        }
        else if (key == SegyTraceHeaderStringConsts::ShotpointNumber) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::ShotpointNumber, FileHeaderDefinitionStringConstsTraceHeader::ShotNumber));
        }
        else if (key == SegyTraceHeaderStringConsts::ScalarForShotpoint) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::ScalarForShotpoint, FileHeaderDefinitionStringConstsTraceHeader::ShotpointScalar));
        }
        else if (key == SegyTraceHeaderStringConsts::TraceValueMeasurementUnit) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::TraceValueMeasurementUnit, FileHeaderDefinitionStringConstsTraceHeader::TraceUnit));
        }
        else if (key == SegyTraceHeaderStringConsts::TransductionConstantMantissa) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::TransductionConstantMantissa, FileHeaderDefinitionStringConstsTraceHeader::TransductionConstantMantisa));
        }
        else if (key == SegyTraceHeaderStringConsts::TransductionConstantExponent) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::TransductionConstantExponent, FileHeaderDefinitionStringConstsTraceHeader::TransductionConstantExponent));
        }
        else if (key == SegyTraceHeaderStringConsts::TransductionUnit) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::TransductionUnit, FileHeaderDefinitionStringConstsTraceHeader::TransductionUnits));
        }
        else if (key == SegyTraceHeaderStringConsts::DeviceTraceidentifier) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::DeviceTraceidentifier, FileHeaderDefinitionStringConstsTraceHeader::DeviceTraceId));
        }
        else if (key == SegyTraceHeaderStringConsts::ScalarForTimeValues) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::ScalarForTimeValues, FileHeaderDefinitionStringConstsTraceHeader::TimeScalar));
        }
        else if (key == SegyTraceHeaderStringConsts::SourceTypeOrientation) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SourceTypeOrientation, FileHeaderDefinitionStringConstsTraceHeader::SourceType));
        }
        else if (key == SegyTraceHeaderStringConsts::SourceEnergyDirectionMantissa) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SourceEnergyDirectionMantissa, FileHeaderDefinitionStringConstsTraceHeader::SourceEnergyDirection));
        }
        else if (key == SegyTraceHeaderStringConsts::SourceEnergyDirectionExponent) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SourceEnergyDirectionExponent, FileHeaderDefinitionStringConstsTraceHeader::SourceEnergyDirectionPositive));
        }
        else if (key == SegyTraceHeaderStringConsts::SourceMeasurementMantissa) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SourceMeasurementMantissa, FileHeaderDefinitionStringConstsTraceHeader::SourceMeasurementMantisa));
        }
        else if (key == SegyTraceHeaderStringConsts::SourceMeasurementExponent) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SourceMeasurementExponent, FileHeaderDefinitionStringConstsTraceHeader::SourceMeasurementExponent));
        }
        else if (key == SegyTraceHeaderStringConsts::SourceMeasurementUnit) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::SourceMeasurementUnit, FileHeaderDefinitionStringConstsTraceHeader::SourceMeasurementUnit));
        }
        else if (key == SegyTraceHeaderStringConsts::Angle) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::Angle, FileHeaderDefinitionStringConstsTraceHeader::Angle));
        }
        else if (key == SegyTraceHeaderStringConsts::Azimuth) {
            traceHdrMapPtr->insert(std::pair(SegyTraceHeaderStringConsts::Azimuth, FileHeaderDefinitionStringConstsTraceHeader::Azimuth));
        }
        else if (key.find("Unmapped") != std::string::npos) {
            traceHdrMapPtr->insert(std::pair(key, key));
        }
    }

    return traceHdrMapPtr;
}

const std::vector<std::shared_ptr<HeaderOverride>>
FileHeaderDescriptionFactory::GetBinaryHeaderOverrides() const
{

    std::vector<std::shared_ptr<HeaderOverride>> binaryHeaderOverrides;
    const auto overrides = context_->getBinaryHeaderOverrideDefinitions();
    const auto binaryHdrMap = getBinaryHeaderMap();

    if (overrides.size() > 0) {
        for (const auto& inHdr : overrides) {

            auto externalKey = inHdr->getKey();
            auto it = binaryHdrMap->find(externalKey);
            std::string key;

            if (it != binaryHdrMap->end()) {
                key = it->second;
            }
            else {
                // This needs to be communicated to the caller, might be a typo for an important value?
                throw SegyFormatException("Invalid Binary header override - Not a recognized literal: " + externalKey);
            }

            const double value = inHdr->getValue();
            binaryHeaderOverrides.push_back(std::make_shared<HeaderOverride>(key, static_cast<float>(value)));
        }
    }

    return binaryHeaderOverrides;
}

const std::vector<std::shared_ptr<HeaderOverride>>
FileHeaderDescriptionFactory::GetTraceHeaderOverrides() const
{

    std::vector<std::shared_ptr<HeaderOverride>> traceHeaderOverrides;
    const auto overrides = context_->getTraceHeaderOverrideDefinitions();
    const auto traceHdrMap = getTraceHeaderMap();

    if (overrides.size() > 0) {
        for (const auto& inHdr : overrides) {

            auto externalKey = inHdr->getKey();
            auto it = traceHdrMap->find(externalKey);
            std::string key;

            if (it != traceHdrMap->end()) {
                key = it->second;
            }
            else {
                // This needs to be communicated to the caller, might be a typo for an important value?
                throw SegyFormatException("Invalid Trace header override - Not a recognized literal: " + externalKey);
            }

            const double value = inHdr->getValue();
            traceHeaderOverrides.push_back(std::make_shared<HeaderOverride>(key, static_cast<float>(value)));
        }
    }

    return traceHeaderOverrides;
}
} // namespace segysdk::segyio
