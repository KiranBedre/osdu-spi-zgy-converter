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

#include "../NavigationExtractorPlugin/NavigationExtractorInterface.h"
#include "../NavigationExtractorPlugin/InputParams.h"

#include "../LibraryLoader/DynamicLibraryLoader.h"


#include "../segysdk_details/DLL_EXPORT.h"

#include <memory>
#include <openzgy/api.h>

namespace segysdk::navigation_extractor
{

class SEGYSDK_DLL_PUBLIC NavigationExtractorPlugin
{
public:

    explicit NavigationExtractorPlugin(std::unique_ptr<segysdk::library_loader::DynamicLibraryLoader> loader);
    ~NavigationExtractorPlugin() = default;

    int64_t ExtractNavigationFromTraceHeaders(InputParams& inputParams);   

private:
    bool InitFunctionPointers();

private:
    typedef NavigationExtractorInterface* (*ExtractFromTraceheaders_t)(InputParams& inputParams);
    typedef void (*DestroyNavigationExtractor_t)(NavigationExtractorInterface*);

    static constexpr char const* const functionExtractNavigationFromTraceHeaders_ = "Segy2DNavigationExtractor";
    static constexpr char const* const functionDestroyNavigationExtractor_ = "DestroyNavigationExtractor";

    std::unique_ptr<segysdk::library_loader::DynamicLibraryLoader> libraryLoader_;

    ExtractFromTraceheaders_t ExtractFromTraceheadersFn_ = nullptr;
    DestroyNavigationExtractor_t DestroyNavExtrFn_ = nullptr;

    std::unique_ptr<NavigationExtractorInterface, decltype(DestroyNavExtrFn_)> pluginInstance_;
};

} // namespace segysdk::navigation_extractor

