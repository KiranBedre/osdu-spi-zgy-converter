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

#include "../OsduMetaDataUtilities/OsduExtensionPropertiesStruct.h"

namespace segysdk::metadata::osdu
{
OsduExtensionPropertiesStruct::OsduExtensionPropertiesStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduExtensionPropertiesStruct::OsduExtensionPropertiesStruct(const OsduExtensionPropertiesStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    propertyName(other.propertyName),
    propertyValue(other.propertyValue)
{
    /* Empty */
}

OsduExtensionPropertiesStruct::OsduExtensionPropertiesStruct(OsduExtensionPropertiesStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    propertyName(std::move(other.propertyName)),
    propertyValue(std::move(other.propertyValue))
{
    /* Empty */
}

OsduExtensionPropertiesStruct&
OsduExtensionPropertiesStruct::operator=(const OsduExtensionPropertiesStruct& rhs)
{
    return *this = OsduExtensionPropertiesStruct(rhs);
}

OsduExtensionPropertiesStruct&
OsduExtensionPropertiesStruct::operator=(OsduExtensionPropertiesStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    propertyName = std::move(rhs.propertyName);
    propertyValue = std::move(rhs.propertyValue);

    return *this;
}

bool
OsduExtensionPropertiesStruct::isValid() const
{
    return true;
}

std::vector<std::string>
OsduExtensionPropertiesStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduExtensionPropertiesStruct::isValid() failed.");
    }

    return results;
}

} // namespace segysdk::metadata::osdu
