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

#include "../segysdk/SegyServiceSession.h"

#include <string>
#include <vector>
#include <json/json.h>

namespace segysdk::metadata::osdu
{
class OsduSeismicTraceData;
}

namespace segysdk::api
{

/**
 * @brief SegyServiceSessionImpl is the main entry point to all helpful information on Segy related structures.
 *
 * SegyServiceSessionImpl is used to access different helpful information for SEG-Y files and related structures.
 *
 */
class SEGYSDK_DLL_PUBLIC SegyServiceSessionImpl : public SegyServiceSession
{

public:
    /**
     * @brief Creates SegyServiceSessionImpl for a particular seismicTraceData.
     *
     **/
    SegyServiceSessionImpl(std::string seismicTraceData);

    /**
     * @brief Default destructor for the SegyService.
     *
     **/
    virtual ~SegyServiceSessionImpl() = default;

    /**
     * @brief Get Trace Navigation Data for a created session.
     *
     * @returns vector of TraceNavigationData associated with a service session
     *
     **/
    std::vector<TraceNavigationData> getTraceNavigation() const override;

private:
    segysdk::metadata::osdu::OsduSeismicTraceData getSeismicTraceData() const;

    bool isDimensionalityTypeId2D(const std::string& dimensionalityTypeId) const;

    std::vector<TraceNavigationData> extractTraceNavigation(const Json::Value& traceData, const std::string& osduPartition) const;

private:
    std::string seismicTraceData_;
};

} // namespace segysdk::api
