/*
Copyright 2021 Schlumberger

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

#include <memory>
#include <string>

#include "IndexBuilderData.h"
#include "../FileIO/FileWriter.h"

namespace segysdk::segyindex
{

class IndexBuilderDataWriter
{
public:
    inline static const int maxBufferedTraceCount = 50000;

private:
    std::string _tmpFileName;

    uint8_t buffer[maxBufferedTraceCount * sizeof(IndexBuilderData)]{};
    std::shared_ptr<fileio::FileWriter> writer_;

    size_t writePtr_ = 0;

public:
    explicit IndexBuilderDataWriter(std::string& tmpFileName);

    bool WriteData(IndexBuilderData& data);

    size_t getTraceCount() const { return writePtr_; }

    void Close() const;
};
} // namespace segysdk::segyindex
