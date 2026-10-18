/*
Copyright 2022 Schlumberger

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

#include "SegyIndexGenerator.h"

#include "../FileIO/FileManager.h"
#include "../FileIO/FileReader.h"
#include "../Logger/Logger.h"
#include "../Logger/ProgressReporting.h"
#include "../SegyFormat/Internal/FileHeaderDefinitionStringConstsBinaryHeader.h"
#include "../SegyFormat/Internal/FileHeaderDefinitionStringConstsTraceHeader.h"
#include "../SegyIO/FileHeaderDescription.h"
#include "../SegyIO/HeaderDefinitionProvider.h"
#include "../SegyIO/Primitives.h"
#include "../SegyIO/TraceSequentialScanner.h"
#include "../SegyIndex/CompleteGridHandler.h"
#include "../SegyIndex/SegyIndexGenerator.h"
#include "../SegyIndex/Utilities/TraceRangeFactory.h"
#include "../Utilities/Environment.h"
#include "../Utilities/MeasureDuration.h"
#include "../Utilities/StringUtils.h"
#include "../segysdk_details/SegyException.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <iostream>
#include <limits>

namespace segysdk::segyindex
{
namespace index_generator
{
using FileReaderPtr = std::shared_ptr<fileio::FileReader>;
using FileReaderVector = std::vector<FileReaderPtr>;

SegyIndexGenerator::SegyIndexGenerator(SegyIndexDataPtr& indexData)
  : indexData_{indexData}
{
    /* EMPTY */
}

bool
SegyIndexGenerator::GenerateIndex(bool forceGenerateIndex)
{
    if (indexData_->segyFilesVector_.empty()) {
        const std::string msg = "Must have at least 1 input SEG-Y file for indexing process!";
        throw SegyInvalidArgumentException(msg);
    }
    if (indexData_->segyFilesVector_.size() >= std::numeric_limits<uint16_t>::max()) {
        const std::string msg = "Too many files for indexing process. Must be fewer than " + std::to_string(std::numeric_limits<uint16_t>::max());
        throw SegyInvalidArgumentException(msg);
    }

    ResetIndexData();

    IndexBuilderCollatedDataVector indexCollatedData;

    double elapsedSecondsProcessing{0.0};
    {
        SEGYLOG_DEBUG("GenerateIndex: Start processing SEG-Y files: ");
        

        utilities::MeasureDuration processingDuration(elapsedSecondsProcessing);

        uint16_t fileNo = 0;
        for (const std::shared_ptr<segyio::SegyFile>& segyFilePtr : indexData_->segyFilesVector_) {
            SEGYLOG_REPORT_INFO("segyIndexGenerator; processing input file [" + std::to_string(fileNo) + "] : " + segyFilePtr->getFileName());
            {
                multi_file::MultiFileStruct fs;
                fs.SegyFileName = segyFilePtr->getFileName();
                fs.FirstTraceOffset = 0;
                fs.PrimaryKeyRange.Min = 0;
                fs.PrimaryKeyRange.Max = 0;
                fs.FileNo = fileNo;
                indexData_->segyFilesIndexed_.push_back(fs);
            }

            double elapsedSecondsGenerateIndex{0.0};
            IndexBuilderCollatedDataPtr indexDataPatch =
                [this, &fileNo, &segyFilePtr, &elapsedSecondsGenerateIndex]() -> auto {
                utilities::MeasureDuration generateIndexDuration(elapsedSecondsGenerateIndex);
                return generateIndexData(fileNo, segyFilePtr);
            }();

            if (indexDataPatch == nullptr) {
                SEGYLOG_INFO("Processing input file : " + segyFilePtr->getFileName() + "  failed!");
                break;
            }

            if (indexData_->is2D() and indexData_->segyFilesVector_.size() > 1) {
                const std::string msg = "segyIndexGenerator; SegyInvalidArgumentException: SEGY File: " + segyFilePtr->getFileName() + " was determined as 2D while list of files to index has several files."
                                        + "Currently indexation of several SEGY files in one process is not supported";
                SEGYLOG_REPORT_ERROR(msg);
                throw SegyInvalidArgumentException(msg);
            }

            SEGYLOG_DEBUG("GenerateIndexData: File: " + std::to_string(fileNo) + " Elapsed seconds: " + std::to_string(elapsedSecondsGenerateIndex));

            indexCollatedData.push_back(indexDataPatch);

            SEGYLOG_REPORT_INFO("segyIndexGenerator; Finished processing input file : " + segyFilePtr->getFileName());
            if (fileNo < std::numeric_limits<uint16_t>::max()) { // This will only be an issue for a run-away process...
                fileNo++;
            }
            else {
                break;
            }
        }
    }

    SEGYLOG_DEBUG("Elapsed time: " + std::to_string(elapsedSecondsProcessing) + " seconds.");

    const auto segyFilePtr = indexData_->segyFilesVector_[0];
    const segyio::VerticalDomain domain = segyFilePtr->getFileHeaderDescription()->getVerticalDomain();
    const segyio::VerticalUOM zUoM = segyFilePtr->getFileHeaderDescription()->getVerticalUOM();
    const auto sampleFormat = segyFilePtr->getBinaryHeaderField(segyformat::internal::FileHeaderDefinitionStringConstsBinaryHeader::SampleDataFormat);
    const auto sampleSize = segyFilePtr->getBytesPerSample(getSampleFormatFromCode(sampleFormat));

    try {
        if (!indexCollatedData.empty()) {
            double elapsedSecondsPostProcessing{0.0};

            SEGYLOG_DEBUG("Start post-processing SEG-Y files: ");

            bool const success =
                [this, &elapsedSecondsPostProcessing, &indexCollatedData, &domain, &zUoM,
                 &sampleSize, &forceGenerateIndex]() -> auto {
                utilities::MeasureDuration postprocessingDuration(elapsedSecondsPostProcessing);

                evaluateStatusFlags(indexCollatedData);
                populateMultiFileInformation(indexCollatedData);
                return ProcessIndexBuildingData(indexCollatedData,
                                                domain,
                                                zUoM,
                                                sampleSize,
                                                forceGenerateIndex);
            }();

            SEGYLOG_DEBUG("Post processing time: " + std::to_string(elapsedSecondsPostProcessing) + " seconds.");
            SEGYLOG_DEBUG("Total running time: " + std::to_string(elapsedSecondsProcessing + elapsedSecondsPostProcessing) + " seconds.");

            return success;
        }
    }
    catch (SegyException& ex) {
        SEGYLOG_REPORT_ERROR("segyIndexGenerator; SegyException: SEG-Y Error processing index data : " + std::string(ex.what()));
        throw;
    }
    catch (std::exception& ex) {
        SEGYLOG_REPORT_ERROR("segyIndexGenerator; exception: Error processing index data : " + std::string(ex.what()));
        throw;
    }
    catch (...) {
        SEGYLOG_REPORT_ERROR("segyIndexGenerator; Unknown error processing index data");
        throw;
    }

    return false;
}

