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

/** @file
 * @brief SegySession.h contains the declaration of the SegySession, which is the main entry point to all SEG-Y operations. Use SegySessionFactory to open an existing SEG-Y file[set].
 *
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "HeaderDataStruct.h"

#include "../BulkAccess/AsyncReader.h"
#include "../BulkAccess/Segy2DReader.h"
#include "../BulkAccess/SegyBulkReader.h"
#include "../Request/Params.h"
#include "../SegyIO/SampleFormat.h"
#include "../SegyIndex/SegyIndex.h"

#include <memory>
#include <string>
#include <vector>

namespace segysdk::api
{

using ReaderCreateParams = request::Params;

struct FileLayout
{
    std::size_t sizeI{};
    std::size_t sizeJ{};
    std::size_t sizeK{};
};

/**
 * @brief SegySession is the main entry point to all SEG-Y operations. Use SegySessionFactory to open an existing SEG-Y file[set].
 *
 * A SEG-Y session is used to set up the operating environment for the SEG-Y SDK.
 *  It integrates user supplied trace header definitions, user provided binary- and trace-header override values,
 *  with the name(s) of the input SEG-Y files.
 *  It also allows the user to provide the SDCredentials required to access data in the cloud.
 *
 */
class SEGYSDK_DLL_PUBLIC SegySession
{

public:
    /**
     * @brief Default destructor for the SegySession.
     *
     **/
    virtual ~SegySession();

    /**
     * @brief Accept and register settings.
     *
     * The current operating context is stored and registered for the session. This includes:
     *   - SEG-Y file(s) included in the session.
     *   - Binary header and trace header definitions and any overrides provided by the user.
     *
     **/
    virtual void acceptAndRegisterSettings() const = 0;

    /**
     * @brief Load stored settings.
     *
     * If current operating context has been saved from a previous session it is retrieved and the current context is set according to the saved values.
     *
     **/
    virtual void loadStoredSettings() = 0;

    /**
     * @brief Gets numeric format of the trace data.
     *
     * @returns SampleFormat enumerable describing the numeric format.
     *
     **/
    virtual segyio::SampleFormat getSampleFormat() const = 0;

    /**
     * @brief Gets the SEG-Y dataset index.
     *
     * @returns a pointer to the generated SEG-Y index class.
     * This is a basic class without much useful content if the index has not yet been generated.
     *
     **/
    virtual std::shared_ptr<segyindex::SegyIndex> getIndex() const = 0;
    virtual std::shared_ptr<segyindex::SegyIndex> getIndex(const request::Params& params) const = 0;

    /**
     * @brief Create or recreate an index file for the dataset.
     *
     * For 3-D SEG-Y data an index is required for efficient reading of the seismic trace data. The index enables reading of randomly located traces without having
     * to scan through the entire SEG-Y volume to locate the requested traces. It also generates a bin-grid for the seismic volume holding:
     * - Inline and Crossline extents
     * - Vertical extent
     *
     * It also creates a set of coordinates for the corner points of the data grid calculated from the trace headers.
     *
     * The indexing process will also perform checks to see if certain values are constant throughout the entire volume, this includes:
     * all scaling factors, trace length, time/depth of first sample and trace weighting factor
     *
     **/
    virtual void generateIndexFile() const = 0;

    /**
     * @brief Create or recreate an index file for the dataset.
     *
     * For 3-D SEG-Y data an index is required for efficient reading of the seismic trace data. The index enables reading of randomly located traces without having
     * to scan through the entire SEG-Y volume to locate the requested traces. It also generates a bin-grid for the seismic volume holding:
     * - Inline and Crossline extents
     * - Vertical extent
     *
     * It also creates a set of coordinates for the corner points of the data grid calculated from the trace headers.
     *
     * The indexing process will also perform checks to see if certain values are constant throughout the entire volume, this includes:
     * all scaling factors, trace length, time/depth of first sample and trace weighting factor
     *
     **/
    virtual void generateIndexFile(bool forceGenerateIndex, bool forceOverwrite) const = 0;

