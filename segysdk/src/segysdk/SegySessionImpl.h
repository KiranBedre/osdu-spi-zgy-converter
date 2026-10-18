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

#include "../segysdk_details/DLL_EXPORT.h"

// TODO literal names are mentioned several times in the documentation comments, but how does the user know what are the exact string values?

#include "../FileIO/FileManager.h"
#include "../SegyFormat/External/SegyContext.h"
#include "../SegyIO/FileHeaderDescription.h"
#include "../SegyIO/SegyFile.h"
#include "../SegyIndex/SegyIndex.h"
#include "../segysdk/HeaderDataStruct.h"
#include "../segysdk/SegySession.h"

#include <json/json.h>

#include <memory>
#include <string>

namespace segysdk::api
{

using SegyContext = std::shared_ptr<segyformat::external::SegyContext>;
using HeaderDefinitions = std::vector<std::shared_ptr<segyio::HeaderDefinition>>;
using HeaderOverrides = std::vector<std::shared_ptr<segyio::HeaderOverride>>;

struct SEGYSDK_DLL_PUBLIC SessionImplParams : public segyio::StorageAccessParams
{
    SessionImplParams& withSegyContext(const SegyContext& value);
    SessionImplParams& withBinaryHeaders(const HeaderDefinitions& value);
    SessionImplParams& withTraceHeaders(const HeaderDefinitions& value);
    SessionImplParams& withBinaryHeaderOverrides(const HeaderOverrides& value);
    SessionImplParams& withTraceHeaderOverrides(const HeaderOverrides& value);
    SegyContext segyContext_{};
    HeaderDefinitions binaryHeaders_{};
    HeaderDefinitions traceHeaders_{};
    HeaderOverrides binaryHeaderOverrides_{};
    HeaderOverrides traceHeaderOverrides_{};
};

class SEGYSDK_DLL_PUBLIC SegySessionImpl : public SegySession
{

private:
    std::shared_ptr<segyformat::external::SegyContext> context_;
    std::shared_ptr<segyio::FileHeaderDescription> fileHeaderDescr_;
    std::vector<std::shared_ptr<segyio::SegyFile>> segyFilesVector_;
    size_t selectedSegyFile_ = 0;
    std::shared_ptr<segyindex::SegyIndex> segyIndex_;
    std::vector<std::shared_ptr<segyio::HeaderDefinition>> binaryHeaders_;
    std::vector<std::shared_ptr<segyio::HeaderDefinition>> traceHeaders_;
    std::vector<std::shared_ptr<segyio::HeaderOverride>> bhOverrides_;
    std::vector<std::shared_ptr<segyio::HeaderOverride>> thOverrides_;
    SDCredentials sdCredentials_;
    CacheSizeOption cacheSize_;

    bool isByteSwapped_;

    int segyRevision_;

    std::shared_ptr<std::vector<std::shared_ptr<segyio::HeaderDefinition>>> getTraceHeaderDefinitions() const;
    std::shared_ptr<std::vector<std::shared_ptr<segyio::HeaderDefinition>>> getBinaryHeaderDefinitions() const;

    std::vector<std::shared_ptr<segyio::HeaderDefinition>> getHeadersToExtractForScaledTraceHeaderDump(bool is3D = true) const;

    std::vector<std::string> getHeaderIdsFromTraceHeaders() const;
    static std::string getScalarUsageStrategyStringFromEnum(segyio::ScalarUsageStrategy scalarUsageStrategy);
    static std::string getDataTypeStringFromEnum(segyio::HeaderDataType dataType);

    Json::Value getJsonRoot() const;
    Json::Value getSegyMetaData() const;
    bool hasSegy3DFieldsDefined() const;

    static Json::Value getTraceHeadersLiteralsAsJson(const std::vector<std::shared_ptr<segyio::HeaderDefinition>>& hdrLiterals);
    static Json::Value getFileNamesAsJson(const std::vector<std::string>& segyFilesVector);
    static Json::Value getHeaderDefsAsJson(const std::vector<std::shared_ptr<segyio::HeaderDefinition>>& headers);
    static Json::Value getHeaderOverrideDefsAsJson(const std::vector<std::shared_ptr<segyio::HeaderOverride>>& overrides);

