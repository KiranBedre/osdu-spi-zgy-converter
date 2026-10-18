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

#pragma once

#include "segysdk_details/DLL_EXPORT.h"

#include <string>

namespace segysdk::api
{
/**
 * @brief The HeaderDataStruct is mainly intended to be used when building up results (binary header extraction or trace header extraction) to be presented back to the end-user.  This is the reason why offset is presented as a 1-referenced rather than 0-referenced value to make offsets correspond to the value used in the SEG-Y standard.
 *
 **/
struct SEGYSDK_DLL_PUBLIC HeaderDataStruct
{
    /**
     * @brief  The id is usually set to be the ExternalHeaderId in the internal HeaderDefinition class when the result is intended for the end user.
     **/
    std::string id;

    /**
     * @brief The offset is stored as a 1-referenced value, representing the binary header or trace header offset to the field. Valid value range is 1-239 for trace header fields, and 1 to 399 for binary header fields.
     **/
    std::size_t offset{};

    /**
     * @brief  A 1-referenced value representing the offset to the last byte of the field in either the binary- or trace header. Valid value range is 3-240 for trace header fields, and 3 to 400 for binary header fields.
     **/
    std::size_t end{};

    /**
     * @brief The extracted value for the field.
     **/
    double value{};

    /**
     * @brief Constructor - Standard constructor for the struct.
     *
     * @param id        String value. If result is intended to be used by end-user this should be external id, if not it should be the headerId of the HeaderDefinition class.
     * @param start     Integer value, the zero-referenced offset to the field in the current header (binary or trace) The value is converted to 1-referenced value when stored.
     * @param length    Integer value representing the number of bytes required to hold the field in the header, typically this is a value of 2 or 4 (bytes).
     * @param hdrValue  Double value representing the binary- or trace header value extracted for the literal.
     **/
    HeaderDataStruct(std::string id, std::size_t start, std::size_t length, double hdrValue);
    HeaderDataStruct() = default;
};

} // namespace segysdk::api
