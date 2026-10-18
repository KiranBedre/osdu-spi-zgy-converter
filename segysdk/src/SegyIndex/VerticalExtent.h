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
 * @brief VerticalExtent.h declares the VerticalExtent struct, which is used to specifies the vertical extent of a SEG-Y volume.
 *
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../SegyIO/VerticalDomain.h"
#include "../SegyIO/VerticalUOM.h"

#include <memory>

namespace segysdk::segyindex
{

/**
 * @struct VerticalExtent
 * @brief Specifies the vertical extent of a SEG-Y volume.
 **/
struct SEGYSDK_DLL_PUBLIC VerticalExtent
{

    /**
     * @brief Constructor for the VerticalExtent. Domain=Time and zUoM=seconds using this method
     *
     * @param first  double sets the first annotation value for the extent.
     * @param last   double sets the last annotation value for the extent.
     * @param step   double sets the increment between steps.
     *
     *
     **/
    VerticalExtent(double first, double last, double step);

    /**
     * @brief Constructor for the HorizontalExtent.
     *
     * @param first  double sets the first annotation value for the extent.
     * @param last   double sets the last annotation value for the extent.
     * @param step   double sets the increment between steps.
     * @param domain segyio::VerticalDomain  defines current domain for teh extent (VerticalDomain::Time or VerticalDomain::Depth).
     * @param zUoM   segyio::VerticalUOM sets the UOM for the extent. (VerticalUOM::meter, VerticalUOM::feet, VerticalUOM::seconds)
     *
     **/
    VerticalExtent(double first, double last, double step, segyio::VerticalDomain domain, segyio::VerticalUOM zUoM);

    /**
     * @brief Overloaded equal operator.
     * @details
     * This method uses strict equality checks on the following fields to determine whether the objects are equal: first, step and count() as well as domain and zUoM.
     * There is a relatively high probability that this test might be too strict, and might return false for comparisons that differ only on a miniscule tolerance level.
     * Please use the almostEqual method in these cases as it allows the use of a user specified tolerance to be applied for the floating point fields.
     *
     * @returns bool value true means values for first, step, count(), domain and zUoM are identical.
     **/
    bool operator==(const VerticalExtent& other) const;

    /**
     * @brief Overloaded not equal operator. Returns true unless the two object satisfies the overloaded equality test above.
     *
     **/
    bool operator!=(const VerticalExtent& other) const;

    /**
     * @brief Method for comparing VerticalExtent using user specified tolerance level.
     *
     * @param other  VerticalExtent object to compare against.
     * @param tolerance  double value of tolerance to be used in comparison.
     *
     * @returns bool value true means comparison matched, false means comparison failed using the specified tolerance.
     *
     **/
    bool
    almostEqual(const VerticalExtent& other, const double tolerance) const;

    /**
     * @brief double Time/depth of the first (shallowest) sample in the trace.
     **/
    double first() const;

    /**
     * @brief double Time/depth of the last (deepest) sample in the trace.
     **/
    double last() const;

    /**
    * @brief double Time or depth increment between each sample.
    **/
    double step() const;

    /**
     * @brief VerticalDomain Indicates whether the volume represents data in Time or Depth domain.
     **/
    segyio::VerticalDomain domain() const;

    /**
     * @brief VerticalUOM Unit of measurement for sample interval.
     **/
    segyio::VerticalUOM zUoM() const;

    /**
     * @brief Method to see total number of samples in a full trace.
     **/
    int count() const;

private:
    struct Impl;
    std::shared_ptr<Impl> impl_;
};

} // namespace segysdk::segyindex
