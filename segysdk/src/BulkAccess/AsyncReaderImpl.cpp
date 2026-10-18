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

#include "../BulkAccess/AsyncReaderImpl.h"
#include "../BulkAccess/ReaderConnector.h"
#include "../Semaphore/Semaphore.h"
#include "../Telemetry/Connector.h"

#include <future>
#include <map>

namespace segysdk::bulkaccess::async
{

ReaderImpl::ReaderImpl(ReaderConnector connector)
  : connector_(std::move(connector))
{
}

static auto
calcNumLines(const ExtentSpan& span, const std::size_t limit)
{
    return std::min(span.count_, limit - span.start_);
}

template<typename T>
std::future<T>
makeReadyStride(T&& buffer)
{
    auto promise = std::promise<T>{};
    promise.set_value(std::forward<T>(buffer));
    return promise.get_future();
}

Samples
ReaderImpl::doRead(const ReaderParams& params)
{
    const auto numInlines = calcNumLines(params.inline_, connector_->getSizeI());
    const auto numXlines = calcNumLines(params.xline_, connector_->getSizeJ());
    const auto numSamples = calcNumLines(params.samples_, connector_->getSizeK());
    const auto sampleSize = connector_->getSampleSize();
    const auto samplesOffset = sampleSize * params.samples_.start_;
    const auto numSamplesBytes = sampleSize * numSamples;
    auto strides = std::map<std::size_t, std::future<ByteBuffer>>{};
    const auto fileReadParams = fileio::FileReadParams{request::Params{params}.withTraceHeader(telemetry::getCurrentTraceHeader())};
    auto numTasksLimiter = Semaphore{params.numParallelTasks_};
    for (std::size_t i = params.inline_.start_; i < params.inline_.start_ + numInlines; i += params.inline_.step_) {
        for (std::size_t j = params.xline_.start_; j < params.xline_.start_ + numXlines; j += params.xline_.step_) {
            const auto strideKey = j * (numInlines * numSamples) + i * numSamples;
            if (connector_->findTrace(i, j)) {
                const auto offset = connector_->getTraceOffset() + samplesOffset;
                strides.try_emplace(
                    strideKey,
                    std::async([this, &fileReadParams, offset, numSamplesBytes, &numTasksLimiter]() {
                        const auto cs = CriticalSection{numTasksLimiter};
                        auto result = connector_->readSamples(
                            fileio::FileReadParams{fileReadParams}.withOffset(offset).withSize(numSamplesBytes));
                        result.resize(numSamplesBytes);
                        return result;
                    }));
            }
            else {
                strides.try_emplace(
                    strideKey,
                    makeReadyStride(ByteBuffer(numSamplesBytes)));
            }
        }
    }
    auto result = Samples{};
    for (auto& [key, value] : strides) {
        appendContainer(result, connector_->convertBytesToSamples(value.get()));
    }
    return result;
}

} // namespace segysdk::bulkaccess::async