    bool isSegyRev1() const { return segyRevision_ > 0; }


public:
    explicit SegySessionImpl(const SessionImplParams& params);

    SegySessionImpl(const SegySessionImpl& other) = default;
    SegySessionImpl(SegySessionImpl&& other) noexcept;
    SegySessionImpl& operator=(const SegySessionImpl& rhs);
    SegySessionImpl& operator=(SegySessionImpl&& rhs) noexcept;

    ~SegySessionImpl() override;

    /**
     * @brief Gets SEG-Y version of current SEG-Y file.
     *
     * @details  Supported return values are:
     * - 0 : SEG-Y file mostly corresponding to the SEG-Y revision standard 0 of 1975
     * - 1 : SEG-Y file corresponding to the SEG-Y revision standard 1.0 of 2002
     * - 2 : SEG-Y file corresponding to the SEG-Y revision standard 2.0 of 2017 Although this is a valid SEG-Y revision level this version of SEG-Y is not yet properly supported by the SEG-Y SDK.
     *
     *  Implications of setting SEG-Y revision level:
     *  - 0 : For this revision level:
     *      No explicit version indication provided by this release, but the two-byte binary header location (3501-3502) used in SEG-Y rev 1 for the version number is almost always unused and will by default be set to 00 00.
     *
     *      Key features:
     *      File consists of a mandatory 3200 byte textual header, followed by a 400 byte binary header and then n trace records.
     *      The trace records are comprised of a 240 byte trace header and a trace data part which is simply the numerical value of each sample in the trace.
     *      This original implementation was mainly designed to accommodate 2D SEG-Y data on digital tape.
     *      In the standard, only the first 180 samples of the trace header had been properly defined, leaving a block of undefined data space between byte location 181 and 240 in the trace header.
     *      SEG-Y producers were able to extend the functionality to offer some support for SEG-Y 3D data by using the undefined area of the trace headers, typically for required 3D specific fields
     *      such as Inline and Crossline values and ensemble coordinate values.
     *      A number of strategies for achieving this were implemented.
     *      Most commonly the inline and crossline values would be written to locations 221 and 225 respectively, but other variants would use the slot for the ensemble number (21) for the crossline value and location 221 for the inline number.
     *      Other strategies tried using the Line number field in the binary header for the inline number and the enseble number (21) for the crossline value. Consequently such files will have full textual and binary headers between each inline.
     *      Yet another strategy involved using a single composite 32-bit field to hold both the inline and crossline values.
     *      Many of these attempts caused the structure of the SEG-Y file to be modified so it no longer followed the SEG-Y standard. The result is a 'standard' that will have some implementations that might not be handled correctly by all SEG-Y readers.
     *      Settings frequently modified in SEG-Y revision level 0 include :
     *      - Field definitions in the trace header might not follow the standard. Inline and crossline values, time of first sample and location of coordinate fields might not follow standard. Several of the 'defined'
     *        binary- and trace header locations might not be populated if these values were not thought to be required.
     *      - As more modern computer hardware became available IEEE floating point values were being used even though this was actually only part of the SEG-Y revision 1 standard.
     *        Similarly there are SEG-Y files where numerical fields have Little Endian representation even though the standard stipulates that numerical fields should be stored using Big Endian representation.
     *      This standard offers only support for numerical field representations in use at the time the standard was defined:
     *      - 32-bit IBM floating point
     *      - 16-bit two's complement integer
     *      - Fixed point with gain code  (Not currently supported)
     *
     *  - 1 : For this revision level:
     *      Binary header locations 3501 and 3502 should hold the 16bit (hexadecimal) value 0x0100. It should also have a line in the textual header indication revision level ("C39  SEG Y REV1")
     *
     *      Key features:
     *      File consists of a mandatory 3200 byte textual header, followed by a 400 byte binary header. After the binary header there might be a number of optional 3200 byte extended  textual headers and then n trace records.
     *      The trace records are comprised of a 240 byte trace header and a trace data part which is simply the numerical value of each sample in the trace.
     *      In this revision, definition of 3D data had been added to the standard trace header definitions, leaving only a small block between trace header byte 233 and 240 undefined.
     *      The extended textual headers can hold a variety of useful metadata, including cartographic reference system data and information of measurement system used for XY and Z based values.
     *      The standard still had some weak areas, mainly in leaving the specification of sample interval undefined for depth indexed seismic.
     *      As this is only stored in a 16-bit value it means that with default scaling providing 3 decimal point precision the field can only hold values up to 65.5 meter or 65.5 ft sample rate
     *      unless the producer has decided to use less than 3 decimal resolution used for time indexed seismic which leaves it up to other SEG-Y readers to interpret the value correctly.
     *      This standard also offers support for additional numerical field representations in the trace header, i.e.
     *      - 32-bit IBM floating point
     *      - 32-bit IEEE floating point
     *      - 32-bit two's complement Integer
     *      - 16-bit two's complement integer
     *      - Fixed point with gain code (Not supported as it has been declared obsolete by the standard)
     *      -  8-bit two's complement integer
     *
     *  - 2 : For this revision level:  (Not currently supported)
     *      Binary header locations 3501 and 3502 should hold the 16bit (hexadecimal) value 0x0200. It should also have a line in the textual header indication revision level ("C39  SEG Y REV2")
     *
     *      Key features:
     *      File consists of a mandatory 3200 byte textual header, followed by a 400 byte binary header. After the binary header there might be a number of optional 3200 byte extended  textual headers and then n trace records. Each trace might have one or more 240 byte trace headers.
     *      New numeric formats have been introduced, including allowing use of little-endian number formats and also pairwise byte swapped byte ordering.
     *      Trace samples can now also be either 24-bit integer or 64-bit floating points in addition to the numeric formats specified in rev 1.
     *      Variable number of samples per trace is explicitly allowed
     *      NOTE: Attempting to use a SEG-Y file of revision 2 as input will fail or possibly produce incorrect results if any of the new features introduced in SEG-Y rev 2 have been used. The only exception to this is using little-ending numeric format which is already supported by the SEG-Y SDK.
     *
     * @returns integer value representing the current SEG-Y file revision. 0, 1  or 2 (for SEG-Y rev 0, rev 1 or rev 2 respectively). A value of 0 will also be returned for files where the extracted value is not one of 0, 1 or 2.
     **/
    int getSegyVersion() const override { return segyRevision_; }

