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


#include "../BulkAccess/TraceHeaderResult.h"
#include "../SegyIndex/FileLayoutSpecification.h"
#include "../SegyIndex/Utilities/TraceIndexStorage.h"
#include "../SegyIndex/Utilities/TraceRangeFactoryBase.h"
#include "../SegyIndex/Utilities/TraceRange.h"
#include "../SegyIndex/Utilities/TraceRangeRecord.h"

#include "TraceRangeRecord.h"
#include <cstdint>
#include <memory>

namespace segysdk::segyindex
{
namespace trace_range_map
{

/**
 * @brief struct MissingTraceRangeFactory Implements the trace range items generator that stores the created ranges to the TraceIndexStorage instance
 **/
struct SEGYSDK_DLL_PUBLIC MissingTraceRangeFactory final : public TraceRangeFactoryBase
{
    /**
     * @brief Constructs the MissingTraceRangeFactory instance
     *
     * @param fileLayout - the geometry information for the entire SEGY data set
     * @param collatedData - the geometry information for an individual input SEGY file which is being processed by the current MissingTraceRangeFactory instance
     * @param outputStorage - the created ranges to be stored to this TraceIndexStorage instance
     **/
    MissingTraceRangeFactory(FileLayoutSpecification fileLayout,
                             segyindex::trace_range_map::TraceIndexStorage& outputStorage);
    ~MissingTraceRangeFactory() override;

protected:
    void checkParameters() const override;
    void init() override;

private:
    MissingTraceRangeFactory(MissingTraceRangeFactory const&);
    MissingTraceRangeFactory& operator=(MissingTraceRangeFactory const&);
    MissingTraceRangeFactory(MissingTraceRangeFactory&&);
    MissingTraceRangeFactory& operator=(MissingTraceRangeFactory&&);
};

} // namespace missing_trace_range_map
} // namespace segysdk::segyindex
