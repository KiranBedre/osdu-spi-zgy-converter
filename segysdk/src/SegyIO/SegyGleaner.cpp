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

#include "SegyGleaner.h"

#include "DerivedHeader.h"
#include "FileHeaderDefinitionFactory.h"
#include "Primitives.h"
#include "SegyIOException.h"
#include "SegyTraceScanner.h"
#include "TraceScanner.h"

#include "../FileIO/GenericFileManager.h"

#include <map>
#include <stdexcept>
#include <utility>
#include <vector>

namespace segysdk::segyio
{

using segysdk::fileio::GenericFileManager;
using segysdk::segyformat::external::SegyContext;

//********************************************************************************
// SegyGleaner::TraceCollector

class SegyGleaner::TraceCollector
{
public:
    //*************************************************
    struct Trace
    {
        size_t traceIndex_{0};
        int inline_{0};
        int crossline_{0};
        int sampleCount_{0};
        double firstSample_{0};
        double sampleRate_{0};

        Trace()
        {}

        Trace(size_t traceIndex, const double* values)
          : traceIndex_(traceIndex),
            inline_(static_cast<int>(values[0])),
            crossline_(static_cast<int>(values[1])),
            sampleCount_(static_cast<int>(values[2])),
            firstSample_(values[3]),
            sampleRate_(values[4])
        {
        }
    };
    //*************************************************

private:
    SegyTraceScanner& scanner_;
    std::map<size_t, Trace> traces_;
    size_t sampleCount_;

public:
    TraceCollector(SegyTraceScanner& scanner, size_t sampleCount)
      : scanner_(scanner),
        sampleCount_(sampleCount)
    {
    }

    void validateSampleCount(const Trace& trace) const
    {
        if (static_cast<std::size_t>(trace.sampleCount_) == sampleCount_)
            return;

        throw SegyFormatException("Variable trace length detected, full scan required. Expected " + std::to_string(sampleCount_) + " got " + std::to_string(trace.sampleCount_));
    }

    Trace getTrace(size_t traceIndex)
    {
        const auto entry = traces_.find(traceIndex);
        if (entry != traces_.end())
            return entry->second;

        return Trace();
    }

    bool collectTraces(size_t firstTraceIndex, size_t count)
    {
        scanner_.reset(request::TraceParams{firstTraceIndex});
        for (size_t i = 0; i < count; ++i) {
            if (!scanner_.moveNext({}))
                return false;

            size_t traceIndex = scanner_.getTraceIndex();
            // Ignore overlapping request
            if (traces_.find(traceIndex) != traces_.end())
                continue;

            Trace trace(traceIndex, scanner_.getHeaderValues());
            validateSampleCount(trace);

            traces_[traceIndex] = trace;
        }

        return true;
    }

    void validateTraces(const Results& results) const
    {
        for (const auto& [id, trace] : traces_) {
            if (trace.inline_ != results.getInlineAt(trace.traceIndex_) || trace.crossline_ != results.getCrosslineAt(trace.traceIndex_))
                throw SegyFormatException("Non-trivial sorting, full scan required.");
        }
    }

    void validateSampleWindows(double firstSample, double sampleRate) const
    {
        for (const auto& [id, trace] : traces_) {
            if (trace.firstSample_ != firstSample || trace.sampleRate_ != sampleRate)
                throw SegyFormatException("Variable sample window detected.");
        }
    }

    std::shared_ptr<Results> determineResults(const Trace& t0, const Trace& t1, const Trace& tn, uint64_t traceCount, SampleFormat sampleFormat) const
    {
        const int xlineStart = t0.crossline_;
        const int xlineStep = t1.crossline_ - t0.crossline_;
        if (xlineStep == 0)
            throw SegyFormatException("Non-trivial sorting.");

        if ((tn.crossline_ - t0.crossline_) % xlineStep != 0)
            throw SegyFormatException("Non-trivial sorting or missing traces.");

        const int xlineCount = (tn.crossline_ - t0.crossline_) / xlineStep + 1;
        if (xlineCount < 2)
            throw SegyFormatException("Non-trivial sorting.");

        if (traceCount % xlineCount != 0)
            throw SegyFormatException("Non-trivial sorting or missing traces.");

        const int inlineStart = t0.inline_;
        const auto inlineCount = static_cast<int>((traceCount / xlineCount));
        if (inlineCount < 2)
            throw SegyFormatException("Non-trivial sorting.");

        const int inlineStep = (tn.inline_ - t0.inline_) / (inlineCount - 1);
        if (inlineStep == 0)
            throw SegyFormatException("Non-trivial sorting.");

        if ((tn.inline_ - t0.inline_) % inlineStep != 0)
            throw SegyFormatException("Non-trivial sorting or missing traces.");

        if (xlineStart + (xlineCount - 1) * xlineStep != tn.crossline_)
            throw SegyFormatException("Non-trivial sorting or missing traces.");

        if (inlineStart + (inlineCount - 1) * inlineStep != tn.inline_)
            throw SegyFormatException("Non-trivial sorting or missing traces.");

        Results::Extent JVectorExtent(inlineStart, inlineStep, inlineCount);
        Results::Extent IVectorExtent(xlineStart, xlineStep, xlineCount);
        Results::Extent sampleExtent(t0.firstSample_, t0.sampleRate_, t0.sampleCount_);

        return std::make_shared<Results>(JVectorExtent, IVectorExtent, sampleExtent, sampleFormat);
    }

