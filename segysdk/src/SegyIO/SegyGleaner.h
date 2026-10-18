/*
Copyright 2021 Schlumberger

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

#include "../SegyIO/SampleFormat.h"
#include "../SegyIO/HeaderDefinitionProvider.h"
#include "../FileIO/FileReader.h"
#include "../FileIO/FileReference.h"
#include "../SegyIO/DerivedHeader.h"

#include <memory>
#include <string>

namespace segysdk::segyio
{

class SEGYSDK_DLL_PUBLIC SegyGleaner
{
public:
    //*************************************************************************
    class Results
    {
    public:
        //*************************************************
        template<typename T>
        class Extent
        {
        private:
            T first_ = 0;
            T increment_ = 0;
            int count_ = 0;

        public:
            Extent() {}
            Extent(T first, T increment, int count)
              : first_(first), increment_(increment), count_(count) {}

            T getFirst() const { return first_; }
            T getIncrement() const { return increment_; }
            int getCount() const { return count_; }

            T getAt(int index) const { return first_ + index * increment_; }
        };
        //*************************************************

    private:
        Extent<int> inline_;
        Extent<int> crossline_;
        Extent<double> sample_;
        SampleFormat sampleFormat_ = SampleFormat::INT3;

    public:
        Results() {}
        Results(const Extent<int>& iline, const Extent<int>& xline, const Extent<double>& sample, SampleFormat sampleFormat)
          : inline_(iline), crossline_(xline), sample_(sample), sampleFormat_(sampleFormat) {}

        const Extent<int>& getInlineExtent() const { return inline_; }
        const Extent<int>& getCrosslineExtent() const { return crossline_; }
        const Extent<double>& getSampleExtent() const { return sample_; }
        SampleFormat getSampleFormat() const { return sampleFormat_; }

        int getInlineAt(uint64_t traceIndex) const { return inline_.getAt(static_cast<int>(traceIndex / crossline_.getCount())); }
        int getCrosslineAt(uint64_t traceIndex) const { return crossline_.getAt(static_cast<int>(traceIndex % crossline_.getCount())); }
    };
    //*************************************************************************

private:
    class TraceCollector;

    std::shared_ptr<fileio::FileReader> file_;
    std::shared_ptr<DerivedHeader> derived_;
    std::shared_ptr<HeaderDefinitionProvider> provider_;
    std::shared_ptr<Results> results_;

    void initialize();
    bool validateSampleWindow() const { return false; }

public:
    explicit SegyGleaner(std::shared_ptr<fileio::FileReader> file);
    explicit SegyGleaner(const fileio::FileReference& fileReference);

    Results getResults() const;

    // throws on problems
    bool glean();

    // sets message and returns false on problems
    bool glean(std::string& errorMessage);
};

} // namespace segysdk::segyio
