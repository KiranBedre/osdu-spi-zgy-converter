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

#include "../OsduMetaDataUtilities/OsduExtensionPropertyArrayStruct.h"

#include <algorithm>


namespace segysdk::metadata::osdu
{

OsduExtensionPropertyArrayStruct::OsduExtensionPropertyArrayStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduExtensionPropertyArrayStruct::OsduExtensionPropertyArrayStruct(const OsduExtensionPropertyArrayStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    extensionPropertyArrayStruct_(other.extensionPropertyArrayStruct_)
{
    /* Empty */
}

OsduExtensionPropertyArrayStruct::OsduExtensionPropertyArrayStruct(OsduExtensionPropertyArrayStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    extensionPropertyArrayStruct_(std::move(other.extensionPropertyArrayStruct_))
{
    /* Empty */
}

OsduExtensionPropertyArrayStruct&
OsduExtensionPropertyArrayStruct::operator=(const OsduExtensionPropertyArrayStruct& rhs)
{
    return *this = OsduExtensionPropertyArrayStruct(rhs);
}

OsduExtensionPropertyArrayStruct&
OsduExtensionPropertyArrayStruct::operator=(OsduExtensionPropertyArrayStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    extensionPropertyArrayStruct_ = std::move(rhs.extensionPropertyArrayStruct_);

    return *this;
}

OsduExtensionPropertyArrayStruct::OsduExtensionPropertyArrayStruct(const std::vector<OsduExtensionPropertiesStruct>& extensionPropertiesVectorStruct)
    : OsduMetadataStructBase(""), extensionPropertyArrayStruct_(extensionPropertiesVectorStruct)
{
    if (!extensionPropertyArrayStruct_.empty()) {
        setPartition(extensionPropertyArrayStruct_[0].getPartition());
    }
}


bool
OsduExtensionPropertyArrayStruct::allStructsOK() const
{
    return std::all_of(extensionPropertyArrayStruct_.cbegin(), extensionPropertyArrayStruct_.cend(), [](const OsduExtensionPropertiesStruct& x) { return x.isValid(); });
}

bool
OsduExtensionPropertyArrayStruct::isValid() const
{
    return allStructsOK();
}

std::vector<std::string>
OsduExtensionPropertyArrayStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduExtensionPropertyArrayStruct::isValid() failed.");
    }
    for (const auto& t : extensionPropertyArrayStruct_) {
        appendStringVector(results, t.performAttributeValidityScan());
    }

    return results;
}

void
OsduExtensionPropertyArrayStruct::propagatePartition(const std::string& partition)
{
        for (auto& tag : extensionPropertyArrayStruct_) {
            tag.setPartition(partition);
        }
}

} // namespace segysdk::metadata::osdu