void
SegyIndexGenerator::ResetIndexData() const
{
    setInitialDefaults();
    setEmptyGeometry3D();
    setEmptyFileLayout();
}

void
SegyIndexGenerator::ProcessIndexBuildingData3D(IndexBuilderCollatedDataVector const& indexCollatedDataVector,
                                               segyio::VerticalDomain domain,
                                               segyio::VerticalUOM zUoM,
                                               bool forceGenerateIndex)
{
    double elapsedSecondsPopulateGeometry{0.0};
    {
        utilities::MeasureDuration populateGeometryDuration(elapsedSecondsPopulateGeometry);
        GeometryCalculationData const calcData = populateGeometryCalculationData(indexCollatedDataVector);
        indexData_->geometry3D_ = populateGeometry3D(indexCollatedDataVector, calcData, domain, zUoM);
        indexData_->fileLayout_ = populateFileLayout(calcData, domain, zUoM);
    }

    SEGYLOG_DEBUG("Calculate Geometry:  Elapsed seconds: " + std::to_string(elapsedSecondsPopulateGeometry));

    if (!indexData_->fileLayout_.isRegular || forceGenerateIndex) {

        for (IndexBuilderCollatedDataPtr const& collatedData : indexCollatedDataVector) {

            trace_range_map::TraceRangeFactory traceRangeFactory(indexData_->fileLayout_,
                                                                 collatedData,
                                                                 indexData_->traceIndexStorage_);

            auto traceDataManager = collatedData->traceDataManager;
            if (traceDataManager == nullptr || !traceDataManager->IsReaderReady())
                continue;

            while (!traceDataManager->AtReaderEnd()) {
                const auto indexData = traceDataManager->ReadNext();

                if (indexData_->fileLayout_.isInlineOrdered) {
                    traceRangeFactory.addTrace(indexData.InlineNo,
                                               indexData.CrosslineNo,
                                               indexData.SampleCount,
                                               indexData.FileNo,
                                               indexData.Offset);
                }
                else {
                    traceRangeFactory.addTrace(indexData.CrosslineNo,
                                               indexData.InlineNo,
                                               indexData.SampleCount,
                                               indexData.FileNo,
                                               indexData.Offset);
                }
            } // while ! AtReaderEnd()

            traceDataManager->CloseReader();
        } // for collatedData

        indexData_->traceIndexMap_.addRanges(indexData_->traceIndexStorage_);

        trace_range_map::PrimaryIndex const prii0 = trace_range_map::TraceRangeFactory::getPrimaryExtentIndexFirst(indexData_->fileLayout_);
        trace_range_map::PrimaryIndex const priiN = trace_range_map::TraceRangeFactory::getPrimaryExtentIndexLast(indexData_->fileLayout_);
        trace_range_map::PrimaryIndex const priStep = trace_range_map::TraceRangeFactory::getPrimaryExtentStep(indexData_->fileLayout_);
        trace_range_map::SecondaryIndex const seci0 = trace_range_map::TraceRangeFactory::getSecondaryExtentIndexFirst(indexData_->fileLayout_);
        trace_range_map::SecondaryIndex const seciN = trace_range_map::TraceRangeFactory::getSecondaryExtentIndexLast(indexData_->fileLayout_);
        trace_range_map::SecondaryIndex const seciStep = trace_range_map::TraceRangeFactory::getSecondaryExtentStep(indexData_->fileLayout_);

        auto firstAddedRange = indexData_->traceIndexMap_.enumerateMissingRanges(prii0, priiN, priStep,
                                                                                 seci0, seciN, seciStep,
                                                                                 indexData_->traceIndexStorage_);

        indexData_->traceIndexMap_.addRanges(firstAddedRange, indexData_->traceIndexStorage_.cend());

        indexData_->indexedTraceCount_ = indexData_->traceIndexMap_.enumerateLiveTraceCount(prii0, priiN, priStep,
                                                                                            seci0, seciN, seciStep);
    } // if !fileLayoutSpecification_.isRegular
}

VerticalExtent
populateSampleExtent(GeometryCalculationData const& calcData,
                     segyio::VerticalDomain domain,
                     segyio::VerticalUOM zUoM)
{
    // We will not unit convert Depth values, rather preserve domain and vertical UoM ...
    // For DEPTH domain, the sample rate should by this stage be in 1000,000 units, so divide by 1000,000
    // However, Time first and last values are in ms so divide by 1000
    // For DEPTH domain, the sample rate should by this stage be in 1000 units, so divide by 1000  (Pre SEG-Y rev 2)
    // Depth first and last values are in whole units (meter or feet) so divide by 1000
    // here we are choosing the smallest step ever
    auto const sampleExtentStep = std::fabs(calcData.getSampleRateRange().Min) < std::fabs(calcData.getSampleRateRange().Max)
                                      ? calcData.getSampleRateRange().Min
                                      : calcData.getSampleRateRange().Max;
    auto const sampleExtentFirst = sampleExtentStep >= 0.0 // positive step means the first value is min, max otherwise
                                       ? calcData.getTofsRange().Min
                                       : calcData.getTraceDepthRange().Max;
    auto const sampleExtentLast = sampleExtentStep >= 0.0 // positive step means the last value is max, min otherwise
                                      ? calcData.getTraceDepthRange().Max
                                      : calcData.getTofsRange().Min;

    const double sampleRateFactor = domain == segyio::VerticalDomain::Time ? 1000000.0 : 1000.0;
    return {
        domain == segyio::VerticalDomain::Time ? sampleExtentFirst / 1000.0 : sampleExtentFirst,
        domain == segyio::VerticalDomain::Time ? sampleExtentLast / 1000.0 : sampleExtentLast,
        sampleExtentStep / sampleRateFactor, // We do not support variable sample rate!
        domain,
        zUoM};
}

void
SegyIndexGenerator::ProcessIndexBuildingData2D(IndexBuilderCollatedDataVector const& indexCollatedDataVector,
                                               segyio::VerticalDomain domain,
                                               segyio::VerticalUOM zUoM,
                                               size_t sampleSize)
{
    double populateIndexData{0.0};

    GeometryCalculationData calculationData;

    for (IndexBuilderCollatedDataPtr const& collatedData : indexCollatedDataVector) {

        // Need that to populate Sample Extents
        calculationData.updateCalculatedRangeValues(collatedData);

        auto traceDataManager = collatedData->traceDataManager;
        if (traceDataManager == nullptr || !traceDataManager->IsReaderReady())
            continue;

        auto& line2dMap = indexData_->indexing2dInfo_;
        while (!traceDataManager->AtReaderEnd()) {
            const auto indexData = traceDataManager->ReadNext();
            line2dMap.updateWithIndexData(indexData);
        }

        traceDataManager->CloseReader();
    }
    indexData_->indexing2dInfo_.setSampleSize(sampleSize);
    indexData_->indexing2dInfo_.setTraceHeaderSize(indexData_->traceHeaderSize_);
    indexData_->indexing2dInfo_.setSeismicLineExtents(populateSampleExtent(calculationData, domain, zUoM));

    indexData_->indexedTraceCount_ = indexData_->indexing2dInfo_.getIndexedTraceCount();

    SEGYLOG_DEBUG("Processing Index Data 2D: Elapsed seconds: " + std::to_string(populateIndexData));
}

