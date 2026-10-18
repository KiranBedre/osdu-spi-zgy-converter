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

#include <memory>
#include <string>

namespace segysdk::api
{

/**
 * @class SegyServiceSessionFactory
 *
 * @brief Used to create an instance of the SegyServiceSession class. This is the interface to the SEG-Y SDK service functionality.
 *
 **/
class SEGYSDK_DLL_PUBLIC SegyServiceSessionFactory
{
public:
    /**
     * @brief Constructs Factory.
     *
     * @param seismicTraceData Defines the seismic trace data for which service session should be created.
     **/
    SegyServiceSessionFactory(std::string seismicTraceData);

    /**
     *  @brief createSegyServiceSession Creates and  returns a new SegyServiceSession instance.
     *
     **/
    std::unique_ptr<SegyServiceSession> createSegyServiceSession() const;

private:
    struct Impl;
    std::shared_ptr<Impl> impl_;
};

} // namespace segysdk::api
