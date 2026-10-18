/*
Copyright 2021-2024 Schlumberger

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

#include "../FileIO/FileReference.h"
#include "../SegyIO/BinaryHeader.h"
#include "../SegyIO/DerivedHeader.h"
#include "../SegyIO/FileHeaderDescription.h"
#include "../SegyIO/SegyTrace.h"
#include "../SegyIO/TraceHeaderReader.h"
#include "ConversionDetails.h"
#include "SegyDefinitionProvider.h"

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @file SegyFile.h
 *
 * @details SegyFile is the class holding the internal definitions required to read and parse the SEG-Y file.
 *
 */

namespace segysdk::segyio
{

struct SEGYSDK_DLL_PUBLIC StorageAccessParams : public request::Params
{
    StorageAccessParams& withSDCredentials(const SDCredentials& value);
    StorageAccessParams& withCacheSizeOption(const CacheSizeOption& value);
    SDCredentials sdCredentials_{SDCredentials::none()};
    CacheSizeOption cacheSizeOption_{CacheSizeOption::none()};
};

/**
 * @class SegyFile
 *
 * @brief Class to hold information required to read and parse the SEG-Y file.
 *
 */
class SEGYSDK_DLL_PUBLIC SegyFile : public SegyDefinitionProvider
{
private:
    typedef std::unordered_map<std::string, std::shared_ptr<HeaderDefinition>, std::hash<std::string>, std::equal_to<std::string>> HeaderCache;

private:
    static const size_t defaultBufferSize_ = 64UL * 1024UL;

private:
    std::shared_ptr<FileHeaderDescription> fileHeaderDescription_;
    std::shared_ptr<BinaryHeader> binaryHeader_;
    std::shared_ptr<DerivedHeader> derivedHeader_;
    const fileio::FileReference fileReference_;
    std::string xCoordinateHeaderId_;
    std::string yCoordinateHeaderId_;
    std::string inlineHeaderId_;
    std::string crosslineHeaderId_;
    using FileReaderPtr = std::shared_ptr<fileio::FileReader>;
    FileReaderPtr file_;
    std::unique_ptr<TraceHeaderReader> traceHeaderReader_;
    std::unique_ptr<TraceHeaderReader> rawTraceHeaderReader_;
    size_t bufferSize_{};
    HeaderCache traceHeaderCache_;
    HeaderCache binaryHeaderCache_;
    bool useTraceWeights_{false};
    int traceLengthIndex_{-99};

private:
    TraceHeaderReader& getTraceHeaderReader();
    TraceHeaderReader& getRawTraceHeaderReader();

    void initializeHeaderOverrideCache();
    static bool isOpen(const FileReaderPtr& file);
    void open(const request::Params& params, FileReaderPtr& file, const bool raw) const;

public:
    SegyFile(std::shared_ptr<FileHeaderDescription> fileHeaderDescription, fileio::FileReference fileReference);

    /**
     *
     * @brief Standard destructor for SegyFile class.
     *
     */
    ~SegyFile() override;

    /**
     *
     * @brief Gets file name mapped by the current SegyFile instance.
     *
     * @returns string with fully qualified file name of the mapped SEG-Y file.
     *
     */
    std::string getFileName() const { return fileReference_.fileName(); }

    /**
     *
     * @brief Gets FileReference used by the current SegyFile instance.
     *
     * @returns reference of FileReference of the mapped SEG-Y file.
     *
     */
    const fileio::FileReference& getFileReference() const { return fileReference_; }

    /**
     *
     * @brief Gets the trace headerid string string of the header definition holding the X coordinate the byte location definition of to be used for coordinate extractions.
     *
     * @returns string with the headerid specifying what X coordinate value to be used.
     *
     */
    std::string getXCoordinateHeaderId() const { return xCoordinateHeaderId_; }

    /**
     *
     * @brief Gets the trace headerid string string of the header definition holding the Y coordinate the byte location definition of to be used for coordinate extractions.
     *
     * @returns string with the headerid specifying what Y coordinate value to be used.
     *
     */
    std::string getYCoordinateHeaderId() const { return yCoordinateHeaderId_; }

