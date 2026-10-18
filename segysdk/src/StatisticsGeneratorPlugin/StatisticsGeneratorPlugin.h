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

#include "../LibraryLoader/DynamicLibraryLoader.h"
#include "../SegyIndex/SegyIndex.h"

#include "../segysdk_details/DLL_EXPORT.h"

#include <memory>

namespace segysdk::statistics_generator
{

class SEGYSDK_DLL_PUBLIC StatisticsGeneratorPlugin
{
public:

    explicit StatisticsGeneratorPlugin(std::unique_ptr<library_loader::DynamicLibraryLoader> loader);

    int64_t GenerateForSegy(const StatisticsInputParams* params);

private:
    bool InitFunctionPointers();

private:
    typedef StatisticsGeneratorInterface* (*GenerateStatisticsHistogramSegy_t)(const StatisticsInputParams* params);

    typedef void (*DestroyStatisticsHistogram_t)(StatisticsGeneratorInterface*);

    static constexpr char const* const functionGenerateStatisticsHistogramSegy_ = "CalculateStatisticsSegy";
    static constexpr char const* const functionDestroyStatisticsHistogram_ = "DestroyStatistics";

    std::unique_ptr<library_loader::DynamicLibraryLoader> libraryLoader_;

    GenerateStatisticsHistogramSegy_t GenerateStatisticsHistogramSegyFn_ = nullptr;

    DestroyStatisticsHistogram_t DestroyStatisticsHistogramFn_ = nullptr;

    std::unique_ptr<StatisticsGeneratorInterface, decltype(DestroyStatisticsHistogramFn_)> pluginInstance_;
};

} // namespace segysdk::statistics_generator

