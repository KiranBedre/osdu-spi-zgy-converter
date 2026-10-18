/*
Copyright 2022 Schlumberger

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

#include "../SegyIndex/Utilities/TraceRangeFactoryBase.h"
#include "../SegyIndex/Utilities/TraceIndexStorage.h"
#include "../SegyIndex/Utilities/TraceRange.h"
#include "../SegyIndex/Utilities/TraceRangeRecord.h"
#include "../SegyIndex/FileLayoutSpecification.h"
#include "../SegyIndex/IndexBuilderCollatedData.h"

#include <cstdint>
#include <memory>

namespace segysdk::segyindex
{
namespace trace_range_map
{

/**
 * @brief struct TraceRangeFactory Implements the trace range items generator that stores the created ranges to the TraceIndexStorage instance
 **/
struct SEGYSDK_DLL_PUBLIC TraceRangeFactory final : public TraceRangeFactoryBase
{
    /**
     * @brief Constructs the TraceRangeFactory instance
     *
     * @param fileLayout - the geometry information for the entire SEGY data set
     * @param collatedData - the geometry information for an individual input SEGY file which is being processed by the current TraceRangeFactory instance
     * @param outputStorage - the created ranges to be stored to this TraceIndexStorage instance
     **/
    TraceRangeFactory(FileLayoutSpecification fileLayout,
                      std::shared_ptr<IndexBuilderCollatedData> const& collatedData,
                      TraceIndexStorage& outputStorage);

    ~TraceRangeFactory() override;

protected:
    void checkParameters() const override;
    void init() override;

private:
    TraceRangeFactory(TraceRangeFactory const&);
    TraceRangeFactory& operator=(TraceRangeFactory const&);
    TraceRangeFactory(TraceRangeFactory&&);
    TraceRangeFactory& operator=(TraceRangeFactory&&);

private:
    std::shared_ptr<IndexBuilderCollatedData> const collatedData_;

};

} // namespace trace_range_map
} // namespace segysdk::segyindex
