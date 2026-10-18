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
 * @brief IndexField.h contains the declaration of the IndexField class used by the SegyIndex
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>

#include <cstddef>
#include <cstdint>
#include <string>

namespace segysdk::segyindex
{
 /**
 * @class IndexField
 * @brief Indexing information for a requested inline/crossline location. (trace)
 **/
class SEGYSDK_DLL_PUBLIC IndexField
{
public:
    /**
     * @brief Standard IndexField constructor.
     *
     * @param segyFileNo uint16_t zero-offset reference to physical file number holding the current trace. This value Is zero if volume is based on a single file.
     * @param inlineNo  Integer value representing the inline number of the trace.
     * @param crosslineNo  Integer value representing the crossline number of the trace.
     * @param offset   64-bit unsigned value representing the offset in the speceified SEG-Y file before reaching the specified trace.
     *
     **/
    IndexField(size_t segyFileNo, int inlineNo, int crosslineNo, size_t offset) noexcept;

    /**
     * @brief Gets the logical file number holding the data for the specified trace.
     *
     * @returns Unsigned short int value representing the file number in the list of input files.
     **/
     uint16_t getSegyFileNumber() const noexcept;

    /**
     * @brief Returns the Inline value of the current trace.
     *
     * @returns int value representing the current inline.
     *
     **/
    int getInline() const noexcept;

    /**
     * @brief Returns the Crossline value of the current trace.
     *
     * @returns int value representing the current crossline.
     **/
    int getCrossline() const noexcept;

    /**
     * @brief Returns the Offset in the specified input file to reach the current trace.
     *
     * @returns 64-bit unsigned integer with the file offset in the current SEG-Y file to get to the requested trace.
     *
     **/
    size_t getOffset() const noexcept;

    /**
     * @brief Compares IndexField class instances.
     *
     **/
    bool operator==(IndexField const& other) const;
    bool operator!=(IndexField const& other) const;

private:
    size_t segyFileNumber_;
    int inlineNo_;
    int crosslineNo_;
    size_t offset_;
};

} // namespace segysdk::segyindex
