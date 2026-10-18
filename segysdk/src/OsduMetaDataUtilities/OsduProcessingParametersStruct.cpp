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

#include "../OsduMetaDataUtilities/OsduProcessingParametersStruct.h"


namespace segysdk::metadata::osdu
{


OsduProcessingParametersStruct::OsduProcessingParametersStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduProcessingParametersStruct::OsduProcessingParametersStruct(const OsduProcessingParametersStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    processingParameterTypeID(other.processingParameterTypeID),
    processingParameterValue(other.processingParameterValue)
{
    /* Empty */
}

OsduProcessingParametersStruct::OsduProcessingParametersStruct(OsduProcessingParametersStruct&& other) noexcept
    : OsduMetadataStructBase(other.getPartition()),
    processingParameterTypeID(std::move(other.processingParameterTypeID)),
    processingParameterValue(std::move(other.processingParameterValue))
{
    /* Empty */
}

OsduProcessingParametersStruct&
OsduProcessingParametersStruct::operator=(const OsduProcessingParametersStruct& rhs)
{
    return *this = OsduProcessingParametersStruct(rhs);
}

OsduProcessingParametersStruct&
OsduProcessingParametersStruct::operator=(OsduProcessingParametersStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    processingParameterTypeID = std::move(rhs.processingParameterTypeID);
    processingParameterValue = std::move(rhs.processingParameterValue);

    return *this;
}

bool
OsduProcessingParametersStruct::isValid() const
{
    return (!processingParameterTypeID.has_value() or isValidOSDUReferenceIdString(processingParameterTypeID.value())) and
    (!processingParameterValue.has_value() or !processingParameterValue.value().empty());
}

std::vector<std::string>
OsduProcessingParametersStruct::performAttributeValidityScan() const
{
    return {};
}

} // namespace segysdk::metadata::osdu