    /**
     * @brief Load the previously generated index file for the dataset.
     *
     * If an index file has been generated in a previous session this is retrieved and used in the current session.
     *
     **/
    virtual void loadIndexFile() const = 0;

    /**
     * @brief File name of the currently selected SEG-Y file.
     *
     * @returns a string with the file name of the currently selected SEG-Y file.
     **/
    virtual std::string getSegyFileName() const = 0;

    /**
     * @brief Gets SEG-Y version of current SEG-Y file.
     * @returns integer value representing the current SEG-Y file revision. 0, 1  or 2 (for SEG-Y rev 0, rev 1 or rev 2 respectively). A value of 0 will also be returned for files where the extracted value is not one of 0, 1 or 2.
     **/
    virtual int getSegyVersion() const = 0;

    /**
     * @brief Does the SEG-Y dataset contain 3D data?
     *
     * This returns true if both trace header fields Inline and Crossline have been specified by the user in the input trace header definition.
     *
     *  @returns true or false. A value of true indicates the file is expected to contain SEG-Y 3D data.
     **/
    virtual bool isSegy3D() const = 0;

    /**
     * @brief Gets the size of the currently selected SEG-Y file
     *
     *  @returns the disk file size of the currently selected SEG-Y file.
     **/
    virtual size_t getSegyFileSize() const = 0;

    /**
     * @brief is the SEG-Y file encoded using standard big-Endian format?
     *
     * This returns true if file uses Big-Endian numerical formatting.
     *
     *  @returns true or false.
     **/
    virtual bool isBigEndianSegy() const = 0;

    /**
     * @brief Specify whether SEGY data is 2D or not.
     *
     * Sets the dimensionality flag which tells how SEGY data should be processed.
     **/
    virtual void setSegy2DFlag(bool isSegy2D) = 0;

    /**
     * @brief Get the internal file id of currently selected SEG-Y file.
     *
     * @returns a uint16_t with the internal id of the currently selected SEG-Y file.
     **/
    virtual uint16_t getSelectedSegyFileId() const = 0;

    /**
     * @brief Gets textual headers in ASCII encoding.
     *
     * @returns the contents of the SEG-Y textual header as a single 3200 byte string.
     * It is the responsibility of the requester to split the string into standard 80 character lines to improve readability for humans if required.
     *
     **/
    virtual std::string getAsciiHeaders() const = 0;

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
    virtual std::string getAsciiHeadersAsJson() const = 0;

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
    virtual std::string getAsciiHeadersByLineAsJson() const = 0;

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
    virtual std::string getExtendedAsciiHeadersAsJson() const = 0;

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
    virtual std::string getBinaryHeadersAsJson() const = 0;

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
    virtual bool setVectorHeaderMapping(std::string vectorHeaderMapping, bool forceRemapping) = 0;

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
    virtual std::string getRawTraceHeadersAsJson(long startTrace, int tracesToDump) const = 0;

    /**
     * @brief Gets trace headers and samples as JSON.
     *
     * @returns the data extracted by the GetTraces method encapsulated in a JSON structure.
     *
     * @param startTrace    Zero-offset trace number to start extraction of header values.
     * @param tracesToDump  Number of traces to extract values from.
     *
     **/
    virtual std::string getTracesAsJson(long startTrace, int tracesToDump) const = 0;

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
    virtual std::string getExtendedAsciiHeadersByLineAsJson() const = 0;

    /**
     * @brief Gets scaled trace headers as JSON.
     *
     * Returns the data extracted by the GetScaledTraceHeaders method encapsulated in a JSON structure.
     *
     * @param startTrace    Zero-offset trace number to start extraction of header values.
     * @param tracesToDump  Number of traces to extract values from.
     * @param is3D          flag to force dataset to be scanned using 2D or 3D trace header values
     *
     * Example of output:
     * @snippet getScaledTraceHeaders.txt json_result
     *
     **/
    virtual std::string getScaledTraceHeadersAsJson(long startTrace, int tracesToDump, bool is3D = true) const = 0;

