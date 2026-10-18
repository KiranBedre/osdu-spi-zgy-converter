/*
Copyright 2023 Schlumberger

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

#include <utility>

#include "../OsduMetaDataUtilities/OsduHistogram.h"

#include "../OsduMetaDataUtilities/OsduExtensionProperties.h"
#include "../OsduMetaDataUtilities/OsduHistogramStruct.h"
#include "../OsduMetaDataUtilities/OsduHistogramLiterals.h"

namespace segysdk::metadata::osdu
{
OsduHistogram::OsduHistogram(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), histogramStruct_(OsduHistogramStruct(osduPartition)) {}

OsduHistogram::OsduHistogram(const OsduHistogram& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      histogramStruct_(other.histogramStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduHistogram::OsduHistogram(OsduHistogram&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      histogramStruct_(std::move(other.histogramStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduHistogram&
OsduHistogram::operator=(const OsduHistogram& rhs)
{
    return *this = OsduHistogram(rhs);
}

OsduHistogram&
OsduHistogram::operator=(OsduHistogram&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    histogramStruct_ = std::move(rhs.histogramStruct_);

    return *this;
}

OsduHistogram::OsduHistogram(OsduHistogramStruct abstractContentStruct)
    : OsduMetadataBase(abstractContentStruct.getPartition()), histogramStruct_(std::move(abstractContentStruct))
{
    populateJsonObject();
}

OsduHistogram::OsduHistogram(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), histogramStruct_(OsduHistogramStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduHistogram::isValidInput() const
{
    return histogramStruct_.isValid();
}

void
OsduHistogram::initializeJson()
{
    removeMember(OsduHistogramLiterals::Name);
    removeMember(OsduHistogramLiterals::Description);
    removeMember(OsduHistogramLiterals::BucketMin);
    removeMember(OsduHistogramLiterals::BucketMax);
    removeMember(OsduHistogramLiterals::BucketSize);
    removeMember(OsduHistogramLiterals::BucketCount);
    removeMember(OsduHistogramLiterals::Buckets);
}


void
OsduHistogram::populateJsonObject()
{
    clearJsonObject();


    setOptionalParameter(OsduHistogramLiterals::Name, histogramStruct_.name);
    setOptionalParameter(OsduHistogramLiterals::Description, histogramStruct_.description);
    setOptionalParameter(OsduHistogramLiterals::BucketMin, histogramStruct_.bucketMin);
    setOptionalParameter(OsduHistogramLiterals::BucketMax, histogramStruct_.bucketMax);
    setOptionalParameter(OsduHistogramLiterals::BucketSize, histogramStruct_.bucketSize);
    setOptionalParameter(OsduHistogramLiterals::BucketCount, histogramStruct_.bucketCount);

    if (histogramStruct_.buckets.has_value()) {
        InitializeJsonArray(OsduHistogramLiterals::Buckets);
        for (const auto& item : histogramStruct_.buckets.value()) {
            appendArrayElement(OsduHistogramLiterals::Buckets, item);
        }
    }
}

void
OsduHistogram::initializeMetadataStruct()
{
    histogramStruct_ = OsduHistogramStruct(getOsduPartition());
}

void
OsduHistogram::populateMetadataStruct()
{
    initializeMetadataStruct();
    histogramStruct_.name = getOptionalParameter<std::string>(OsduHistogramLiterals::Name);
    histogramStruct_.description = getOptionalParameter<std::string>(OsduHistogramLiterals::Description);
    histogramStruct_.bucketMin = getOptionalParameter<double>(OsduHistogramLiterals::BucketMin);
    histogramStruct_.bucketMax = getOptionalParameter<double>(OsduHistogramLiterals::BucketMax);
    histogramStruct_.bucketSize = getOptionalParameter<double>(OsduHistogramLiterals::BucketSize);
    histogramStruct_.bucketCount = getOptionalParameter<size_t>(OsduHistogramLiterals::BucketCount);

    if (isMember(OsduHistogramLiterals::Buckets)) {
        histogramStruct_.buckets = getAllArrayElements<size_t>(OsduHistogramLiterals::Buckets);
    } 
}

std::shared_ptr<OsduMetadataStructBase>
OsduHistogram::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduHistogramStruct>(histogramStruct_);
}

} // namespace segysdk::metadata::osdu