    segyio::SampleFormat getSampleFormat() const override;

    /**
     * @brief Does the SEG-Y dataset contain 3D data?
     *
     * This returns true if SEGY has inline/crossline progression. Works properly only after SEGY Index is loaded.
     *
     *  @returns true or false. A value of true indicates the file is expected to contain SEG-Y 3D data.
     **/
    bool isSegy3D() const override { return segyIndex_->isSegy3D(); }

    /**
     * @brief Gets the size of the currently selected SEG-Y file
     *
     *  @returns the disk file size of the currently selected SEG-Y file.
     **/
    size_t getSegyFileSize() const override;

    /**
     * @brief Check if data file is Big or Little Endian
     *
     * @returns true if Big-endian, false otherwise (Little-Endian)
     **/
    bool isBigEndianSegy() const override;

    void setSegy2DFlag(bool isSegy2D) override;

    /**
     * @brief Gets trace header fields count.
     *
     * @returns a size_t value representing the number of literals (fields) defined for the SEG-Y trace header.
     *
     **/
    size_t getTraceHeaderFieldCount() const override { return fileHeaderDescr_->getTraceHeaders()->size(); }

    /**
     * @brief Gets textual headers in ASCII encoding.
     *
     * @returns the contents of the SEG-Y textual header as a single 3200 byte string.
     * It is the responsibility of the requester to split the string into standard 80 character lines to improve readability for humans if required.
     *
     **/
    std::string getAsciiHeaders() const override;

    /**
     * @brief Gets textual headers in ASCII encoding.
     *
     * @returns the contents of the SEG-Y textual header split into multiple, up to 80 bytes long strings.
     * The lines will be split either by textual header flags, or split into 80 byte sections if no header flags exists. (C01 -- C40)
     *
     **/
    std::vector<std::string> getAsciiHeadersByLine() const override;

    /**
     * @brief Gets extended textual headers in ASCII encoding.
     *
     * @returns the contents of all the extended SEG-Y textual header(s) as a vector of 3200 byte strings.
     * It is the responsibility of the requester to split the result into suitable blocks for human readability.
     *
     **/
    std::vector<std::string> getExtendedAsciiHeaders() const override;

