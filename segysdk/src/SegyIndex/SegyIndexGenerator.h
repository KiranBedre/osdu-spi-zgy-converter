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

#pragma once

#include "SegyIndexConfig.h"

#include "../SegyIO/HeaderDefinition.h"
#include "../SegyIO/HeaderOverride.h"
#include "../SegyIO/SegyFile.h"
#include "../SegyIO/SegyTraceScanner.h"
#include "../SegyIndex/CollationOffset.h"
#include "../SegyIndex/FileLayoutSpecification.h"
#include "../SegyIndex/Geometry3D.h"
#include "../SegyIndex/GeometryCalculationData.h"
#include "../SegyIndex/IndexBuilderCollatedData.h"
#include "../SegyIndex/SegyIndexData.h"
#include "../SegyIO/VerticalDomain.h"
#include "../SegyIO/VerticalUOM.h"

#include <cstddef>
#include <memory>
#include <vector>

namespace segysdk::segyindex
{
namespace index_generator
{
using SegyFilePtr = std::shared_ptr<segyio::SegyFile>;
using SegyFileVector = std::vector<SegyFilePtr>;
using IndexBuilderCollatedDataPtr = std::shared_ptr<IndexBuilderCollatedData>;
using IndexBuilderCollatedDataVector = std::vector<IndexBuilderCollatedDataPtr>;
using HeaderDefinitionPtr = std::shared_ptr<segyio::HeaderDefinition>;
using HeaderDefinitionVector = std::vector<HeaderDefinitionPtr>;
using SegyIndexDataPtr = index_data::SegyIndexDataPtr;

/**
 * @brief SegyIndexGenerator struct scans SEGY file(s) to generate the trace information
 **/
struct SegyIndexGenerator final
{
    /**
     * @brief Constructs SegyIndexGenerator struct instance
     *
     * @param indexData gives the shared pointer to the index base data instance the trace information is stored to
     *
     **/
    SegyIndexGenerator(SegyIndexDataPtr& indexData);

    /**
     * @brief Scans the SEGY file(s) and generates the trace information
     *
     * @param forceGenerateIndex specifies whether to force the trace information generation for the binary index file
     *
     * @return true if the trace information generation for the binary index is successful and false otherwise
     *
     **/
    bool GenerateIndex(bool forceGenerateIndex);

    /**
     * @brief Initializes data to generate index from the scratch
     *
     **/
    void ResetIndexData() const;

    /**
     * @brief Processes the collated data to generate binary index data
     *
     * @param indexCollatedDataVector contains the collated data itself
     * @param domain specifies the domain of a SEG-Y volume
     * @param zUoM specifies the Unit of measurement for the sample interval in a SEG-Y volume
     * @param sampleSize specifies the trace sample size in bytes
     * @param forceGenerateIndex if true then the index generation will be forced
     *
     * @return true on success that means the following saving will produce the correct binary index contents
     **/
    bool ProcessIndexBuildingData(IndexBuilderCollatedDataVector const& indexCollatedDataVector,
                                  segyio::VerticalDomain domain,
                                  segyio::VerticalUOM zUoM,
                                  size_t sampleSize,
                                  bool forceGenerateIndex = false);

    //
    // functions below are shared with SegyIndexStorage class
    //

    /**
      * @brief Estimates the read buffer size for trace scanner
      *
      * @return the read buffer size for trace scanner in bytes
      **/
    static size_t getIndexReadBufferSize();

    /**
     * @brief Estimates the offsets to retrieve the collated data from traces
     *
     * @param indexHeaders vector that contains IDs for trace header values
     *
     * @return CollationOffset that contains offsets
     **/
    static CollationOffset prepareCollatedOffsets(HeaderDefinitionVector const& indexHeaders);

    /**
     * @brief Extracts values of the trace header
     *
     * @param collatedOffsets contains offsets for trace header values
     * @param values points to the header values obtained by the trace scanner
     * @param fileNo index of the input SEGY file being processed
     * @param offset the trace offset in the input SEGY file being processed
     *
     * @return IndexBuilderData that contains the trace header information
     **/
    static IndexBuilderData extractIndexBuilderDataFromTraceHeaders(const CollationOffset& collatedOffsets,
                                                                    const double* values, size_t fileNo, size_t offset);