void
SegyIndexGenerator::checkSegyDimension(DimensionTraceHeaders& traceDimensionInfo, const std::string& segyFileName) const
{
    if (not indexData_->is2D()) {
        for (size_t i = 1; i < traceDimensionInfo.size(); ++i) {
            int inlineStep = traceDimensionInfo[i].inlineNo - traceDimensionInfo[i - 1].inlineNo;
            int crosslineStep = traceDimensionInfo[i].crosslineNo - traceDimensionInfo[i - 1].crosslineNo;
            // There could be holes. But if 2 steps are 0 - most probably it's 2D
            if (inlineStep == 0 and crosslineStep == 0) {
                const std::string msg = "SEGY: " + segyFileName + " was set to have 3D data while the data seems to be 2D";
                throw SegyInvalidArgumentException(msg);
            }
        }
    }
    else {
        // Check that there are no duplicates
        for (auto it = traceDimensionInfo.begin(); it != traceDimensionInfo.end();) {
            const int checkCMP = it->CMP;
            it = traceDimensionInfo.erase(it);
            if (std::any_of(traceDimensionInfo.begin(), traceDimensionInfo.end(),
                            [&checkCMP](const auto& t) { return checkCMP == t.CMP; })) {
                const std::string msg = "SEGY: " + segyFileName + " was set to have 2D data while the data seems to be 3D";
                throw SegyInvalidArgumentException(msg);
            }
        }
    }
}

bool
SegyIndexGenerator::ProcessIndexBuildingData(IndexBuilderCollatedDataVector const& indexCollatedDataVector,
                                             segyio::VerticalDomain domain,
                                             segyio::VerticalUOM zUoM,
                                             size_t sampleSize,
                                             bool forceGenerateIndex)
{
    if (indexCollatedDataVector.empty()) {
        return false;
    }

    if (indexData_->is2D()) {
        ProcessIndexBuildingData2D(indexCollatedDataVector, domain, zUoM, sampleSize);
    }
    else {
        ProcessIndexBuildingData3D(indexCollatedDataVector, domain, zUoM, forceGenerateIndex);
    }

    for (IndexBuilderCollatedDataPtr const& collatedData : indexCollatedDataVector) {
        if (collatedData->SampleAmplitudeRange.Min < indexData_->amplitudeMinimum_) {
            indexData_->amplitudeMinimum_ = collatedData->SampleAmplitudeRange.Min;
        }
        if (collatedData->SampleAmplitudeRange.Max > indexData_->amplitudeMaximum_) {
            indexData_->amplitudeMaximum_ = collatedData->SampleAmplitudeRange.Max;
        }
        collatedData->traceDataManager->CloseReader();
    }

    // Need a way for index lookup to tell what file to use when using calculated index!!!
    setOffsetToFirstSEGYTrace(indexCollatedDataVector[0]->FirstTraceOffset);
    if (indexCollatedDataVector[0]->TraceLengthRange.Min == indexCollatedDataVector[0]->TraceLengthRange.Max) {
        setTraceLength(indexCollatedDataVector[0]->TraceLengthRange.Min);
    }

    setSampleSize(sampleSize);

    return true;
}

size_t
SegyIndexGenerator::getIndexReadBufferSize()
{
    const auto tmp = utilities::Environment::instance().SegyIndexReadBufferSize();

    if (!tmp) {
        return 25U * 1024U * 1024U; // 25 MB
    }

    return tmp;
}

CollationOffset
SegyIndexGenerator::prepareCollatedOffsets(HeaderDefinitionVector const& indexHeaders)
{
    int ix = 0;
    CollationOffset offsets;

    for (HeaderDefinitionPtr const& hdr : indexHeaders) {
        auto id = hdr->getHeaderId();

        if (id == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ScalarOnCoordinates) {
            offsets.CoordinateScalar = ix;
        }
        else if (id == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ShotToRecordingLagTime) {
            offsets.Tofs = ix;
        }
        else if (id == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SampleRate) {
            offsets.SampleRate = ix;
        }
        else if (id == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::TraceLength) {
            offsets.SampleCount = ix;
        }
        else if (id == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::TraceWeightFactor) {
            offsets.TraceWeightingFactor = ix;
        }
        else if (id == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Inline) {
            offsets.InlineNo = ix;
        }
        else if (id == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Crossline) {
            offsets.CrosslineNo = ix;
        }
        else if (id == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SourceXCoordinate) {
            offsets.SourceXCoordinate = ix;
        }
        else if (id == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SourceYCoordinate) {
            offsets.SourceYCoordinate = ix;
        }
        else if (id == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::TraceSequenceLine) {
            offsets.TraceNo = ix;
        }
        else if (id == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CMP) {
            offsets.CMP = ix;
        }
        ix++;
    }

    return offsets;
}

IndexBuilderData
SegyIndexGenerator::extractIndexBuilderDataFromTraceHeaders(const CollationOffset& collatedOffsets,
                                                            const double* values, size_t fileNo, size_t offset)
{
    IndexBuilderData ibd{};

    ibd.FileNo = fileNo;
    ibd.Offset = offset;

    if (collatedOffsets.InlineNo >= 0) {
        ibd.InlineNo = static_cast<int>(values[collatedOffsets.InlineNo]);
    }
    if (collatedOffsets.CrosslineNo >= 0) {
        ibd.CrosslineNo = static_cast<int>(values[collatedOffsets.CrosslineNo]);
    }
    if (collatedOffsets.CMP >= 0) {
        ibd.CMP = static_cast<int>(values[collatedOffsets.CMP]);
    }
    if (collatedOffsets.TraceNo >= 0) {
        ibd.TraceNo = static_cast<size_t>(values[collatedOffsets.TraceNo]);
    }
    if (collatedOffsets.SourceXCoordinate >= 0) {
        ibd.X = values[collatedOffsets.SourceXCoordinate];
    }
    if (collatedOffsets.SourceYCoordinate >= 0) {
        ibd.Y = values[collatedOffsets.SourceYCoordinate];
    }
    if (collatedOffsets.SampleCount >= 0) {
        ibd.SampleCount = static_cast<size_t>(values[collatedOffsets.SampleCount]);
    }

    return ibd;
}

