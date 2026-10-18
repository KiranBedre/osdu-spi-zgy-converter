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

#include "TraceScannerBase.h"

#include "DerivedHeader.h"
#include "Primitives.h"
#include "SegyDefinitionProvider.h"

#include "../SegyIO/SegyIOException.h"

#include <algorithm>
#include <stdexcept>

namespace segysdk::segyio
{

using valueconversion::HeaderValueArrayExtractor;

//********************************************************************************
// TraceScannerBase::HeaderCollector

class TraceScannerBase::HeaderCollector
{
private:
    SegyDefinitionProvider& provider_;
    std::vector<std::string>& headers_;

public:
    HeaderCollector(SegyDefinitionProvider& headerProvider, std::vector<std::string>& headers)
      : provider_(headerProvider), headers_(headers)
    {}

private:
    int getIndex(const std::string& header) const
    {
        const auto it = std::find(headers_.begin(), headers_.end(), header);
        if (it == headers_.end())
            return -1;

        return static_cast<int>(it - headers_.begin());
    }

    bool isValid(const std::string& header) const
    {
        return provider_.getTraceHeaderDefinition(header) != nullptr;
    }

    int addValidatedHeader(const std::string& header) const
    {
        int pos = getIndex(header);
        if (pos < 0) {
            pos = static_cast<int>(headers_.size());
            headers_.push_back(header);
        }

        return pos;
    }

public:
    int addRequiredHeader(const std::string& header) const
    {
        if (!isValid(header))
            throw SegyFormatException("Missing definition of required trace header: " + header);

        return addValidatedHeader(header);
    }

    void addRequiredHeaders(const std::vector<std::string>& headers) const
    {
        for (auto it = headers.begin(); it != headers.end(); ++it) {
            addRequiredHeader(*it);
        }
    }

