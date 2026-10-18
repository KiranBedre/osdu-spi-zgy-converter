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

#include "../segysdk_details/DLL_EXPORT.h"

#include "../BulkAccess/BulkIndex.h"
#include "../BulkAccess/SegyBulkReader.h"
#include "../FileIO/BufferedFileReader.h"
#include "../FileIO/BufferedFileReaderBase.h"
#include "../FileIO/FileReader.h"
#include "../SegyIO/ValueConversion/SampleValueConverter.h"

#include <cstddef>
#include <memory>

namespace segysdk::bulkaccess
{
using segyio::valueconversion::SampleValueConverter;

struct SEGYSDK_DLL_PUBLIC BulkReaderBase : public SegyBulkReader
{
    static constexpr size_t const defaultBufferSize_ = 1024UL * 1024UL;

    BulkReaderBase(size_t sampleSize);

protected:
    virtual BulkIndex const& getBulkIndex() const = 0;
    virtual BulkIndex::TraceIndexer& getTraceIndexer() const = 0;
    virtual fileio::BufferedFileReaderBase& getFileReader() const = 0;
    virtual fileio::BufferedFileReaderBase& getTraceIndexReader() const = 0;
    size_t getSampleSize() const;
    SampleValueConverter& getSampleValueConverter();

    virtual void setBufferSize(size_t newSize) override;
    virtual size_t getBufferSize() const override;
    virtual void setBufferLoadChunk(size_t newChunk) override;
    uint8_t const* readAligned(uint64_t offset, size_t size, size_t alignment) const;
    void adjustReadSize(size_t traceCount) const;

    size_t getByteCount(size_t sampleCount) const;
    size_t getTraceSize() const;

    virtual bool read(size_t i0, size_t j0, size_t k0, size_t ni, size_t nj, size_t nk, float* buffer) override;
    virtual bool read(ReadDirection readDirection, size_t inlineIndex, size_t xlineIndex, size_t sampleIndex, size_t xCount, size_t sampleCount, float* buffer) override;

    bool read_impl(size_t i0, size_t j0, size_t k0, size_t ni, size_t nj, size_t nk, float* buffer);

    virtual void set_inline_read_increment(size_t value);
    void set_inline_read_increment_impl(size_t value) { inlineIncrement_ = value; }

    virtual bool read_trace_header_data(size_t i0, size_t j0, size_t ni, size_t nj, const std::vector<segyformat::external::TraceHeaderField>& headers_list, std::vector<std::vector<TraceHeaderResult>>& result) override;
    bool read_trace_header_data_impl(size_t i0, size_t j0, size_t ni, size_t nj, const std::vector<segyformat::external::TraceHeaderField>& headers_list, std::vector<std::vector<TraceHeaderResult>>& result);

private:
    BulkReaderBase(const BulkReaderBase& other) = delete;
    BulkReaderBase& operator=(const BulkReaderBase& other) = delete;

    inline void read_chunk(bool const swapAxis,
                           BulkIndex::TraceIndexer& traceIndexer,
                           SampleValueConverter const& converter,
                           size_t nk, size_t k0, size_t k1, size_t strideJ,
                           bool hasTraceWeights,
                           size_t sampleAlignment, size_t sampleOffset, size_t inputSize,
                           size_t i, size_t j,
                           float*& output) const;

    bool read_chunk_trace_headers(const size_t offset, const size_t byte_count, const std::vector<segyformat::external::TraceHeaderField>& headers_list, std::vector<std::vector<TraceHeaderResult>>& trace_header_data, const size_t result_ix);
    
    SampleValueConverter& get_th_converter(segyformat::external::DataFormat data_format, unsigned short get_length);
    SampleValueConverter& getInt2ToFloatIEEEValueConverter();
    SampleValueConverter& getInt4ToFloatIEEEValueConverter();
    SampleValueConverter& getUInt2ToFloatIEEEValueConverter();
    SampleValueConverter& getUInt4ToFloatIEEEValueConverter();
    SampleValueConverter& getFloatIEEEToFloatIEEEValueConverter();
    SampleValueConverter& getFloatIBMToFloatIEEEValueConverter();


private:
    size_t sampleSize_;
    size_t inlineIncrement_;

    std::unique_ptr<SampleValueConverter> sampleValueConverter_;
    std::unique_ptr<SampleValueConverter> int2ToFloatIEEEValueConverter_;
    std::unique_ptr<SampleValueConverter> int4ToFloatIEEEValueConverter_;
    std::unique_ptr<SampleValueConverter> uint2ToFloatIEEEValueConverter_;
    std::unique_ptr<SampleValueConverter> uint4ToFloatIEEEValueConverter_;

    std::unique_ptr<SampleValueConverter> floatIEEEToFloatIEEEValueConverter_;
    std::unique_ptr<SampleValueConverter> floatIBMToFloatIEEEValueConverter_;
};

} // namespace segysdk::bulkaccess