void
SegyIndexGenerator::updateCollatedDataRanges(IndexBuilderCollatedDataPtr const& collated, const IndexBuilderData& ibd,
                                             const CollationOffset& collatedOffsets, const double* values)
{
    double tofs = tofsMax_;
    double sampleRate = 0;
    int sampleCount = 0;

    if (collatedOffsets.CoordinateScalar >= 0)
        collated->CoordinateScalarRange.UpdateMinMax(static_cast<int>(values[collatedOffsets.CoordinateScalar]));

    if (collatedOffsets.Tofs >= 0) {
        tofs = values[collatedOffsets.Tofs];
        collated->TofsRange.UpdateMinMax(tofs);
    }

    if (collatedOffsets.SampleRate >= 0) {
        sampleRate = values[collatedOffsets.SampleRate];
        collated->SampleRateRange.UpdateMinMax(static_cast<int>(values[collatedOffsets.SampleRate]));
    }

    if (collatedOffsets.SampleCount >= 0) {
        sampleCount = static_cast<int>(values[collatedOffsets.SampleCount]);
        collated->TraceLengthRange.UpdateMinMax(static_cast<uint16_t>(values[collatedOffsets.SampleCount]));

        if (ibd.MaximumAmplitude > ibd.MinimumAmplitude) {
            collated->SampleAmplitudeRange.UpdateMinMax(ibd.MinimumAmplitude);
            collated->SampleAmplitudeRange.UpdateMinMax(ibd.MaximumAmplitude);
        }
    }

    if (collatedOffsets.TraceWeightingFactor >= 0)
        collated->TraceWeightingFactorRange.UpdateMinMax(static_cast<int>(values[collatedOffsets.TraceWeightingFactor]));

    if (sampleCount > 0 && tofs < tofsMax_ && std::abs(sampleRate) > 0) {

        //
        // Note:
        //
        // Here is the range [ A, B ] special treatment.
        //
        // The last value in the range is estimated by the following formula:
        //    B = A + ( f / 1000.0 ) N
        // where
        //    A - first value in the range [ A, B ]
        //    f - sample rate
        //    N - samples count
        //
        // E.g. for [ 0.00, 35.04 ] range with the sample rate of 40.0 the number of samples of 876 pcs gives.
        //     0.00 + ( 40.0 / 1000.0) / 876 = 35.04
        //
        const double traceDepth = tofs + sampleRate / 1000.0 * static_cast<double>(sampleCount);
        collated->TraceDepthRange.UpdateMinMax(traceDepth);
    }

    collated->InlineNoRange.UpdateMinMax(ibd.InlineNo);
    collated->CrosslineNoRange.UpdateMinMax(ibd.CrosslineNo);
    collated->CMPRange.UpdateMinMax(ibd.CMP);
    // As read is performed trace by trace, last updated TraceNo will be the last in the file
    collated->LastTraceNo = ibd.TraceNo;
}

Range<float>
SegyIndexGenerator::getTraceAmplitudeRange(const float* samples, size_t sampleCount) const
{
    auto min = std::numeric_limits<float>::max();
    auto max = -min;

    size_t i;

    try {
        if (samples != NULL) {
            for (i = 0; i < sampleCount; i++) {
                const auto value = samples[i];  
                if (value < min)
                    min = value;
                if (value > max)
                    max = value;
            }
        }
    }
    catch (std::exception* e) {
        std::string msg = "getTraceAmplitudeRange: Error extracting trace samples. sampleNo = " + std::to_string(i) + "\nException : " + std::string(e->what());
        SEGYLOG_REPORT_ERROR(msg);
    }

    return {min, max};
}

