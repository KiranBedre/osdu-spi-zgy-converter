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

#include "../BulkAccess/Segy2DReader.h"

#include "../BulkAccess/BulkIndex.h"
#include "../FileIO/FileReader.h"
#include "../SegyIO/ValueConversion/SampleValueConverter.h"

#include <memory>

namespace segysdk::bulkaccess
{

class SEGYSDK_DLL_PUBLIC Segy2DReaderImpl : public Segy2DReader
{
public:
    static constexpr size_t const defaultBufferSize_ = 1024UL * 1024UL;

    Segy2DReaderImpl(const Segy2DReaderImpl& other) = delete;
    Segy2DReaderImpl& operator=(const Segy2DReaderImpl& other) = delete;
    Segy2DReaderImpl(const std::shared_ptr<fileio::FileReader>& file,
                     const std::shared_ptr<fileio::FileReader>& indexFile,
                     const std::shared_ptr<BulkIndex>& bulkIndex);

    size_t readByCmp(int cmpStart, int cmpEnd, size_t sampleStart, size_t sampleEnd, float* buffer) override;
    size_t readByTraceNo(size_t traceNoStart, size_t traceNoEnd, size_t sampleStart, size_t sampleEnd, float* buffer) override;
    size_t readTracesByCmp(int cmpStart, size_t traces, size_t sampleStart, size_t sampleEnd, float* buffer) override;

    void setBufferLoadChunk(size_t newChunk) const;

private:
    size_t getByteCount(size_t sampleCount) const;
    size_t getTraceSize() const;

    uint8_t const* readAligned(uint64_t offset, size_t size, size_t alignment) const;
    void convert(uint8_t const* bytes, float* target, size_t sampleCount) const;

    size_t getTraceNoByCmp(int cmp) const;
    Samples doRead(const Segy2DReaderParams& params) override;

    size_t sampleSize_;
    std::shared_ptr<fileio::BufferedFileReaderBase> fileReader_;
    std::shared_ptr<fileio::BufferedFileReaderBase> traceIndexReader_;
    std::shared_ptr<BulkIndex> bulkIndex_;
    std::unique_ptr<segyio::valueconversion::SampleValueConverter> converter_;
    std::shared_ptr<BulkIndex::TraceIndexer> traceIndexer_;
};

} // namespace segysdk::bulkaccess
