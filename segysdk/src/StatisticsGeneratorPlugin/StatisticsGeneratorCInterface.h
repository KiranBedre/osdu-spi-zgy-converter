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

#include "../StatisticsGeneratorPlugin/StatisticsGeneratorInterface.h"
#include "../StatisticsGeneratorPlugin/StatisticsInputParams.h"

#include "../SegyIndex/SegyIndex.h"

#include "../segysdk_details/DLL_EXPORT.h"


/**
 * @brief This is the interface to Compute Polygon Shared Library
 *
 **/

extern "C" {

/**
 * @brief Creates instance of StatisticsGeneratorInterface for SEGY file based on it's index.
 *
 * @param [IN] params pointer to the CmdLineParams object
 *
 * @return a pointer to the just created and initialized StatisticsGeneratorInterface object instance on success, NULL otherwise
 *
 **/
SEGYSDK_DLL_PUBLIC segysdk::statistics_generator::StatisticsGeneratorInterface* CalculateStatisticsSegy(const segysdk::statistics_generator::StatisticsInputParams* params);

/**
 * @brief Deletes instance of StatisticsGeneratorInterface.
 *
 * @param [IN] instance pointer to the StatisticsGeneratorInterface object
 *
 **/
SEGYSDK_DLL_PUBLIC void DestroyStatistics(segysdk::statistics_generator::StatisticsGeneratorInterface* instance);

} /* extern "C" */
