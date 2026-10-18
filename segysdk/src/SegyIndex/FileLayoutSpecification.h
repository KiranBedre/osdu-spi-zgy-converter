/*
Copyright 2022 Schlumberger

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
 * @brief FileLayoutSpecification.h contains the declaration of the file layout specification (FileLayoutSpecification) struct which describes how the traces are organized in the SEG-Y file.
 *        This can be thought of as a local coordinate system whereas the Geometry3D contains the Cartesian coordinate system.
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../SegyIndex/HorizontalExtent.h"
#include "../SegyIndex/VerticalExtent.h"

namespace segysdk::segyindex
{

/**
   * @class FileLayoutSpecification
   *
   * @brief The FileLayoutSpecification specifies the progression of inline and crossline values in the file.
   *        A SEG-Y file will usually be sorted first by inline number, then by crossline number.
   *        The sorting order can be either ascending or descending.
   *        Very occasionally a SEG-Y file might be sorted first by Crossline value then by Inline value.
   *
   **/
struct SEGYSDK_DLL_PUBLIC FileLayoutSpecification
{
    /**
     * @brief default constructor for the FileLayoutSpecification class.
     *
     **/
    FileLayoutSpecification();

    /**
     * @brief default destructor for the FileLayoutSpecification class.
     *
     **/
    ~FileLayoutSpecification() = default;


    /**
     * @brief copy constructor for the FileLayoutSpecification class.
     *
     **/
    FileLayoutSpecification(FileLayoutSpecification const& other);

    /**
     * @brief copy assignment operator for the FileLayoutSpecification class.
     *
     **/
    FileLayoutSpecification& operator=(FileLayoutSpecification const& other);

    /**
     * @brief move constructor for the FileLayoutSpecification class.
     *
     **/
    FileLayoutSpecification(FileLayoutSpecification&& other);

    /**
     * @brief move assignment operator for the FileLayoutSpecification class.
     *
     **/
    FileLayoutSpecification& operator=(FileLayoutSpecification&& other);

    /**
     * @brief Overloaded equal operator.
     * @details
     * This method uses strict equality checks on the following fields to determine whether the objects are equal: IVectorExtent, JVectorExtent, sampleExtent, dataGrid, isRegular and isInlineOrdered.
     * There is a relatively high probability that this test might be too strict, and might return false for comparisons that differ only on a miniscule tolerance level.
     * Please use the almostEqual method in these cases as it allows the use of user specified tolerance values to be applied when comparing the floating point fields.
     *
     * @returns bool value true means values for all objects listed above are identical.
     **/
    bool operator==(const FileLayoutSpecification& other) const;

    /**
     * @brief Overloaded not equal operator. Returns true unless the two object satisfies the overloaded equality test above.
     *
     **/
    bool operator!=(const FileLayoutSpecification& other) const;

    /**
     * @brief Method for comparing FileLayoutSpecification using user specified tolerance level.
     *
     * @param other  FileLayoutSpecification object to compare against.
     * @param angleTolerance  double value of tolerance to be used in comparison of angle fields.
     *
     * @returns bool value true means comparison matched, false means comparison failed using the specified tolerance.
     *
     **/
    bool almostEqual(const FileLayoutSpecification& other, double angleTolerance) const;

    /**
     * @brief Test to see if a FileLayoutSpecification class has been populated with data or if it is just an empty shell.
     *
     *  @returns bool value true means index has been populated, false means just an empty class.
     *
     **/
    bool IsDefined() const;

    /**
     * @brief Describes the i range of the J vector. Also known as the Inline Extent.
     *
     **/
    HorizontalExtent JExtent{0, 0, 0};

    /**
     * @brief Describes the j range of the I vector. Also known as the Crossline Extent.
     *
     **/
    HorizontalExtent IExtent{0, 0, 0};

    /**
     * @brief Describes the vertical range of the dataset.
     *
     **/
    VerticalExtent sampleExtent{0, 0, 0};

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
     **/
    bool isInlineOrdered = true;

private:
    static constexpr double const tolerance_ = 0.00000001;
};

} // namespace segysdk::segyindex
