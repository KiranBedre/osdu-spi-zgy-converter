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

#include "Geometry3D.h"

#include <cmath>

namespace segysdk::segyindex
{

Geometry3D::Geometry3D()
{
    dataGrid.JVectorRotationAngle = 0;
    dataGrid.IVectorUnitDistance = 0;
    dataGrid.IVectorRotationAngle = 0;
    dataGrid.JVectorUnitDistance = 0;

    dataGrid.gridCell_11.i = 0;
    dataGrid.gridCell_11.j = 0;
    dataGrid.gridCell_11.location = {0, 0};

    dataGrid.gridCell_1N.i = 0;
    dataGrid.gridCell_1N.j = 0;
    dataGrid.gridCell_1N.location = {0, 0};

    dataGrid.gridCell_N1.i = 0;
    dataGrid.gridCell_N1.j = 0;
    dataGrid.gridCell_N1.location = {0, 0};

    dataGrid.gridCell_NN.i = 0;
    dataGrid.gridCell_NN.j = 0;
    dataGrid.gridCell_NN.location = {0, 0};
}

bool
Geometry3D::IsDefined() const
{
    static constexpr double tolerance = 0.00000001;

    return JVectorExtent.last() - JVectorExtent.first() != 0 && IVectorExtent.last() - IVectorExtent.first() != 0 && std::abs(sampleExtent.last() - sampleExtent.first()) > tolerance && std::abs(dataGrid.IVectorUnitDistance) > tolerance && std::abs(dataGrid.JVectorUnitDistance) > tolerance;
}

bool
Geometry3D::isRightHanded() const
{
    if (!IsDefined()) return false;

    // Assuming dataGrid.IVectorRotationAngle and dataGrid.JVectorRotationAngle to have values in the interval 0 >= angle < 360.0;
    
    // Inline direction is described by dataGrid.IVectorRotationAngle
    // Crossline direction is described by dataGrid.JVectorRotationAngle
    if (dataGrid.IVectorRotationAngle < 270) {
        // For a right-handed cube, the crossline direction should be 90 degrees larger than the inline direction
        if (dataGrid.JVectorRotationAngle < 270 && dataGrid.IVectorRotationAngle > dataGrid.JVectorRotationAngle) {
            return true;
        }
    }
    else if (dataGrid.IVectorRotationAngle >= 270) {
        // For a right-handed cube, the crossline direction should be 90 degrees larger than the inline direction
        if (dataGrid.JVectorRotationAngle <= 90) {
            return true;
        }
    }

    return false;
}

bool
Geometry3D::operator==(const Geometry3D& other) const
{
    return IVectorExtent == other.IVectorExtent && JVectorExtent == other.JVectorExtent && sampleExtent == other.sampleExtent && dataGrid == other.dataGrid && isInlineOrdered == other.isInlineOrdered && isRegular == other.isRegular;
}

bool
Geometry3D::operator!=(const Geometry3D& other) const
{
    return !(*this == other);
}

bool
Geometry3D::almostEqual(const Geometry3D& other, double angleTolerance, double distanceTolerance) const
{
    return IVectorExtent == other.IVectorExtent && JVectorExtent == other.JVectorExtent && sampleExtent.almostEqual(other.sampleExtent, angleTolerance) && dataGrid.almostEqual(other.dataGrid, angleTolerance, distanceTolerance) && isInlineOrdered == other.isInlineOrdered && isRegular == other.isRegular;
}

} // namespace segysdk::segyindex
