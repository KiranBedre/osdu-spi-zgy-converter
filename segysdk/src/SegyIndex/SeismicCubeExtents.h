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

#include "../SegyIndex/HorizontalExtent.h"
#include "../SegyIndex/VerticalExtent.h"

#include <memory>

namespace segysdk::segyindex
{

/**
 * @struct SeismicCubeExtents
 * @brief Specifies the horizontal and vertical extents of a SEG-Y volume.
 **/
struct SEGYSDK_DLL_PUBLIC SeismicCubeExtents
{

    /**
     * @brief Constructor for the SeismicCubeExtents.
     *
     * @param iVectorExtent  HorizontalExtent object defining the IVector extent.
     * @param jVectorExtent  HorizontalExtent object defining the JVector extent.
     * @param sampleExtent   VerticalExtent object defining the sample extent.
     *
     **/
    SeismicCubeExtents(HorizontalExtent iVectorExtent, HorizontalExtent jVectorExtent, VerticalExtent sampleExtent);

     /**
     * @brief Gets the IVector extent.
     *
     * @returns HorizontalExtent struct specifying the IVector.
     *
     **/
    HorizontalExtent getIVectorExtent() const;

    /**
     * @brief Gets the JVector extent.
     *
     * @returns HorizontalExtent struct specifying the JVector.
     *
     **/
    HorizontalExtent getJVectorExtent() const;

    /**
     * @brief Gets the IVector extent.
     *
     * @returns VerticalExtent struct specifying the SampleExtent.
     *
     **/
    VerticalExtent getSampleExtent() const;

private:
    struct Impl;
    std::shared_ptr<Impl> impl_;
};

} // namespace segysdk::segyindex
