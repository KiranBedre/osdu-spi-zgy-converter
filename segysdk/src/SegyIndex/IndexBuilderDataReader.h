/*
Copyright 2021-2022 Schlumberger

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

#include "IndexBuilderData.h"

#include "../FileIO/FileReader.h"

#include <memory>
#include <string>
#include <vector>

namespace segysdk::segyindex
{

class IndexBuilderDataReader
{
public:
    inline static const int maxBufferedTraceCount = 50000;

private:
    std::string tmpFileName_;
    size_t tmpFileSize_ = 0;
    size_t tmpFileTraceCount_ = 0;

    std::vector<uint8_t> buffer_;
    std::shared_ptr<fileio::FileReader> reader_;

    size_t myBufferSize_ = 0;
    size_t bufferStartTraceNo_ = 0;
    size_t bufferTraceNo_ = 0;

private:
    IndexBuilderData getDataAtTraceOffset(size_t offset) const;

public:
    explicit IndexBuilderDataReader(std::string& tmpFileName);

    IndexBuilderData ReadTrace(size_t traceNo);
    IndexBuilderData ReadNext();

    void Close() const;

    bool AtEnd() const;

    bool ReaderInitialized() const;
};
} // namespace segysdk::segyindex
