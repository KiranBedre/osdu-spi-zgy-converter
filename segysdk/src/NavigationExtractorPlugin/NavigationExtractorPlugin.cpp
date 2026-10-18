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

#include "../NavigationExtractorPlugin/NavigationExtractorPlugin.h"
#include "../NavigationExtractorPlugin/NavigationExtractorPluginException.h"
#include "../NavigationExtractorPlugin/StatusCodes.h"
#include "../Logger/Logger.h"

namespace segysdk::navigation_extractor {

NavigationExtractorPlugin::NavigationExtractorPlugin(std::unique_ptr<segysdk::library_loader::DynamicLibraryLoader> loader)
  : libraryLoader_(std::move(loader)), pluginInstance_(nullptr, [](auto p) { (void)p; })
{
    if (not InitFunctionPointers()) {
        throw NavigationExtractorException("Couldn't initialize function pointers for Extract Navigation plugin");
    }
}

int64_t
NavigationExtractorPlugin::ExtractNavigationFromTraceHeaders(InputParams& inputParams)
{
    SEGYLOG_INFO("Start of Navigation Data Extraction Plugin... ");
    try {
        auto extractNavInterface = ExtractFromTraceheadersFn_(inputParams);
        if (extractNavInterface == nullptr) {
            throw NavigationExtractorException("failed to acquire navigation extractor");
        }
        pluginInstance_ = std::unique_ptr<NavigationExtractorInterface, decltype(DestroyNavExtrFn_)>(extractNavInterface, DestroyNavExtrFn_);
        return pluginInstance_->extractFromTraceHeaders();
    }
    catch(...) {
        throw;
    }
}

bool
NavigationExtractorPlugin::InitFunctionPointers()
{
    ExtractFromTraceheadersFn_ = reinterpret_cast<ExtractFromTraceheaders_t>(libraryLoader_->getFunction(functionExtractNavigationFromTraceHeaders_));
  
    DestroyNavExtrFn_ = reinterpret_cast<DestroyNavigationExtractor_t>(libraryLoader_->getFunction(functionDestroyNavigationExtractor_));

    return ExtractFromTraceheadersFn_ != nullptr
           and DestroyNavExtrFn_ != nullptr;
}

} // namespace segysdk::navigation_extractor 
