/*
Copyright 2024-2025 Schlumberger

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

#include "../Request/Params.h"
#include "../Utilities/Containers.h"
#include "../segysdk_details/DLL_EXPORT.h"

#include <cstddef>
#include <vector>

namespace segysdk::bulkaccess
{

struct SEGYSDK_DLL_PUBLIC Segy2DReaderParams : public request::Params
{
    Segy2DReaderParams& withSampleStart(const std::size_t value);
    Segy2DReaderParams& withSampleEnd(const std::size_t value);
    Segy2DReaderParams& withTraceStart(const std::size_t value);
    Segy2DReaderParams& withTraceEnd(const std::size_t value);
    std::size_t sampleStart_{};
    std::size_t sampleEnd_{};
    std::size_t traceStart_{};
    std::size_t traceEnd_{};
};

class SEGYSDK_DLL_PUBLIC Segy2DReader
{
public:
    virtual ~Segy2DReader() = default;

    virtual size_t readByCmp(int cmpStart, int cmpEnd, size_t sampleStart, size_t sampleEnd, float* buffer) = 0;
    virtual size_t readByTraceNo(size_t traceNoStart, size_t traceNoEnd, size_t sampleStart, size_t sampleEnd, float* buffer) = 0;
    virtual size_t readTracesByCmp(int cmpStart, size_t traces, size_t sampleStart, size_t sampleEnd, float* buffer) = 0;
    Samples read(const Segy2DReaderParams& params);

private:
    virtual Samples doRead(const Segy2DReaderParams& params) = 0;
};

} // namespace segysdk::bulkaccess
