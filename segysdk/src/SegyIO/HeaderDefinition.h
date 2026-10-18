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

/**
* @file HeaderDefinition.h
*
* @details HeaderDefinition is the class holding the internal byte location definitions for binary and trace header fields.
*
*/

#include "../segysdk_details/DLL_EXPORT.h"

#include "../SegyIO/HeaderDataType.h"
#include "../SegyIO/ScalarUsageStrategy.h"

#include "../SegyIO/HeaderOverride.h"

#include <memory>
#include <string>

namespace segysdk::segyio
{

/**
  * @brief The header definition (sometimes called literal)  is describing a single attribute in either the binary- or the trace header.
  *
  */
class SEGYSDK_DLL_PUBLIC HeaderDefinition
{
private:
    std::string id_;
    std::string externalId_;
    std::string displayName_;
    uint64_t offset_;
    ScalarUsageStrategy scalarUsageStrategy_{ScalarUsageStrategy::NoScaling};
    std::string attribute_uom_;

    float extraUnitScalar_{1};
    int scalarOverride_{0};

    std::unique_ptr<HeaderOverride> cachedOverride_;

private:
    std::string scalarHeaderId_;
    bool isWellKnown_{false};
    unsigned short byteCount_;
    HeaderDataType headerDataType_{HeaderDataType::UnsignedInteger};

public:
    /**
  * @brief Copy constructor for class.
  *
  */
    HeaderDefinition(const HeaderDefinition& other);

    /**
  * @brief Constructor for class.
  *
  */
    HeaderDefinition(std::string id, std::string displayName, uint64_t offset, unsigned short byteCount);

    /**
  * @brief Assignment operator for class.
  *
  */
    HeaderDefinition& operator=(HeaderDefinition other);

    /**
  * @brief Default destructor for class.
  *
  */
    virtual ~HeaderDefinition();

    /**
  * @brief Gets the header id string for the current byte location literal. This is the id used internally in the SEG-Y SDK. See ExternalHeaderId to get the header id used by the end user.
  *
  * @returns string with the header id of the literal.
  */
    std::string getHeaderId() const;

    /**
    * @brief Sets the header id string for the current byte location literal. 
    * @param headerId is the new value of HeaderId
    */
    void setHeaderId(const std::string& headerId);

    /**
  * @brief Gets the display name for the current byte location literal.
  *
  * @returns string with the display name of the literal.
  */
    std::string getDisplayName() const;

    /**
     * @brief Sets the header id string for the current byte location literal.
     * @param displayName is the new value of displayName
     */
    void setDisplayName(const std::string& displayName);

    /**
  * @brief Gets the external id for the current byte location literal.
  *
  * @details The external id is used to map between user provided header definitions and the internal representation. 
  * This allows the SEG-Y SDK to allow slightly different id values for the literals when running in different configurations, 
  * e.g. DELFI and OSDU use slightly different header id values. This will enable a consumer to present the current header definitions back to the user 
  * and still work with the header id values tehy are familiar with rather than the internal id.
  *
  * @returns string with the external header id of the literal.
  */
    std::string getExternalHeaderId() const;

    /**
  * @brief Sets the external id for the current byte location literal.
  *
  * @details The external id is used to map between user provided header definitions and the internal representation. This allows the SEG-Y SDK to 
  * allow slightly different id values for the literals when running in different configurations, e.g. DELFI and OSDU use slightly different header id 
  * values. This will enable a consumer to present the current header definitions back to the user and still work with the header id values they are 
  * familiar with rather than the internal id.
  *
  * @param externalHeaderId   String with the external header id of the literal.
  */
    void setExternalHeaderId(const std::string& externalHeaderId);

    /**
  * @brief Gets the scalar header id to be used for the current byte location literal.
  *
  * @details The SEG-Y standard uses various scaling factors for a number of trace header field, e.g. coordinate values, elevation values, 
  * Shotpoint values etc. This field allows for specification of what regular scalar field to use for the current literal.
  *
  * @returns string with the scalar header id of the literal.
  */
    std::string getScalarHeaderId() const;

    /**
  * @brief Sets the scalar header id to be used for the current byte location literal.
  *
  * @details The SEG-Y standard uses various scaling factors for a number of trace header field, e.g. coordinate values, elevation values, Shotpoint values etc. 
  * This field allows for specification of what regular scalar field to use for the current literal.
  *
  * @param scalarHeaderId   String with the scalar header id of the literal.
  */
    void setScalarHeaderId(const std::string& scalarHeaderId);

    /**
  * @brief Gets the header data type used for the current byte location literal.
  *
  * @details The header data type corresponds to data type definitions in the SEG-Y standard. Currently these are:
  *  Text = 1,
  *  Integer = 2,
  *  UnsignedInteger = 3,
  *  Float = 4,
  *  Other = 5,
  *  IBMFloat = 6,
  *  IEEEFloat = 7
  *
  * @returns DataType enumerable with the field type
  */
    HeaderDataType getHeaderDataType() const;

