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

#include "../OsduMetaDataUtilities/OsduOverridesTraceHeaderElementStruct.h"

#include <algorithm>


namespace segysdk::metadata::osdu
{
OsduOverridesTraceHeaderElementStruct::OsduOverridesTraceHeaderElementStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduOverridesTraceHeaderElementStruct::OsduOverridesTraceHeaderElementStruct(const OsduOverridesTraceHeaderElementStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    keyName(other.keyName),
    overrideByConstantValue(other.overrideByConstantValue),
    overrideByExpressions(other.overrideByExpressions)
{
    /* Empty */
}

OsduOverridesTraceHeaderElementStruct::OsduOverridesTraceHeaderElementStruct(OsduOverridesTraceHeaderElementStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    keyName(std::move(other.keyName)),
    overrideByConstantValue(other.overrideByConstantValue),
    overrideByExpressions(std::move(other.overrideByExpressions))
{
    /* Empty */
}

OsduOverridesTraceHeaderElementStruct&
OsduOverridesTraceHeaderElementStruct::operator=(const OsduOverridesTraceHeaderElementStruct& rhs)
{
    return *this = OsduOverridesTraceHeaderElementStruct(rhs);
}

OsduOverridesTraceHeaderElementStruct&
OsduOverridesTraceHeaderElementStruct::operator=(OsduOverridesTraceHeaderElementStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    overrideByConstantValue = rhs.overrideByConstantValue;
    overrideByExpressions = std::move(rhs.overrideByExpressions);

    return *this;
}

bool
OsduOverridesTraceHeaderElementStruct::isValid() const
{

    if (keyName.empty()) {
        return false;
    }

    // Shouldn't have both override by constant and override by formula, but accept it ...
    if (overrideByConstantValue.has_value() and overrideByExpressions.has_value()) {
        // return false;
    }

    // But it must have one override type defined ...
    if (!overrideByConstantValue.has_value() and !overrideByExpressions.has_value()) {
        return false;
    }

    if (overrideByExpressions.has_value()) {
        return std::all_of(overrideByExpressions.value().cbegin(), overrideByExpressions.value().cend(), [](const OsduOverrideByFormulaStruct& x) { return x.isValid(); });
    }

    // Only constant value left ...
    return true;
}

std::vector<std::string>
OsduOverridesTraceHeaderElementStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        results.emplace_back("OsduOverridesTraceHeaderElementStruct::isValid() failed.");
    }

    if (!isValidOSDUReferenceIdString(keyName)) {
        results.emplace_back("OsduOverridesTraceHeaderElementStruct: KeyName " + keyName + " is not a valid OSDU reference string");
    }

    if (overrideByExpressions.has_value()) {
        for (const auto& formula : overrideByExpressions.value()) {
            appendStringVector(results, formula.performAttributeValidityScan());
        }
    }
    return results;
}

void
OsduOverridesTraceHeaderElementStruct::propagatePartition(const std::string& partition)
{
    if (overrideByExpressions.has_value()) {
        for (auto& override : overrideByExpressions.value()) {
            override.setPartition(partition);
        }
    }
}

} // namespace segysdk::metadata::osdu
