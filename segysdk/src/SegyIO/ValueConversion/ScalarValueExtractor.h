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

#include "../SegyIO/ValueConversion/ByteConverter.h"
#include "../SegyIO/ValueConversion/ValueExtractor.h"
#include "../SegyIO/ConversionDetails.h"
#include "../SegyIO/HeaderDefinition.h"

#pragma warning(push)
#pragma warning(disable : 4995) // 'function': name was marked as #pragma deprecated
#include <stdint.h>
#pragma warning(pop)

namespace segysdk::segyio::valueconversion
{

class SEGYSDK_DLL_PUBLIC ScalarValueExtractor : public ValueExtractor
{
private:
    bool hasScaleValue_ = false;
    double scaleValue_ = 1.0;

public:
    ScalarValueExtractor(const ScalarValueExtractor& other) = delete;
    ScalarValueExtractor& operator=(const ScalarValueExtractor& other) = delete;

    ScalarValueExtractor(const std::shared_ptr<const HeaderDefinition>& definition, const ConversionDetails& conversionDetails);
    virtual ~ScalarValueExtractor();

    double getScaleValue() const { return scaleValue_; }

    bool hasScaleValue() const { return hasScaleValue_; }

    void prepareExtraction();

    inline void setScaleValue(double headerValue)
    {
        if (headerValue > 1.0) {
            scaleValue_ = headerValue;
            hasScaleValue_ = true;
        }
        else if (headerValue < -1.0) {
            scaleValue_ = -1.0 / headerValue;
            hasScaleValue_ = true;
        }
        else {
            scaleValue_ = 1.0;
            hasScaleValue_ = false;
        }
    }

    inline double extractValue(const uint8_t* buffer)
    {
        if (hasFixedValue()) {
            return getFixedValue();
        }

        const double value = converter_->convert(buffer + getBufferOffset());
        setScaleValue(value);

        return value;
    }
};

} // namespace segysdk::segyio::valueconversion