    static std::vector<std::string> getHeaderNames()
    {
        std::vector<std::string> headerNames;
        headerNames.emplace_back("Inline");
        headerNames.emplace_back("Crossline");
        headerNames.emplace_back("TraceLength");
        headerNames.emplace_back("ShotToRecordingLagTime");
        headerNames.emplace_back("SampleRate");

        return headerNames;
    }
};

//********************************************************************************
// SegyGleaner

SegyGleaner::SegyGleaner(const fileio::FileReference& fileReference)
  : file_(GenericFileManager::createReader(fileio::FileManagerParams{fileReference}))
{
    initialize();
}

SegyGleaner::SegyGleaner(std::shared_ptr<fileio::FileReader> file)
  : file_(std::move(file))
{
    initialize();
}

void
SegyGleaner::initialize()
{
    const auto context = std::make_shared<SegyContext>();
    auto binaryHdrs = SegyContext::getSegyStandardBinaryHeaderDefinitions();
    context->setBinaryHeaderDefinitions(binaryHdrs);

    const FileHeaderDescriptionFactory factory(context);
    auto binaryHeaders = factory.Generate_binaryHeaders();

    const auto traceHeaderOverrides = factory.GetFileHeaderDesctription()->getTraceHeaderOverrides();

    derived_ = std::make_shared<DerivedHeader>(file_, binaryHeaders, traceHeaderOverrides);

    // Variable sample count is not supported yet, but give it a try in case it is fixed after all
    if (!derived_->hasFixedSampleCount()) {
        derived_->setFixedSampleCount(true);
    }

    if (derived_->getRevisionLevel() == 0) {
        auto traceHdrs = SegyContext::getSegyRev0Locations();
        context->setTraceHeaderDefinitions(traceHdrs);
    }
    else {
        auto traceHdrs = SegyContext::getSegyRev1TraceHdrLocations();
        context->setTraceHeaderDefinitions(traceHdrs);
    }

    auto traceHeaders = factory.Generate_traceHeaders();
    provider_ = std::make_shared<HeaderDefinitionProvider>(traceHeaders);
}

SegyGleaner::Results
SegyGleaner::getResults() const
{
    if (results_ != nullptr)
        return *results_;

    return Results();
}

bool
SegyGleaner::glean()
{
    results_.reset();

    const uint64_t fileSize = derived_->getFileSize();
    const auto sampleCount = derived_->getDefaultSampleCount();
    const size_t traceSize = Primitives::getTraceSize(derived_->getSampleFormat(), sampleCount);
    const uint64_t traceCount = (fileSize - derived_->getFirstTraceOffset()) / traceSize;
    const uint64_t batchCount = std::min(static_cast<uint64_t>(16ull), traceCount);

    if (traceCount < 4)
        throw SegyFormatException("Unexpected file size, only found " + std::to_string(traceCount) + " traces.");

    if ((fileSize - derived_->getFirstTraceOffset()) % traceSize != 0)
        throw SegyFormatException("Unexpected file size, assuming variable trace size.");

    TraceScanner scanner = TraceScanner(file_, derived_, *provider_, TraceCollector::getHeaderNames());
    TraceCollector collector(scanner, sampleCount);

    collector.collectTraces(0, batchCount);
    collector.collectTraces(traceCount - batchCount, batchCount);

    const auto trace0 = collector.getTrace(0);
    const auto trace1 = collector.getTrace(1);
    const auto traceN = collector.getTrace(traceCount - 1);

    const auto results = collector.determineResults(trace0, trace1, traceN, traceCount, derived_->getSampleFormat());

    // So far, so good. Collect a few additional samples for verification
    collector.collectTraces(traceCount / 3, batchCount);
    collector.collectTraces(traceCount * 2 / 3, batchCount);

    collector.validateTraces(*results);

    if (validateSampleWindow()) {
        collector.validateSampleWindows(trace0.firstSample_, trace0.sampleRate_);
    }

    results_ = results;

    return true;
}

bool
SegyGleaner::glean(std::string& errorMessage)
{
    try {
        return glean();
    }
    catch (const std::runtime_error& e) {
        errorMessage = e.what();
        return false;
    }
}

} // namespace segysdk::segyio
