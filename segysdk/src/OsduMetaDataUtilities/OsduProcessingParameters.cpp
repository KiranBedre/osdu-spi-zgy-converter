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

#include "../OsduMetaDataUtilities/OsduProcessingParameters.h"

#include "../OsduMetaDataUtilities/OsduProcessingParametersLiterals.h"

namespace segysdk::metadata::osdu
{

OsduProcessingParameters::OsduProcessingParameters(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), processingParamsStruct_(OsduProcessingParametersStruct(osduPartition))
{
    /* Empty */
}

OsduProcessingParameters::OsduProcessingParameters(const OsduProcessingParameters& other)
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      processingParamsStruct_(other.processingParamsStruct_)
{
    setOsduPartition(other.getOsduPartition());
    setJson(other.getJson());
}

OsduProcessingParameters::OsduProcessingParameters(OsduProcessingParameters&& other) noexcept
    : OsduMetadataBase(other.getJson(), other.getOsduPartition(), other.getIsArrayElement()),
      processingParamsStruct_(std::move(other.processingParamsStruct_))
{
    setOsduPartition(other.getOsduPartition());
    setJson(other.getJson());
}

OsduProcessingParameters&
OsduProcessingParameters::operator=(const OsduProcessingParameters& rhs)
{
    return *this = OsduProcessingParameters(rhs);
}

OsduProcessingParameters&
OsduProcessingParameters::operator=(OsduProcessingParameters&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setIsArrayElement(rhs.getIsArrayElement());
    setJson(rhs.getAsJson());

    processingParamsStruct_ = std::move(rhs.processingParamsStruct_);

    return *this;
}

OsduProcessingParameters::OsduProcessingParameters(OsduProcessingParametersStruct processingParams)
    : OsduMetadataBase(processingParams.getPartition()), processingParamsStruct_(std::move(processingParams))
{
    populateJsonObject();
}

OsduProcessingParameters::OsduProcessingParameters(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), processingParamsStruct_(OsduProcessingParametersStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduProcessingParameters::isValidInput() const
{
    return processingParamsStruct_.isValid();
}

std::shared_ptr<OsduMetadataStructBase>
OsduProcessingParameters::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduProcessingParametersStruct>(processingParamsStruct_);
}

void
OsduProcessingParameters::initializeJson()
{
    clearJsonObject();
}

void
OsduProcessingParameters::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduProcessingParametersLiterals::ProcessingParameterTypeID, processingParamsStruct_.processingParameterTypeID);
    setOptionalParameter(OsduProcessingParametersLiterals::ProcessingParameterValue, processingParamsStruct_.processingParameterValue);
}

void
OsduProcessingParameters::initializeMetadataStruct() {
    processingParamsStruct_ = OsduProcessingParametersStruct(getOsduPartition());
}

void
OsduProcessingParameters::populateMetadataStruct()
{
    processingParamsStruct_.processingParameterTypeID = getOptionalParameter<std::string>(OsduProcessingParametersLiterals::ProcessingParameterTypeID);
    processingParamsStruct_.processingParameterValue = getOptionalParameter<std::string>(OsduProcessingParametersLiterals::ProcessingParameterValue);
}

} // namespace segysdk::metadata::osdu
