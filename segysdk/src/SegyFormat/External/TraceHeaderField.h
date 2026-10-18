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

#include "SegyHeaderField.h"
#include "UoMStringConsts.h"

#include <string>

namespace segysdk::segyformat::external
{

enum class SEGYSDK_DLL_PUBLIC ScalarIndicator
{
    STANDARD = 0,
    NOSCALE = 1,
    OVERRIDE = 2
};

class SEGYSDK_DLL_PUBLIC TraceHeaderField final : public SegyHeaderField
{

private:
    ScalarIndicator scalarIndicator_{ScalarIndicator::NOSCALE};
    std::string scalarAttribute_{};
    int scalarOverrideValue_{0};  
    std::string uom_{UoMStringConsts::Unitless};

public:
    TraceHeaderField();

    TraceHeaderField(const std::string& key, const std::string& originalKey, const DataFormat& format, const int length, const int offset);
    TraceHeaderField(const std::string& key, const std::string& originalKey, const DataFormat& format, const int length, const int offset, std::string uom);
    TraceHeaderField(const std::string& key, const std::string& originalKey, const DataFormat& format, const int length, const int offset, ScalarIndicator scalarIndicator, std::string scalarField, std::string uom);
    TraceHeaderField(const std::string& key, const std::string& originalKey, const DataFormat& format, const int length, const int offset, ScalarIndicator scalarIndicator, std::string scalarField, const int scalarOverrideValue, std::string uom);

    virtual bool IsTraceHeader() const { return true; }

    std::string getScalarAttribute() const { return scalarAttribute_; }
    int getScalarOverrideValue() const { return scalarOverrideValue_; }
    ScalarIndicator getScalarIndicatorValue() const { return scalarIndicator_; }
    std::string getUoM() const { return uom_; }
};

} // namespace segysdk::segyformat::external