    int addOptionalHeader(const std::string& header) const
    {
        if (!isValid(header))
            return -1;

        return addValidatedHeader(header);
    }
};

//********************************************************************************
// TraceScannerBase

TraceScannerBase::TraceScannerBase(const std::shared_ptr<fileio::FileReader>& fileReader, const std::shared_ptr<DerivedHeader>& derived, SegyDefinitionProvider& headerProvider, const std::vector<std::string>& headers)
  : SegyTraceScanner(),
    derived_(derived),
    fileSize_(fileReader->getFileSize()),
    firstOffset_(derived->getFirstTraceOffset()),
    headerByteSize_(derived->getTraceHeaderSize()),
    defaultTraceSize_(Primitives::getTraceSize(derived->getSampleFormat(), derived->getDefaultSampleCount())),
    sampleSize_(derived->getSampleSize()),
    sampleConverter_(valueconversion::SampleValueConverter::create(derived->getSampleFormat(), derived->isByteSwapped())),
    hasVariableSampleCount_(!derived->hasFixedSampleCount()),
    requestedCount_(headers.size()),
    currentSampleCount_(derived->getDefaultSampleCount()),
    maxSampleCount_(derived->getDefaultSampleCount()),
    preFetchManager_(fileReader, utilities::Environment::instance().scannnerLookAhead())
{
    reset();
    initTraceHeaders(headerProvider, headers);
}

TraceScannerBase::~TraceScannerBase()
{
}

void
TraceScannerBase::initTraceHeaders(SegyDefinitionProvider& headerProvider, const std::vector<std::string>& headers)
{
    const HeaderCollector collector(headerProvider, traceHeaders_);
    collector.addRequiredHeaders(headers);

    ixSampleCount_ = -1;
    if (hasVariableSampleCount_) {
        ixSampleCount_ = collector.addRequiredHeader("TraceLength");
    }
}

bool
TraceScannerBase::isCanceled() const
{
    return (asyncActivity_ != nullptr && asyncActivity_->isCanceled());
}

uint64_t
TraceScannerBase::getTraceSize(double value)
{
    // For the current version provide this simple hack to recover from bad values for number of samples per trace
    // Moving on, we should be able to do some more sophisticated sanity checks looking ahead to see if we can get
    // realigned with expected values for inlines/crosslines or SP/CDP values.
    // Current strategy is to look at the highest value of samples per trace encountered so far, and accept a value
    // up to 3 times higher.
    // We decided not to implement a check for lower values as it is possible someone might have created a file with
    // certain traces having either very few or even zero samples.

    // Test to validate count of number of samples per trace. If sample count is invalid, for now skip the entire trace.
    // We probably should log this to the user but without causing too much noise that it is impossible to stop the job.
    // Another override to force the loader to respect the trace header values for number of samples might also be a good idea.

    auto sampleCount = static_cast<uint64_t>(value);

    if ((sampleCount / 3 > maxSampleCount_) || (sampleCount > std::numeric_limits<short>::max()) || (sampleCount <= 0)) // Ouch
    {
        sampleCount = derived_->getDefaultSampleCount();
        warningCount_++;
        if (warningCount_ < 5) {
            // ToDo:
            // Figure out how to get the following line of code work:
            // Slb::Ocean::Petrel::PetrelLogger::Info("Illegal number of samples for trace " + currentSampleCount_  +  " trace at offset= " + current_.offset_ + " will be skipped.");
        }
    }
    else if (sampleCount > maxSampleCount_) {
        maxSampleCount_ = sampleCount;
    }

    return headerByteSize_ + sampleCount * sampleSize_;
}

bool
TraceScannerBase::moveNext(const request::TraceParams& params)
{
    return moveNextWithPrefetch(params);
}

void
TraceScannerBase::reset()
{
    hasTrace_ = false;
    current_.offset_ = 0;
    current_.index_ = 0;
    current_.size_ = defaultTraceSize_;
    nextOffset_ = firstOffset_;
    startTraceIndex = 0;
}

void
TraceScannerBase::reset(const request::TraceParams& params)
{
    if (params.startTrace_ == 0)
        return reset();

    if (current_.index_ == params.startTrace_ - 1)
        return;

    if (hasVariableSampleCount()) {
        if (current_.index_ > params.startTrace_) {
            reset();
        }

        // TODO: make a faster spooler for this case
        while (moveNext(params) && current_.index_ < params.startTrace_ - 1) {
            /* EMPTY */
        }
    }
    else {
        hasTrace_ = false;
        current_.offset_ = 0;
        current_.index_ = 0;

        nextOffset_ = firstOffset_ + params.startTrace_ * defaultTraceSize_;
        startTraceIndex = params.startTrace_;
    }
}

bool
TraceScannerBase::readAt(uint64_t traceIndex)
{
    reset(request::TraceParams{traceIndex});
    return moveNextWithPrefetch({}, false);
}

const double*
TraceScannerBase::getHeaderValues() const
{
    if (hasTrace_)
        return headerValues_.get();

    return nullptr;
}

const float*
TraceScannerBase::getTraceSamples() const
{
    if (samples_)
        return samples_.get();

    return nullptr;
}

uint64_t
TraceScannerBase::getTraceSampleCount() const
{
    return currentSampleCount_;
}

void
TraceScannerBase::setAsyncActivity(const std::shared_ptr<AsyncActivity>& activity)
{
    asyncActivity_ = activity;
}

void
TraceScannerBase::setReaderBufferSize(size_t bufferSize) const
{
    if (bufferSize < SegyTraceScanner::MinBufferSize) {
        // Limit min read buffer size
        bufferSize = SegyTraceScanner::MinBufferSize;
    }
    else if (bufferSize > SegyTraceScanner::MaxBufferSize) {
        // Limit max read buffer size
        bufferSize = SegyTraceScanner::MaxBufferSize;
    }
    reader_->setBufferSize(bufferSize);
}

bool
TraceScannerBase::moveNextWithPrefetch(const request::TraceParams& params, bool prefetch)
{
    if (current_.offset_ >= fileSize_) {
        hasTrace_ = false;
        return false;
    }

    if (current_.offset_ == 0) {
        current_.offset_ = nextOffset_;
        current_.index_ = startTraceIndex;
    }
    else {
        ++current_.index_;
    }

    if (prefetch) {
        preFetchManager_.lookAhead(current_.offset_);
    }

    if (!findNextTrace(params, headerValues_.get()))
        return false;

    if (isCanceled()) {
        reset();
        return false;
    }

    return true;
}

TraceScannerBase::PreFetchManager::PreFetchManager(const std::shared_ptr<fileio::FileReader>& fileReader, unsigned int lookahead_count)
  : fileReader_(fileReader),
    lookaheadCount_(lookahead_count),
    pageSize_(segysdk::utilities::Environment::instance().sdReadCachePageSize()),
    isLoadingOrLoaded_(),
    cacheMutex_(),
    cacheLoaders_(),
    pagesToLoadQueue_(lookahead_count * LookaheadCountMultiplier)
{
    if (isEnabled()) {
        auto cacheLoaderJob = [this]() {
            size_t pageIndex = INVALID_PAGE_INDEX;
            std::vector<uint8_t> buffer(pageSize_);
            while (pagesToLoadQueue_.Consume(pageIndex) && pageIndex != INVALID_PAGE_INDEX) {
                fileReader_->read(fileio::FileReadParams{}.withOffset(pageIndex * pageSize_).withSize(pageSize_));
            }
        };

        for (auto i = 0U; i < segysdk::utilities::Environment::instance().scannnerLookAheadThreads(); i++) {
            cacheLoaders_.push_back(std::thread(cacheLoaderJob));
        }
    }
}

TraceScannerBase::PreFetchManager::~PreFetchManager()
{
    if (isEnabled()) {
        auto loaderCount = cacheLoaders_.size();

        for (size_t i = 0; i < loaderCount; i++) {
            size_t dismiss_loader = INVALID_PAGE_INDEX;
            pagesToLoadQueue_.Produce(dismiss_loader);
        }

        for (size_t i = 0; i < loaderCount; i++) {
            cacheLoaders_[i].join();
        }
    }
}

void
TraceScannerBase::PreFetchManager::lookAhead(size_t offset)
{
    std::unique_lock<std::mutex> lock{cacheMutex_};
    if (isEnabled()) {
        size_t initialPageIndex = (offset / pageSize_) + 1;
        for (size_t pageIndex = initialPageIndex; pageIndex < (initialPageIndex + lookaheadCount_); pageIndex++) {
            const auto& it = isLoadingOrLoaded_.find(pageIndex);
            if (it == isLoadingOrLoaded_.end()) { //
                isLoadingOrLoaded_.insert(pageIndex);
                pagesToLoadQueue_.Produce(pageIndex);
            }
        }
    }
}

} // namespace segysdk::segyio
