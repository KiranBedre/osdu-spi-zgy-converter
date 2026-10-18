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

#include "GridCell.h"

#include "../SegyIndex/HorizontalExtent.h"

namespace segysdk::segyindex
{

enum class SEGYSDK_DLL_PUBLIC VectorDirection
{
    I = 0,
    J = 1
};

class SEGYSDK_DLL_PUBLIC Vector
{

private:
    GridCell cell1_;
    GridCell cell2_;
    VectorDirection direction_;

    // degrees are in Grid North
    double degrees_;
    double radians_;

    // dx_ and dy_ are unit deltas in grid x and y direction
    double dx_;
    double dy_;

private: // Make constructors private to enforce use of static factory methods  to enforce consistent use of Inline/Crossline vectors
    Vector(GridCell c1, GridCell c2, VectorDirection direction);
    Vector(GridCell c1, double degrees, double unitDistance, VectorDirection direction, int traceGaps);

private:
    static double cleanRadians(double radian);
    static double cleanDegrees(double degrees);
    static double radiansFromDegrees(double degrees);

    GridCell calculateMissingGridCell(GridCell origin, double angle, double unitDistance, VectorDirection direction, int traceGaps) const;

    double calculateDX(GridCell c1, GridCell c2) const;
    double calculateDX(double angle, double unitDistance) const;
    double calculateDX(double unitDistance) const;

    double calculateDY(GridCell c1, GridCell c2) const;
    double calculateDY(double angle, double unitDistance) const;
    double calculateDY(double unitDistance) const;
    static double calculateDelta(double x1, double x2, int gaps);

    double calculateDegrees(GridCell c1, GridCell c2) const;
    double calculateDegrees(Coordinate c1, Coordinate c2) const;

    double calculateRadians(GridCell c1, GridCell c2) const;
    double calculateRadians(Coordinate origin, Coordinate c2) const;

    double calculateStandarDegrees(GridCell c1, GridCell c2) const;
    double calculateStandarDegrees(Coordinate c1, Coordinate c2) const;

    double getUnitDistance(int start, int stop, int step) const;

    int getTraceGaps(GridCell c1, GridCell c2) const;

public:
    // i vector is vector describing a section of posted is for the same j
    static Vector GenerateIVector(GridCell c1, GridCell c2);
    static Vector GenerateIVector(GridCell c1, double degrees, double unitDistance, int traceGaps);

    // j vector is vector describing a section of posted js for the same i
    static Vector GenerateJVector(GridCell c1, GridCell c2);
    static Vector GenerateJVector(GridCell c1, double degrees, double unitDistance, int traceGaps);

public:
    double getDegrees() const { return degrees_; }
    double getRadians() const { return radians_; }

    GridCell getGridCell1() const { return cell1_; }
    GridCell getGridCell2() const { return cell2_; }

    double getDX() const { return dx_; }
    double getDY() const { return dy_; }

    VectorDirection getDirection() const { return direction_; }

    HorizontalExtent getHorizontalExtent() const;

    double getUnitDistance() const;

    double getDistance() const;

    int getTraceGaps();
    int getTraceCount();
};

} // namespace segysdk::segyindex