    /**
     *
     * @brief Gets the trace headerid string string of the header definition holding the inline value to be used.
     *
     * @returns string with the headerid specifying what inline value to be used.
     *
     */
    std::string getInlineHeaderId() const { return inlineHeaderId_; }

    /**
     *
     * @brief Gets the trace headerid string string of the header definition holding the crossline value to be used.
     *
     * @returns string with the headerid specifying what crossline value to be used.
     *
     */
    std::string getCrosslineHeaderId() const { return crosslineHeaderId_; }

    /**
     *
     * @brief Gets the file reader instance to use for reading data in the current SegyFile instance.
     *
     * @returns std::shared_ptr<fileio::FileReader> with the file reader instance.
     *
     */
    std::shared_ptr<fileio::FileReader> getFile();

    /**
     *
     * @brief Gets the new/clone file reader instance to use for concurrent data reading in the current SegyFile instance.
     *
     * @param raw use Raw Reader.
     *
     * @returns std::shared_ptr<fileio::FileReader> with the file reader instance.
     *
     */
    std::shared_ptr<fileio::FileReader> getCloneFile(const bool raw = false) const;
    std::shared_ptr<fileio::FileReader> getCloneFile(const request::Params& params, const bool raw = false) const;

    /**
     *
     * @brief Gets the file size of the current SEG-Y file.
     *
     * @returns size_t  with the file size.
     *
     */
    uint64_t getFileSize();

    /**
     *
     * @brief Gets the current file header description used to map the current SEG-Y file.
     *
     * @returns std::shared_ptr<FileHeaderDescription>  pointer to the FileHeaderDescription instance used for the current SEG-Y file.
     *
     */
    std::shared_ptr<FileHeaderDescription> getFileHeaderDescription() const;

    /**
     *
     * @brief Sets the file header description used to map the current SEG-Y file.
     *
     */
    void setFileHeaderDescription(std::shared_ptr<FileHeaderDescription> fileHeaderDescription);

    /**
     *
     * @brief Gets the current binary header definition used to map the current SEG-Y file.
     *
     * @returns std::shared_ptr<BinaryHeader>  vector holding complete binary header definition.
     *
     */
    std::shared_ptr<BinaryHeader> getBinaryHeader();
    std::shared_ptr<DerivedHeader> getDerivedHeader();

    /**
     *
     * @brief Gets the offset in the current SEG-Y file used by various SEG-Y header. This will always be 3600 bytes for SEG-Y rev 0, but might be much larger for SEG-Y rev 1 or later as this might include space used to hold extended headers.
     *
     * @returns uint64_t  Unsigned 64-bit integer holding number of bytes offset before data for the first trace can be read.
     *
     */
    uint64_t getFirstTraceOffset();

    /**
     *
     * @brief Gets the number of bytes required for the trace header.
     *
     * @returns uint32_t  Unsigned 32-bit integer holding number of bytes required to hold the trace header. For SEG-Y rev 0 and rev 1 this is 240.
     *
     */
    static uint32_t getTraceHeaderByteSize();
    ConversionDetails getConversionDetails();

    /**
     *
     * @brief Gets numeric format for trace samples.
     *
     * @returns SampleFormat  enumerable specifying numeric format for trace samples.
     *
     */
    SampleFormat getSampleFormat();

    /**
     *
     * @brief Gets bytes required to hold a trace sample.
     *
     * @returns size_t value For SEG-Y rev 0 and rev 1 this is either 1, 2 or 4 bytes.
     *
     */
    static size_t getBytesPerSample(SampleFormat sampleFormat);

    /**
     *
     * @brief Gets the unit of measurement for length or coordinate values.
     *
     * @returns string value to be used for length or coordinate values. This is normally meter or some version of foot.
     *
     */
    std::string getLengthUnit();

    /**
     *
     * @brief Gets the default trace length.
     *
     * @returns uint64_t specifying default value to be used for number of samples per trace. This value might not be correct for every trace in the current SEG-Y file.
     *
     */
    uint64_t getDefaultTraceLength();

