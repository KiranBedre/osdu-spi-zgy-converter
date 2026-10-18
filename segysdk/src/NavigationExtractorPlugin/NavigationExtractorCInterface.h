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

#include "../NavigationExtractorPlugin/InputParams.h"
#include "../NavigationExtractorPlugin/NavigationExtractorInterface.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <memory>
#include <stdint.h>

/**
 * @brief This is the interface to Extract Navigation Shared Library
 *
 **/

extern "C" {

/**
 * @brief Interface for Extracting 2D line navigation from 2D SEG-Y data files
 *
 * @param [IN] pointer to the InputParams object
 *
 * @return a pointer to the just created and initialized NavigationExtractorInterface object instance on success, NULL otherwise
 *
 **/
SEGYSDK_DLL_PUBLIC segysdk::navigation_extractor::NavigationExtractorInterface* Segy2DNavigationExtractor(segysdk::navigation_extractor::InputParams* params);

/**
 * @brief Deletes instance of NavigationExtractorInterface.
 *
 * @param [IN] pointer to the NavigationExtractorInterface object
 *
 **/
SEGYSDK_DLL_PUBLIC void DestroyNavigationExtractor(segysdk::navigation_extractor::NavigationExtractorInterface* instance);

} /* extern "C" */
