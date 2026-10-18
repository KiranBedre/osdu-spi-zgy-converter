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

#include "../segysdk/TraceNavigationData.h"

#include <memory>
#include <vector>

namespace segysdk::api
{

/**
 * @brief SegyServiceSession is the main entry point to all helpful information on Segy related structures.
 *
 * SegyServiceSession is used to access different helpful information related to SEG-Y files and related structures.
 *
 */
class SEGYSDK_DLL_PUBLIC SegyServiceSession
{

public:
    /**
     * @brief Default destructor for the SegyService.
     *
     **/
    virtual ~SegyServiceSession() = default;

    /**
     * @brief Get Trace Navigation Data for a created session.
     *
     * @returns vector of TraceNavigationData associated with a service session
     *
     **/
    virtual std::vector<TraceNavigationData> getTraceNavigation() const = 0;
};

} // namespace segysdk::api
