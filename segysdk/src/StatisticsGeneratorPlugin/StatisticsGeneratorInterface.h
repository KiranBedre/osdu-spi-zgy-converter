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

#include <json/json.h>
#include <memory>

namespace segysdk::statistics_generator
{

/**
 * @class StatisticsGeneratorInterface
 *
 * @brief The Statistics Generator Interface
 *
 * Simple interface to be used by a statistics_generator plugin instance. Intended to simplify generation of trace histogram and statistics
 * from plugin shared object.
 *
 */
class SEGYSDK_DLL_PUBLIC StatisticsGeneratorInterface
{
public:
    StatisticsGeneratorInterface() = default;
    StatisticsGeneratorInterface(StatisticsGeneratorInterface&&) = delete;
    StatisticsGeneratorInterface(const StatisticsGeneratorInterface&) = delete;
    virtual ~StatisticsGeneratorInterface() = default;

    
    virtual int64_t calculateStatistics() const = 0;

};

} // namespace segysdk::statistics_generator
