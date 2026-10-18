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

#include "../FileIO/BufferedFileReader.h"
#include "../FileIO/MultiBufferedFileReader.h"
#include "../SegyIO/SampleFormat.h"
#include "../SegyIO/SegyIOException.h"
#include "../SegyIndex/SegyIndex.h"

#include <cstddef>
#include <memory>

namespace segysdk::bulkaccess
{
using segyio::SampleFormat;

class SEGYSDK_DLL_PUBLIC BulkIndex
{
public:
    // ************************************************************
    class TraceIndexer
    {
    private:
        uint16_t fileIndex_{};
        size_t offset_{};
        int sampleIndex_{};
        size_t sampleCount_{};
        float traceWeight_{1.0F};

    protected:
        explicit TraceIndexer(size_t sampleCount);
        virtual ~TraceIndexer();

        void setFileIndex(uint16_t fileIndex) { fileIndex_ = fileIndex; }
        void setOffset(size_t offset) { offset_ = offset; }
        void setSampleIndex(int sampleIndex) { sampleIndex_ = sampleIndex; }
        void setSampleCount(size_t sampleCount) { sampleCount_ = sampleCount; }
        void setTraceWeight(float traceWeight) { traceWeight_ = traceWeight; }

    public:
        uint16_t getFileIndex() const { return fileIndex_; }
        size_t getOffset() const { return offset_; }
        int getSampleIndex() const { return sampleIndex_; }
        size_t getSampleCount() const { return sampleCount_; }
        float getTraceWeight() const { return traceWeight_; }

        virtual bool findTrace(size_t i, size_t j) = 0;
        virtual bool findTraceByIndex(size_t i, size_t j) = 0;

        virtual bool findTraceByCmp([[maybe_unused]] int cmp)
        {
            throw segyio::SegyRuntimeException("Should not be called for 3D Segy Index");
        }

        virtual bool findTraceByTraceNo([[maybe_unused]] size_t traceNo)
        {
            throw segyio::SegyRuntimeException("Should not be called for 3D Segy Index");
        }

        virtual size_t getTraceNoForCMP([[maybe_unused]] int cmp, [[maybe_unused]] bool findNextClosest = false)
        {
            throw segyio::SegyRuntimeException("Should not be called for 3D Segy Index");
        }
    };
    // ************************************************************

private:
    size_t sizeI_{};
    size_t sizeJ_{};
    size_t sizeK_{};
    size_t tracesCount_{};
    SampleFormat sampleFormat_{SampleFormat::INT3};
    bool isByteSwapped_{true};
    bool hasTraceWeights_{false};
    bool isCrosslineOrdered_{false};

protected:
    BulkIndex();
    BulkIndex(size_t sizeI, size_t sizeJ, size_t sizeK, SampleFormat sampleFormat);
    virtual ~BulkIndex();

    void setSize(size_t sizeI, size_t sizeJ, size_t sizeK)
    {
        sizeI_ = sizeI;
        sizeJ_ = sizeJ;
        sizeK_ = sizeK;
    }
    void setSamplesCount(size_t sampleCount);
    void setTracesCount(size_t tracesCount);
    void setSampleFormat(SampleFormat sampleFormat) { sampleFormat_ = sampleFormat; }
    void setByteSwapped(bool isByteSwapped) { isByteSwapped_ = isByteSwapped; }
    void setTraceWeighting(bool hasTraceWeights) { hasTraceWeights_ = hasTraceWeights; }
    void setCrosslineOrdered(bool isCrosslineOrdered) { isCrosslineOrdered_ = isCrosslineOrdered; }

public:
    size_t getSizeI() const { return sizeI_; }
    size_t getSizeJ() const { return sizeJ_; }
    size_t getSizeK() const { return sizeK_; }
    size_t getSamplesCount() const { return sizeK_; }
    size_t getTracesCount() const { return tracesCount_; }
    SampleFormat getSampleFormat() const { return sampleFormat_; }
    bool isByteSwapped() const { return isByteSwapped_; }
    bool hasTraceWeights() const { return hasTraceWeights_; }
    bool isCrosslineOrdered() const { return isCrosslineOrdered_; }

    virtual std::shared_ptr<BulkIndex::TraceIndexer> createTraceIndexer(std::shared_ptr<fileio::BufferedFileReaderBase> reader) = 0;
};

} // namespace segysdk::bulkaccess
