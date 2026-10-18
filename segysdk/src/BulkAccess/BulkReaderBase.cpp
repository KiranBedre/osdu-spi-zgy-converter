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

#include "../BulkAccess/BulkReaderBase.h"

#include "../BulkAccess/BulkIndex.h"
#include "../SegyIO/Primitives.h"
#include "../SegyIO/SampleFormat.h"
#include "../SegyIO/SegyIOException.h"
#include "../Logger/Logger.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <stdexcept>
#include <string>

namespace segysdk::bulkaccess
{

using segyio::Primitives;

BulkReaderBase::BulkReaderBase(size_t sampleSize)
  : sampleSize_(sampleSize), inlineIncrement_(1)
{
    /* EMPTY */
}

size_t
BulkReaderBase::getSampleSize() const
{
    return sampleSize_;
}

size_t
BulkReaderBase::getByteCount(size_t sampleCount) const
{
    return getSampleSize() * sampleCount;
}

size_t
BulkReaderBase::getTraceSize() const
{
    return Primitives::getTraceHeaderSize() + getByteCount(static_cast<size_t>(getBulkIndex().getSizeK()));
}

SampleValueConverter&
BulkReaderBase::getSampleValueConverter()
{
    if (sampleValueConverter_ == nullptr) {
        const bool needsByteSwap = getBulkIndex().isByteSwapped();
        const SampleFormat sampleFormat = getBulkIndex().getSampleFormat();

        sampleValueConverter_ = SampleValueConverter::create(sampleFormat, needsByteSwap);
    }

    return *sampleValueConverter_.get();
}

SampleValueConverter&
BulkReaderBase::getInt2ToFloatIEEEValueConverter()
{
    if (int2ToFloatIEEEValueConverter_ == nullptr) {
        const bool needsByteSwap = getBulkIndex().isByteSwapped();

        int2ToFloatIEEEValueConverter_ = SampleValueConverter::create(SampleFormat::INT2, needsByteSwap);
    }

    return *int2ToFloatIEEEValueConverter_.get();
}

SampleValueConverter&
BulkReaderBase::getInt4ToFloatIEEEValueConverter()
{
    if (int4ToFloatIEEEValueConverter_ == nullptr) {
        const bool needsByteSwap = getBulkIndex().isByteSwapped();

        int4ToFloatIEEEValueConverter_ = SampleValueConverter::create(SampleFormat::INT4, needsByteSwap);
    }

    return *int4ToFloatIEEEValueConverter_.get();
}

SampleValueConverter&
BulkReaderBase::getUInt2ToFloatIEEEValueConverter()
{
    if (uint2ToFloatIEEEValueConverter_ == nullptr) {
        const bool needsByteSwap = getBulkIndex().isByteSwapped();

        uint2ToFloatIEEEValueConverter_ = SampleValueConverter::create(SampleFormat::UINT2, needsByteSwap);
    }

    return *uint2ToFloatIEEEValueConverter_.get();
}

SampleValueConverter&
BulkReaderBase::getUInt4ToFloatIEEEValueConverter()
{
    if (uint4ToFloatIEEEValueConverter_ == nullptr) {
        const bool needsByteSwap = getBulkIndex().isByteSwapped();

        uint4ToFloatIEEEValueConverter_ = SampleValueConverter::create(SampleFormat::UINT4, needsByteSwap);
    }

    return *uint4ToFloatIEEEValueConverter_.get();
}

SampleValueConverter&
BulkReaderBase::getFloatIEEEToFloatIEEEValueConverter()
{
    if (floatIEEEToFloatIEEEValueConverter_ == nullptr) {
        const bool needsByteSwap = getBulkIndex().isByteSwapped();

        floatIEEEToFloatIEEEValueConverter_ = SampleValueConverter::create(SampleFormat::IeeeFloat4, needsByteSwap);
    }

    return *floatIEEEToFloatIEEEValueConverter_.get();
}

SampleValueConverter&
BulkReaderBase::getFloatIBMToFloatIEEEValueConverter()
{
    if (floatIBMToFloatIEEEValueConverter_ == nullptr) {
        const bool needsByteSwap = getBulkIndex().isByteSwapped();

        floatIBMToFloatIEEEValueConverter_ = SampleValueConverter::create(SampleFormat::IeeeFloat4, needsByteSwap);
    }

    return *floatIBMToFloatIEEEValueConverter_.get();
}

bool
BulkReaderBase::read_trace_header_data(size_t i0, size_t j0, size_t ni, size_t nj, const std::vector<segyformat::external::TraceHeaderField>& headers_list, std::vector<std::vector<TraceHeaderResult>>& result)
{
    return read_trace_header_data_impl(i0, j0, ni, nj, headers_list, result);
}

bool
BulkReaderBase::read_trace_header_data_impl(size_t i0, size_t j0, size_t ni, size_t nj, const std::vector<segyformat::external::TraceHeaderField>& headers_list, std::vector<std::vector<TraceHeaderResult>>& result)
{
    if (i0 + ni > getBulkIndex().getSizeI())
        throw SegyInvalidArgumentException("Invalid inline extent.");

    if (j0 + nj > getBulkIndex().getSizeJ())
        throw SegyInvalidArgumentException("Invalid crossline extent.");

    // Swapping read order may improve performance in these cases, should be measured, though
    const bool swapAxis = getBulkIndex().isCrosslineOrdered() && ni > 1;
    if (swapAxis) {
        std::swap(i0, j0);
        std::swap(ni, nj);
    }

    adjustReadSize(Primitives::getTraceHeaderSize());
    if (result.size() != ni) {
        result.resize(ni);
    }
    BulkIndex::TraceIndexer& traceIndexer = getTraceIndexer();

    const size_t i1 = i0 + ni;
    const size_t j1 = j0 + nj;

    constexpr size_t byteCount = Primitives::getTraceHeaderSize();

    for (size_t i = i0; i < i1; i += inlineIncrement_) {
        for (size_t j = j0; j < j1; ++j) {
            if (traceIndexer.findTrace(i, j)) {
                const size_t offset = traceIndexer.getOffset() - Primitives::getTraceHeaderSize();
                read_chunk_trace_headers(offset, byteCount, headers_list, result, i - i0);
            }
        }
    }
    if (result.size() > 0 and result[0].size() > 0) {
        std::string dbg = "ReadTraceHeaders: Offset: [" + std::to_string(result[0][0].getOffset()) + "], IL[" + std::to_string(result[0][0].getInline()) + "], XL[" + std::to_string(result[0][0].getCrossline()) + "], TraceSampleCount: [" + std::to_string(result[0][0].getTraceSampleCount()) + "]";
        SEGYLOG_DEBUG(dbg);
    }
    return true;
}

SampleValueConverter&
BulkReaderBase::get_th_converter(segyformat::external::DataFormat data_format, unsigned short length)
{
    if (data_format == segyformat::external::DataFormat::Integer) {
        if (length == 2) {
            return getInt2ToFloatIEEEValueConverter();
        }
        if (length == 4) {
            return getInt4ToFloatIEEEValueConverter();
        }
    }
    if (data_format == segyformat::external::DataFormat::UnsignedInt) {
        return getUInt2ToFloatIEEEValueConverter();
    }

    if (data_format == segyformat::external::DataFormat::FloatIBM) {
        return getFloatIBMToFloatIEEEValueConverter();
    }

    return getFloatIEEEToFloatIEEEValueConverter();
}

void
dump_trace_header(const bool do_it, const uint8_t* bytes)
{
    if (!do_it)
        return;

    std::cout << "read_chunk_trace_headers: trace header dump:" << std::endl;
    std::cout << "--------------------------------------------" << std::endl;
    uint8_t const* dbg_bytes = bytes;
    for (size_t dbg_ix = 0; dbg_ix < Primitives::getTraceHeaderSize(); dbg_ix++) {
        std::cout << "bytes[" + std::to_string(dbg_ix) << "] = " << std::to_string(*dbg_bytes) << "  ";
        if (dbg_ix > 0 && dbg_ix % 10 == 0) {
            std::cout << std::endl;
        }
        dbg_bytes += sizeof(uint8_t);
    }
    std::cout << std::endl;
}

bool
BulkReaderBase::read_chunk_trace_headers(
    const size_t offset, const size_t byte_count,
    const std::vector<segyformat::external::TraceHeaderField>& headers_list,
    std::vector<std::vector<TraceHeaderResult>>& trace_header_data,
    const size_t result_ix)
{

    try {
        uint8_t const* bytes = readAligned(offset, byte_count, 1);
        if (bytes == nullptr) {
            return false;
        }

        const bool show_debug_info = false;
        dump_trace_header(show_debug_info, bytes);

        int inline_no = 0;
        int crossline_no = 0;

        bool has_inline_no = false;
        bool has_crossline_no = false;
        bool has_trace_id_code = false;
        bool has_sample_count = false;

        std::vector<std::tuple<std::string, float>> header_values;

        int16_t trace_id_code = 1;
        uint16_t samples_per_trace = 0;
        for (const auto& fld : headers_list) {
            const size_t fld_offset = (fld.getOffset() - 1) * sizeof(unsigned char);
            uint8_t const* ptr = bytes + fld_offset;

            const SampleValueConverter& th_fld_converter = get_th_converter(fld.getNumericFormat(), fld.getLength());

            float my_value = 0;
            th_fld_converter.convert(ptr, &my_value, 1);

            if (fld.getKey() == "Inline") {
                inline_no = static_cast<int>(my_value);
                has_inline_no = true;
            }
            else if (fld.getKey() == "Crossline") {
                crossline_no = static_cast<int>(my_value);

                has_crossline_no = true;
            }
            else if (fld.getKey() == "TraceIdCode") {
                trace_id_code = static_cast<int16_t>(my_value);
                has_trace_id_code = true;
            }
            else if (fld.getKey() == "SamplesPerTrace") {
                samples_per_trace = static_cast<uint16_t>(my_value);
                has_sample_count = true;
            }
        }
        if (has_inline_no && has_crossline_no && has_trace_id_code && has_sample_count) {
            TraceHeaderResult hdr_result(inline_no, crossline_no, trace_id_code, samples_per_trace);
            trace_header_data[result_ix].emplace_back(hdr_result);
        }

        return true;
    }
    catch (std::exception& e) {
        const std::string msg = "Error reading trace header data. " + std::string(e.what());
        throw segyio::SegyFileIOException(msg);
    }
}

void
BulkReaderBase::setBufferSize(size_t newSize)
{
    getFileReader().setBufferSize(newSize);
}

size_t
BulkReaderBase::getBufferSize() const
{
    return getFileReader().getBufferSize();
}

void
BulkReaderBase::setBufferLoadChunk(size_t newChunk)
{
    getFileReader().setBufferSizeBoundary(newChunk);
    getFileReader().setBufferLoadChunk(newChunk);
}

uint8_t const*
BulkReaderBase::readAligned(uint64_t offset, size_t size, size_t alignment) const
{
    return getFileReader().readAligned(offset, size, alignment);
}

void
BulkReaderBase::adjustReadSize(size_t traceCount) const
{
    // TODO: Consider underlying caching/buffering

    // We would like individual buffers to be large enough to hold
    // traceCount x whole traces. So:
    const size_t readSize = traceCount * getTraceSize();

    fileio::BufferedFileReaderBase& reader = getFileReader();

    const size_t totalBufferSize = readSize * reader.getReadersCount();
    if (totalBufferSize > reader.getBufferSize()) {
        // multi-buffered setReadSize(readSize) is applying the new readSize to all individual read buffers
        reader.setReadSize(readSize);
    }
}

bool
BulkReaderBase::read(size_t i0, size_t j0, size_t k0, size_t ni, size_t nj, size_t nk, float* buffer)
{
    (void)i0;
    (void)j0;
    (void)k0;
    (void)ni;
    (void)nj;
    (void)nk;
    (void)buffer;

    throw SegyNotImplementedException("the segysdk::bulkaccess::SegyBulkReader::read is not implemented.");
}

bool
BulkReaderBase::read(ReadDirection readDirection, size_t inlineIndex, size_t xlineIndex, size_t sampleIndex, size_t xCount, size_t sampleCount, float* buffer)
{
    (void)readDirection;
    (void)inlineIndex;
    (void)xlineIndex;
    (void)sampleIndex;
    (void)xCount;
    (void)sampleCount;
    (void)buffer;

    throw SegyNotImplementedException("the segysdk::bulkaccess::SegyBulkReader::read is not implemented.");
}

bool
BulkReaderBase::read_impl(size_t i0, size_t j0, size_t k0, size_t ni, size_t nj, size_t nk, float* buffer)
{
    if (i0 + ni > getBulkIndex().getSizeI())
        throw SegyInvalidArgumentException("Invalid inline extent.");

    if (j0 + nj > getBulkIndex().getSizeJ())
        throw SegyInvalidArgumentException("Invalid crossline extent.");

    if (k0 + nk > getBulkIndex().getSizeK())
        throw SegyInvalidArgumentException("Invalid sample extent.");

    if (buffer == nullptr)
        throw SegyInvalidArgumentException("Null output buffer.");

    size_t strideI = nj * nk;
    size_t strideJ = nk;

    // Swapping read order may improve performance in these cases, should be measured, though
    const bool swapAxis = getBulkIndex().isCrosslineOrdered() && ni > 1;
    if (swapAxis) {
        std::swap(i0, j0);
        std::swap(ni, nj);
        std::swap(strideI, strideJ);
    }

    adjustReadSize(nj);

    BulkIndex::TraceIndexer& traceIndexer = getTraceIndexer();
    const SampleValueConverter& converter = getSampleValueConverter();
    const bool hasTraceWeights = getBulkIndex().hasTraceWeights();
    const size_t sampleAlignment = converter.getRequiredAlignment();
    const size_t sampleOffset = getByteCount(k0);
    const size_t inputSize = getByteCount(nk);

    const size_t i1 = i0 + ni;
    const size_t j1 = j0 + nj;
    const size_t k1 = k0 + nk;

    // buffer should be a pointer to brick_buffer_whole_/part_ ... ( = 64 x 64 x n samples of type float) 
    for (size_t i = i0; i < i1; i += inlineIncrement_) {
        float* output = buffer + (i - i0) * strideI;

        for (size_t j = j0; j < j1; ++j) {
            read_chunk(swapAxis,
                       traceIndexer, converter,
                       nk, k0, k1, strideJ,
                       hasTraceWeights,
                       sampleAlignment, sampleOffset, inputSize,
                       i, j,
                       output);
        }
    }

    return true;
}

void
BulkReaderBase::set_inline_read_increment(size_t value)
{
    (void) value;

    /* DO NOTHING */
}

void
BulkReaderBase::read_chunk(bool const swapAxis,
                           BulkIndex::TraceIndexer& traceIndexer,
                           SampleValueConverter const& converter,
                           size_t const nk, size_t const k0, size_t const k1, size_t const strideJ,
                           bool const hasTraceWeights,
                           size_t const sampleAlignment, size_t const sampleOffset, size_t const inputSize,
                           size_t i, size_t j,
                           float*& output) const
{
    if (!(swapAxis ? traceIndexer.findTrace(j, i) : traceIndexer.findTrace(i, j))) {
        memset(output, 0, nk * sizeof(float));
        output += strideJ;
        return;
    }

    size_t offset = traceIndexer.getOffset() + sampleOffset;
    size_t byteCount = inputSize;
    size_t sampleCount = nk;
    float* target = output;

    if (traceIndexer.getSampleCount() < getBulkIndex().getSizeK()) {
        const size_t c0 = std::max(k0, static_cast<size_t>(traceIndexer.getSampleIndex()));
        const size_t c1 = std::min(k1, static_cast<size_t>(traceIndexer.getSampleIndex() + traceIndexer.getSampleCount()));

        if (c0 >= c1) // no overlap
        {
            memset(output, 0, nk * sizeof(float));
            output += strideJ;
            return;
        }

        if (c0 > k0) // pad top
        {
            memset(target, 0, (c0 - k0) * sizeof(float));
            target += c0 - k0;
        }

        if (c1 < k1) // pad bottom
        {
            memset(target + c1 - c0, 0, (k1 - c1) * sizeof(float));
        }

        offset = traceIndexer.getOffset() + getByteCount(static_cast<size_t>(c0 - traceIndexer.getSampleIndex()));
        sampleCount = static_cast<size_t>(c1 - c0);
        byteCount = getByteCount(sampleCount);
    }

    uint8_t const* bytes = readAligned(offset, byteCount, sampleAlignment);
    if (bytes == nullptr) {
        memset(output, 0, nk * sizeof(float));
        return;
    }

    if (hasTraceWeights) {
        converter.convertScaled(bytes, target, sampleCount, traceIndexer.getTraceWeight());
    }
    else {
        converter.convert(bytes, target, sampleCount);
    }

    output += strideJ;
}

} // namespace segysdk::bulkaccess