IndexBuilderCollatedDataPtr
SegyIndexGenerator::generateIndexData(size_t fileNo,
                                      SegyFilePtr const& segyFilePtr) const
{
    IndexBuilderCollatedDataPtr result = std::make_shared<IndexBuilderCollatedData>();

    HeaderDefinitionVector const traceHeaders = *segyFilePtr->getFileHeaderDescription()->getTraceHeaders();
    HeaderDefinitionVector const indexHeaders = getHeadersToExtractForIndex(traceHeaders);
    if (indexHeaders.empty()) {
        return result;
    }

    try {
        constexpr std::chrono::seconds reportFrequency{10};
        std::string taskName = "Indexing for ";
        taskName.append(segyFilePtr->getFileName());
        segysdk::logger::ProgressReporting progress(reportFrequency, taskName);
        const std::string reportMessage = "Indexing in progress";
        progress.report(reportMessage, 0.0);

        auto fileSize = segyFilePtr->getFileSize();

        CollationOffset collatedOffsets = prepareCollatedOffsets(indexHeaders);

        size_t const generateIndexPrintPoint_ = segysdk::fileio::FileManager::isSDDataset(segyFilePtr->getFileName())
                                                    ? generateIndexPrintPointCloud_
                                                    : generateIndexPrintPointLocal_;

        SEGYLOG_DEBUG("Reporting every " + std::to_string(generateIndexPrintPoint_) + " traces ... ");

        FileReaderVector files(index_config::numberOfTraceScannerPrefetchReaders_, nullptr);
        std::generate(
            files.begin(), files.end(), [&segyFilePtr]() -> auto {
                return segyFilePtr->getCloneFile();
            });

        constexpr bool raw = false;
        segyio::HeaderDefinitionProvider hdp(indexHeaders, raw);
        segyio::TraceSequentialScanner scanner(std::move(files), segyFilePtr->getDerivedHeader(), hdp, hdp.getHeaderNames());

        if (segysdk::fileio::FileManager::isSDDataset(segyFilePtr->getFileName())) {
            // Temporary performance work: This should probably only be done for local files ...
            size_t bufferSize = getIndexReadBufferSize();

            if (bufferSize > fileSize) {
                bufferSize = fileSize;
            }

            SEGYLOG_DEBUG("Using read buffer size = " + std::to_string(bufferSize));
            scanner.setReaderBufferSize(bufferSize); // Set buffer size 25MB
        }
        constexpr long const startTrace{0L};
        // Initialize scanner
        scanner.reset(request::TraceParams{startTrace});
        auto traceDataManager = std::make_shared<IndexBuilderDataManager>();
        result->traceDataManager = traceDataManager;

        struct TraceCollatedInfo
        {
            int inlineNo;
            int crosslineNo;
            size_t fileNo;
        };
        std::deque<TraceCollatedInfo> traceCollatedInfo;

        constexpr size_t dimensionHeadersToAnalyze = 100;
        bool dimensionChecked{false};
        DimensionTraceHeaders traceDimensionInfo;

        bool inlineIsPrimaryKey{false};
        bool crosslineIsPrimaryKey{false};
        bool isFirstTrace{true};

        size_t traceNo{0U};

        double moveNext_total{0.0};
        double segyRead_total{0.0};
        double extract_total{0.0};
        double collated_total{0.0};
        double evaluated_total{0.0};
        double writeData_total{0.0};

        double moveNext_aggregated{0.0};
        double segyRead_aggregated{0.0};
        double extract_aggregated{0.0};
        double collated_aggregated{0.0};
        double evaluated_aggregated{0.0};
        double writeData_aggregated{0.0};

        long lineNo{0L};

        auto credentials = segyFilePtr->getFileReference().readerOptions().sdCredentials;
        if (segysdk::fileio::FileManager::isSDDataset(segyFilePtr->getFileName())) {
            if (credentials.tokenSource()) {
                const auto dummy = credentials.token();
            }
        }
        constexpr int64_t oneHour = 3600;
        auto token_checked_time = std::chrono::steady_clock::now();
        double scanMoveNextTimer{0.0};
        double elapsedSecondsScanning{0.0};
        {
            utilities::MeasureDuration scanningDuration(elapsedSecondsScanning);
            while (advanceScanner(scanner, scanMoveNextTimer)) {

                moveNext_aggregated += scanMoveNextTimer;

                const double* values = scanner.getHeaderValues();

                uint64_t offset{};
                {
                    utilities::MeasureDuration segyReadDuration(segyRead_aggregated);

                    if (scanner.getHeaderValueCount() != indexHeaders.size()) {
                        throw SegyInvalidArgumentException("Returned trace header values do not match requested index fields!");
                    }

                    offset = scanner.getTraceInfo().offset_;
                    if (traceNo == 0) {
                        result->FirstTraceOffset = offset;
                    }
                }
                double currentProgress = (offset / static_cast<double>(fileSize)) * 100.0;
                progress.report(reportMessage, currentProgress);

                IndexBuilderData ibd = [&extract_aggregated, &collatedOffsets, &values, &fileNo, &offset]() {
                    utilities::MeasureDuration extractDuration(extract_aggregated);
                    return extractIndexBuilderDataFromTraceHeaders(collatedOffsets, values, fileNo, offset);
                }();

                {
                    auto rawSamples = scanner.getTraceSamples();
                    auto sampleMinMax = getTraceAmplitudeRange(rawSamples, ibd.SampleCount);
                    ibd.MinimumAmplitude = sampleMinMax.Min;
                    ibd.MaximumAmplitude = sampleMinMax.Max;
                }

                {
                    utilities::MeasureDuration updateCollatedDuration(collated_aggregated);
                    updateCollatedDataRanges(result, ibd, collatedOffsets, values);
                }

                {
                    utilities::MeasureDuration evaluatePrimaryDuration(evaluated_aggregated);
                    evaluatePrimaryAndSecondaryKeyDirection(result, ibd, inlineIsPrimaryKey, crosslineIsPrimaryKey, isFirstTrace);
                }

                result->InlineIsPrimary = inlineIsPrimaryKey;

                {
                    utilities::MeasureDuration writeDataDuration(writeData_aggregated);

                    // perform the trace info collecting since the `result->InlineIsPrimary` value is not correct
                    // for the very first primary index here
                    traceCollatedInfo.push_back({ibd.InlineNo, ibd.CrosslineNo, ibd.FileNo});

                    traceDataManager->WriteData(ibd);
                }

                if (traceNo == 0 || traceNo % generateIndexPrintPoint_ == 0) {
                    lineNo++;
                    double const total = moveNext_aggregated
                                         + segyRead_aggregated
                                         + extract_aggregated
                                         + collated_aggregated
                                         + evaluated_aggregated
                                         + writeData_aggregated;

                    SEGYLOG_DEBUG("Perf. stats: Line number: " + std::to_string(lineNo) + " TraceNo: [" + std::to_string(traceNo) + "] Inline= " + std::to_string(ibd.InlineNo) + " Crossline: " + std::to_string(ibd.CrosslineNo) + "  MoveNext: " + std::to_string(moveNext_aggregated) + "  SegyRead: " + std::to_string(segyRead_aggregated) + "  Extract: " + std::to_string(extract_aggregated) + " Update collated : " + std::to_string(collated_aggregated) + "  Eval primary : " + std::to_string(evaluated_aggregated) + " WriteData : " + std::to_string(writeData_aggregated) + " Total interval time: " + std::to_string(total) + "  Total elapsed time: " + std::to_string(scanningDuration.getElapsedDuration()));

                    moveNext_total += moveNext_aggregated;
                    segyRead_total += segyRead_aggregated;
                    extract_total += extract_aggregated;
                    collated_total += collated_aggregated;
                    evaluated_total += evaluated_aggregated;
                    writeData_total += writeData_aggregated;

                    moveNext_aggregated = 0.0;
                    segyRead_aggregated = 0.0;
                    extract_aggregated = 0.0;
                    collated_aggregated = 0.0;
                    evaluated_aggregated = 0.0;
                    writeData_aggregated = 0.0;
                }

                traceNo++;

                if (segysdk::fileio::FileManager::isSDDataset(segyFilePtr->getFileName())) {
                    auto const now = std::chrono::steady_clock::now();
                    auto const duration = now.time_since_epoch() - token_checked_time.time_since_epoch();
                    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration).count();
                    if (seconds > oneHour) {
                        auto dummy = credentials.token();
                        token_checked_time = now;
                    }
                }

                if (not dimensionChecked) {
                    if (traceDimensionInfo.size() < dimensionHeadersToAnalyze) {
                        traceDimensionInfo.push_back({ibd.InlineNo, ibd.CrosslineNo, ibd.CMP});
                    }
                    else {
                        checkSegyDimension(traceDimensionInfo, segyFilePtr->getFileName());
                        dimensionChecked = true;
                    }
                }
            }

            // perform the multiFileMap_ initialization
            if (result->InlineIsPrimary) {
                // inline is the primary index
                for (auto const& trace : traceCollatedInfo) {
                    indexData_->multiFileMap_.insert(decltype(indexData_->multiFileMap_)::value_type{trace.inlineNo, trace.fileNo});
                }
            }
            else {
                // crossline is the primary index
                for (auto const& trace : traceCollatedInfo) {
                    indexData_->multiFileMap_.insert(decltype(indexData_->multiFileMap_)::value_type{trace.crosslineNo, trace.fileNo});
                }
            }
        }

        SEGYLOG_DEBUG("Total traces scanned: " + std::to_string(traceNo));
        lineNo++;

        moveNext_total += moveNext_aggregated;
        segyRead_total += segyRead_aggregated;
        extract_total += extract_aggregated;
        collated_total += collated_aggregated;
        evaluated_total += evaluated_aggregated;
        writeData_total += writeData_aggregated;

        double const total = moveNext_total
                             + segyRead_total
                             + extract_total
                             + collated_total
                             + evaluated_total
                             + writeData_total;

        SEGYLOG_DEBUG("Perf. stats: Accumulated values Line number: " + std::to_string(lineNo) + "  MoveNext: " + std::to_string(moveNext_total) + "  SegyRead: " + std::to_string(segyRead_total) + "  Extract: " + std::to_string(extract_total) + "  Update collated : " + std::to_string(collated_total) + "  Eval primary : " + std::to_string(evaluated_total) + " WriteData : " + std::to_string(writeData_total) + " Total elapsed time: " + std::to_string(total) + "  Running total time: " + std::to_string(elapsedSecondsScanning));

        SEGYLOG_DEBUG("Collated data:");
        SEGYLOG_DEBUG("SEGY 2D :" + std::to_string(indexData_->is2D()));
        if (not indexData_->is2D()) {
            SEGYLOG_DEBUG("Inline is Primary :" + std::to_string(result->InlineIsPrimary));
            SEGYLOG_DEBUG("Min Inline :" + std::to_string(result->InlineNoRange.Min));
            SEGYLOG_DEBUG("Max Inline :" + std::to_string(result->InlineNoRange.Max));
            SEGYLOG_DEBUG("Min Crossline :" + std::to_string(result->CrosslineNoRange.Min));
            SEGYLOG_DEBUG("Max Crossline :" + std::to_string(result->CrosslineNoRange.Max));
        }
        else {
            SEGYLOG_DEBUG("Min CMP :" + std::to_string(result->CMPRange.Min));
            SEGYLOG_DEBUG("Max CMP :" + std::to_string(result->CMPRange.Max));
        }
        SEGYLOG_DEBUG("Min Depth :" + std::to_string(result->TraceDepthRange.Min));
        SEGYLOG_DEBUG("Max Depth :" + std::to_string(result->TraceDepthRange.Max));
        SEGYLOG_DEBUG("Min Trace length :" + std::to_string(result->TraceLengthRange.Min));
        SEGYLOG_DEBUG("Max Trace length :" + std::to_string(result->TraceLengthRange.Max));
        SEGYLOG_DEBUG("Min Sample amplitude : " + std::to_string(result->SampleAmplitudeRange.Min));
        SEGYLOG_DEBUG("Max Sample amplitude : " + std::to_string(result->SampleAmplitudeRange.Max));

        traceDataManager->CloseWriter();

        return result;
    }
    catch (const std::system_error& e) {
        SEGYLOG_REPORT_ERROR("segyIndexGenerator; generateIndexData; system_error: " + std::string(e.what())
                             + "; code: " + std::to_string(e.code().value())
                             + "; category: " + std::string(e.code().category().name())
                             + "; message: " + e.code().message());
    }
    catch (const std::exception& e) {
        SEGYLOG_REPORT_ERROR("segyIndexGenerator; generateIndexData; exception: " + std::string(e.what()));
    }
    catch (...) {
        SEGYLOG_REPORT_ERROR("segyIndexGenerator; generateIndexData; unexpected exception");
    }

    return nullptr;
}