    /**
     *
     * @brief Gets the interval between each sample in the trace.
     *
     * @returns float specifying the interval in time or depth between each trace sample.
     *
     */
    float getSampleInterval();

    /**
     *
     * @brief Gets the size of the current read buffer.
     *
     * @returns size_t specifying the size of the current read buffer.
     *
     */
    size_t getBufferSize() const;

    /**
     *
     * @brief Gets the maximum number of traces in a gather collection. (Pre-stack only)
     *
     * @returns int specifying the maximum number of traces per gather in the current SEG-Y file.
     *
     */
    int getMaxTracesPerGather();

    /**
     *
     * @brief Gets the value of a requested binary header id. (literal)
     *
     * @param headerId string specifying the binary header literal.
     *
     * @returns int  giving the extracted value for the requested headerid.
     *
     */
    int getBinaryHeaderField(const std::string& headerId);

    /**
     *
     * @brief Gets the time or depth value to be used for the first sample in a trace.
     *
     * @returns float  giving the time or depth of the first sample in the first trace in the SEG-Y file.
     *
     */
    float getFirstSampleAt(); // From header of first trace

    /**
     *
     * @brief Gets the number of extended headers in the current SEG-Y file.
     *
     * @returns int giving the number of extended headers used in the current SEG-Y file. This is 0 for SEG-Y rev 0 files, and a value of 0 or larger for SEG-Y rev 1 or later.
     *
     */
    int64_t getExtendedTextualHeaderCount();

    /**
     *
     * @brief Gets the textual header of the SEG-Y file.
     *
     * @returns string holding the 3200 byte textual header of the SEG-Y file. This should normally be split into 40 lines each of 80-bytes when presented to the end user.
     *
     */
    std::string getTextualHeader();

    /**
     *
     * @brief Gets the textual header of the SEG-Y file.
     *
     * @returns a vector of string holding up to 80 characters representing the textual header of the SEG-Y file.
     *
     */
    std::vector<std::string> getTextualHeaderByLine();

    /**
     *
     * @brief Gets the extended textual header of the SEG-Y file if SEG-Y rev 1 or later.
     *
     * @param headerIndex zero offset integer specifying what extended header to extract. a value of 0 extracts the first extended header.
     *
     * @returns string holding the requested 3200 byte extended textual header of the SEG-Y file. An empty string is returned if headerIndex is larger than the available number of extended headers.
     *
     */
    std::string getExtendedTextualHeader(int64_t headerIndex);

    /**
     *
     * @brief Gets the extended textual header of the SEG-Y file if SEG-Y rev 1 or later.
     *
     * @param headerIndex zero offset integer specifying what extended header to extract. a value of 0 extracts the first extended header.
     *
     * @returns a vector of strings holding the extended textual header of the SEG-Y file. An empty vector of strings is returned if headerIndex is larger than the available number of extended headers.
     *
     */
    std::vector<std::string> getExtendedTextualHeaderByLine(int64_t headerIndex);

    std::shared_ptr<HeaderDefinition> getTraceHeaderDefinition(const std::string& headerId) override final;
    std::shared_ptr<HeaderDefinition> getHeaderDefinition(const std::string& headerId);

    /**
     *
     * @brief Gets the byte ordering used in numeric values
     *
     * @returns bool true if byte order is Big-endian, false if byte order is little-endian.
     *
     **/
    bool isByteSwappped();

    /**
     *
     * @brief Does the current SEG-Y file have fixed trace length.
     *
     * @details The method determines if the SEG-Y file is marked as guaranteed to have constant number of samples for every trace.
     * This is true if the binary header SegyRevisionLevel value is coded as SEG-Y rev 1 or higher and the binary header has the Fixed length trace flag set to true, otherwise it returns false even if the file happens to have constant number of samples per trace.
     *
     * @return true if fixed length trace flag set in binary header otherwise false.
     **/
    bool hasFixedTraceLength();

    /**
     *
     * @brief Should trace sample values be scaled.
     *
     * @returns true if trace weighting factor is specified greater than 0 for a trace.
     **/
    bool useTraceWeights() const { return useTraceWeights_; }