    /**
     * @brief Gets extended textual headers in ASCII encoding.
     *
     * @returns the contents of all the extended SEG-Y textual header(s) as a vectors of 80 byte strings.
     * It is the responsibility of the requester to split the result into suitable blocks for human readability.
     *
     **/
    std::vector<std::vector<std::string>> getExtendedAsciiHeadersByLine() const override;

    /**
     * @brief Gets binary headers.
     *
     * @returns the contents of the SEG-Y binary header  as a vector of header data, i.e. a struct comprising a string with the literal (field) name and its corresponding binary header value.
     * Fields are extracted as per the SEG-Y standard for the current detected SEG-Y revision level.
     * For SEG-Y rev 0 this means field in the byte range 1 - 180
     * For SEG-Y rev 1 this means field in the byte range 1 - 232
     *
     * Example of output:
     * @snippet getBinaryHeaders.txt result
     *
     **/
    std::vector<HeaderDataStruct> getBinaryHeaders() const override;

    /**
     * @brief Gets the SEG-Y dataset index.
     *
     * @returns a pointer to the generated SEG-Y index class.
     * This is a basic class without much useful content if the index has not yet been generated.
     *
     **/
    std::shared_ptr<segyindex::SegyIndex> getIndex() const override;
    std::shared_ptr<segyindex::SegyIndex> getIndex(const request::Params& params) const override;

    /**
     * @brief Gets raw trace headers.
     *
     * @param startTrace    Zero-offset trace number to start extraction of header values.
     * @param tracesToDump  Number of traces to extract values from.
     *
     * @returns vector of HeaderDataStruct holding actual trace header values, irrespective of any overrides set by the user or scale factors defined for the field. All defined trace header literals are extracted.
     *
     * Example of output:
     * @snippet getRawTraceHeaders.txt result
     *
     **/
    std::vector<std::vector<HeaderDataStruct>> getRawTraceHeaders(size_t startTrace, size_t tracesToDump) const override;

    /**
     * @brief Gets trace headers and trace samples.
     *
     * @param startTrace    Zero-offset trace number to start extraction of header values.
     * @param tracesToDump  Number of traces to extract values from.
     *
     * @returns vector of HeaderDataStruct holding actual trace header values and the sample data for each trace irrespective of any overrides set by the user or scale factors defined for the field. All defined trace header literals are extracted.
     *
     **/
    std::vector<Trace_t> getTraces(size_t startTrace, size_t tracesToDump) const override;
    std::vector<Trace_t> getTraces(const request::TraceParams& params) const override;

    /**
     * @brief Gets trace samples.
     *
     * @param result std::vector<std::vector<float>> where the collected output sample values will be stored
     * @param startTrace    Zero-offset trace number to start extraction of header values.
     * @param tracesToDump  Number of traces to extract values from.
     * @param sampleOffset  Zero-offset sample number to start extraction of trace sample values.
     * @param samplesToDump Number of samples to extract, starting from sampleOffset.
     * @returns bool indicating operation success or failure
     *
     **/
    bool getTraceSamples(std::vector<std::vector<float>>& result, size_t startTrace, size_t tracesToDump, size_t sampleOffset, size_t samplesToDump) const override;
    bool getTraceSamples(std::vector<std::vector<float>>& result, const request::TraceParams& params) const override;

    /**
     * @brief Gets trace headers, intended for operational use rather than for header mapping purposes. \nOnly key headers (For 2D: SP, CMP, X and Y or for 3D: Inline, Crossline, X and Y) will be returned
     *
     * @param headerKeys    reference vector of string holding the extracted trace header keys
     * @param headerValues  reference std::vector<std::vector<double>> holding the extracted headers per trace (ordered in same sequence as headerKeys array)
     * @param startTrace    Zero-offset trace number to start extraction of header values.
     * @param tracesToDump  Number of traces to extract values from.
     * @returns bool indicating operation success or failure
     *
     **/
    bool getTraceHeaderValues(std::vector<std::string>& headerKeys, std::vector<std::vector<double>>& headerValues, size_t startTrace, size_t tracesToDump) const override;
    bool getTraceHeaderValues(std::vector<std::string>& headerKeys, std::vector<std::vector<double>>& headerValues, const request::TraceParams& params) const override;

