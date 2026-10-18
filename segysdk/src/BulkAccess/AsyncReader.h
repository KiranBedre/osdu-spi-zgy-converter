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

namespace segysdk::bulkaccess::async
{

struct SEGYSDK_DLL_PUBLIC ExtentSpan
{
    ExtentSpan() = default;
    ExtentSpan(const std::size_t start, const std::size_t count);
    ExtentSpan& withStep(const std::size_t value);
    std::size_t start_{};
    std::size_t count_{};
    std::size_t step_{1};
};

struct SEGYSDK_DLL_PUBLIC ReaderParams : public request::Params
{
    ReaderParams& withInline(const ExtentSpan& value);
    ReaderParams& withXline(const ExtentSpan& value);
    ReaderParams& withSamples(const ExtentSpan& value);
    ReaderParams& withNumParallelTasks(const int value);
    ExtentSpan inline_{};
    ExtentSpan xline_{};
    ExtentSpan samples_{};
    int numParallelTasks_{4};
};

class SEGYSDK_DLL_PUBLIC Reader
{
public:
    virtual ~Reader() = default;
    Samples read(const ReaderParams& params);

private:
    virtual Samples doRead(const ReaderParams& params) = 0;
};

} // namespace segysdk::bulkaccess::async