namespace
{
template<typename ENUM>
constexpr std::underlying_type_t<ENUM>
getEnumValue(ENUM value)
{
    return static_cast<std::underlying_type_t<ENUM>>(value);
}
} // namespace

segyio::SampleFormat
SegyIndexGenerator::getSampleFormatFromCode(int sampleFormat)
{
    switch (sampleFormat) {

    case getEnumValue(segyio::SampleFormat::IBMFloat4):
        return segyio::SampleFormat::IBMFloat4;
    case getEnumValue(segyio::SampleFormat::INT4):
        return segyio::SampleFormat::INT4;
    case getEnumValue(segyio::SampleFormat::INT2):
        return segyio::SampleFormat::INT2;
    case getEnumValue(segyio::SampleFormat::FixedPointWithGain4):
        return segyio::SampleFormat::FixedPointWithGain4;
    case getEnumValue(segyio::SampleFormat::IeeeFloat4):
        return segyio::SampleFormat::IeeeFloat4;
    case getEnumValue(segyio::SampleFormat::INT1):
        return segyio::SampleFormat::INT1;
    case getEnumValue(segyio::SampleFormat::IeeeFloat8):
        return segyio::SampleFormat::IeeeFloat8;
    case getEnumValue(segyio::SampleFormat::INT3):
        return segyio::SampleFormat::INT3;
    case getEnumValue(segyio::SampleFormat::UINT4):
        return segyio::SampleFormat::UINT4;
    case getEnumValue(segyio::SampleFormat::UINT2):
        return segyio::SampleFormat::UINT2;
    case getEnumValue(segyio::SampleFormat::UINT3):
        return segyio::SampleFormat::UINT3;
    case getEnumValue(segyio::SampleFormat::UINT1):
        return segyio::SampleFormat::UINT1;
    default:
        return segyio::SampleFormat::INT4; // TODO unsafe default
    }
}

void
SegyIndexGenerator::evaluateStatusFlags(IndexBuilderCollatedDataVector const& indexCollatedDataPatches) const
{
    if (indexCollatedDataPatches.empty()) {
        return;
    }

    indexData_->hasConstantCoordinateScalar_ = true;
    indexData_->hasConstantTraceLength_ = true;
    indexData_->hasConstantTraceWeightingFactor_ = true;

    for (IndexBuilderCollatedDataPtr const& collated : indexCollatedDataPatches) {
        indexData_->hasConstantCoordinateScalar_ = indexData_->hasConstantCoordinateScalar_ && collated->CoordinateScalarRange.Min == collated->CoordinateScalarRange.Max;

        indexData_->hasConstantTraceLength_ = indexData_->hasConstantTraceLength_ && collated->TraceLengthRange.Min == collated->TraceLengthRange.Max;

        indexData_->hasConstantTraceWeightingFactor_ = indexData_->hasConstantTraceWeightingFactor_ && collated->TraceWeightingFactorRange.Min == collated->TraceWeightingFactorRange.Max;
    }

    if (indexData_->hasConstantCoordinateScalar_) {
        indexData_->constantCoordinateScalar_ = indexCollatedDataPatches[0]->CoordinateScalarRange.Min;
    }

    if (indexData_->hasConstantTraceLength_) {
        indexData_->constantTraceLength_ = indexCollatedDataPatches[0]->TraceLengthRange.Min;
    }

    if (indexData_->hasConstantTraceWeightingFactor_) {
        indexData_->constantTraceWeightingFactor_ = static_cast<unsigned short>(indexCollatedDataPatches[0]->TraceWeightingFactorRange.Min);
    }
}

void
SegyIndexGenerator::populateMultiFileInformation(IndexBuilderCollatedDataVector const& indexCollatedDataVector) const
{
    if (indexCollatedDataVector.empty()) {
        throw SegyInvalidArgumentException("populateMultiFileInformation: No collated trace data to process!");
    }
    uint16_t fileNo = 0;
    const bool isInlineOriented = indexCollatedDataVector[0]->InlineIsPrimary;
    for (const auto& collated : indexCollatedDataVector) {
        if (isInlineOriented != collated->InlineIsPrimary) {
            throw SegyInvalidArgumentException(
                "populateMultiFileInformation: All SEG-Y files must have the same primary index!");
        }
        updateMultiFileRecord(fileNo, collated->FirstTraceOffset, Range(isInlineOriented ? collated->InlineNoRange : collated->CrosslineNoRange));
        fileNo++;
    }
}