    /**
     * @brief Gets scaled trace headers.
     *
     * Returns a two-dimensional vector of HeaderDataStruct holding trace header values as they are seen by the internal methods. This means scaling factors and/or user defined overrides is applied for each trace.
     * All defined trace header literals are extracted.
     *
     * @param startTrace    Zero-offset trace number to start extraction of header values.
     * @param tracesToDump  Number of traces to extract values from.
     *
     * Example of output:
     * @snippet getRawTraceHeaders.txt result
     *
     **/
    std::vector<std::vector<HeaderDataStruct>> getScaledTraceHeaders(size_t startTrace, size_t tracesToDump) const override;
    std::vector<std::vector<HeaderDataStruct>> getScaledTraceHeaders(const request::TraceParams& params) const override;

    /**
     * @brief A performance-optimized way to get the values of only those trace header fields which are needed for the caller, for several traces in one batch.
     *
     * @returns a vector of trace header values only, as they are seen by the internal methods. This means scaling factors and/or user defined overrides are applied.
     * Only the trace header literals specified in the header_ids vector are extracted.
     *
     * @param startTrace    Zero-offset trace number to start extraction of header values.
     * @param tracesToDump  Number of traces to extract values from.
     * @param header_ids    Vector holding the name of the trace header literals to be extracted.
     *
     **/
    std::vector<std::vector<double>> getSelectedScaledTraceHeaderValues(size_t startTrace, size_t tracesToDump, std::vector<std::string> header_ids) override;

    // JSON output data

    /**
     * @brief Gets textual headers as JSON.
     *
     * @returns the data extracted by the GetAsciiHeaders method encapsulated in a JSON structure.
     *
     * The structure is as follows:
     *
     * @snippet getAsciiHeaders.txt json_structure
     *
     * The Textualheader will typically be a 3200 byte string. It will be the responsibility of the requester to convert this into 80 character lines for display purposes.
     *
     * metadata:
     *  Filenames is an array of strings holding the fully qualified file names of all SEG-Y file(s) included in the current logical volume.
     *  SegyRevision is an integer value representing the current SEG-Y file revision. 0, 1  or 2 (for SEG-Y rev 0, rev 1 or rev 2 respectively). A value of 0 will also be returned for files where the extracted value is not one of 0, 1 or 2.
     *
     * Example of output:
     * @snippet getAsciiHeaders.txt json_result
     **/
    std::string getAsciiHeadersAsJson() const override;

    /**
     * @brief Gets textual headers as JSON.
     *
     * @returns the data extracted by the GetAsciiHeaders method encapsulated in a JSON structure.
     *
     * The Textualheader is a 3200 byte string. This will be converted into strings of up to 80 character lines by this method.
     *
     * metadata:
     *  Filenames is an array of strings holding the fully qualified file names of all SEG-Y file(s) included in the current logical volume.
     *  SegyRevision is an integer value representing the current SEG-Y file revision. 0, 1  or 2 (for SEG-Y rev 0, rev 1 or rev 2 respectively). A value of 0 will also be returned for files where the extracted value is not one of 0, 1 or 2.
     *
     * Example of output:
     * @snippet getAsciiHeaders.txt json_result_split
     **/
    std::string getAsciiHeadersByLineAsJson() const override;

    /**
     * @brief Gets extended textual headers as JSON.
     *
     * @returns the data extracted by the GetExtendedAsciiHeaders method encapsulated in a JSON structure.
     *
     * The structure is as follows:
     * Each of the ExtendedTextualHeaders should normally be 3200 bytes long.
     *
     * @snippet getExtendedAsciiHeaders.txt json_structure
     *
     * The ExtendedTextualheader will typically be a 3200 byte string. It will be the responsibility of the requester to convert this into 80 character lines for display purposes.
     *
     * metadata:
     *  Filenames is an array of strings holding the fully qualified file names of all SEG-Y file(s) included in the current logical volume.
     *  SegyRevision is an integer value representing the current SEG-Y file revision. 0, 1  or 2 (for SEG-Y rev 0, rev 1 or rev 2 respectively). A value of 0 will also be returned for files where the extracted value is not one of 0, 1 or 2.
     *
     * Example of output:
     * @snippet getExtendedAsciiHeaders.txt json_result
     *
     **/
    std::string getExtendedAsciiHeadersAsJson() const override;

