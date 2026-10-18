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

#include "SegyInputGeometry.h"

using namespace segysdk;
using namespace segysdk::segyindex;

SegyInputGeometry::SegyInputGeometry(const SeismicCubeExtents g)
  : inline_start_(g.getJVectorExtent().first()),
    inline_count_(g.getJVectorExtent().count()),
    inline_increment_(g.getJVectorExtent().step()),
    crossline_start_(g.getIVectorExtent().first()),
    crossline_count_(g.getIVectorExtent().count()),
    crossline_increment_(g.getIVectorExtent().step()),
    vertical_count_(g.getSampleExtent().count())
{
    switch (g.getSampleExtent().domain()) {
    case segyio::VerticalDomain::Depth:
        vertical_start_ = g.getSampleExtent().first();
        vertical_increment_ = g.getSampleExtent().step();
        vertical_unit_dimension_ = OpenZGY::UnitDimension::length;
        switch (g.getSampleExtent().zUoM()) {
        case segyio::VerticalUOM::meter:
            vertical_unit_name_ = "m";
            vertical_unit_value_ = 1.0;
            break;
        case segyio::VerticalUOM::feet:
            vertical_unit_name_ = "ft";
            vertical_unit_value_ = 0.3048;
            break;
        default:
            throw std::invalid_argument("Invalid vertical unit for depth domain");
        }
        break;
    case segyio::VerticalDomain::Time:
        vertical_start_ = g.getSampleExtent().first() * 1000.0;
        vertical_increment_ = g.getSampleExtent().step() * 1000.0;
        vertical_unit_dimension_ = OpenZGY::UnitDimension::time;
        switch (g.getSampleExtent().zUoM()) {
        case segyio::VerticalUOM::seconds:
            vertical_unit_name_ = "ms";
            vertical_unit_value_ = .001;
            break;
        default:
            throw std::invalid_argument("Invalid vertical unit for time domain");
        }
        break;
    default:
        throw std::invalid_argument("Unknown vertical domain value");
    }
}
