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
 * @brief Coordinate.h contains the interface to coordinate values mainly used by the data grid calculated calculated as part of the indexing process.
 *
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include <cmath>
#include <limits>

namespace segysdk::segyindex
{

// TODO has implementation in SegyIndex/Coordinate.cpp
// TODO consider immutable

/**
* @struct Coordinate
*
* @brief Used to represent a projected coordinate.
*
**/
struct SEGYSDK_DLL_PUBLIC Coordinate
{

    /**
* @brief Overloaded equal operator.
* @details
* This method uses strict equality checks on the following fields to determine whether the objects are equal: X and Y.
* There is a relatively high probability that this test might be too strict, and might return false for comparisons that differ only on a miniscule tolerance level.
* Please use the almostEqual method in these cases as it allows the use of a user specified tolerance to be applied for the floating point fields.
*
* @returns bool value true means values for i, j, and location are identical.
**/
    bool operator==(const Coordinate& other) const
    {
        return X == other.X && Y == other.Y;
    }

    /**
  * @brief Overloaded not equal operator. Returns true unless the two object satisfies the overloaded equality test above.
  *
  **/
    bool operator!=(const Coordinate& other) const
    {
        return !(*this == other);
    }

    /**
   * @brief Method for comparing Coordinate using user specified tolerance level when comparing fields X and Y.
   *
   * @param other  Coordinate object to compare against.
   * @param tolerance  double value of tolerance to be used in comparison.
   *
   * @returns bool value true means comparison matched, false means comparison failed using the specified tolerance.
   *
   **/
    bool
    almostEqual(const Coordinate& other, const double tolerance) const
    {

        return std::abs(X - other.X) < tolerance && std::abs(Y - other.Y) < tolerance;
    }

    /**
  * @brief X coordinate.
  **/
    double X = std::numeric_limits<double>::min();

    /**
  * @brief Y coordinate.
  **/
    double Y = std::numeric_limits<double>::min();

    /**
  * @brief Convenience method. Returns true if both X and Y components have values that lie between double::min() and double::max() values.
  * Note that this returns true for a coordinate of (0, 0) or potentially coordinates with X and Y values that fall way off the range limited by a chosen Coordinate Reference System, it is purely an indication of whether or not the coordinate has been created or updated with legal X and Y values.
  * 
  **/
    bool IsValid() const;

    /** 
  * @brief Default constructor, creates an empty struct.
  **/
    Coordinate();
    /**
  * @brief constructor, creates a populated coordinate struct object.
  * 
  * @param x    double value of the X component of the coordinate (also known as Easting) A negative value normally indicates a location west of local origin or west of the local coordinate system's false easting.
  * @param y    double value of the Y component of the coordinate (also known as Northing) A negative value indicates a location south of the Equator.
  **/
    Coordinate(double x, double y);
};

} // namespace segysdk::segyindex
