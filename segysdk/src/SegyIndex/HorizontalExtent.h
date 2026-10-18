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
 * @brief HorizontalExtent.h contains the declaration of the struct used to specifies the vertical extent of a SEG-Y volume.
 *
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include <memory>

namespace segysdk::segyindex
{

/**
 * @struct HorizontalExtent
 * @brief Trace annotation and trace count for the current Horizontal extent. (inline or crossline extent)
 *
 * @details
 * For Inline extent, the first last and increment values represent the crossline annotations and count for an inline.
 * For Crossline extent, the first last and increment values represent the inline annotations and count for an crossline.
 *
 **/
struct SEGYSDK_DLL_PUBLIC HorizontalExtent
{

    /**
     * @brief Constructor for the HorizontalExtent.
     *
     * @param first  int sets the first annotation value for the extent.
     * @param last   int sets the last annotation value for the extent.
     * @param step   int sets the increment between steps.
     *
     **/
    HorizontalExtent(int first, int last, int step);

    /**
     * @brief Overloaded equal operator.
     * @details
     * This method uses strict equality checks on the following fields to determine whether the objects are equal: first, last, step and count().
     *
     * @returns bool value true means values for first, last, step and count() are identical.
     **/
    bool operator==(const HorizontalExtent& other) const;

    /**
     * @brief Overloaded not equal operator. Returns true unless the two object satisfies the overloaded equality test above.
     *
     **/
    bool operator!=(const HorizontalExtent& other) const;

    /**
     * @brief int  Trace annotation of the first (min) trace in the current horizontal section.
     **/
    int first() const;

    /**
     * @brief int  Trace annotation of the last (max) trace in the current horizontal section.
     **/
    int last() const;

    /**
     * @brief int  interval between traces in the current horizontal extent.
     **/
    int step() const;

    /**
     * @brief int  Total number of traces in the current horizontal extent.
     **/
    int count() const;

    /**
     * @brief int  Total number of trace intervals in the current horizontal extent. This is count - 1.
     **/
    int gaps() const;

private:
    struct Impl;
    std::shared_ptr<Impl> impl_;
};

} // namespace segysdk::segyindex
