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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../FileIO/FileReader.h"
#include "../SegyIO/HeaderDefinition.h"
#include "../SegyIO/HeaderOverride.h"
#include "../SegyIO/SampleFormat.h"

#include <map>
#include <memory>
#include <string>
#include <vector>

/**
* @file BinaryHeader.h
*
* @details BinaryHeader is the class holding the internal byte location definitions for binary header fields.
*
*/

namespace segysdk::segyio
{

/**
  * @class BinaryHeader
  *
  * @brief Class to hold information extracted from the SEG-Y Binary header.
  *
  **/
class SEGYSDK_DLL_PUBLIC BinaryHeader
{
private:
    bool isByteSwapped_ = true;
    std::map<std::string, int> valueMap_;

private:
    void InitializeSegyRevisionLevelStatus(const std::map<std::string, std::shared_ptr<HeaderOverride>>& overrides);

public:
    BinaryHeader(const std::shared_ptr<fileio::FileReader>& file,
                 const std::vector<std::shared_ptr<HeaderDefinition>>& headers,
                 const std::map<std::string, std::shared_ptr<HeaderOverride>>& overrides = std::map<std::string, std::shared_ptr<HeaderOverride>>());

    /**
  * @brief Test to see if the reader has managed to extract any data from the binary header.
  *
  * @returns  bool value returns true if one or more binary header fields have been successfully extracted.
  *
  **/
    bool isValid() const;

    /**
  * @brief Test to see if the data in the SEG-Y file is Big-endian or not.
  *
  * @details The SEG-Y standards rev 0 and rev 0 specifies that the byte ordering for numeric fields shall be Big-endian as this was the default byte 
  * ordering of the computers around the time when these standard revisions were created, but with the PC and Linux based systems files with 
  * Little-endian byte ordering are getting more common.
  *
  * @returns  bool value returns true if numerical format is big-endian. false means little-endian.
  *
  **/
    bool isByteSwapped() const { return isByteSwapped_; }

    /**
  * @brief SEG-Y revision 1 specific field specifying SEG-Y revision level.
  *
  * @details
  * This was introduced in SEG-Y revision 1. A value of 0x100 in location 3501-3502 indicates SEG-Y rev 1, a value of 0x200 indicates SEG-Y rev 2 and so on.
  * For pragmatic reasons a value of 1 is also interpreted as SEG-Y rev 1 and 2 indicates SEG-Y rev 2. Any values other than exact multiples of 0x100 
  * will be interpreted as representing SEG-Y rev 0.
  *
  * @returns  int value returns 0 if SEG-Y rev 0, 1 if SEG-Y rev 1 and 2 if SEG-Y rev 2.
  *
  **/
    int getRevisionLevel() const;

    /**
  * @brief Gets the Sample format for the trace data samples.
  * @details The numerical format for the trace sample values is set in the binary header and remains constant for the entire file.
  * @returns  SampleFormat enumerated value indicating what numerical format is used for the trace samples.
  *
  **/
    SampleFormat getSampleFormat() const;

    /**
  * @brief Gets the Sample count. If this varies throughout the SEG-Y file, this value should be the best possible approximation.
  * @details The number of samples per trace in the SEG-Y file can vary from trace to trace, but historically SEG-Y 3D dataset produced for use with seismic 
  * workstations tend to have constant number of samples per trace as not many products support variable number of samples per trace e.g. for performance reasons.
  * @returns  int representing numnber of samples per trace.
  *
  **/
    int getSampleCount() const;

    /**
 * @brief SEG-Y rev 1 or greated has fixed trace length flag and when set to true it guarantees all traces have equal length thoughout entire file.
 *
 * @details SEG-Y rev 1 or greated has introduced a fixed trace length flag in binary header location  3503-3504 and when set to true it guarantees all traces have 
 * equal length thoughout the entire file, removing the need to check sample count for every trace read. As this field was not known previous to SEG-Y rev 1,  
 * SEG-Y revision 0 files will have false returned here even if the trace sample count happens to be constant.
 *
 * @returns  bool value where true means every trace has constant number of samples, false means the flag has not been set (or the file is SEG-Y rev 0).
 *
 **/
    bool hasFixedSampleCount() const;

    /**
 * @brief Indicates whether or not the measurement system specified in the binary header has been set to a value of 1 (meter).
 *
 * @details Binary header location 3255-3256 should contain the value 1 if the measurement system used in the file is meter, and 2 if it is feet. 
 * Note that this does not make a distinction between Length units and depth units, and in real life these often are not the same! Depth units 
 * are often specified in feet while for UTM based coordinates the coordinates are specified as meter. This introduces the added complication 
 * that there are a number of different foot based measurement systems in use, and for large values e.g. in coordinate values it is vital that 
 * the correct type of foot is known. This leaves the values 2 indicating feet and  0 as unspecified.
 *
 * @returns  bool value indicating if binary header has specified using UoM meter.  For all other values it returns false.
 *
 **/
    bool isMetric() const;

    /**
  * @brief SEG-Y rev 1 or greated has concept of Extended trace headers. If extended headers are used the number of such headers _might_ be specificed in location 3505-3506.
  *
  * @details SEG-Y rev 1 or greated has introduced the facility of using Extended textual headers to record additional meta data about the SEG-Y data. If the required 
  * number of extended headers required is known when writing of the SEG-Y file start it should be recorded in this field, otherwise a value of -1 should be used to 
  * indicate that a variable number of extended headers has been used and that the occurrence of a header containing the ((SEG: EndText)) value indicates it is the 
  * last of the extended headers.
  *
  * @returns  int value where 0 means no extended headers, a positive value indicates that exactly this number of extended headers has been used, and the value -1 
  * means a sequence of 3200 byte extended headers has been recorded, the sequence ends when a header containing only the EndText flag is encountered.
  *
  **/
    int getExtendedTextualHeaderCount() const;

    /**
  * @brief Method to check if the requested header string is known and can be retrieved.
  * @param header  string holding trace header literal
  *
  * @returns bool True if the requested binary header literal is well known and can be extracted, otherwise the value false is returned.
  **/
    bool hasValue(const std::string& header) const;

    /**
  * @brief Used to request a value for the specified header literal to be extracted from the binary header.
  * @param header  string holding trace header literal
  *
  * @returns int value representing the content of the binary header from the byte location specified in the header definition for the specified header. 
  * A value of -1 is returned if the definition for the specified header literal is not found.
  *
  **/
    int getValue(const std::string& header) const;

    /**
  * @brief Used to request a value for the specified header literal to be extracted from the binary header, but returns the specified default value is returned if the definition for the specified header literal is not found.
  *
  * @param header  string holding trace header literal
  * @param defaultValue  int default value to be returned if header literal is not found
  *
  * @returns int value representing the content of the binary header from the byte location specified in the header definition for the specified header. The default value is returned in case requested header literal is not found.
  *
  **/
    int getValue(const std::string& header, int defaultValue) const;

    /**
 * @brief Gets values for every mapped binary header field.
 * @details This method extracts and returns a map holding every defined binary header literal and their values.
 *
 * @returns a map structure of string and int tuples holding the binary header literal and the integeer value it represents in the binary header.
 *
 **/
    const std::map<std::string, int>& getAllValues() const { return valueMap_; }

public:
    static SampleFormat getSampleFormat(int headerValue);
};

} // namespace segysdk::segyio