    /**
  * @brief Sets the header data type used for the current byte location literal.
  *
  * @details The header data type corresponds to data type definitions in the SEG-Y standard. Currently these are:
  *  Text = 1,
  *  Integer = 2,
  *  UnsignedInteger = 3,
  *  Float = 4,
  *  Other = 5,
  *  IBMFloat = 6,
  *  IEEEFloat = 7
  *
  * @param headerDataType   DataType enumerable with the field type as described above.
  */
    void setHeaderDataType(HeaderDataType headerDataType) { this->headerDataType_ = headerDataType; }

    /**
  * @brief Gets the number of bytes used to represent the current byte location literal.
  *
  * @details This will normally be either 2 or 4 bytes, but the standard also have cases where the length is 6 bytes (composite constant and mantissa fields). 
  * This should be represented as 2 separate fields where the first 4 bytes represent the constant and the last two represent the mantissa.
  *
  * @returns short integer with the byte count.
  *
  **/
    uint16_t getByteCount() const;

    /**
 * @brief Sets the number of bytes used to represent the current byte location literal.
 *
 * @details This will normally be either 2 or 4 bytes, but the standard also have cases where the length is 6 bytes (composite constant and mantissa fields). 
 * This should be represented as 2 separate fields where the first 4 bytes represent the constant and the last two represent the mantissa.
 *
 * @param byteCount   Unsigned short integer value. The sum of the offset (getOffset) and byte count must not exceed the size of the trace header size 
 * (i.e. if the trace header offset (zero referenced)  is 238 the maximum byte count value is 2 (i.e. will populate the two last/highest bytes in the trace header). 
 * If the header offset is less than or equal to 236 the value might be 4.
 *
 **/
    void setByteCount(unsigned short byteCount) { this->byteCount_ = byteCount; }

    /**
  * @brief Gets the zero referenced byte offset for the literal in the binary or trace header.
  *
  * @details For a binary header field this will be a value between 0 and 399, for a trace header field this is a value between 0 and 239.
  *
  * @returns unsigned long integer with the header offset.
  *
  **/
    uint64_t getOffset() const;

    /**
  * @brief Gets the zero referenced byte offset for the literal in the binary or trace header.
  *
  * @details For a binary header field this is a value between 0 and 399, for a trace header field this is a value between 0 and 239.
  *
  * @param newOffset    Unsigned long integer with the header offset. Make sure an appropriate value for the current header type is used.
  *
  **/
    void setOffset(uint64_t newOffset);

    /**
  * @brief Gets the unit of measurement used for the current header literal.
  *
  * @details Several of the trace header fields have unit of measurement associated with them, e.g. elvation or coordinate values.
  *
  * @returns  String with the current unit of measurement.
  *
  **/
    std::string getUoM() const { return attribute_uom_; }

    /**
  * @brief Sets the unit of measurement used for the current header literal.
  *
  * @details Several of the trace header fields have unit of measurement associated with them, e.g. elvation or coordinate values.
  *
  * @param unitSymbol     String with the new  unit of measurement.
  *
  **/
    void setUoM(std::string unitSymbol);

    /**
  * @brief Gets the well known for the current header literal.
  *
  * @details The isWellKnown value is true if there was a valid mapping between the external header id and the internal id.
  *
  * @returns  bool indicating isWellKnown (true) or is not wekk known (false).
  *
  **/
    bool getIsWellKnown() const;

    /**
  * @brief Sets the well known for the current header literal.
  *
  * @details The isWellKnown value is true if there is a valid mapping between the external header id and the internal id.
  *
  * @param  isWellKnown   bool indicating isWellKnown (true) or is not wekk known (false).
  *
  **/
    void setIsWellKnown(bool isWellKnown) { this->isWellKnown_ = isWellKnown; }

    /**
  * @brief Gets the scalar override value.
  *
  * @details This is a mechanism allowing a field to use a scalar value different than the default scalar for the field. Returns 0 if unspecified or 
  * if ScalarUsageStrategy is not set to Override. The scalar value should conform to the standard scalar definition used by the SEG-Y standard 
  * where a positive value means Multiply value by the scalar, and a negative value means Divide by the absolute scalar value. The absolute scalar 
  * value should be 0, 10, 100, 1000 or 10000.
  *
  * @returns Integer value holding the scalar override value.
  **/
    int getScalarOverride() const;

