/*
Copyright 2024 Schlumberger

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

#include <string>

namespace segysdk::api
{
/**
 * @brief The TraceNavigationData represents data containing main 2D values from Trace Header: CDP, SP, X and Y coordinates.
 *
 **/
struct SEGYSDK_DLL_PUBLIC TraceNavigationData
{

    /**
     * @brief CDP/CMP value of the trace header.
     **/
    const size_t cdp_;

    /**
     * @brief Shot Point value of the trace header.
     **/
    const double sp_;

    /**
     * @brief X coordinate of the point for particular line.
     **/
    const double x_;

    /**
     * @brief Y coordinate of the point for particular line.
     **/
    const double y_;

    /**
     * @brief Constructor.
     *
     * @param cdp  CDP/CMP value of the Trace Header.
     * @param sp   Shot Point of the Trace header. Scaled.
     * @param x    X coordinate of the line point.
     * @param y    Y coordinate of the line point.
     **/
    TraceNavigationData(size_t cdp, double sp, double x, double y);

    TraceNavigationData();

    TraceNavigationData(TraceNavigationData const&) = default;
    TraceNavigationData& operator=(const TraceNavigationData&) = default;
};

} // namespace segysdk::api