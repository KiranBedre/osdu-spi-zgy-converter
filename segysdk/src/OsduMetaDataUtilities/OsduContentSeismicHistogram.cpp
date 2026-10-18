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

#include "../OsduMetaDataUtilities/OsduContentSeismicHistogram.h"

#include "../OsduMetaDataUtilities/OsduExtensionPropertyArray.h"
#include "../OsduMetaDataUtilities/OsduAbstractContent.h"
#include "../OsduMetaDataUtilities/OsduAbstractContentLiterals.h"
#include "../OsduMetaDataUtilities/OsduContentSeismicHistogramStruct.h"
#include "../OsduMetaDataUtilities/OsduContentSeismicHistogramLiterals.h"
#include "../OsduMetaDataUtilities/OsduHistogram.h"

namespace segysdk::metadata::osdu
{
OsduContentSeismicHistogram::OsduContentSeismicHistogram(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition),
    seismicHistogramStruct_(OsduContentSeismicHistogramStruct(osduPartition)) {}

OsduContentSeismicHistogram::OsduContentSeismicHistogram(const OsduContentSeismicHistogram& other)
  : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
    seismicHistogramStruct_(other.seismicHistogramStruct_)
{
    setJson(other.getJson());
}

OsduContentSeismicHistogram::OsduContentSeismicHistogram(OsduContentSeismicHistogram&& other) noexcept
  : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
    seismicHistogramStruct_(std::move(other.seismicHistogramStruct_))
{
    setJson(other.getJson());
}

OsduContentSeismicHistogram&
OsduContentSeismicHistogram::operator=(const OsduContentSeismicHistogram& rhs)
{
    return *this = OsduContentSeismicHistogram(rhs);
}

OsduContentSeismicHistogram&
OsduContentSeismicHistogram::operator=(OsduContentSeismicHistogram&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    seismicHistogramStruct_ = std::move(rhs.seismicHistogramStruct_);

    return *this;
}

OsduContentSeismicHistogram::OsduContentSeismicHistogram(OsduContentSeismicHistogramStruct fileHistogramStruct)
  : OsduMetadataBase(fileHistogramStruct.getPartition(), false), seismicHistogramStruct_(std::move(fileHistogramStruct))
{
    populateJsonObject();
}

OsduContentSeismicHistogram::OsduContentSeismicHistogram(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), seismicHistogramStruct_(OsduContentSeismicHistogramStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduContentSeismicHistogram::isValidInput() const
{
    return seismicHistogramStruct_.isValid();
}

void
OsduContentSeismicHistogram::initializeJson()
{
    clearJsonObject();
}

void
OsduContentSeismicHistogram::populateJsonObject()
{
    initializeJson();

    mergeInJson(OsduAbstractContent(seismicHistogramStruct_.abstractContentStruct).getAsJson());

    setOptionalParameter(OsduContentSeismicHistogramLiterals::AmplitudeUnitOfMeasureID, seismicHistogramStruct_.amplitudeUnitOfMeasureID);
    setOptionalParameter(OsduContentSeismicHistogramLiterals::SeismicRecordID, seismicHistogramStruct_.seismicRecordID);
    setOptionalParameter(OsduContentSeismicHistogramLiterals::RangeAmplitudeMin, seismicHistogramStruct_.rangeAmplitudeMin);
    setOptionalParameter(OsduContentSeismicHistogramLiterals::RangeAmplitudeMax, seismicHistogramStruct_.rangeAmplitudeMax);
    setOptionalParameter(OsduContentSeismicHistogramLiterals::RangeAmplitudeMean, seismicHistogramStruct_.rangeAmplitudeMean);
    setOptionalParameter(OsduContentSeismicHistogramLiterals::RangeAmplitudeRMS, seismicHistogramStruct_.rangeAmplitudeRms);
    setOptionalParameter(OsduContentSeismicHistogramLiterals::TraceCountBinGrid, seismicHistogramStruct_.traceCountBinGrid);
    setOptionalParameter(OsduContentSeismicHistogramLiterals::TraceCountFileCollection, seismicHistogramStruct_.traceCountFileCollection);
    setOptionalParameter(OsduContentSeismicHistogramLiterals::TraceCountLive, seismicHistogramStruct_.traceCountLive);

    setOptionalArrayStructParameter<OsduHistogram, OsduHistogramStruct>(OsduContentSeismicHistogramLiterals::Histograms, seismicHistogramStruct_.histograms);
}

void
OsduContentSeismicHistogram::initializeMetadataStruct()
{
    // This bit should be done for composite structures to allow the partition to be propagated to sub-elements.
    seismicHistogramStruct_ = OsduContentSeismicHistogramStruct(getOsduPartition());
}

void
OsduContentSeismicHistogram::populateMetadataStruct()
{
    initializeMetadataStruct();

    seismicHistogramStruct_.abstractContentStruct = getStructParameter<OsduAbstractContent, OsduAbstractContentStruct>();

    seismicHistogramStruct_.seismicRecordID = getOptionalParameter<std::string>(OsduContentSeismicHistogramLiterals::SeismicRecordID);
    seismicHistogramStruct_.amplitudeUnitOfMeasureID = getOptionalParameter<std::string>(OsduContentSeismicHistogramLiterals::AmplitudeUnitOfMeasureID);
    seismicHistogramStruct_.rangeAmplitudeMin = getOptionalParameter<double>(OsduContentSeismicHistogramLiterals::RangeAmplitudeMin);
    seismicHistogramStruct_.rangeAmplitudeMean = getOptionalParameter<double>(OsduContentSeismicHistogramLiterals::RangeAmplitudeMean);
    seismicHistogramStruct_.rangeAmplitudeMax = getOptionalParameter<double>(OsduContentSeismicHistogramLiterals::RangeAmplitudeMax);
    seismicHistogramStruct_.rangeAmplitudeRms = getOptionalParameter<double>(OsduContentSeismicHistogramLiterals::RangeAmplitudeRMS);
    seismicHistogramStruct_.traceCountBinGrid = getOptionalParameter<size_t>(OsduContentSeismicHistogramLiterals::TraceCountBinGrid);
    seismicHistogramStruct_.traceCountFileCollection = getOptionalParameter<size_t>(OsduContentSeismicHistogramLiterals::TraceCountFileCollection);
    seismicHistogramStruct_.traceCountLive = getOptionalParameter<size_t>(OsduContentSeismicHistogramLiterals::TraceCountLive);

    seismicHistogramStruct_.histograms = getOptionalArrayStructParameter<OsduHistogram, OsduHistogramStruct>(OsduContentSeismicHistogramLiterals::Histograms);
}

std::shared_ptr<OsduMetadataStructBase>
OsduContentSeismicHistogram::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduContentSeismicHistogramStruct>(seismicHistogramStruct_);
}

} // namespace segysdk::metadata::osdu
