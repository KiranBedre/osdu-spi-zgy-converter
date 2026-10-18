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
 * @brief Geometry3D.h contains the declaration of the Geometry3D struct in the SEG-Y index.
 *
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "DataGrid.h"
#include "../SegyIndex/HorizontalExtent.h"
#include "../SegyIndex/VerticalExtent.h"

namespace segysdk::segyindex
{

/**
   * @class Geometry3D
   *
   * @brief Used to hold horizontal and vertical extents of the volume and also the calculated data grid geometry for a SEG-Y volume.
   *
   **/
class SEGYSDK_DLL_PUBLIC Geometry3D
{

public:
    /**
     * @brief default constructor for the Geometry3D class.
     *
     **/
    Geometry3D();

public:
    /**
     * @brief Overloaded equal operator.
     * @details
     * This method uses strict equality checks on the following fields to determine whether the objects are equal: IVectorExtent, JVectorExtent, sampleExtent, dataGrid, isRegular and isInlineOrdered.
     * There is a relatively high probability that this test might be too strict, and might return false for comparisons that differ only on a miniscule tolerance level.
     * Please use the almostEqual method in these cases as it allows the use of user specified tolerance values to be applied when comparing the floating point fields.
     *
     * @returns bool value true means values for all objects listed above are identical.
     **/
    bool operator==(const Geometry3D& other) const;

    /**
     * @brief Overloaded not equal operator. Returns true unless the two object satisfies the overloaded equality test above.
     *
     **/
    bool operator!=(const Geometry3D& other) const;

    /**
     * @brief Method for comparing Geometry3D using user specified tolerance level.
     *
     * @param other  Geometry3D object to compare against.
     * @param angleTolerance  double value of tolerance to be used in comparison of angle fields.
     * @param distanceTolerance  double value of tolerance to be used in comparison of distance and coordinate fields.
     *
     * @returns bool value true means comparison matched, false means comparison failed using the specified tolerance.
     *
     **/
    bool almostEqual(const Geometry3D& other, double angleTolerance, double distanceTolerance) const;

    /**
     * @brief Test to see if a Geometry3D class has been populated with data or if it is just an empty shell.
     *
     *  @returns bool value true means index has been populated, false means just an empty class.
     *
     **/
    bool IsDefined() const;

    /**
     * @brief Test to verify if the current SEG-Y volume is right-handed or left-handed.
     *
     * @details
     * This returns true if the following conditions are true: (Using normalized angles so every angle is >= 0 and < 360 degrees)
     * - Inline bearing is > 90 degrees and crossline bearing is < inline direction OR
     * - Inline bearing is < 90 degrees and crossline bearing is > 270 degrees0
     **/
    bool isRightHanded() const;

    /**
     * @brief Describes the i range of the J vector. Also known as the Inline Extent.
     *
     **/
    HorizontalExtent JVectorExtent{0, 0, 0};

    /**
     * @brief Describes the j range of the I vector. Also known as the Crossline Extent.
     *
     **/
    HorizontalExtent IVectorExtent{0, 0, 0};

    /**
     * @brief Describes the vertical range of the dataset.
     *
     **/
    VerticalExtent sampleExtent{0, 0, 0};

    /**
     * @brief The calculated data grid for the SEG-Y volume.
     *
     **/
    DataGrid dataGrid;

    /**
     * @brief This method verifies whether the SEG-Y volume is complete or not.
     * 
     * @details
     * Returns true if all inlines in the volume are complete and have traces of equal number of samples.
     * If this is the case, a trace's location in the SEG-Y file can easily be calculated by working out the number of traces into the volume you would have to be to find a desired trace.
     * If this is false, the file offset to reach a certain trace will have to be read from a generated index.
     *
     *  @returns bool value indicating whether a SEG-Y volume is complete, i.e. having all traces and each trace is the same length.
     **/
    bool isRegular = true;

    /**
     * @brief This method determines whether a SEG-Y volume is organized with inlines or crosslines as the primary index.
     *
     *  @returns bool  A value of true signifies that the SEG-Y volume is organized with Inlines as the primary index, a value of false means the volume is organized by Crosslines.
     * 
     **/
    bool isInlineOrdered = true;
};

} // namespace segysdk::segyindex
