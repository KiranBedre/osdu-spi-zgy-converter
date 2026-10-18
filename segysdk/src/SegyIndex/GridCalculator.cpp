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

#include "GridCalculator.h"

#include <cmath>
#include <limits>

namespace segysdk::segyindex
{

GridCalculator::GridCalculator(const Geometry3D& geometry)
  : iVector_(Vector::GenerateIVector(geometry.dataGrid.gridCell_11, geometry.dataGrid.IVectorRotationAngle, geometry.dataGrid.IVectorUnitDistance, std::abs(geometry.dataGrid.gridCell_1N.j - geometry.dataGrid.gridCell_11.j))),
    jVector_(Vector::GenerateJVector(geometry.dataGrid.gridCell_11, geometry.dataGrid.JVectorRotationAngle, geometry.dataGrid.JVectorUnitDistance, std::abs(geometry.dataGrid.gridCell_N1.i - geometry.dataGrid.gridCell_11.i))),
    iExtent_({geometry.dataGrid.gridCell_11.i, geometry.dataGrid.gridCell_N1.i, 1}),
    jExtent_({geometry.dataGrid.gridCell_11.j, geometry.dataGrid.gridCell_1N.j, 1})
{
}

GridCalculator::GridCalculator(const Vector& iVector, const Vector& jVector)
  : iVector_(iVector),
    jVector_(jVector),
    iExtent_({jVector.getGridCell1().i, jVector.getGridCell2().i, 1}),
    jExtent_({iVector.getGridCell1().j, iVector.getGridCell2().j, 1})
{
}

Coordinate
GridCalculator::GetXYLocation(int i, int j) const
{
    if (iExtent_.step() == 0 || jExtent_.step() == 0)
        return Coordinate(std::numeric_limits<double>::max(), std::numeric_limits<double>::max());

    // As we are using iVector.cell1 as reference point, useiVector only for calculating grid offset
    // to mitigate situation where i and j vectors do not have same first cell!
    const GridCell cell = iVector_.getGridCell1();
    const int inlineGaps = (i - cell.i);
    const int crosslineGaps = (j - cell.j);

    // iVector describes inline sections, having  arange of traces
    // jVector describes crossline sections, having a range of inlines
    const double x = cell.location.X + (jVector_.getDX() * inlineGaps) + (iVector_.getDX() * crosslineGaps);
    const double y = cell.location.Y + (jVector_.getDY() * inlineGaps) + (iVector_.getDY() * crosslineGaps);

    return Coordinate(x, y);
}

/*  Various possible scenarios here:
        *  1) Inline positive X, Crossline positive Y
        *  2) Inline positive X, Crossline negative Y
        *  3) Inline negative X, Crossline positive Y
        *  4) Inline negative X, Crossline negative Y
        *  5) Inline positive Y, Crossline positive X
        *  6) Inline positive Y, Crossline negative X
        *  7) Inline negative Y, Crossline positive X
        *  8) Inline negative Y, Crossline negative Y
      */

GridCell
GridCalculator::GetLocation(Coordinate coordinate) const
{
    const double dX = coordinate.X - iVector_.getGridCell1().location.X;
    const double dY = coordinate.Y - iVector_.getGridCell1().location.Y;

    const double hypotenuse = sqrt(dX * dX + dY * dY);
    const long double radians = PI_half - std::atan2(dY, dX);
    const long double deltaRadians = iVector_.getRadians() - radians;
    long double near = std::sin(deltaRadians) * hypotenuse;
    long double far = std::cos(deltaRadians) * hypotenuse;

    // When InlineAlignedX : (InlinePositiveXCrossline*Y orInlineNegativeXCrossline*Y )
    // Inline increment is linked to sin(angle) and crossline increment is cos(angle)
    // Otherwise inline increment is linked to cos(angle) and crossline increment is sin(angle)
    // UnitDistance is positive, and ensure near and far will always be positive at the start!

    far = std::abs(far);
    near = std::abs(near);

    // iVector has crossline traces, jVector has inline traces!
    const long double j = isDirectionPositive(iVector_.getRadians(), radians) ? far / iVector_.getUnitDistance() : -far / iVector_.getUnitDistance();
    const long double i = isDirectionPositive(jVector_.getRadians(), radians) ? near / jVector_.getUnitDistance() : -near / jVector_.getUnitDistance();

    // This is ablolutely intentional
    const int inlineNo = iVector_.getGridCell1().i + static_cast<int>(round(i));
    const int crosslineNo = iVector_.getGridCell1().j + static_cast<int>(round(j));

    const GridCell cell = {inlineNo, crosslineNo, coordinate};

    return cell;
}

long double
GridCalculator::cleanRadians(long double radian)
{

    while (radian < 0)
        radian += Two_PI;
    while (radian >= Two_PI)
        radian -= Two_PI;

    return radian;
}

bool
GridCalculator::isDirectionPositive(long double axisRadians, long double deltaRadians) const
{
    axisRadians = cleanRadians(axisRadians);
    deltaRadians = cleanRadians(deltaRadians);

    if (axisRadians >= PI_half && axisRadians < Three_PI_half) {
        return std::abs(axisRadians - deltaRadians) <= PI_half;
    }
    else if (axisRadians < PI_half) {
        const long double min = Three_PI_half + axisRadians;
        const long double max = axisRadians + PI_half;
        if (deltaRadians <= max)
            return true;
        if (deltaRadians >= min)
            return true;

        return false;
    }
    else if (axisRadians >= Three_PI_half) {
        const long double min = axisRadians - PI_half;
        const long double max = -Three_PI_half + axisRadians;
        if (deltaRadians >= min)
            return true;
        if (deltaRadians <= max)
            return true;
        return false;
    }
    return false;
}

long double
GridCalculator::getGridTraceIncrement(long double increment, long double axisRadians, long double radians) const
{
    return isDeltaPositiveDirection(axisRadians, radians) ? std::abs(increment) : -std::abs(increment);
}

bool
GridCalculator::isDeltaPositiveDirection(long double axisRadians, long double offsetRadians)
{
    axisRadians = cleanRadians(axisRadians);
    offsetRadians = cleanRadians(offsetRadians);

    // Radians will ALWAYS be between 0 and 2 * PI
    if (axisRadians >= PI_half && axisRadians <= 3 * PI_half) {
        // Angles can be directly compared
        const MinMaxRadians radiansMinMax(axisRadians - PI_half, axisRadians + PI_half);
        if (offsetRadians <= radiansMinMax.max() && offsetRadians >= radiansMinMax.min())
            return true;

        return false;
    }
    else if (axisRadians < PI_half) {
        // Angle verflow on low end, so check high angles as well

        const MinMaxRadians radiansMinMax(Three_PI_half + axisRadians, axisRadians + PI_half);
        if (offsetRadians <= radiansMinMax.max())
            return true;
        if (offsetRadians >= radiansMinMax.min())
            return true;

        return false;
    }
    else if (axisRadians > Three_PI_half) {
        // Angle overflow on top end, so check low angles as well

        const MinMaxRadians radiansMinMax(axisRadians - PI_half, -Three_PI_half - axisRadians);

        if (offsetRadians >= radiansMinMax.min())
            return true;
        if (offsetRadians <= radiansMinMax.max())
            return true;

        return false;
    }

    return false;
}

long double
MinMaxRadians::cleanRadiansMax(long double radian)
{
    // Max will go from but not including 0 and up to and including 2 * PI
    while (radian <= 0)
        radian += GridCalculator::Two_PI;
    while (radian > GridCalculator::Two_PI)
        radian -= GridCalculator::Two_PI;

    return radian;
}

MinMaxRadians::MinMaxRadians(long double min, long double max)
  : minRadian_(cleanRadiansMin(min)),
    maxRadian_(cleanRadiansMax(max))
{
}

long double
MinMaxRadians::cleanRadiansMin(long double radian)
{
    // Min will go from >= 0 to but not including 2 * PI
    while (radian < 0)
        radian += GridCalculator::Two_PI;
    while (radian >= GridCalculator::Two_PI)
        radian -= GridCalculator::Two_PI;

    return radian;
}

} // namespace segysdk::segyindex