void
SegyIndexGenerator::setInitialDefaults() const
{
    indexData_->setInitialDefaults();
}

void
SegyIndexGenerator::setEmptyGeometry3D() const
{
    indexData_->setEmptyGeometry3D();
}

void
SegyIndexGenerator::setEmptyFileLayout() const
{
    indexData_->setEmptyFileLayout();
}

HeaderDefinitionVector
SegyIndexGenerator::getHeadersToExtractForIndex(HeaderDefinitionVector const& traceHeaders)
{
    HeaderDefinitionVector indexHeaders;

    for (HeaderDefinitionPtr const& hdr : traceHeaders) {
        if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ScalarOnCoordinates) {
            indexHeaders.push_back(hdr);
        }
        else if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Inline) {
            indexHeaders.push_back(hdr);
        }
        else if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::Crossline) {
            indexHeaders.push_back(hdr);
        }
        else if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::CMP) {
            indexHeaders.push_back(hdr);
        }
        else if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::TraceSequenceLine) {
            indexHeaders.push_back(hdr);
        }
        else if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SourceXCoordinate) {
            indexHeaders.push_back(hdr);
        }
        else if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SourceYCoordinate) {
            indexHeaders.push_back(hdr);
        }
        else if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::SampleRate) {
            indexHeaders.push_back(hdr);
        }
        else if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::TraceLength) {
            indexHeaders.push_back(hdr);
        }
        else if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::ShotToRecordingLagTime) {
            indexHeaders.push_back(hdr);
        }
        else if (hdr->getHeaderId() == segyformat::internal::FileHeaderDefinitionStringConstsTraceHeader::TraceWeightFactor) {
            indexHeaders.push_back(hdr);
        }
    }

    return indexHeaders;
}

bool
SegyIndexGenerator::advanceScanner(segyio::SegyTraceScanner& scanner, double& executionSeconds)
{
    executionSeconds = 0.0;
    utilities::MeasureDuration advanceScannerDuration(executionSeconds);

    return scanner.moveNext({});
}

void
SegyIndexGenerator::evaluatePrimaryAndSecondaryKeyDirection(IndexBuilderCollatedDataPtr const& collated, const IndexBuilderData& ibd,
                                                            bool& inlineIsPrimaryKey, bool& crosslineIsPrimaryKey, bool& isFirstTrace)
{
    if (isFirstTrace) {
        isFirstTrace = false;
    }
    else {
        if (!inlineIsPrimaryKey && !crosslineIsPrimaryKey) {
            evaluatePrimaryKey(collated->InlineNoRange, collated->CrosslineNoRange, ibd.InlineNo, ibd.CrosslineNo, inlineIsPrimaryKey, crosslineIsPrimaryKey);
        }
    }
}

void
SegyIndexGenerator::updateMultiFileRecord(uint16_t fileNo, size_t fileOffset, Range<int> primaryRange) const
{
    uint16_t const ix = findMultiFileEntry(fileNo);
    if (ix <= indexData_->segyFilesIndexed_.size()) {
        indexData_->segyFilesIndexed_[ix].FirstTraceOffset = fileOffset;
        indexData_->segyFilesIndexed_[ix].PrimaryKeyRange = primaryRange;
    }
}

Geometry3D
SegyIndexGenerator::populateGeometry3D(IndexBuilderCollatedDataVector const& collatedIndexData,
                                       GeometryCalculationData const& calcData,
                                       segyio::VerticalDomain domain,
                                       segyio::VerticalUOM zUoM)
{
    // isRegular = !forceGenerateIndex && isRegular && (minTraceLength == maxTraceLength) && (minTraceTraceGap == maxTraceTraceGap);

    Geometry3D geometry3d;

    {
        // here we are choosing the smallest step ever
        auto const jExtentStep = std::abs(calcData.getInlineStepRange().Min) < std::abs(calcData.getInlineStepRange().Max)
                                     ? calcData.getInlineStepRange().Min
                                     : calcData.getInlineStepRange().Max;
        geometry3d.JVectorExtent = {
            calcData.getInlineRange().Min,
            calcData.getInlineRange().Max,
            std::abs(jExtentStep) // for the DataGrid, the J-extent step value should be positive
        };
    }

    if (geometry3d.JVectorExtent.count() < 0) {
        throw SegyInvalidArgumentException("Illegal Geometry 3D for inline: number of inlines cannot be less then zero.");
    }
    if (geometry3d.JVectorExtent.first() > geometry3d.JVectorExtent.last()) {
        std::string const msg = "Illegal Geometry 3D for inline: first annotation value (" + std::to_string(geometry3d.JVectorExtent.first()) + ") is greater then the last one (" + std::to_string(geometry3d.JVectorExtent.last()) + ").";
        throw SegyInvalidArgumentException("");
    }

    {
        // here we are choosing the smallest step ever
        auto const iExtentStep = std::abs(calcData.getCrosslineStepRange().Min) < std::abs(calcData.getCrosslineStepRange().Max)
                                     ? calcData.getCrosslineStepRange().Min
                                     : calcData.getCrosslineStepRange().Max;
        geometry3d.IVectorExtent = {
            calcData.getCrosslineRange().Min,
            calcData.getCrosslineRange().Max,
            std::abs(iExtentStep) // for the DataGrid, the I-extent step value should be positive
        };
    }

    if (geometry3d.IVectorExtent.count() < 0) {
        throw SegyInvalidArgumentException("Illegal Geometry 3D for crossline: number of crosslines cannot be less then zero.");
    }
    if (geometry3d.IVectorExtent.first() > geometry3d.IVectorExtent.last()) {
        std::string const msg = "Illegal Geometry 3D for crossline: first annotation value (" + std::to_string(geometry3d.IVectorExtent.first()) + ") is greater then the last one (" + std::to_string(geometry3d.IVectorExtent.last()) + ").";
        throw SegyInvalidArgumentException("");
    }

    // We will not unit convert Depth values, rather preserve domain and vertical UoM ...
    // For TIME domain, the sample rate should by this stage be in 1000,000 units, so divide by 1000,000
    // Time first and last values are in ms so divide by 1000
    // For DEPTH domain, the sample rate should by this stage be in 1000 units, so divide by 1000  (Pre SEG-Y rev 2)
    // Depth first and last values are in whole units (meter or feet) so divide by 1000
    {
        // here we are choosing the smallest step ever
        auto const sampleExtentStep = std::fabs(calcData.getSampleRateRange().Min) < std::fabs(calcData.getSampleRateRange().Max)
                                          ? calcData.getSampleRateRange().Min
                                          : calcData.getSampleRateRange().Max;

        const double sampleRateFactor = domain == segyio::VerticalDomain::Time ? 1000000.0 : 1000.0;
        geometry3d.sampleExtent = {
            domain == segyio::VerticalDomain::Time ? calcData.getTofsRange().Min / 1000.0 : calcData.getTofsRange().Min,
            domain == segyio::VerticalDomain::Time ? calcData.getTraceDepthRange().Max / 1000.0 : calcData.getTraceDepthRange().Max,
            std::fabs(sampleExtentStep) / sampleRateFactor, // We do not support variable sample rate!
                                                            // for the DataGrid, the sample step value should be positive
            domain,
            zUoM};
    }

    geometry3d.isRegular = calcData.IsRegular();
    geometry3d.isInlineOrdered = calcData.IsInlineOrdered();

    double elapsedSecondsCompleteGeometry{0.0};
    {
        utilities::MeasureDuration completeGeometryDuration(elapsedSecondsCompleteGeometry);

        CompleteGridHandler handler(collatedIndexData, calcData);

        if (geometry3d.isRegular) {
            geometry3d.dataGrid = handler.GenerateRegularDataGrid(geometry3d);
        }
        else {
            geometry3d.dataGrid = handler.GenerateIrregularDataGrid(geometry3d);
        }
    }

    SEGYLOG_DEBUG("Calculate Geometry Grid : Elapsed time: " + std::to_string(elapsedSecondsCompleteGeometry) + " seconds");

    return geometry3d;
}

