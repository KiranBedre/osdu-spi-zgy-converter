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

#include "Vector.h"

#include "../SegyIO/SegyIOException.h"
#include <cmath>
#include <stdexcept>

namespace segysdk::segyindex
{

Vector::Vector(GridCell c1, GridCell c2, VectorDirection direction)
  : cell1_(c1),
    cell2_(c2),
    direction_(direction),
    degrees_(calculateDegrees(c1, c2)),
    radians_(calculateRadians(c1, c2)),
    dx_(calculateDX(c1, c2)),
    dy_(calculateDY(c1, c2))
{
}

Vector::Vector(GridCell c1, double degrees, double unitDistance, VectorDirection direction, int traceGaps)
  : cell1_(c1),
    cell2_(calculateMissingGridCell(c1, degrees, unitDistance, direction, traceGaps)),
    direction_(direction),
    degrees_(degrees),
    radians_(radiansFromDegrees(degrees)),
    dx_(calculateDX(unitDistance)),
    dy_(calculateDY(unitDistance))
{
}

Vector
Vector::GenerateIVector(GridCell c1, GridCell c2)
{
    const VectorDirection direction = VectorDirection::I;
    if (c1.i != c2.i)
        throw SegyInvalidArgumentException("Vector::GenerateIVector(c1, c2) : Inline Vector must fall on a single inline.");

    return Vector(c1, c2, direction);
}

Vector
Vector::GenerateIVector(GridCell c1, double degrees, double unitDistance, int traceGaps)
{
    const VectorDirection direction = VectorDirection::I;

    if (traceGaps < 1)
        throw SegyInvalidArgumentException("Vector::GenerateIVector(c1, degrees, unitDistance) : Inline Vector must have more than a single point.");

    if (unitDistance < 1)
        throw SegyInvalidArgumentException("Vector::GenerateIVector(c1, degrees, unitDistance) : Unit distance cannot be zero.");

    return Vector(c1, degrees, unitDistance, direction, traceGaps);
}

Vector
Vector::GenerateJVector(GridCell c1, GridCell c2)
{
    const VectorDirection direction = VectorDirection::J;
    if (c1.j != c2.j)
        throw SegyInvalidArgumentException("Vector::GenerateJVector(c1, c2) : Crossline Vector must fall on a single crossline.");

    return Vector(c1, c2, direction);
}

Vector
Vector::GenerateJVector(GridCell c1, double degrees, double unitDistance, int traceGaps)
{
    const VectorDirection direction = VectorDirection::J;

    if (traceGaps < 1)
        throw SegyInvalidArgumentException("Vector::GenerateJVector(c1, degrees, unitDistance) : Crossline Vector must have more than a single point.");

    if (unitDistance < 1)
        throw SegyInvalidArgumentException("Vector::GenerateJVector(c1, degrees, unitDistance) : Unit distance cannot be zero.");

    return Vector(c1, degrees, unitDistance, direction, traceGaps);
}

double
Vector::calculateRadians(GridCell c1, GridCell c2) const
{
    return calculateRadians(c1.location, c2.location);
}

double
Vector::calculateRadians(Coordinate c1, Coordinate c2) const
{
    const long double PI_half = 3.1415926535897932384626433832795L / 2;
    // We use Grid north angles
    const double radians = static_cast<double>(PI_half) - calculateStandarDegrees(c1, c2);

    return cleanRadians(radians);
}

double
Vector::calculateStandarDegrees(GridCell c1, GridCell c2) const
{
    return calculateStandarDegrees(c1.location, c2.location);
}

double
Vector::calculateStandarDegrees(Coordinate c1, Coordinate c2) const
{
    const double dx = c2.X - c1.X;
    const double dy = c2.Y - c1.Y;
    const double result = atan2(dy, dx);

    return result;
}

double
Vector::calculateDegrees(GridCell c1, GridCell c2) const
{
    return calculateDegrees(c1.location, c2.location);
}

double
Vector::calculateDegrees(Coordinate c1, Coordinate c2) const
{
    const long double FromRadians = 180 / 3.1415926535897932384626433832795L;
    const double degrees = calculateRadians(c1, c2) * static_cast<double>(FromRadians);

    return degrees;
}

double
Vector::getDistance() const
{
    const double dx = cell2_.location.X - cell1_.location.X;
    const double dy = cell2_.location.Y - cell1_.location.Y;
    const double distance = sqrt(dx * dx + dy * dy);

    return distance;
}

int
Vector::getTraceGaps(GridCell c1, GridCell c2) const
{
    const int gaps = direction_ == VectorDirection::I ? std::abs(c2.j - c1.j) : std::abs(c2.i - c1.i);
    if (gaps > 0)
        return gaps;

    throw SegyInvalidArgumentException("Vector::getTraceGaps(c1, c2) : Vector must have more than a single point.");
}

int
Vector::getTraceGaps()
{
    const HorizontalExtent extent = getHorizontalExtent();

    return std::abs(extent.last() - extent.first());
}

int
Vector::getTraceCount()
{
    return getTraceGaps() + 1;
}

double
Vector::getUnitDistance() const
{
    return sqrt(dx_ * dx_ + dy_ * dy_);
}

double
Vector::getUnitDistance(int start, int stop, int step) const
{
    const int traceCount = stop - start;

    if (traceCount != 0 && step != 0)
        return getDistance() / traceCount * step;

    return 0;
}

HorizontalExtent
Vector::getHorizontalExtent() const
{
    HorizontalExtent extent{
        direction_ == VectorDirection::I ? cell1_.j : cell1_.i,
        direction_ == VectorDirection::I ? cell2_.j : cell2_.i,
        1};
    if (extent.first() == extent.last())
        throw SegyInvalidArgumentException("Vector::getTraceGaps() : Vector must have more than a single point.");

    return extent;
}

GridCell
Vector::calculateMissingGridCell(GridCell c1, double angle, double unitDistance, VectorDirection direction, int traceGaps) const
{
    const double dx = calculateDX(angle, unitDistance);
    const double dy = calculateDY(angle, unitDistance);

    const double x = c1.location.X + dx * traceGaps;
    const double y = c1.location.Y + dy * traceGaps;

    GridCell cell;

    if (direction == VectorDirection::I) {
        cell.i = c1.i;
        cell.j = c1.j + traceGaps;
    }
    else {
        cell.i = c1.i + traceGaps;
        cell.j = c1.j;
    }

    cell.location = Coordinate(x, y);

    return cell;
}

double
Vector::cleanRadians(double radian)
{
    const long double Two_PI = 3.1415926535897932384626433832795L * 2;

    while (radian < 0)
        radian += static_cast<double>(Two_PI);
    while (radian > Two_PI)
        radian -= static_cast<double>(Two_PI);

    return radian;
}

double
Vector::cleanDegrees(double degrees)
{
    while (degrees < 0)
        degrees += 360;
    while (degrees >= 360)
        degrees -= 360;

    return degrees;
}

double
Vector::radiansFromDegrees(double degrees)
{
    const long double ToRadians = 3.1415926535897932384626433832795L / 180;
    return degrees * static_cast<double>(ToRadians);
}

double
Vector::calculateDX(GridCell c1, GridCell c2) const
{
    const int gaps = getTraceGaps(c1, c2);
    return calculateDelta(c1.location.X, c2.location.X, gaps);
}

double
Vector::calculateDX(double angle, double unitDistance) const
{
    const double radians = radiansFromDegrees(angle);
    const double result = sin(radians) * unitDistance;
    return result;
}

double
Vector::calculateDX(double unitDistance) const
{
    return unitDistance * sin(radians_);
}

double
Vector::calculateDY(GridCell c1, GridCell c2) const
{
    const int gaps = getTraceGaps(c1, c2);
    return calculateDelta(c1.location.Y, c2.location.Y, gaps);
}

double
Vector::calculateDY(double angle, double unitDistance) const
{
    const double radians = radiansFromDegrees(angle);
    const double result = cos(radians) * unitDistance;
    return result;
}

double
Vector::calculateDY(double unitDistance) const
{
    return unitDistance * cos(radians_);
}

double
Vector::calculateDelta(double x1, double x2, int gaps)
{
    const double result = (x2 - x1) / gaps;
    return result;
}

} // namespace segysdk::segyindex
