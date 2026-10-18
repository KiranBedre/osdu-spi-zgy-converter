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


#include "../StatisticsGeneratorPlugin/StatisticsGeneratorPlugin.h"

#include "../Logger/Logger.h"
#include "../StatisticsGeneratorPlugin/StatisticsGeneratorPluginException.h"
#include "../tools/SegyIndexerService/SegyIndexUtilities/IndexingReturnStatus.h"

namespace segysdk::statistics_generator
{

StatisticsGeneratorPlugin::StatisticsGeneratorPlugin(std::unique_ptr<library_loader::DynamicLibraryLoader> loader)
  : libraryLoader_(std::move(loader)), pluginInstance_(nullptr, [](auto p) { (void)p; })
{
    if (not InitFunctionPointers()) {
        throw StatisticsPluginException("Couldn't initialize function pointers for Statistics and Histogram Generation plugin");
    }
}


int64_t
StatisticsGeneratorPlugin::GenerateForSegy(const StatisticsInputParams* params)
{
    SEGYLOG_INFO("Start of Statistics and Histogram Generation");
    auto statisticsIf = GenerateStatisticsHistogramSegyFn_(params);
    if (statisticsIf == nullptr) {
        throw StatisticsPluginException("Couldn't initialize function pointers for CalculateStatistics plugin");
    }

    pluginInstance_ = std::unique_ptr<StatisticsGeneratorInterface, decltype(DestroyStatisticsHistogramFn_)>(statisticsIf, DestroyStatisticsHistogramFn_);
    const auto status = pluginInstance_->calculateStatistics();
    if (status == 0) {
        SEGYLOG_REPORT_INFO("statisticsGenerator;  Statistics and Histogram successfully generated.");
    }
    else {
        SEGYLOG_REPORT_INFO("statisticsGenerator; Statistics and Histogram generation failed.");
    }

    return status;
}

bool
StatisticsGeneratorPlugin::InitFunctionPointers()
{
    GenerateStatisticsHistogramSegyFn_ = reinterpret_cast<GenerateStatisticsHistogramSegy_t>(libraryLoader_->getFunction(functionGenerateStatisticsHistogramSegy_));
    
    DestroyStatisticsHistogramFn_ = reinterpret_cast<DestroyStatisticsHistogram_t>(libraryLoader_->getFunction(functionDestroyStatisticsHistogram_));

    return GenerateStatisticsHistogramSegyFn_ != nullptr
           and DestroyStatisticsHistogramFn_ != nullptr;
}

} // namespace segysdk::statistics_generator
