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

#include "../SegyIndex/SeismicCubeExtents.h"

#include <openzgy/api.h>

#include <limits>

class SegyInputGeometry
{
public:
    explicit SegyInputGeometry(segysdk::segyindex::SeismicCubeExtents g);

    float getInlineStart() const { return inline_start_; }
    size_t getInlineCount() const { return inline_count_; }
    float getInlineIncrement() const { return inline_increment_; }
    float getCrosslineStart() const { return crossline_start_; }
    size_t getCrosslineCount() const { return crossline_count_; }
    float getCrosslineIncrement() const { return crossline_increment_; }
    float getVerticalStart() const { return vertical_start_; }
    size_t getVerticalCount() const { return vertical_count_; }
    float getVerticalIncrement() const { return vertical_increment_; }

    OpenZGY::UnitDimension getVerticalUnitDimension() const { return vertical_unit_dimension_; }
    std::string getVerticalUnitName() const { return vertical_unit_name_; }
    double getVerticalUnitValue() const { return vertical_unit_value_; }
    float getDataRangeLow() const { return data_range_low_; }
    float getDataRangeHigh() const { return data_range_high_; }

private:
    float inline_start_;
    size_t inline_count_;
    float inline_increment_;
    float crossline_start_;
    size_t crossline_count_;
    float crossline_increment_;
    size_t vertical_count_;

    float vertical_start_;
    float vertical_increment_;
    OpenZGY::UnitDimension vertical_unit_dimension_;
    std::string vertical_unit_name_;
    double vertical_unit_value_;
    float data_range_low_ = std::numeric_limits<float>::min();
    float data_range_high_ = std::numeric_limits<float>::max();
};
