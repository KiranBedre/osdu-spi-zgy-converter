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

#include "../OsduMetaDataUtilities/OsduGeoJsonLabelPropertiesElementStruct.h"


namespace segysdk::metadata::osdu
{
OsduGeoJsonLabelPropertiesElementStruct::OsduGeoJsonLabelPropertiesElementStruct(const std::string& osduPartition)
  : OsduMetadataStructBase(osduPartition)
{
    /* Empty */
}

OsduGeoJsonLabelPropertiesElementStruct::OsduGeoJsonLabelPropertiesElementStruct(const OsduGeoJsonLabelPropertiesElementStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    attributeName(other.attributeName),
    boolValue(other.boolValue),
    stringValue(other.stringValue),
    intValue(other.intValue),
    doubleValue(other.doubleValue)
{
    /* Empty */
}

OsduGeoJsonLabelPropertiesElementStruct::OsduGeoJsonLabelPropertiesElementStruct(OsduGeoJsonLabelPropertiesElementStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    attributeName(std::move(other.attributeName)),
    boolValue(other.boolValue),
    stringValue(std::move(other.stringValue)),
    intValue(other.intValue),
    doubleValue(other.doubleValue)
{
    /* Empty */
}

OsduGeoJsonLabelPropertiesElementStruct&
OsduGeoJsonLabelPropertiesElementStruct::operator=(const OsduGeoJsonLabelPropertiesElementStruct& rhs)
{
    return *this = OsduGeoJsonLabelPropertiesElementStruct(rhs);
}

OsduGeoJsonLabelPropertiesElementStruct&
OsduGeoJsonLabelPropertiesElementStruct::operator=(OsduGeoJsonLabelPropertiesElementStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    attributeName = std::move(rhs.attributeName);
    boolValue = rhs.boolValue;
    stringValue = std::move(rhs.stringValue);
    intValue = rhs.intValue;
    doubleValue = rhs.doubleValue;

    return *this;
}

bool
OsduGeoJsonLabelPropertiesElementStruct::isValid() const
{
    return !attributeName.empty() and (boolValue.has_value() or stringValue.has_value() or intValue.has_value() or doubleValue.has_value());
}

std::vector<std::string>
OsduGeoJsonLabelPropertiesElementStruct::performAttributeValidityScan() const
{
    std::vector<std::string> result;
    if (!isValid()) {
        result.emplace_back("OsduGeoJsonLabelPropertiesElementStruct::isValid() failed.");
    }

    if (attributeName.empty()) {
        result.emplace_back("OsduGeoJsonLabelPropertiesElementStruct: attributeName cannot be empty.");
    }
    if (!boolValue.has_value()  and !stringValue.has_value() and !intValue.has_value() and !doubleValue.has_value()) {
        result.emplace_back("OsduGeoJsonLabelPropertiesElementStruct: at least one of the value properties must have a value.");
    }
    return result;
}

void
OsduGeoJsonLabelPropertiesElementStruct::propagatePartition(const std::string& /* partition */)
{
    /* Empty */
}

} // namespace segysdk::metadata::osdu