    /**
  * @brief Sets the scalar override value.
  *
  * @details This is a mechanism allowing a field to use a scalar value different than the default scalar for the field. Returns 0 if unspecified or 
  * if ScalarUsageStrategy is not set to Override. The scalar value should conform to the standard scalar definition used by the SEG-Y standard where 
  * a positive value means Multiply value by the scalar, and a negative value means Divide by the absolute scalar value. The absolute scalar value should 
  * be 0, 10, 100, 1000 or 10000.
  *
  * @param scalarOverride  Integer value as indicated above.
  **/
    void setScalarOverride(int scalarOverride);

    /**
  * @brief Gets the scalar override strategy.
  *
  *
  * @returns ScalarUsageStrategy enumerable.
  **/
    ScalarUsageStrategy getScalarUsageStrategy() const;

    /**
  * @brief Sets the scalar override strategy.
  *
  * @param ScalarUsageStrategy  ScalarUsageStrategy enumerable describing override strategy for the current header literal.
  *
  **/
    void setScalarUsageStrategy(ScalarUsageStrategy scalarUsageStrategy);

    /**
  * @brief Gets the extra unit scalar.
  *
  * @details The extra unit scalar value is a floating point value, intended primarily to be used to handle large sample increment values for depth seismic. 
  * In rev 0 and rev 1 the SEG-Y standard does not specify how sample increment should be represented for depth seismic, but is using micro units for time seismic, 
  * allowing sample rate with 3 decimal precision for milliseconds. When depth seimic started to become more commonplace, the same convention was applied, 
  * where the sample increment would be given as feet or meter multiplied by 1000 to get the same precision. This caused anissue in cases where sample interval is large,
  * as the highest integer value possible to store in a 16 bit field would be only 65 meter or feet. This has been sorted in SEG-Y rev 2 where the depth sample increment 
  * is to be given in whole meter or feet. Using a extra unit scalar value of 0.001 indicates the value should be divided by 1000 as per the older SEG-Y standards, 
  * whereas a value of 1 means the value is given as whole meter or feet.
  *
  * @returns  floating point value the extra unit scalar value for the current field.
  **/
    float getExtraUnitScalar() const { return this->extraUnitScalar_; }

    /**
  * @brief Sets the extra unit scalar.
  *
  * @details The extra unit scalar value is a floating point value, intended primarily to be used to handle large sample increment values for depth seismic. 
  * In rev 0 and rev 1 the SEG-Y standard does not specify how sample increment should be represented for depth seismic, but is using micro units for time seismic, 
  * allowing sample rate with 3 decimal precision for milliseconds. When depth seimic started to become more commonplace, the same convention was applied, where the 
  * sample increment would be given as feet or meter multiplied by 1000 to get the same precision. This caused anissue in cases where sample interval is large, as the 
  * highest integer value possible to store in a 16 bit field would be only 65 meter or feet. This has been sorted in SEG-Y rev 2 where the depth sample increment is 
  * to be given in whole meter or feet. Using a extra unit scalar value of 0.001 indicates the value should be divided by 1000 as per the older SEG-Y standards, 
  * whereas a value of 1 means the value is given as whole meter or feet.
  *
  * @param  value    floating point value the extra unit scalar value to be used for the current field.
  **/
    void setExtraUnitScalar(float value);

    /**
  * @brief Checks if the current trace header literal is defined as using a scalar field or not.
  *
  * @returns  Bool value indicating whether regular scaling operation is to be performed for the field. Returns true if the scalarId is not an empty string.
  *
  **/
    bool needsScalar() const { return !this->scalarHeaderId_.empty(); }

    /**
    * @brief Checks if the current trace header literal is defined as using ScalarIndicator == Override or not.
    *
    * @returns  Bool value indicating whether field specific scaling operation is to be performed for the field. Returns true if the ScalarIndicator equals Override.
    *
    **/
    bool needsScalarOverride() const {return !(this->scalarUsageStrategy_ == ScalarUsageStrategy::Override); }

    /**
     * @brief Checks if the trace header value should be overridden by a user specified value or not.
     *
     * @returns  Bool value indicating whether there is an override value to be used for the field. Returns true if there is an override specified for the current headerId, false if not.
     *
     **/
    bool isOverridden() const { return this->cachedOverride_ != nullptr; }

    /**
     * @brief Method to get the cached overrid)e value for the current trace header literal.
  *
  * @details  Provided for temporary access to the returned value only.  Calling code must not store or delete the returned pointer.
  *
  * @returns pointer to cached override value.
  *
  **/
    const HeaderOverride* getCachedHeaderOverride() const { return this->cachedOverride_.get(); }

    /**
  * @brief Sets the override strategy for the current trace header literal.
  *
  * @details  Provided for temporary access to the returned value only.  Calling code must not store or delete the returned pointer.
  *
  * @param headerOverride  pointer to cached override value.
  *
  **/
    void setCachedHeaderOverride(const HeaderOverride& headerOverride);
};

} // namespace segysdk::segyio
