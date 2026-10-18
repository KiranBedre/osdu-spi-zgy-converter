/*
Copyright 2021-2022 Schlumberger

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

#include "BinGrid.h"

#include <cmath>
#include <stdexcept>

namespace segysdk::utilities
{

static constexpr double pi = 3.14159265358979323846;

BinGrid::Coordinates::Coordinates(const double easting, const double northing)
  : easting(easting), northing(northing)
{
}
BinGrid::Coordinates
BinGrid::getCoordinatesIJ(const double i, const double j) const
{
    /*
Bin grid origin (Io) Ordinate 1 of evaluation point in source CRS (XSO)
Bin grid origin (Jo) Ordinate 2 of evaluation point in source CRS (YSO)
Map grid easting of bin grid origin (Eo) Ordinate 1 of evaluation point in target CRS (XTO)
Map grid northing of bin grid origin (No) Ordinate 2 of evaluation point in target CRS (YTO)
Scale factor of bin grid (SF) Point scale factor (k)
Nominal bin width along I axis (I_bin_width) Scale factor for source coordinate reference system
first axis (MX)
Nominal bin width along J axis (J_bin_width) Scale factor for source coordinate reference system
second axis (My)
Grid bearing of bin grid J axis (θ) Rotation angle of source coordinate reference
system axes (θ)
Bin node increment on I axis (I_bin_inc) Bin node increment on I-axis
Bin node increment on J axis (J_bin_inc) Bin node increment on J-axis
*/

    const auto XTO = binGridOriginEasting;
    const auto YTO = binGridOriginNorthing;
    const auto XSO = binGridOriginI;
    const auto YSO = binGridOriginJ;
    const auto XS = i;
    const auto YS = j;
    const auto PHI = mapGridBearingJ * pi / 180;
    const auto k = binGridScaleFactor;
    const auto MX = binWidthI;
    const auto MY = binWidthJ;
    const auto IncSX = binNodeIncrementI;
    const auto IncSY = binNodeIncrementJ;

    switch (transformationMethod) {
    case TransformationMethod::RightHanded:
        return Coordinates(
            XTO + ((XS - XSO) * std::cos(PHI) * k * MX / IncSX) + ((YS - YSO) * std::sin(PHI) * k * MY / IncSY),
            YTO - ((XS - XSO) * std::sin(PHI) * k * MX / IncSX) + ((YS - YSO) * std::cos(PHI) * k * MY / IncSY));
    case TransformationMethod::LeftHanded:
        return Coordinates(
            XTO - ((XS - XSO) * std::cos(PHI) * k * MX / IncSX) + ((YS - YSO) * std::sin(PHI) * k * MY / IncSY),
            YTO + ((XS - XSO) * std::sin(PHI) * k * MX / IncSX) + ((YS - YSO) * std::cos(PHI) * k * MY / IncSY));
    default:
        throw std::runtime_error("Unknown or unspecified transformation method");
    }
}

} // namespace segysdk::utilities::osdu