FileLayoutSpecification
SegyIndexGenerator::populateFileLayout(GeometryCalculationData const& calcData,
                                       segyio::VerticalDomain domain,
                                       segyio::VerticalUOM zUoM)
{
    FileLayoutSpecification result;

    {
        // here we are choosing the smallest step ever
        auto const jExtentStep = std::abs(calcData.getInlineStepRange().Min) < std::abs(calcData.getInlineStepRange().Max)
                                     ? calcData.getInlineStepRange().Min
                                     : calcData.getInlineStepRange().Max;
        auto const jExtentFirst = jExtentStep >= 0 // positive step means the first value is min, max otherwise
                                      ? calcData.getInlineRange().Min
                                      : calcData.getInlineRange().Max;
        auto const jExtentLast = jExtentStep >= 0 // positive step means the last value is max, min otherwise
                                     ? calcData.getInlineRange().Max
                                     : calcData.getInlineRange().Min;
        result.JExtent = {
            jExtentFirst,
            jExtentLast,
            jExtentStep};
    }

    if (result.JExtent.count() < 0) {
        throw SegyInvalidArgumentException("Illegal FileLayoutSpecification for inline: number of inlines cannot be less then zero.");
    }

    {
        // here we are choosing the smallest step ever
        auto const iExtentStep = std::abs(calcData.getCrosslineStepRange().Min) < std::abs(calcData.getCrosslineStepRange().Max)
                                     ? calcData.getCrosslineStepRange().Min
                                     : calcData.getCrosslineStepRange().Max;
        auto const iExtentFirst = iExtentStep >= 0 // positive step means the first value is min, max otherwise
                                      ? calcData.getCrosslineRange().Min
                                      : calcData.getCrosslineRange().Max;
        auto const iExtentLast = iExtentStep >= 0 // positive step means the last value is max, min otherwise
                                     ? calcData.getCrosslineRange().Max
                                     : calcData.getCrosslineRange().Min;
        result.IExtent = {
            iExtentFirst,
            iExtentLast,
            iExtentStep};
    }

    if (result.IExtent.count() < 0) {
        throw SegyInvalidArgumentException("Illegal FileLayoutSpecification for crossline: number of crosslines cannot be less then zero.");
    }

    result.sampleExtent = populateSampleExtent(calcData, domain, zUoM);

    result.isRegular = calcData.IsRegular();
    result.isInlineOrdered = calcData.IsInlineOrdered();

    return result;
}

GeometryCalculationData
SegyIndexGenerator::populateGeometryCalculationData(IndexBuilderCollatedDataVector const& collatedIndexData)
{
    GeometryCalculationData result;
    int lastInline = 0;
    int lastCrossline = 0;
    bool onFirstTrace = true;

    double elapsedSecondsCollateGeometry{0.0};
    {
        utilities::MeasureDuration collateGeometryDuration(elapsedSecondsCollateGeometry);

        for (IndexBuilderCollatedDataPtr const& collatedData : collatedIndexData) {

            if (collatedData->traceDataManager == nullptr) {
                continue;
            }

            result.updateCalculatedRangeValues(collatedData);

            auto& traceDataManager = collatedData->traceDataManager;
            if (!traceDataManager->IsReaderReady()) {
                continue;
            }

            while (!traceDataManager->AtReaderEnd()) {
                IndexBuilderData traceData = traceDataManager->ReadNext();
                result.updateCalculationData(traceData, onFirstTrace, lastInline, lastCrossline, collatedData->InlineIsPrimary);

                lastInline = traceData.InlineNo;
                lastCrossline = traceData.CrosslineNo;
                onFirstTrace = false;
            }
            traceDataManager->CloseReader();
        }

        result.finalizeCalculationData();
    }

    SEGYLOG_DEBUG("Collate Geometry data: Elapsed time: " + std::to_string(elapsedSecondsCollateGeometry) + " seconds");

    return result;
}

void
SegyIndexGenerator::setOffsetToFirstSEGYTrace(size_t offset) const
{
    indexData_->offsetFirtstSEGYTrace_ = offset;
}

void
SegyIndexGenerator::setSampleSize(size_t size) const
{
    indexData_->sampleSize_ = size;
}

void
SegyIndexGenerator::setTraceLength(size_t size) const
{
    indexData_->traceLength_ = size;
}

void
SegyIndexGenerator::evaluatePrimaryKey(Range<int> inlineRange, Range<int> crosslineRange, int inlineNo, int crosslineNo, bool& inlineIsPrimary, bool& crosslineIsPrimary)
{
    // primaryKey will be monotonously increasing or decreasing, so should be either range Min or Max
    // If one value is at range limit and the other is in between min and max, then the first key is primary

    if (inlineNo == inlineRange.Max || inlineNo == inlineRange.Min) {
        if (crosslineNo > crosslineRange.Min && crosslineNo < crosslineRange.Max) {
            inlineIsPrimary = true;
        }
    }
    else {
        if ((crosslineNo == crosslineRange.Min || crosslineNo == crosslineRange.Max) && (inlineNo > inlineRange.Min && inlineNo < inlineRange.Max)) {
            crosslineIsPrimary = true;
        }
    }
}

uint16_t
SegyIndexGenerator::findMultiFileEntry(uint16_t fileNo) const
{
    // TODO use STL find_if
    for (std::size_t ix = 0; ix < indexData_->segyFilesIndexed_.size(); ix++) {
        if (indexData_->segyFilesIndexed_[ix].FileNo == fileNo) {
            return static_cast<uint16_t>(ix);  // Casting back to uint16_t when returning
        }
    }
    return std::numeric_limits<uint16_t>::max();
}

} // namespace index_generator
} // namespace segysdk::segyindex
