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

#include "../OsduMetaDataUtilities/OsduOverridesBinaryHeaderStruct.h"

#include <algorithm>


namespace segysdk::metadata::osdu
{
OsduOverridesBinaryHeaderStruct::OsduOverridesBinaryHeaderStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduOverridesBinaryHeaderStruct::OsduOverridesBinaryHeaderStruct(const OsduOverridesBinaryHeaderStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    binaryHeaderOverrides_(other.binaryHeaderOverrides_)
{
    /* Empty */
}

OsduOverridesBinaryHeaderStruct::OsduOverridesBinaryHeaderStruct(OsduOverridesBinaryHeaderStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    binaryHeaderOverrides_(std::move(other.binaryHeaderOverrides_))
{
    /* Empty */
}

OsduOverridesBinaryHeaderStruct&
OsduOverridesBinaryHeaderStruct::operator=(const OsduOverridesBinaryHeaderStruct& rhs)
{
    return *this = OsduOverridesBinaryHeaderStruct(rhs);
}

OsduOverridesBinaryHeaderStruct&
OsduOverridesBinaryHeaderStruct::operator=(OsduOverridesBinaryHeaderStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    binaryHeaderOverrides_ = std::move(rhs.binaryHeaderOverrides_);

    return *this;
}

bool
OsduOverridesBinaryHeaderStruct::isValid() const
{
    if (!binaryHeaderOverrides_.has_value()) {
        return true;
    }
    return std::all_of(binaryHeaderOverrides_.value().cbegin(), binaryHeaderOverrides_.value().cend(), [](const OsduOverrideByConstantValueStruct& x) { return x.isValid(); });
}

std::vector<std::string>
OsduOverridesBinaryHeaderStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduOverridesBinaryHeaderStruct::isValid() failed.");
    }

    if (binaryHeaderOverrides_.has_value()) {
        for (const auto& bho : binaryHeaderOverrides_.value()) {
            appendStringVector(results, bho.performAttributeValidityScan());
        }
    }

    return results;
}

void
OsduOverridesBinaryHeaderStruct::propagatePartition(const std::string& partition)
{
    if (binaryHeaderOverrides_.has_value()) {
        for (auto& bho : binaryHeaderOverrides_.value()) {
            bho.setPartition(partition);
        }
    }
}

} // namespace segysdk::metadata::osdu
