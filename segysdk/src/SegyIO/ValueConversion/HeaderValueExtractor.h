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

#include "ScalarValueExtractor.h"

#include "../SegyIO/ValueConversion/ByteConversion.h"
#include "../SegyIO/ValueConversion/ValueExtractor.h"
#include "../SegyIO/HeaderDefinition.h"
#include "../SegyIO/ConversionDetails.h"
// Erik ToDo:
// #include <Foundation/Primitives/Utility/IUnitDescriptor.h>
// #include <Units/Api/IUnitConverter.h>

#pragma warning(push)
#pragma warning(disable : 4995) // 'function': name was marked as #pragma deprecated
#include <stdint.h>
#pragma warning(pop)

namespace segysdk::segyio::valueconversion
{

class SEGYSDK_DLL_PUBLIC HeaderValueExtractor : public ValueExtractor
{
private:
    bool hasScalar_ = false;
    bool hasUnitScaleValue_ = false;
    bool hasUnitConversion_ = false;
    ScalarValueExtractor* scalar_ = nullptr;
    double unitScaleValue_ = 1.0;
    // units::IUnitDescriptorPtr unitDescriptor_;
    // const Slb::Units::IUnitConverter *unitConverter_;
    bool hasScalarOverride_ = false;
    double scalarOverrideValue_ = 1.0;
    bool hasValueOffset_ = false;

private:
    bool hasScalar() const { return hasScalar_; }
    bool hasUnitScaleValue() const { return hasUnitScaleValue_; }
    bool hasUnitConversion() const { return hasUnitConversion_; }

public:
    HeaderValueExtractor(const HeaderValueExtractor& other) = delete;
    HeaderValueExtractor& operator=(const HeaderValueExtractor& other) = delete;

    HeaderValueExtractor(const std::shared_ptr<const HeaderDefinition>& definition, const ConversionDetails& conversionDetails);
    HeaderValueExtractor(const std::string& headerId, int valueOffset);

    virtual ~HeaderValueExtractor();

    bool hasScalarOverride() const { return hasScalarOverride_; }
    double getScalarOverrideValue() const { return scalarOverrideValue_; }

    bool hasProcessing() const { return hasScalar() || hasUnitScaleValue() || hasUnitConversion(); }

    void setScalar(ScalarValueExtractor* scalar);

    void prepareExtraction() override;

    inline void applyScalar(double& value) const
    {
        // Priority order:
        // 1. If field has ScaleFactorOverride, use it.
        // 2. If field has Scalar defined, use it.
        // 3. If Scalar value is overridden, use the override value
        // 4. Applay standard default scalar value
        if (hasScalarOverride()) {
            // Scalar override can be 0, 1 or another scalar value.
            // 0 or 1 means do not do scaling.
            value *= getScalarOverrideValue();
        }
        else {
            // ScaleValue should be either overridden scale value or
            // scale factor value from trace header.
            if (hasScalar() && scalar_->hasScaleValue()) {
                value *= scalar_->getScaleValue();
            }
        }
    }

    inline void applyUnitConversion(double& value) const
    {
        if (hasUnitScaleValue()) {
            value *= unitScaleValue_;
        }

        if (hasUnitConversion()) {
            // Erik ToDo:
            // value = unitConverter_->Convert(value);
        }
    }

    inline double extractValue(const uint8_t* buffer) const
    {
        // Action/Priority order:
        // 1) Does it have a fixed value? 
        //    If so, use it without even looking at trace header value
        // 2) Does it have definition with scalar override? 
        //    If so, use it
        // 3) Does it have definition indicating use of scalar?
        //    If so, use it
        // 4) No scalar? 
        //    Just return converted value

        if (hasFixedValue()) {
            return getFixedValue();
        }

        double value = converter_->convert(buffer + getBufferOffset());

        applyScalar(value);
        // Erik ToDo:
        // applyUnitConversion(value);

        return value;
    }
};

} // namespace segysdk::segyio::valueconversion
