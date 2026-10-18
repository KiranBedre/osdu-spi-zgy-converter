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

#include "SegyInputGeometry.h"

#include "../segysdk/SegySession.h"
#include "../SegyIO/SampleFormat.h"

#include <openzgy/api.h>

class SegyInputParameters
{
public:
    explicit SegyInputParameters(segysdk::api::SegySession& segySession);

    float getInlineStart() const { return geometry_.getInlineStart(); }
    size_t getInlineCount() const { return geometry_.getInlineCount(); }
    float getInlineIncrement() const { return geometry_.getInlineIncrement(); }
    float getCrosslineStart() const { return geometry_.getCrosslineStart(); }
    size_t getCrosslineCount() const { return geometry_.getCrosslineCount(); }
    float getCrosslineIncrement() const { return geometry_.getCrosslineIncrement(); }
    float getVerticalStart() const { return geometry_.getVerticalStart(); }
    size_t getVerticalCount() const { return geometry_.getVerticalCount(); }
    float getVerticalIncrement() const { return geometry_.getVerticalIncrement(); }

    OpenZGY::UnitDimension getVerticalUnitDimension() const { return geometry_.getVerticalUnitDimension(); }
    std::string getVerticalUnitName() const { return geometry_.getVerticalUnitName(); }
    double getVerticalUnitValue() const { return geometry_.getVerticalUnitValue(); }
    float getDataRangeLow() const { return geometry_.getDataRangeLow(); }
    float getDataRangeHigh() const { return geometry_.getDataRangeHigh(); }
    segysdk::segyio::SampleFormat getSampleFormat() const { return sampleFormat_; }


    std::shared_ptr<segysdk::segyindex::SegyIndex> loadOrGenerateIndex() const;

private:
    std::shared_ptr<segysdk::segyindex::SegyIndex> loadOrGenerateIndex(segysdk::api::SegySession& segySession) const;
    segysdk::api::SegySession& segySession_;
    SegyInputGeometry geometry_;
    segysdk::segyio::SampleFormat sampleFormat_;
};