    /**
     * @brief Gets textual headers in ASCII encoding.
     *
     * @returns the contents of the SEG-Y textual header split into multiple, up to 80 bytes long strings.
     * The lines will be split either by textual header flags, or split into 80 byte sections if no header flags exists. (C01 -- C40)
     *
     **/
    virtual std::vector<std::string> getAsciiHeadersByLine() const = 0;

    /**
     * @brief Gets extended textual headers in ASCII encoding.
     *
     * @returns the contents of all the extended SEG-Y textual header(s) as a vector of 3200 byte strings.
     * It is the responsibility of the requester to split the result into suitable blocks for human readability.
     *
     **/
    virtual std::vector<std::string> getExtendedAsciiHeaders() const = 0;

    /**
     * @brief Gets extended textual headers in ASCII encoding.
     *
     * @returns the contents of all the extended SEG-Y textual header(s) as a vectors of 80 byte strings.
     * It is the responsibility of the requester to split the result into suitable blocks for human readability.
     *
     **/
    virtual std::vector<std::vector<std::string>> getExtendedAsciiHeadersByLine() const = 0;

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
    virtual std::vector<HeaderDataStruct> getBinaryHeaders() const = 0;

    /**
     * @brief Gets trace header fields count.
     *
     * @returns a size_t value representing the number of literals (fields) defined for the SEG-Y trace header.
     *
     **/
    virtual size_t getTraceHeaderFieldCount() const = 0;

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
    virtual std::vector<std::vector<HeaderDataStruct>> getRawTraceHeaders(size_t startTrace, size_t tracesToDump) const = 0;

    /**
     * @brief Gets ALL mapped trace headers and every trace sample for the requested trace range.
     *
     * @param startTrace    Zero-offset trace number to start extraction of header values.
     * @param tracesToDump  Number of traces to extract values from.
     *
     * @returns vector of HeaderDataStruct holding actual trace header values and the sample data for each trace irrespective of any overrides set by the user or scale factors defined for the field. All defined trace header literals are extracted.
     *
     **/
    using Trace_t = std::pair<std::vector<HeaderDataStruct>, std::vector<float>>;
    virtual std::vector<Trace_t> getTraces(size_t startTrace, size_t tracesToDump) const = 0;
    virtual std::vector<Trace_t> getTraces(const request::TraceParams& params) const = 0;

    /**
     * @brief Gets trace samples.
     *
     * @param result std::vector<std::vector<float>> where the collected output sample values will be stored
     * @param startTrace    Zero-offset trace number to start extraction of header values.
     * @param tracesToDump  Number of requested/returned traces to extract values from.
     * @param sampleOffset  Zero-offset sample number to start extraction of trace sample values.
     * @param samplesToDump Number of requested/returned samples to extract, starting from sampleOffset.
     * @returns bool indicating operation success or failure
     *
     **/
    virtual bool getTraceSamples(std::vector<std::vector<float>>& result, size_t startTrace, size_t tracesToDump, size_t sampleOffset = 0, size_t samplesToDump = 0) const = 0;
    virtual bool getTraceSamples(std::vector<std::vector<float>>& result, const request::TraceParams& params) const = 0;

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
    virtual bool getTraceHeaderValues(std::vector<std::string>& headerKeys, std::vector<std::vector<double>>& headerValues, size_t startTrace, size_t tracesToDump) const = 0;
    virtual bool getTraceHeaderValues(std::vector<std::string>& headerKeys, std::vector<std::vector<double>>& headerValues, const request::TraceParams& params) const = 0;

    /**
     * @brief Gets scaled trace headers.
     *
     * Returns a two-dimensional vector of HeaderDataStruct holding trace header values as they are seen by the internal methods. This means scaling factors and/or user defined overrides is applied for each trace.
     * All defined trace header literals are extracted.
     *
     * @param startTrace    Zero-offset trace number to start extraction of header values.
     * @param tracesToDump  Number of traces to extract values from.
     *
     * @param params  Request parameters. ScaledHeaders flag is irrelevant for this call.
     *
     * Example of output:
     * @snippet getRawTraceHeaders.txt result
     *
     **/
    virtual std::vector<std::vector<HeaderDataStruct>> getScaledTraceHeaders(size_t startTrace, size_t tracesToDump) const = 0;
    virtual std::vector<std::vector<HeaderDataStruct>> getScaledTraceHeaders(const request::TraceParams& params) const = 0;

