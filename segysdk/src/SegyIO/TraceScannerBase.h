/*
Copyright 2022 Schlumberger

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

#include "../Concurrent/BufferQueue.h"
#include "../Concurrent/ThreadPool.h"
#include "../FileIO/BufferedFileReaderBase.h"
#include "../FileIO/FileReader.h"
#include "../Request/Params.h"
#include "../SegyIO/DerivedHeader.h"
#include "../SegyIO/SegyDefinitionProvider.h"
#include "../SegyIO/SegyTraceScanner.h"
#include "../SegyIO/ValueConversion/HeaderValueArrayExtractor.h"
#include "../SegyIO/ValueConversion/SampleValueConverter.h"
#include "../Utilities/Environment.h"

#include <condition_variable>
#include <memory>
#include <mutex>
#include <set>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#define INVALID_PAGE_INDEX 0xFFFFFFFFFFFFFFFF

namespace segysdk::segyio
{

class SEGYSDK_DLL_PUBLIC TraceScannerBase : public SegyTraceScanner
{
private:
    // non-copyable - not implemented
    TraceScannerBase(const TraceScannerBase& other) = delete;
    TraceScannerBase& operator=(const TraceScannerBase& other) = delete;

protected:
    class HeaderCollector;

    struct PreFetchManager
    {
        PreFetchManager(const std::shared_ptr<fileio::FileReader>& fileReader, unsigned int lookahead_count);
        ~PreFetchManager();

        void lookAhead(size_t offset);
        bool isEnabled() { return (lookaheadCount_ > 0) && (pageSize_ > 0); }

    private:
        const static unsigned int LookaheadCountMultiplier = 16;
        const std::shared_ptr<fileio::FileReader> fileReader_;
        unsigned int lookaheadCount_;
        size_t pageSize_;
        std::set<size_t> isLoadingOrLoaded_;
        std::mutex cacheMutex_;
        std::vector<std::thread> cacheLoaders_;
        segysdk::concurrent::BufferQueue<size_t> pagesToLoadQueue_;
    };

protected:
    std::unique_ptr<valueconversion::HeaderValueArrayExtractor> extractor_;
    std::unique_ptr<fileio::BufferedFileReaderBase> reader_;
    std::unique_ptr<fileio::BufferedFileReaderBase> cacheReader_;
    std::shared_ptr<AsyncActivity> asyncActivity_;
    std::vector<std::string> traceHeaders_;
    std::shared_ptr<DerivedHeader> derived_;
    uint64_t fileSize_;
    uint64_t firstOffset_;
    uint64_t headerByteSize_;
    size_t defaultTraceSize_;
    size_t sampleSize_;
    std::shared_ptr<valueconversion::SampleValueConverter> sampleConverter_;

    bool hasVariableSampleCount_;

    size_t requestedCount_;
    int ixSampleCount_;
    std::unique_ptr<double[]> headerValues_;
    std::unique_ptr<float[]> samples_;
    uint64_t currentSampleCount_;
    uint64_t maxSampleCount_;
    int warningCount_{};
    uint64_t nextOffset_;
    uint64_t startTraceIndex;
    TraceInfo current_;
    bool hasTrace_;
    PreFetchManager preFetchManager_;

protected:
    void initTraceHeaders(SegyDefinitionProvider& provider, const std::vector<std::string>& headers);

    bool isCanceled() const;
    virtual bool hasVariableSampleCount() const { return hasVariableSampleCount_; }

    uint64_t getTraceSize(double value);
    virtual bool findNextTrace(const request::TraceParams& params, double* values) = 0;

protected:
    const std::vector<std::string>& getTraceHeaders() const { return traceHeaders_; }
    fileio::BufferedFileReaderBase* getReader() const { return reader_.get(); }

private:
    bool moveNextWithPrefetch(const request::TraceParams& params, bool prefetch = true);

public:
    TraceScannerBase(const std::shared_ptr<fileio::FileReader>& file, const std::shared_ptr<DerivedHeader>& derived, SegyDefinitionProvider& provider, const std::vector<std::string>& headers);
    virtual ~TraceScannerBase() override;

    virtual void setAsyncActivity(const std::shared_ptr<AsyncActivity>& activity) override;
    virtual void setReaderBufferSize(size_t bufferSize) const override;

    virtual bool moveNext(const request::TraceParams& params) override;
    virtual void reset() override;
    virtual void reset(const request::TraceParams& params) override;

    virtual bool readAt(uint64_t traceIndex) override;

    virtual uint64_t getTraceIndex() const override { return current_.index_; }
    virtual const TraceInfo& getTraceInfo() const override { return current_; }
    virtual const double* getHeaderValues() const override;
    virtual const float* getTraceSamples() const override;
    virtual uint64_t getTraceSampleCount() const override;
    virtual uint64_t getHeaderValueCount() const override { return requestedCount_; }
};

} // namespace segysdk::segyio