    /**
     * @brief Gets extended textual headers as JSON.
     *
     * @returns the data extracted by the GetExtendedAsciiHeaders method encapsulated in a JSON structure.
     *
     * The structure is as follows:
     * Each of the ExtendedTextualHeaders should normally be 3200 bytes long, and these will again be split into up to 80 byte lines.
     *
     * @snippet getExtendedAsciiHeaders.txt json_structure_split
     *
     * The ExtendedTextualheader will be a 3200 byte string. It will be the responsibility of the requester to convert this into 80 character lines for display purposes.
     *
     * metadata:
     *  Filenames is an array of strings holding the fully qualified file names of all SEG-Y file(s) included in the current logical volume.
     *  SegyRevision is an integer value representing the current SEG-Y file revision. 0, 1  or 2 (for SEG-Y rev 0, rev 1 or rev 2 respectively). A value of 0 will also be returned for files where the extracted value is not one of 0, 1 or 2.
     *
     * Example of output:
     * @snippet getExtendedAsciiHeaders.txt json_result
     *
     **/
    std::string getExtendedAsciiHeadersByLineAsJson() const override;

    /**
     * @brief Gets binary headers as JSON.
     *
     * @returns the data extracted by the GetBinaryHeaders method encapsulated in a JSON structure.
     * This is a struct comprising a string with the literal (field) name and its corresponding binary header value.
     *
     * Fields are extracted as per the SEG-Y standard for the current detected SEG-Y revision level.
     * For SEG-Y rev 0 this means field in the byte range 1 - 180
     * For SEG-Y rev 1 this means field in the byte range 1 - 232
     *
     * Example of output:
     *
     * @snippet getBinaryHeaders.txt json_result
     *
     **/
    std::string getBinaryHeadersAsJson() const override;

    /**
     * @brief Set the trace header mapping for the SEGY dataset based on the OSDU Vector Header Mapping
     * .
     *
     * @returns true if it was possible to remap the SegySession and false if something went wrong (i.e.: index already created)
     *
     * @param vectorHeaderMapping    Header mapping based on https://community.opengroup.org/osdu/data/data-definitions/-/blob/master/E-R/abstract/AbstractVectorHeaderMapping.1.0.0.md.
     * @param forceRemapping         For the trace header remapping, which will drop existed index.
     *
     **/
    bool setVectorHeaderMapping(std::string vectorHeaderMapping, bool forceRemapping) override;

    /**
     * @brief Gets raw trace headers as JSON.
     *
     * @returns the data extracted by the GetRawTraceHeaders method encapsulated in a JSON structure.
     *
     * @param startTrace    Zero-offset trace number to start extraction of header values.
     * @param tracesToDump  Number of traces to extract values from.
     *
     * Example of output:
     * @snippet getRawTraceHeaders.txt json_result
     *
     **/
    std::string getRawTraceHeadersAsJson(long startTrace, int tracesToDump) const override;

    /**
     * @brief Gets trace headers and samples as JSON.
     *
     * @returns the data extracted by the GetTraces method encapsulated in a JSON structure.
     *
     * @param startTrace    Zero-offset trace number to start extraction of header values.
     * @param tracesToDump  Number of traces to extract values from.
     *
     **/
    std::string getTracesAsJson(long startTrace, int tracesToDump) const override;

    /**
     * @brief Gets scaled trace headers as JSON.
     *
     * Returns the data extracted by the GetScaledTraceHeaders method encapsulated in a JSON structure.
     *
     * @param startTrace    Zero-offset trace number to start extraction of header values.
     * @param tracesToDump  Number of traces to extract values from.
     * @param is3D          is a flag to force data set to be scanned using 2D or 3D headers, defaults to true
     *
     * Example of output:
     * @snippet getScaledTraceHeaders.txt json_result
     *
     **/
    std::string getScaledTraceHeadersAsJson(long startTrace, int tracesToDump, bool is3D = true) const override;

    /**
     * @brief Accept and register settings.
     *
     * The current operating context is stored and registered for the session. This includes:
     *   - SEG-Y file(s) included in the session.
     *   - Binary header and trace header definitions and any overrides provided by the user.
     *
     **/
    void acceptAndRegisterSettings() const override;