    /**
     * @brief A performance-optimized way to get the values of only those trace header fields which are needed for the caller, for several traces in one batch.
     *
     * @returns a vector of trace header values only, as they are seen by the internal methods. This means scaling factors and/or user defined overrides are applied.
     * Only the trace header literals specified in the header_ids vector are extracted.
     *
     * @param startTrace    Zero-offset trace number to start extraction of header values.
     * @param tracesToDump  Number of traces to extract values from.
     * @param headerIds    Vector holding the name of the trace header literals to be extracted.
     *
     **/
    virtual std::vector<std::vector<double>> getSelectedScaledTraceHeaderValues(size_t startTrace, size_t tracesToDump, std::vector<std::string> headerIds) = 0;

    /**
     * @brief A performance-optimized way to get Survey Bin Grid definition. Approximation is made if there are missing traces in the survey.
     * In case of different trace size Bin Grid could not be calculated and default structure is returned.
     *
     * @returns a std::string formatted as Json which contains calculated P6 params for the selected SEGY file with Min and Max values for the Inline and Crossline.
     *
     **/
    virtual std::string getSurveyBinGrid() const = 0;

    /**
     * @brief Create bulk data reader for the SEG-Y dataset.
     *
     * @returns a pointer to the bulk data (trace data) reader for the current context.
     **/
    virtual std::unique_ptr<bulkaccess::SegyBulkReader> createBulkReader() = 0;

    /**
     * @brief Create multi-bulk data reader for the SEG-Y dataset.
     *
     * @param segyIndex SEG-Y file index to create the multi-bulk data reader with
     * @param numberOfTraces specifies the number of traces ahead the reader is aware of
     * @param bulksCount specifies number of bulk readers will be created implicitly
     *
     * @returns a pointer to the multi-bulk data (trace data) reader for the current context.
     **/
    virtual std::unique_ptr<bulkaccess::SegyBulkReader> createMultiBulkReader(std::shared_ptr<segyindex::SegyIndex> segyIndex, size_t numberOfTraces, size_t bulksCount) = 0;

    /**
     * @brief Create inline/crossline section data reader for the SEG-Y dataset.
     *
     * @param readersCount specifies number of bulk readers will be created implicitly
     *
     * @returns a pointer to the inline/crossline section data (trace data) reader for the current context.
     **/
    virtual std::unique_ptr<bulkaccess::SegyBulkReader> createSectionBulkReader(size_t readersCount) = 0;

    /**
     * @brief Create samples reader with focus on parallel SEG-Y dataset strides access.
     *
     * @returns a pointer to reader for current context.
     **/
    virtual std::unique_ptr<bulkaccess::async::Reader> createAsyncReader(const ReaderCreateParams& params) = 0;

    /**
     * @brief Create samples reader for 2D SEGY file
     *
     * @returns a pointer to 2D reader for the current context
     **/
    virtual std::unique_ptr<bulkaccess::Segy2DReader> create2DReader() = 0;

    /**
     * @brief Read text file from either local or sd-path
     *
     * @returns textual content as a string
     **/
    virtual std::string readTextFile(const std::string& textFilePath) const = 0;

    /**
     * @brief OSDU metadata from either sd-path or id
     *
     * @returns textual content as a string
     **/
    virtual std::string getOsduMetadata(const std::string& path_or_id) const = 0;

    virtual bool validateSession() const = 0;

    /**
     * @brief File layout based on dimensionality
     *
     * @returns File layout structure
     **/
    virtual FileLayout getFileLayout(const request::Params& params) const = 0;

    /**
     * @brief File reference with session credentials and cache option
     *
     * @param fileName name of the file to reference. Use SEG-Y file name if empty
     *
     * @returns File reference
     **/
    virtual fileio::FileReference getFileReference(const std::string& fileName) const = 0;
};

} // namespace segysdk::api
