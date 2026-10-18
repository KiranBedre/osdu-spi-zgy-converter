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

#include "../segysdk_details/DLL_EXPORT.h"

#include "IndexBuilderData.h"
#include "IndexBuilderDataReader.h"
#include "IndexBuilderDataWriter.h"

#include <string>

namespace segysdk::segyindex
{

class SEGYSDK_DLL_PUBLIC IndexBuilderDataManager
{
private:
    std::string tempFileName_;

    std::shared_ptr<IndexBuilderDataReader> reader_;
    std::shared_ptr<IndexBuilderDataWriter> writer_;

    size_t traceCount_{};

    bool readerOK = false;
    bool writerOK;

private:
    void startRead();

public:
    IndexBuilderDataManager();

    ~IndexBuilderDataManager();

    // non-copyable - not implemented
    IndexBuilderDataManager(const IndexBuilderDataManager& other) = delete;
    IndexBuilderDataManager& operator=(const IndexBuilderDataManager& other) = delete;

    // non-movable - not implemented
    IndexBuilderDataManager(IndexBuilderDataManager&&) = delete;

public:
    size_t GetTraceCount() const;
    bool WriteData(IndexBuilderData& data);
    bool CloseWriter();

    bool IsReaderReady();
    bool AtReaderEnd() const;

    IndexBuilderData ReadNext() const;

    void CloseReader();
};
} // namespace segysdk::segyindex