    /**
     * @brief Get the file reference for the parameter file
     *
     * @details Utility method for getting the file reference for the stored parameter file if this has been saved (not relevant for OSDU).
     *
     * @param fileName    string holding the fully qualified file name of the current SEG-Y file.
     * @param options     fileio::FileReaderOptions  with buffering parameter setting (Only used relevant if accessing cloud stored SEG-Y file).
     **/
    static fileio::FileReference getParameterFileReference(const std::string& fileName, const fileio::FileReaderOptions& options);

    /**
     * @brief Load stored settings.
     *
     * If current operating context has been saved from a previous session it is retrieved and the current context is set according to the saved values.
     *
     **/
    void loadStoredSettings() override;

    /**
     * @brief Generate the index file for a properly defined SEG-Y file.
     *
     * @param fileName    string holding the fully qualified file name of the current SEG-Y file.
     * @param options     fileio::FileReaderOptions  with buffering parameter setting (Only used relevant if accessing cloud stored SEG-Y file).
     **/
    void generateIndexFile() const override;

    /**
     * @brief Generate the index file for a properly defined SEG-Y file (this method is intended for testing purposes).
     *
     * @param forceGenerateIndex    bool. If true, forces generation of index values to be generated and stored, even if the file is completely regular
     * @param forceOverwrite        bool. If true, will overwrite an existing index without prompting for confirmation
     **/
    void generateIndexFile(bool forceGenerateIndex, bool forceOverwrite) const override;

    void loadIndexFile() const override;

    /**
     * @brief A performance-optimized way to get Survey Bin Grid definition. Approximation is made if there are missing traces in the survey.
     * In case of different trace size Bin Grid could not be calculated and default structure is returned.
     *
     * @returns a std::string formatted as Json which contains calculated P6 params for the selected SEGY file with Min and Max values for the Inline and Crossline.
     *
     **/
    virtual std::string getSurveyBinGrid() const override;

    /**
     * @brief Select a SEG-Y file inside a multi-file SEG-Y dataset.
     *
     * In a multi-file context this will set the reference to the file to use. This is useful when examining e.g. trace header values in a multi-file context,
     * but will not be required when requesting trace data as this is registered in the SEG-Y index.
     *
     * @param fileNumber    Zero-offset reference to what input file number to use. Index must be a value greater or equal to 0 and less than the actual number of input SEG-Y files specified in the current context.
     **/
    void setSelectedSegyFile(size_t fileNumber);

    /**
     * @brief get the currently selected SEG-Y file inside a (possibly multi-file) SEG-Y data set.
     *
     * @returns uint16_t value holding the file id of the currently selected SEG-Y file in a SEG-Y dataset
     **/
    uint16_t getSelectedSegyFileId() const { return static_cast<uint16_t>(selectedSegyFile_); }

    /**
     * @brief return the fully qualified name of the currently selected SEG-Y file in a SEG-Y dataset.
     *
     * @returns std::string holding the fully qualified name of the currently selected SEG-Y file in a SEG-Y dataset
     **/
    std::string getSegyFileName() const override { return segyFilesVector_[selectedSegyFile_]->getFileName(); }

private:
    std::string TraceHeadersAsJson(long startTrace,
                                   const std::vector<std::vector<HeaderDataStruct>>& result,
                                   const std::vector<std::shared_ptr<segyio::HeaderDefinition>>& hdrLiterals) const;

    std::unique_ptr<bulkaccess::SegyBulkReader> createBulkReader() override;

    std::unique_ptr<bulkaccess::SegyBulkReader> createMultiBulkReader(std::shared_ptr<segyindex::SegyIndex> segyIndex, size_t numberOfTraces, size_t bulksCount) override;

    std::unique_ptr<bulkaccess::SegyBulkReader> createSectionBulkReader(size_t readersCount) override;

    std::unique_ptr<bulkaccess::async::Reader> createAsyncReader(const ReaderCreateParams& params) override;

    std::unique_ptr<bulkaccess::Segy2DReader> create2DReader() override;

    std::string readTextFile(const std::string& textFilePath) const override;

    std::string getOsduMetadata(const std::string& path_or_id) const override;

    bool validateSession() const override;

    FileLayout getFileLayout(const request::Params& params) const override;

    fileio::FileReference getFileReference(const std::string& fileName) const override;
};

} // namespace segysdk::api
