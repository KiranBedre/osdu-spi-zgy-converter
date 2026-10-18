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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include <string>
#include <vector>

namespace segysdk::statistics_generator
{

class SEGYSDK_DLL_PUBLIC StatisticsInputParams
{

public:
    StatisticsInputParams() = default;
    StatisticsInputParams(int errorCode);
    explicit StatisticsInputParams(std::string fileCollectionSegyId, std::string seismicTraceDataId, std::string workProductId, bool forceOverwrite = false, bool verbose = false);
    explicit StatisticsInputParams(std::string fileCollectionSegyId, std::string seismicTraceDataId, std::string workProductId, bool forceOverwrite, bool verbose, int exitCode);

public:
    std::string getOsduPartition() const { return osduPartition_; }
    void setOsduPartition(const std::string& value) { osduPartition_ = value; }

    std::string getCorrelationId() const { return correlationId_; }
    void setCorrelationId(const std::string& value) { correlationId_ = value; }

    std::vector<std::string> getSEGYFileList() const { return segyFileList_; }
    void setSEGYFileList(std::vector<std::string> segyFileList);

    std::string getFileCollectionSegyId() const { return osduFileCollectionSegyId_; }
    std::string getSeismicTraceDataId() const { return osduSeismicTraceDataId_; }
    void setSeismicTraceDataId(const std::string& value) { osduSeismicTraceDataId_ = value; }

    std::string getWorkProductId() const { return osduWorkProductId_; }

    size_t getHistogramBucketCount() const;
    size_t getHistogramBucketCountAt(size_t ix) const;
    void setHistogramBucketCounts(std::vector<size_t> histogramBucketCount);

    bool getVerbosity() const { return verbosity_; }
    bool getForceOverwrite() const { return forceOverwrite_; }

    int getExitCode() const { return errorCode_; }
    void setExitCode(const int value) { errorCode_ = value; }

    bool IsValid();

    std::vector<std::string> getErrorList() const { return errorList_; }

private:
    void validateParameters();

    std::string retrieveSeismicTraceData(std::string& osduWorkProductId);
    static std::string getPartitionFromOsduId(const std::string& osduId);
    static std::string getPartitionFromOsduId(const std::vector<std::string>& osduIds);

private:
    std::string osduPartition_{};
    std::string correlationId_{};
    std::vector<std::string> segyFileList_{};
    std::string osduFileCollectionSegyId_{};
    std::string osduSeismicTraceDataId_{};
    std::string osduWorkProductId_{};

    // Bucket sizes for generated histograms
    std::vector<size_t> histogramBucketCount_ = {255, 1023, 4095};

    bool forceOverwrite_ = false;
    bool verbosity_ = true;

    int errorCode_ = 0;
    std::vector<std::string> errorList_{};

    inline static const std::string DefaultPartition = "***NotDefined***";
};

} // namespace segysdk::statistics_generator
