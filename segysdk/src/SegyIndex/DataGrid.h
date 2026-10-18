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

/** @file
 * @brief DataGrid.h contains the interface to the calculated data grid in the SEG-Y index.
 *
 */

#pragma once

#include "GridCell.h"

#include <cmath>

namespace segysdk::segyindex
{

/**
  * @struct DataGrid
  *
  * @brief Used to represent the calculated data grid of a SEG-Y 3D data set.
  *
  **/
struct DataGrid
{

    /**
     * @brief Overloaded equal operator.
     * @details
     * This method uses strict equality checks on the following fields to determine whether the objects are equal: gridCell_11, gridCell_1N, gridCell_N1, gridCell_NN 
     * as well as IVectorRotationAngle, IVectorUnitDistance, JVectorRotationAngle and JVectorUnitDistance.
     * There is a relatively high probability that this test might be too strict, and might return false for comparisons that differ only on a miniscule tolerance level.
     * Please use the almostEqual method in these cases as it allows the use of user specified tolerance values to be applied when comparing the floating point fields.
     *
     * @returns bool value true means values for first, step, count, domain and zUoM are identical.
     **/
    bool operator==(const DataGrid& other) const
    {
        return gridCell_11 == other.gridCell_11 && gridCell_1N == other.gridCell_1N && gridCell_N1 == other.gridCell_N1 && gridCell_NN == other.gridCell_NN && IVectorRotationAngle == other.IVectorRotationAngle && IVectorUnitDistance == other.IVectorUnitDistance && JVectorRotationAngle == other.JVectorRotationAngle && JVectorUnitDistance == other.JVectorUnitDistance;
    }

    /**
     * @brief Overloaded not equal operator. Returns true unless the two object satisfies the overloaded equality test above.
     *
     **/
    bool operator!=(const DataGrid& other) const
    {
        return !(*this == other);
    }

    /**
     * @brief Method for comparing DataGrid using user specified tolerance level.
     *
     * @param other  DataGrid object to compare against.
     * @param angleTolerance  double value of tolerance to be used in comparison of rotation angles.
     * @param distanceTolerance  double value of tolerance to be used in comparison of coordinate and distance values.
     *
     * @returns bool value true means comparison matched, false means comparison failed using the specified tolerances.
     *
     **/
    bool
    almostEqual(const DataGrid& other, const double angleTolerance, const double distanceTolerance) const
    {

        return gridCell_11.almostEqual(other.gridCell_11, distanceTolerance) && gridCell_1N.almostEqual(other.gridCell_1N, distanceTolerance) && gridCell_N1.almostEqual(other.gridCell_N1, distanceTolerance) && gridCell_NN.almostEqual(other.gridCell_NN, distanceTolerance) && std::abs(IVectorRotationAngle - other.IVectorRotationAngle) < angleTolerance && std::abs(IVectorUnitDistance - other.IVectorUnitDistance) < distanceTolerance && std::abs(JVectorRotationAngle - other.JVectorRotationAngle) < angleTolerance && std::abs(JVectorUnitDistance - other.JVectorUnitDistance) < distanceTolerance;
    }

    /**
     * @brief Start of first primary index Inline/Crossline and Coordinate.
     *
     * @details Struct holding the Inline/Crossline and the Coordinate for the corner point at the start of the first primary index (usually this is the origin point (trace at the beginning of the first inline).
     **/
    GridCell gridCell_11;

    /**
     * @brief End of first primary index Inline/Crossline and Coordinate.
     * 
     * @details Struct holding the Inline/Crossline and the Coordinate for the corner point at the end of the first primary index (usually this is the trace at the end of the first inline).
     *
     **/
    GridCell gridCell_1N;

    /**
     * @brief Start of last primary index Inline/Crossline and Coordinate.
     * 
     * @details Struct holding the Inline/Crossline and the Coordinate for the corner point at the beginning of the last primary index (usually this is the trace at the beginning of the last inline).
     *
     **/
    GridCell gridCell_N1;

    /**
     * @brief End of last primary index Inline/Crossline and Coordinate.
     * 
     * @details Struct holding the Inline/Crossline and the Coordinate for the corner point at the end of the last primary index (usually this is the trace at the end of the last inline).
     *
     **/
    GridCell gridCell_NN;

    /**
     * @brief IVectorRotationAngle: Inline rotation angle is the plotting angle of a posted inline, i.e. from 1:1 to 1 : N.
     * 
     * @details
     * - InlineRotationAngle = > IVectorRotationAngle as this is where the angle is calculated. (angle between two cells with same i and different j)
     * - Angle between two cells with same i and different j.
     **/
    double IVectorRotationAngle;

    /**
     * @brief JVectorRotationAngle: Crossline rotation angle is the plotting angle of a posted crossline, i.e. from 1:1 to N : 1.
     *
     * @details
     * - CrosslineRotationAngle = > JVectorRotationAngle as this is where the angle is calculated. (angle between two cells with same j and different i)
     * - Angle between two cells with same j and different i.
     **/
    double JVectorRotationAngle;

    /**
     * @brief InlineUnitDistance = > JVectorUnitDistance as this is where the value gets calculated. (distance between two cells with same i)
     * 
     **/
    double JVectorUnitDistance; //Unit distance between two cells with same i but different j.

    /**
     * @brief CrosslineUnitDistance = > IVectorUnitDistance as this is where the value gets calculated. (distance between two cells with same j)
     * 
     **/
    double IVectorUnitDistance; // Unit distance between two cells with same j but different I.
};

} // namespace segysdk::segyindex
