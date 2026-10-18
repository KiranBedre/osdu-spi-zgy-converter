/*
Copyright 2021-2022 Schlumberger

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
 * @brief SegyBulkReader.h contains the declaration of the SegyBulkReader class used for reading the trace sample data in the SEG-Y file.
 *
 */

#pragma once

#include "TraceHeaderResult.h"
#include "../SegyFormat/External/TraceHeaderField.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <cstddef>
#include <optional>
#include <vector>

namespace segysdk::bulkaccess
{

/**
 * @brief Seismic data reader's direction.
 */
enum class SEGYSDK_DLL_PUBLIC ReadDirection
{
    /**
     * @brief Reading is done from initial inline coordinate towards ending inline coordinate numbers
     */
    Inline,
    /**
     * @brief Reading is done from initial crossline coordinate towards ending crossline coordinate numbers
     */
    Crossline
};

/**
 * @brief Seismic data reader.
 *
 * Reads and converts seismic bulk data. Use SegySession::createBulkReader() to create an instance.
 *
 */
class SEGYSDK_DLL_PUBLIC SegyBulkReader
{
public:
    /**
   * @brief Virtual destructor.
   */
    virtual ~SegyBulkReader();

    /**
   * @brief Read seismic data bounded by a 3-D rectangular box.
   *
   * Data is converted to machine-native single precision floating point.
   * Buffer must be able to accommodate inlineCount*xlineCount*sampleCount floats.
   *
   * Accessing an individual value at (i, j, k):
   * @code
   * buffer[
   *     (i - inlineIndex) * inlineCount * xlineCount +
   *     (j - xlineIndex) * xlineCount +
   *     (k - sampleIndex)]
   * @endcode
   *
   * @param inlineIndex Inline (I) start index of data to read. 0-based.
   * @param xlineIndex  Crossline (J) start index of data to read. 0-based.
   * @param sampleIndex Vertical (K) start index of data to read. 0-based.
   * @param inlineCount Inline (I) size of data to read.
   * @param xlineCount  Crossline (J) size of data to read.
   * @param sampleCount Vertical (K) size of data to read.
   * @param buffer      Preallocated memory area to receive the data.
   * @returns           Success (always true).
   * @throws            std::invalid_argument if buffer is null.
   * @throws            std::invalid_argument if any part of the requested bounding box if outside the seismic cube.
   */
    // TODO always returns true, can be converted to void
    virtual bool read(size_t inlineIndex, size_t xlineIndex, size_t sampleIndex, size_t inlineCount, size_t xlineCount, size_t sampleCount, float* buffer) = 0;

    /**
   * @brief Read seismic data bounded by a 2-D inline/crossline section.
   *
   * Data is converted to machine-native single precision floating point.
   * Buffer must be able to accommodate xCount*sampleCount floats.
   *
   * Accessing an individual value at (n, k):
   * where (n) is (i) or (j) depending on (readDirection)
   * @code
   * buffer[
   *     (n - xIndex) * xCount +
   *     (k - sampleIndex)]
   * @endcode
   * where (xIndex) is (inlineIndex) or (xlineIndex) depending on (readDirection)
   *
   * @param readDirection Determines reading direction: alone the inline or crossline.
   * @param inlineIndex (I) start index of data to read. 0-based.
   * @param xlineIndex (J) start index of data to read. 0-based.
   * @param sampleIndex Vertical (K) start index of data to read. 0-based.
   * @param xCount Inline/Crossline depending on readDirection parameter, (I) or (J) size of data to read.
   * @param sampleCount Vertical (K) size of data to read.
   * @param buffer      Preallocated memory area to receive the data.
   * @returns           Success (always true).
   * @throws            std::invalid_argument if buffer is null.
   * @throws            std::invalid_argument if any part of the requested bounding box if outside the seismic cube.
   */
    // TODO always returns true, can be converted to void
    virtual bool read(ReadDirection readDirection, size_t inlineIndex, size_t xlineIndex, size_t sampleIndex, size_t xCount, size_t sampleCount, float* buffer) = 0;

    /**
     * @brief Read seismic trace header data bounded by a 3-D rectangular box.
     *
     * Data is converted to machine-native single precision floating point.
     * Buffer must be able to accommodate inlineCount*xlineCount*sampleCount floats.
     *
     * Accessing an individual value at (i, j, k):
     * @code
     * buffer[
     *     (i - inlineIndex) * inlineCount * xlineCount +
     *     (j - xlineIndex) * xlineCount +
     *     (k - sampleIndex)]
     * @endcode
     *
     * @param inlineIndex Inline (I) start index of data to read. 0-based.
     * @param xlineIndex  Crossline (J) start index of data to read. 0-based.
     * @param inlineCount Inline (I) size of data to read.
     * @param xlineCount  Crossline (J) size of data to read.
     * @param headers_list      std::vector holding trace header fields to extract.
     * @param trace_header_data            std::vector holding a vector of TraceHeaderResults with the extracted values
     * @returns           Success (always true).
     * @throws            std::invalid_argument if buffer is null.
     * @throws            std::invalid_argument if any part of the requested bounding box if outside the seismic cube.
     */
    virtual bool read_trace_header_data(size_t inlineIndex, size_t xlineIndex, size_t inlineCount, size_t xlineCount, const std::vector<segyformat::external::TraceHeaderField>& headers_list, std::vector<std::vector<TraceHeaderResult>>& trace_header_data) = 0;

    /**
    * @brief Sets size of cache buffer in bytes.
    *        The cache buffer is used to minimize the direct file reading.
    *
    * @param newSize Requested size of the cache buffer in bytes.
    */
    virtual void setBufferSize(size_t newSize) = 0;

    /**
    * @brief Gets the actual size of cache buffer in bytes.
    *        The cache buffer is used to minimize the direct file reading.
    *
    * @returns newSize size of the cache buffer in bytes.
    */
    virtual size_t getBufferSize() const = 0;

    /**
    * @brief Sets chunk size to load the cache buffer data in bytes.
    *        The chunk size is used (1) to match the optimum read chunk of the file storage (2) to round cache buffer size nearest boundary.
    *
    * @param newChunk Requested cache buffer load chunk size in bytes.
    */
    virtual void setBufferLoadChunk(size_t newChunk) = 0;

    /**
    * @brief Marks internal cache buffer as empty.
    *        The cache buffer is used to minimize the direct file reading.
    *
    */
    virtual void dropCachedData() = 0;

    /**
    * @brief Performs reading to cache buffer specified portion of the data
    *        The cache buffer is used to minimize the direct file reading.
    *
    * @param fileOffset Requested data file offset in bytes.
    * @param dataSize Requested data portion size in bytes; zero (0) value is ignored and the present size is used.
    */
    virtual void LoadDataToCache(size_t fileOffset, size_t dataSize) = 0;

    /**
    * @brief Sets internal cache buffer read offset values.
    *        The cache buffer is used to minimize the direct file reading.
    *        Brick reading requires to know offsets per inline reader to load the buffers from.
    * @returns (optional) size_t file offset value the internal cache will be initialized from
    *
    */
    virtual std::optional<size_t> setReadOffsetHint(size_t inlineIndex, size_t xlineIndex, size_t sampleIndex) = 0;

    /**
    * @brief Forces the reader to forget/clean all the read offset hints information.
    *
    */
    virtual void clearReadOffsetHints() = 0;

    /**
     * @brief Forces the reader use the specified read increment.
     *
     * @param value forces the bulk reader to use value as the increment between inlines.
     *
     */
    virtual void set_inline_read_increment(size_t value) = 0;
};

} // namespace segysdk::bulkaccess
