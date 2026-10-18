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
 * @brief GridCell.h contains the declaration of the GridCell struct used by the SegyIndex.
 *
 */

#pragma once

#include "Coordinate.h"

namespace segysdk::segyindex
{

/**
 * @struct GridCell
 *
 * @brief Used to represent the intersection of an inline and crossline and the coordinate of this intersection
 *
 **/
struct GridCell
{

    /**
     * @brief Overloaded equal operator.
     * @details
     * This method uses strict equality checks on the following fields to determine whether the objects are equal: i and j  as well as the location coordinate field.
     * There is a relatively high probability that this test might be too strict, and might return false for comparisons that differ only on a miniscule tolerance level.
     * Please use the almostEqual method in these cases as it allows the use of a user specified tolerance to be applied for the floating point fields.
     *
     * @returns bool value true means values for i, j, and location are identical.
     **/
    bool operator==(const GridCell& other) const
    {
        return i == other.i && j == other.j && location == other.location;
    }

    /**
     * @brief Overloaded not equal operator. Returns true unless the two object satisfies the overloaded equality test above.
     *
     **/
    bool operator!=(const GridCell& other) const
    {
        return !(*this == other);
    }

    /**
     * @brief Method for comparing GridCell using user specified tolerance level.
     *
     * @param other  GridCell object to compare against.
     * @param tolerance  double value of tolerance to be used in comparison.
     *
     * @returns bool value true means comparison matched, false means comparison failed using the specified tolerance.
     *
     **/
    bool
    almostEqual(const GridCell& other, const double tolerance) const
    {

        return i == other.i && j == other.j && location.almostEqual(other.location, tolerance);
    }

    /**
     * @brief primary index of the intersection - Usually the inline number.
     *
     **/
    int i;

    /**
     * @brief secondary index of the intersection - Usually the crossline number.
     *
     **/
    int j;

    /**
     * @brief The X, Y coordinate of the inline and crossline. (i, j)
     *
     **/
    Coordinate location;
};

} // namespace segysdk::segyindex
