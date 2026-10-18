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

#include "../FileIO/FileReader.h"
#include "../FileIO/ReadCache.h"

#include <functional>

namespace segysdk::fileio
{

class SEGYSDK_DLL_PUBLIC CachedFileReader : public FileReader
{
public:
    // backwards compatibility for tests - single thread only
    CachedFileReader(const std::shared_ptr<FileReader>& fileReader, const std::shared_ptr<ReadCache>& cache, uint16_t segyFileNumber = 0);

    // for multithread: factory method for reader for each thread
    CachedFileReader(const std::string& fileName, const std::function<std::shared_ptr<FileReader>()>& fileReaderFactory, const std::shared_ptr<ReadCache>& cache, uint16_t segyFileNumber = 0);

    virtual ~CachedFileReader() override;

    virtual size_t getBufferSize() const override;
    virtual uint64_t getFileSize() override;
    virtual void close() override;

private:
    ByteBuffer doRead(const FileReadParams& params) override;
    std::vector<std::shared_ptr<FileReader>> fileReaders_;
    std::shared_ptr<ReadCache> cache_;
    const uint16_t segyFileNumber_{};
};

} // namespace segysdk::fileio