    /**
     * @brief Accumulates the collated data by the trace information
     *
     * @param collated contains offsets for trace header values
     * @param ibd contains the trace information
     * @param collatedOffsets contains offsets for trace header values
     * @param values points to the header values obtained by the trace scanner
     **/
    static void updateCollatedDataRanges(IndexBuilderCollatedDataPtr const& collated, const IndexBuilderData& ibd,
                                         const CollationOffset& collatedOffsets,
                                         const double* values);

private:
    
    Range<float> getTraceAmplitudeRange(const float* samples, size_t sampleCount) const;

    IndexBuilderCollatedDataPtr generateIndexData(size_t fileNo,
                                                  SegyFilePtr const& segyFilePtr) const;

    static segyio::SampleFormat getSampleFormatFromCode(int sampleFormat);
    void evaluateStatusFlags(IndexBuilderCollatedDataVector const& indexCollatedDataPatches) const;
    void populateMultiFileInformation(IndexBuilderCollatedDataVector const& indexCollatedDataVector) const;
    void setInitialDefaults() const;
    void setEmptyGeometry3D() const;
    void setEmptyFileLayout() const;
    static HeaderDefinitionVector
    getHeadersToExtractForIndex(HeaderDefinitionVector const& traceHeaders);
    static bool advanceScanner(segyio::SegyTraceScanner& scanner, double& executionSeconds);
    static void evaluatePrimaryAndSecondaryKeyDirection(IndexBuilderCollatedDataPtr const& collated, const IndexBuilderData& ibd,
                                                        bool& inlineIsPrimaryKey, bool& crosslineIsPrimaryKey, bool& isFirstTrace);
    void updateMultiFileRecord(uint16_t fileNo, size_t fileOffset, Range<int> primaryRange) const;
    static Geometry3D populateGeometry3D(IndexBuilderCollatedDataVector const& collatedIndexData,
                                         GeometryCalculationData const& calcData,
                                         segyio::VerticalDomain domain,
                                         segyio::VerticalUOM zUoM);
    static FileLayoutSpecification populateFileLayout(GeometryCalculationData const& calcData,
                                                      segyio::VerticalDomain domain,
                                                      segyio::VerticalUOM zUoM);
    static GeometryCalculationData populateGeometryCalculationData(IndexBuilderCollatedDataVector const& collatedIndexData);
    void setOffsetToFirstSEGYTrace(size_t offset) const;
    void setSampleSize(size_t size) const;
    void setTraceLength(size_t size) const;
    static void evaluatePrimaryKey(Range<int> inlineRange, Range<int> crosslineRange, int inlineNo, int crosslineNo,
                                   bool& inlineIsPrimary, bool& crosslineIsPrimary);
    uint16_t findMultiFileEntry(uint16_t fileNo) const;
    void ProcessIndexBuildingData3D(IndexBuilderCollatedDataVector const& indexCollatedDataVector,
                                    segyio::VerticalDomain domain,
                                    segyio::VerticalUOM zUoM,
                                    bool forceGenerateIndex = false);
    void ProcessIndexBuildingData2D(IndexBuilderCollatedDataVector const& indexCollatedDataVector,
                                    segyio::VerticalDomain domain,
                                    segyio::VerticalUOM zUoM,
                                    size_t sampleSize);

    // Helper structure to determine whether SEGY is 2D or 3D
    struct DimensionTraceHeader
    {
        int inlineNo;
        int crosslineNo;
        int CMP;
    };
    using DimensionTraceHeaders = std::deque<DimensionTraceHeader>;

    void checkSegyDimension(DimensionTraceHeaders& traceDimensionInfo, const std::string& segyFileName) const;

private:
    SegyIndexGenerator(SegyIndexGenerator const&) = delete;
    SegyIndexGenerator& operator=(SegyIndexGenerator const&) = delete;
    SegyIndexGenerator(SegyIndexGenerator&&) = delete;
    SegyIndexGenerator& operator=(SegyIndexGenerator&&) = delete;

private:
    static constexpr double const tofsMax_{10000000.0};
    static constexpr size_t const generateIndexPrintPointLocal_{5000000U};
    static constexpr size_t const generateIndexPrintPointCloud_{50000U};  // Will require 300+ mb of data for a standard SEG-Y file.

private:
    SegyIndexDataPtr indexData_{nullptr};
};

} // namespace index_generator
} // namespace segysdk::segyindex
