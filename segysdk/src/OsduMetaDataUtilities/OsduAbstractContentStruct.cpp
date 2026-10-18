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

#include "../OsduMetaDataUtilities/OsduAbstractContentStruct.h"


namespace segysdk::metadata::osdu
{
OsduAbstractContentStruct::OsduAbstractContentStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduAbstractContentStruct::OsduAbstractContentStruct(const OsduAbstractContentStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    schemaKind(other.schemaKind),
    name(other.name),
    description(other.description),
    extensionProperties(other.extensionProperties)
{
    /* Empty */
}

OsduAbstractContentStruct::OsduAbstractContentStruct(OsduAbstractContentStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    schemaKind(std::move(other.schemaKind)),
    name(std::move(other.name)),
    description(std::move(other.description)),
    extensionProperties(std::move(other.extensionProperties))
{
    /* Empty */
}

OsduAbstractContentStruct&
OsduAbstractContentStruct::operator=(const OsduAbstractContentStruct& rhs)
{
    return *this = OsduAbstractContentStruct(rhs);
}

OsduAbstractContentStruct&
OsduAbstractContentStruct::operator=(OsduAbstractContentStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    schemaKind = std::move(rhs.schemaKind);
    name = std::move(rhs.name);
    description = std::move(rhs.description);
    extensionProperties = std::move(rhs.extensionProperties);

    return *this;
}

bool
OsduAbstractContentStruct::isValid() const
{
    return !schemaKind.empty() && (!extensionProperties.has_value() || extensionProperties.value().isValid());
}

bool
OsduAbstractContentStruct::extensionPropertiesValid() const
{
    return !extensionProperties.has_value() || extensionProperties.value().isValid();
}

bool
OsduAbstractContentStruct::abstractContentValid() const
{
    return !schemaKind.empty() && isValidOSDUReferenceIdString(schemaKind);
}

std::vector<std::string>
OsduAbstractContentStruct::performAttributeValidityScan() const
{
    if (!extensionPropertiesValid()) {
        return std::vector<std::string>{"ExtensionPropertiesStruct::isValid() failed."};
    }
    if (!abstractContentValid()) {
        return std::vector<std::string>{"OsduAbstractContentStruct::isValid() failed."};
    }
    return {};
}

void
OsduAbstractContentStruct::propagatePartition(const std::string& partition)
{
    if (extensionProperties.has_value()) {
        extensionProperties.value().setPartition(partition);
    }
}

} // namespace segysdk::metadata::osdu
