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

#include "../OsduMetaDataUtilities/OsduOverridesTraceHeaderStruct.h"

#include <algorithm>


namespace segysdk::metadata::osdu
{
OsduOverridesTraceHeaderStruct::OsduOverridesTraceHeaderStruct(std::string osduPartition)
    : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduOverridesTraceHeaderStruct::OsduOverridesTraceHeaderStruct(const OsduOverridesTraceHeaderStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    traceHeaderOverrides_(other.traceHeaderOverrides_)
{
    /* Empty */
}

OsduOverridesTraceHeaderStruct::OsduOverridesTraceHeaderStruct(OsduOverridesTraceHeaderStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    traceHeaderOverrides_(std::move(other.traceHeaderOverrides_))
{
    /* Empty */
}

OsduOverridesTraceHeaderStruct&
OsduOverridesTraceHeaderStruct::operator=(const OsduOverridesTraceHeaderStruct& rhs)
{
    return *this = OsduOverridesTraceHeaderStruct(rhs);
}

OsduOverridesTraceHeaderStruct&
OsduOverridesTraceHeaderStruct::operator=(OsduOverridesTraceHeaderStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    traceHeaderOverrides_ = std::move(rhs.traceHeaderOverrides_);

    return *this;
}

bool
OsduOverridesTraceHeaderStruct::isValid() const
{
    if (!traceHeaderOverrides_.has_value()) {
        return true;
    }
    return all_of(traceHeaderOverrides_.value().cbegin(), traceHeaderOverrides_.value().cend(), [](const OsduOverridesTraceHeaderElementStruct& x) { return x.isValid(); });
}

std::vector<std::string>
OsduOverridesTraceHeaderStruct::performAttributeValidityScan() const
{
    if (!isValid()) {
        return std::vector<std::string>{"OsduOverridesTraceHeaderStruct::isValid() failed."};
    }
    return {};
}

void
OsduOverridesTraceHeaderStruct::propagatePartition(const std::string& partition)
{
    if (traceHeaderOverrides_.has_value()) {
        for (auto& tho : traceHeaderOverrides_.value()) {
            tho.setPartition(partition);
        }
    }
}

} // namespace segysdk::metadata::osdu
