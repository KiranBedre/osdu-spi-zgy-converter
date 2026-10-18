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

#include "StatisticsInputParams.h"
#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"
#include "../OsduMetaDataUtilities/OsduWorkProduct.h"
#include "../OsduMetaDataUtilities/OsduSeismicTraceDataStruct.h"
#include "../Utilities/JsonUtilities.h"

namespace segysdk::statistics_generator
{

StatisticsInputParams::StatisticsInputParams(const int errorCode)
  : errorCode_(errorCode)
{
    validateParameters();
}

StatisticsInputParams::StatisticsInputParams(std::string fileCollectionSegyId, std::string seismicTraceDataId, std::string workProductId, const bool forceOverwrite, const bool verbose)
  : osduFileCollectionSegyId_(std::move(fileCollectionSegyId)),
    osduSeismicTraceDataId_(std::move(seismicTraceDataId)),
    osduWorkProductId_(std::move(workProductId)),
    forceOverwrite_(forceOverwrite),
    verbosity_(verbose)
{
    osduPartition_ = getPartitionFromOsduId({osduFileCollectionSegyId_, osduSeismicTraceDataId_, osduWorkProductId_});
    validateParameters();
}

StatisticsInputParams::StatisticsInputParams(std::string fileCollectionSegyId, std::string seismicTraceDataId, std::string workProductId, const bool forceOverwrite, const bool verbose, int exitCode)
  : osduFileCollectionSegyId_(std::move(fileCollectionSegyId)),
    osduSeismicTraceDataId_(std::move(seismicTraceDataId)),
    osduWorkProductId_(std::move(workProductId)),
    forceOverwrite_(forceOverwrite),
    verbosity_(verbose),
    errorCode_(exitCode)
{
    osduPartition_ = getPartitionFromOsduId({osduFileCollectionSegyId_, osduSeismicTraceDataId_, osduWorkProductId_});
    validateParameters();
}


void
StatisticsInputParams::setSEGYFileList(std::vector<std::string> segyFileList)
{
    segyFileList_ = std::move(segyFileList);
}

size_t
StatisticsInputParams::getHistogramBucketCount() const
{
    return histogramBucketCount_.size();
}

size_t
StatisticsInputParams::getHistogramBucketCountAt(const size_t ix) const
{
    if (ix < getHistogramBucketCount()) {
        return histogramBucketCount_.at(ix);
    }

    return 0;
}

void
StatisticsInputParams::setHistogramBucketCounts(std::vector<size_t> histogramBucketCount)
{
    histogramBucketCount_ = std::move(histogramBucketCount);
}

std::string
StatisticsInputParams::getPartitionFromOsduId(const std::string& osduId)
{
    if (not osduId.empty()) {
        const auto idSection = segysdk::metadata::osdu::OsduMetadataHelper::parseReferenceAttribute(osduId);
        if (idSection.size() >= 3 and !idSection[0].empty()) {
            return idSection[0];
        }
    }

    return "";
}

std::string
StatisticsInputParams::getPartitionFromOsduId(const std::vector<std::string>& osduIds)
{
    for (const auto& osduId : osduIds) {
        if (osduId.empty()) {
            continue;
        }

        const auto partition = getPartitionFromOsduId(osduId);
        if (!partition.empty()) {
            return partition;
        }
    }

    return DefaultPartition;
}

bool
StatisticsInputParams::IsValid()
{
    validateParameters();

    return not osduPartition_.empty() and not osduFileCollectionSegyId_.empty() and not osduSeismicTraceDataId_.empty();
}

void
StatisticsInputParams::validateParameters()
{
    errorList_.clear();

    if (osduPartition_.empty()) {
        errorList_.emplace_back("OSDU Partition parameter must be defined!");
    }

    if (osduFileCollectionSegyId_.empty()) {
        errorList_.emplace_back("Input FileCollectionSegy Id parameter must be a valid file name or id!");
    }

    if (osduSeismicTraceDataId_.empty() and not osduWorkProductId_.empty()) {
        osduSeismicTraceDataId_ = retrieveSeismicTraceData(osduWorkProductId_);
    }

    if (osduSeismicTraceDataId_.empty() and osduWorkProductId_.empty()) {
        errorList_.emplace_back("Input seismic_trace_data_id or work_product_id parameter must be a valid file name or id!");
    } 
}

std::string
StatisticsInputParams::retrieveSeismicTraceData(std::string& osduWorkProductId)
{
    segysdk::metadata::osdu::OsduWorkProduct work_product(segysdk::utilities::JsonUtilities::getJsonObjectFromPathOrOsduId(osduWorkProductId), osduPartition_);
    const auto ptr = std::reinterpret_pointer_cast<segysdk::metadata::osdu::OsduWorkProductStruct>(work_product.getMetaDataStruct());
    if (ptr != nullptr and ptr->dataStruct.has_value() and ptr->dataStruct.value().components.has_value() and !ptr->dataStruct.value().components.value().empty()) {
        for (const auto& refId : ptr->dataStruct.value().components.value()) {
            if (refId.find(segysdk::metadata::osdu::OsduSeismicTraceDataStruct::getClassName()) != std::string::npos) {
                return refId;
            }
        }
    }
    return {};
}
    

} // namespace segysdk::statistics_generator
