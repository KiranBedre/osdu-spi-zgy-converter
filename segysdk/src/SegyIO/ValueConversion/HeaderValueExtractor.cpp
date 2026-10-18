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

#include "HeaderValueExtractor.h"

#include "../SegyIO/HeaderDefinition.h"

namespace segysdk::segyio::valueconversion
{

HeaderValueExtractor::HeaderValueExtractor(const std::shared_ptr<const HeaderDefinition>& definition, const ConversionDetails& conversionDetails)
  : ValueExtractor(definition, conversionDetails),
    unitScaleValue_(definition->getExtraUnitScalar())
{
    //  Erik ToDo:
    //  unitDescriptor_(definition.getUnitDescriptor()),
    //  unitConverter_(nullptr)
    //{
    //  if (unitDescriptor_ != nullptr)
    //  {
    //    hasUnitScaleValue_ = unitScaleValue_ != 1.0;
    //    if (!unitDescriptor_->isSIConverterUnity())
    //    {
    //      unitConverter_ = unitDescriptor_->getConverterToSI();
    //      hasUnitConversion_ = unitConverter_ != nullptr;
    //    }
    //  }

    hasScalarOverride_ = definition->getScalarUsageStrategy() == ScalarUsageStrategy::Override;
    if (hasScalarOverride_) {
        const auto tmp = definition->getScalarOverride();
        if (tmp == 0) {
            scalarOverrideValue_ = 1.0;
        }
        else {
            scalarOverrideValue_ = tmp < 0 ? -1.0 / tmp : tmp;
        }
    }
}

HeaderValueExtractor::HeaderValueExtractor(const std::string& headerId, int valueOffset)
  : ValueExtractor(headerId, valueOffset)
// Erik ToDo:
//unitDescriptor_(),
//unitConverter_(nullptr)
{
    // Dummy NaN producer, only used on error conditions
}

HeaderValueExtractor::~HeaderValueExtractor()
{
    /* EMPTY */
}

void
HeaderValueExtractor::setScalar(ScalarValueExtractor* scalar)
{
    scalar_ = scalar;
    hasScalar_ = scalar != nullptr;
}

void
HeaderValueExtractor::prepareExtraction()
{
    ValueExtractor::prepareExtraction();
}

} // namespace segysdk::segyio::valueconversion
