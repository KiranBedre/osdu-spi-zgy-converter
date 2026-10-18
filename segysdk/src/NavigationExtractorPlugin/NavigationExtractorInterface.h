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

#include <memory>

namespace segysdk::navigation_extractor
{

/**
 * @class NavigationExtractorInterface
 *
 * @brief The Extract Navigation Interface
 *
 * Simple interface to be used by a Extract Navigation plugin instance. Intended to simplify acquiring of a computed Json Polygons
 * from plugin shared object.
 *
 */
class SEGYSDK_DLL_PUBLIC NavigationExtractorInterface
{
public:
    NavigationExtractorInterface() = default;

    virtual ~NavigationExtractorInterface() = default;

    virtual int64_t extractFromTraceHeaders() const = 0;

private:
    NavigationExtractorInterface(NavigationExtractorInterface&&) = delete;
    NavigationExtractorInterface(const NavigationExtractorInterface&) = delete;
};

} // namespace segysdk::navigation_extractor
