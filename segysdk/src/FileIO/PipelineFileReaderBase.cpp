/*
Copyright 2022-2024 Schlumberger

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

#include "../FileIO/PipelineFileReaderBase.h"

#include "../FileIO/BufferedFileReader.h"
#include "../FileIO/FileIOException.h"
#include "../FileIO/FileReader.h"
#include "../Telemetry/Connector.h"

#include <algorithm>
#include <cerrno>
#include <chrono>
#include <cstring>
#include <iterator>
#include <numeric>
#include <thread>
#include <utility>

namespace segysdk::fileio
{

PipelineFileReaderBase::PipelineFileReaderBase(const PipelineFileReaderParams& params)
  : readersCount_{params.readers_.size()},
    readersPool_(readersCount_)
{
    if (readersCount_ < minReadersCount) {
        throw FileIOException("Error: PipelineFileReaderBase has got too few readers");
    }

    std::transform(std::cbegin(params.readers_), std::cend(params.readers_), std::back_insert_iterator<ReaderUnitVector>(readerUnits_),
                   [](auto const& file) {
                       return ReaderUnit(std::make_shared<BufferedFileReader>(file), sleepDurationMs_);
                   });

    recentReader_ = readerUnits_.begin();

    initReadersPool(params);
}

PipelineFileReaderBase::~PipelineFileReaderBase()
{
    finishEveryReaderThread();
}

size_t
PipelineFileReaderBase::getBufferSize() const
{
    return totalBufferSize_;
}

size_t
PipelineFileReaderBase::getReadSize() const
{
    return totalReadSize_;
}

uint64_t
PipelineFileReaderBase::getFileSize() const
{
    return readerUnits_[0].getFileSize();
}

std::string const&
PipelineFileReaderBase::getFileName() const
{
    return readerUnits_[0].getFileName();
}

bool
PipelineFileReaderBase::isCacheHit(size_t offset, size_t size) const
{
    return std::any_of(readerUnits_.cbegin(), readerUnits_.cend(),
                       [&offset, &size](auto const& reader) {
                           return reader.isCacheHit(offset, size);
                       });
}

void
PipelineFileReaderBase::dropCache()
{
    std::for_each(readerUnits_.begin(), readerUnits_.end(),
                  [this](auto& reader) {
                      reader.dropCache();
                  });
}

uint8_t const*
PipelineFileReaderBase::getPointerToCached(uint64_t offset, size_t size) const
{
    const auto found = std::find_if(readerUnits_.cbegin(), readerUnits_.cend(),
                                    [&offset, &size](auto const& reader) {
                                        return reader.isCacheHit(offset, size);
                                    });

    if (readerUnits_.cend() == found) {
        throw FileIOException("Error: cache hit is false while getPointerToCached");
    }

    return found->getPointerToCached(offset, size);
}

size_t
PipelineFileReaderBase::getFirstCachedOffset() const
{
    auto const minElement = std::min_element(readerUnits_.cbegin(), readerUnits_.cend(),
                                             [](auto const& r1, auto const& r2) {
                                                 return r1.getRequestedCacheOffset() < r2.getRequestedCacheOffset();
                                             });
    return minElement->getRequestedCacheOffset();
}

size_t
PipelineFileReaderBase::getCachedSize() const
{
    // here the cached size means the total cached size,
    // it is expected that the overlapping portion is also taken into account
    return std::accumulate(readerUnits_.cbegin(), readerUnits_.cend(),
                           0U,
                           [](size_t cachedSize, auto const& reader) {
                               return cachedSize + reader.getCachedSize();
                           });
}

size_t
PipelineFileReaderBase::getReadersCount() const
{
    return readerUnits_.size();
}

void
PipelineFileReaderBase::initReadersPool(const request::Params& params)
{
    const auto threadRequestParams = request::Params{params}.withTraceHeader(telemetry::getCurrentTraceHeader());
    for (auto& reader : readerUnits_) {
        readersPool_.addWorker(std::thread{PipelineFileReaderBase::PrefetchThread,
                                           std::ref(reader),
                                           std::ref(isPrefetchThreadExit_),
                                           threadRequestParams});
    }
}

void
PipelineFileReaderBase::waitForReaderIsReady(ReaderUnit const& reader)
{
    static constexpr auto const sleepDuration = std::chrono::milliseconds(sleepDurationMs_);

    while (reader.isBusy()) {
        std::this_thread::sleep_for(sleepDuration);
    }
}

void
PipelineFileReaderBase::waitForEveryReaderIsReady()
{
    for (auto& reader : readerUnits_) {
        waitForReaderIsReady(reader);
    }
}

void
PipelineFileReaderBase::finishEveryReaderThread()
{
    isPrefetchThreadExit_ = true;
    for (auto& reader : readerUnits_) {
        reader.wakeUp();
    }
}

void
PipelineFileReaderBase::notifyReader(ReaderUnit& reader, details::RequestedCacheItem const& chunkRequest)
{
    reader.notifyReader(chunkRequest.first, chunkRequest.second);

    // let the just woken up thread to launch...
    std::this_thread::yield();
}

void
PipelineFileReaderBase::PrefetchThread(ReaderUnit& reader, std::atomic_bool& isExit, const request::Params& params)
try {
    do {
        // block this thread and wait for
        // the next load cache iteration
        bool const resumedToRead = reader.sleep();

        if (isExit) {
            break;
        }

        // load the cache synchronously
        if (resumedToRead) {
            reader.loadReaderCache(params);
        }

    } while (true);
}
catch (...) {
    // suppress any exceptions to keep the application reliable

    // clear the busy flag to prevent possible blockage
    reader.setIsBusy(false);
}

PipelineFileReaderParams&
PipelineFileReaderParams::withFileReaders(const FileReaders& value)
{
    readers_ = value;
    return *this;
}

} // namespace segysdk::fileio
