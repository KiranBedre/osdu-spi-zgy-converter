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

#include "../FileIO/FileReader.h"
#include "../Utilities/Containers.h"
#include "../segysdk_details/DLL_EXPORT.h"

namespace segysdk::bulkaccess
{

class SEGYSDK_DLL_PUBLIC ReaderConnector
{
public:
    virtual ~ReaderConnector() = default;
    std::size_t getSampleSize() const;
    std::size_t getSizeI() const;
    std::size_t getSizeJ() const;
    std::size_t getSizeK() const;
    bool findTrace(const std::size_t i, const std::size_t j);
    std::size_t getTraceOffset() const;
    ByteBuffer readSamples(const fileio::FileReadParams& params) const;
    Samples convertBytesToSamples(const ByteBuffer& bytes) const;

private:
    virtual std::size_t doGetSampleSize() const = 0;
    virtual std::size_t doGetSizeI() const = 0;
    virtual std::size_t doGetSizeJ() const = 0;
    virtual std::size_t doGetSizeK() const = 0;
    virtual bool doFindTrace(const std::size_t i, const std::size_t j) = 0;
    virtual std::size_t doGetTraceOffset() const = 0;
    virtual ByteBuffer doReadSamples(const fileio::FileReadParams& params) const = 0;
    virtual Samples doConvertBytesToSamples(const ByteBuffer& bytes) const = 0;
};

} // namespace segysdk::bulkaccess
