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

#include "../OsduMetaDataUtilities/OsduOverridesTraceHeaderElement.h"

#include "../OsduMetaDataUtilities/OsduOverrideByFormula.h"
#include "../OsduMetaDataUtilities/OsduOverridesTraceHeaderLiterals.h"
#include "../OsduMetaDataUtilities/OsduOverridesTraceHeaderElementStruct.h"

#include <algorithm>
#include <utility>

namespace segysdk::metadata::osdu
{
OsduOverridesTraceHeaderElement::OsduOverridesTraceHeaderElement(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), thOverrideElementStruct_(OsduOverridesTraceHeaderElementStruct(osduPartition))
{
    /* Empty */
}

OsduOverridesTraceHeaderElement::OsduOverridesTraceHeaderElement(const OsduOverridesTraceHeaderElement& other)
    : OsduMetadataBase(other.getOsduPartition()), thOverrideElementStruct_(other.thOverrideElementStruct_) {}

OsduOverridesTraceHeaderElement::OsduOverridesTraceHeaderElement(OsduOverridesTraceHeaderElement&& other) noexcept
    : OsduMetadataBase(other.getOsduPartition()), thOverrideElementStruct_(std::move(other.thOverrideElementStruct_)) {}

OsduOverridesTraceHeaderElement&
OsduOverridesTraceHeaderElement::operator=(const OsduOverridesTraceHeaderElement& rhs)
{
    return *this = OsduOverridesTraceHeaderElement(rhs);
}

OsduOverridesTraceHeaderElement&
OsduOverridesTraceHeaderElement::operator=(OsduOverridesTraceHeaderElement&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    thOverrideElementStruct_ = std::move(rhs.thOverrideElementStruct_);

    return *this;
}

OsduOverridesTraceHeaderElement::OsduOverridesTraceHeaderElement(OsduOverridesTraceHeaderElementStruct traceHeaderOverrideStruct)
    : OsduMetadataBase(traceHeaderOverrideStruct.getPartition()), thOverrideElementStruct_(std::move(traceHeaderOverrideStruct))
{
    populateJsonObject();
}

OsduOverridesTraceHeaderElement::OsduOverridesTraceHeaderElement(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), thOverrideElementStruct_(OsduOverridesTraceHeaderElementStruct(osduPartition))
{
    populateMetadataStruct();
}

bool
OsduOverridesTraceHeaderElement::isValidInput() const
{
    return thOverrideElementStruct_.isValid();
}

void
OsduOverridesTraceHeaderElement::initializeJson()
{
    removeMember(OsduOverridesTraceHeaderLiterals::KeyName);
    removeMember(OsduOverridesTraceHeaderLiterals::OverrideByConstant);
    removeMember(OsduOverridesTraceHeaderLiterals::OverrideByExpressions);
}

void
OsduOverridesTraceHeaderElement::populateJsonObject()
{
    initializeJson();

    setParameter(OsduOverridesTraceHeaderLiterals::KeyName, thOverrideElementStruct_.keyName);
    setOptionalParameter(OsduOverridesTraceHeaderLiterals::OverrideByConstant, thOverrideElementStruct_.overrideByConstantValue);
    if (thOverrideElementStruct_.overrideByExpressions.has_value()) {
        for (const auto& node : thOverrideElementStruct_.overrideByExpressions.value()) {
            const OsduOverrideByFormula formula(node);
            appendArrayElement(OsduOverridesTraceHeaderLiterals::OverrideByExpressions, formula.getAsJson());
        }
    }
}

void
OsduOverridesTraceHeaderElement::initializeMetadataStruct()
{
    thOverrideElementStruct_ = OsduOverridesTraceHeaderElementStruct(getOsduPartition());
}

void
OsduOverridesTraceHeaderElement::populateMetadataStruct()
{
    initializeMetadataStruct();

    if (!isMember(OsduOverridesTraceHeaderLiterals::KeyName)) {
        return;
    }
    thOverrideElementStruct_.keyName = getParameter<std::string>(OsduOverridesTraceHeaderLiterals::KeyName);
    thOverrideElementStruct_.overrideByConstantValue = getOptionalParameter<double>(OsduOverridesTraceHeaderLiterals::OverrideByConstant);

    const auto nodeArray = getOptionalParameter<Json::Value>(OsduOverridesTraceHeaderLiterals::OverrideByExpressions);
    if (nodeArray.has_value() and nodeArray.value().isArray()) {
        std::vector<OsduOverrideByFormulaStruct> result;
        for (const Json::Value& node : nodeArray.value()) {
            OsduOverrideByFormula formula(node, getOsduPartition());
            const auto formulaStruct = std::reinterpret_pointer_cast<OsduOverrideByFormulaStruct>(formula.getMetaDataStruct());
            if (formulaStruct != nullptr) {
                result.emplace_back(*formulaStruct);
            }
        }
        thOverrideElementStruct_.overrideByExpressions = result;
    }
}

std::shared_ptr<OsduMetadataStructBase>
OsduOverridesTraceHeaderElement::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduOverridesTraceHeaderElementStruct>(thOverrideElementStruct_);
}

} // namespace segysdk::metadata::osdu