    void setFileHeaderDescription(FileHeaderDescription* newDescription);
    void setTraceHeaderStorageUnit(const std::string& headerId, const std::string& unitMeasurement, const std::string& storageUnit, float extraScalar);
    void setHeaderStorageUnit(const std::string& headerId, const std::string& unitMeasurement, const std::string& storageUnit, float extraScalar);

    /**
     *
     * @brief Set headerid to be used for coordinate values.
     *
     * @details Coordinates might be stored in several locations: Source coordinate, receiver coordinates and cdp coordinate for SEG-Y rev 1 files. Additionally coordinates might have been written to random location in the trace header for a SEG-Y rev 0 file.
     *
     * @param xCoordinateHeaderId  Trace header literal to be used to extract X coordinate value.
     * @param yCoordinateHeaderId  Trace header literal to be used to extract Y coordinate value.
     *
     **/
    void setCoordinateHeaders(const std::string& xCoordinateHeaderId, const std::string& yCoordinateHeaderId);

    /**
     *
     * @brief Set headerid to be used for inline and crossline values.
     *
     * @details Inline and crossline values might be stored in several potential trace header locations:
     *   - In the inline and crossline defined locations for SEG-Y rev 1 files.
     *   - In n old SEG-Y revision 0 file it might be re-using locations really defined for 2D use in the trace header  e.g. 21 for crosslines, and 9 or 17 for inline values.
     *   - Using locations in the unallocated part of the rev. 0 trace header definition. Locations such as  221 and 225 is fairly common.
     *
     * @param inlineHeaderId  Trace header literal to be used to extract inline value.
     * @param crosslineHeaderId  Trace header literal to be used to extract crosslilne value.
     *
     **/
    void setLineHeaders(const std::string& inlineHeaderId, const std::string& crosslineHeaderId);

    /**
     *
     * @brief Set whether or not to perform trace scaling - even if trace weighting factors have been specified.
     *
     * @param value  bool value true means trace scaling should be performed, false means no scaling to be performed.
     **/
    void setUseTraceWeights(bool value);

    /**
     *
     * @brief Open the SEG-Y file for read.
     *
     **/
    void open();

    /**
     *
     * @brief Has the current SEG-Y file already been opened for read?
     *
     * @returns true if file is open otherwise false.
     *
     **/
    bool isOpen() const;

    /**
     *
     * @brief Close the SEG-Y file for reading.
     *
     **/
    void close();

    /**
     *
     * @brief Set the size of the input read buffer to use.
     *
     * @param newBufferSize size_t value specifying number of bytes to use for the current read buffer.
     **/
    void setBufferSize(size_t newBufferSize);

    /**
     *
     * @brief Prime bulk access before starting reading bulk data. (Trace samples)
     **/
    void primeBulkAccess();

    float getScaledHeaderFloat(const HeaderDefinition& header);

    uint64_t getActualTraceLength(SegyTrace& trace);

    /**
     *
     * @brief Gets scaled or overridden value of a trace header as a double.
     *
     * @param traceOffset uint64_t value representing number of bytes offset for the requested trace can be read.
     * @param headerId    string   headerid to be extracted.
     *
     * @returns requested trace header value with scaling applied if required or the override value if one is specified.
     *
     **/
    double getScaledTraceHeaderAsDouble(uint64_t traceOffset, const std::string& headerId);

    /**
     *
     * @brief Gets raw value of a trace header as a double.
     *
     * @param traceOffset uint64_t value representing number of bytes offset for the requested trace can be read.
     * @param headerId    string   headerid to be extracted.
     *
     * @returns requested trace header value as it is in the trace header, with no scaling applied.
     *
     **/
    double getRawTraceHeaderAsDouble(uint64_t traceOffset, const std::string& headerId);

    double getScaledTraceHeaderAsDouble(const HeaderDefinition& header, const SegyTrace& trace);
    double getRawTraceHeaderAsDouble(const HeaderDefinition& header, const SegyTrace& trace);

    void readSamples(const SegyTrace& trace, size_t offsetInTrace, float* samples, size_t sampleCount, size_t samplesOffset);
};

} // namespace segysdk::segyio
