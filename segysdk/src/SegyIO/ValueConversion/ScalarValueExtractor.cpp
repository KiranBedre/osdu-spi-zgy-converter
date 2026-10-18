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

#include "ScalarValueExtractor.h"

#include "ByteConverter.h"

#pragma warning(push)
#pragma warning(disable : 4995) // 'function': name was marked as #pragma deprecated
#include <cmath>
#pragma warning(pop)

namespace segysdk::segyio::valueconversion
{
ScalarValueExtractor::ScalarValueExtractor(const std::shared_ptr<const HeaderDefinition>& definition, const ConversionDetails& conversionDetails)
  : ValueExtractor(definition, conversionDetails) {}

ScalarValueExtractor::~ScalarValueExtractor()
{}

void
ScalarValueExtractor::prepareExtraction()
{
    ValueExtractor::prepareExtraction();
    if (hasScalarOverrideValue()) {
        setScaleValue(getScalarOverrideValue());
    }
    else if (hasFixedValue() && !std::isnan(getFixedValue())) {
        setScaleValue(getFixedValue());
    }
}

} // namespace segysdk::segyio::valueconversion
