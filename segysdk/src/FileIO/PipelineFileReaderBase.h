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

#include "../Concurrent/ThreadPool.h"
#include "../FileIO/BufferedFileReaderBase.h"
#include "../FileIO/FileReader.h"
#include "../FileIO/Utilities/ReaderUnit.h"

#include <atomic>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace segysdk::fileio
{

namespace details
{
using ThreadPool = segysdk::concurrent::ThreadPool;
using RequestedCacheItem = std::pair<size_t, size_t>;
} // namespace details

using FileReaders = std::vector<std::shared_ptr<fileio::FileReader>>;

struct SEGYSDK_DLL_PUBLIC PipelineFileReaderParams : public request::Params
{
    PipelineFileReaderParams& withFileReaders(const FileReaders& value);
    FileReaders readers_{};
};

class SEGYSDK_DLL_PUBLIC PipelineFileReaderBase : public BufferedFileReaderBase
{
public:
    using ReaderUnit = details::ReaderUnit;
    using ReaderUnitVector = std::vector<ReaderUnit>;
    static constexpr size_t const minReadersCount = 3U;

public:
    explicit PipelineFileReaderBase(const PipelineFileReaderParams& params);
    virtual ~PipelineFileReaderBase() override;

    virtual size_t getBufferSize() const override;
    virtual size_t getReadSize() const override;
    virtual uint64_t getFileSize() const override;
    virtual std::string const& getFileName() const override;
    virtual bool isCacheHit(size_t offset, size_t size) const override;
    virtual void dropCache() override;
    virtual uint8_t const* getPointerToCached(uint64_t offset, size_t size) const override;
    virtual size_t getFirstCachedOffset() const override;
    virtual size_t getCachedSize() const override;
    virtual size_t getReadersCount() const override;

protected:
    PipelineFileReaderBase(const PipelineFileReaderBase&) = delete;
    PipelineFileReaderBase(PipelineFileReaderBase&&) noexcept = delete;
    PipelineFileReaderBase& operator=(const PipelineFileReaderBase&) = delete;
    PipelineFileReaderBase& operator=(PipelineFileReaderBase&&) = delete;

    void initReadersPool(const request::Params& params);
    void waitForReaderIsReady(ReaderUnit const& reader);
    void waitForEveryReaderIsReady();
    void finishEveryReaderThread();
    void notifyReader(ReaderUnit& reader, details::RequestedCacheItem const& chunkRequest);

    static void PrefetchThread(ReaderUnit& reader, std::atomic_bool& isExit, const request::Params& params);

protected:
    size_t const readersCount_;
    ReaderUnitVector readerUnits_;
    ReaderUnitVector::iterator recentReader_;
    details::ThreadPool readersPool_;
    size_t totalBufferSize_{};
    size_t totalReadSize_{};
    size_t readerBufferSize_{};
    size_t readerReadSize_{};
    std::atomic_bool isPrefetchThreadExit_{false};

    static constexpr size_t const sleepDurationMs_ = 10U;
};

} // namespace segysdk::fileio
