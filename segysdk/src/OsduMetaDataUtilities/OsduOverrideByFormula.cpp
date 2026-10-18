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

#include "../OsduMetaDataUtilities/OsduOverrideByFormula.h"

#include "../OsduMetaDataUtilities/OsduOverrideByFormulaStruct.h"
#include "../OsduMetaDataUtilities/OsduOverrideByFormulaLiterals.h"

namespace segysdk::metadata::osdu
{
OsduOverrideByFormula::OsduOverrideByFormula(const std::string& osduPartition)
  : OsduMetadataBase(osduPartition), foStruct_(OsduOverrideByFormulaStruct(osduPartition)) {}

OsduOverrideByFormula::OsduOverrideByFormula(const OsduOverrideByFormula& other)
    : OsduMetadataBase(other.getOsduPartition()), foStruct_(other.foStruct_)
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduOverrideByFormula::OsduOverrideByFormula(OsduOverrideByFormula&& other) noexcept
    : OsduMetadataBase(other.getOsduPartition()), foStruct_(std::move(other.foStruct_))
{
    setJson(other.getJson());
    setIsArrayElement(other.getIsArrayElement());
}

OsduOverrideByFormula&
OsduOverrideByFormula::operator=(const OsduOverrideByFormula& rhs)
{
    return *this = OsduOverrideByFormula(rhs);
}

OsduOverrideByFormula&
OsduOverrideByFormula::operator=(OsduOverrideByFormula&& rhs) noexcept
{
    setOsduPartition(rhs.getOsduPartition());
    setJson(rhs.getJson());
    setIsArrayElement(rhs.getIsArrayElement());

    foStruct_ = std::move(rhs.foStruct_);

    return *this;
}

OsduOverrideByFormula::OsduOverrideByFormula(OsduOverrideByFormulaStruct formulaOverridesStruct)
    : OsduMetadataBase(formulaOverridesStruct.getPartition()), foStruct_(std::move(formulaOverridesStruct))
{
    populateJsonObject();
}

OsduOverrideByFormula::OsduOverrideByFormula(Json::Value json, const std::string& osduPartition)
  : OsduMetadataBase(std::move(json), osduPartition), foStruct_(OsduOverrideByFormulaStruct(osduPartition))
{
    populateMetadataStruct();
}


bool
OsduOverrideByFormula::isValidInput() const
{
    return foStruct_.isValid();
}

void
OsduOverrideByFormula::initializeJson()
{
    clearJsonObject();
}

void
OsduOverrideByFormula::populateJsonObject()
{
    initializeJson();

    setOptionalParameter(OsduOverrideByFormulaLiterals::Input, foStruct_.input);
    setOptionalParameter(OsduOverrideByFormulaLiterals::Operator1, foStruct_.operator1);
    setOptionalParameter(OsduOverrideByFormulaLiterals::Operand1Variable, foStruct_.operand1Variable);
    setOptionalParameter(OsduOverrideByFormulaLiterals::Operand1Constant, foStruct_.operand1Constant);
    setOptionalParameter(OsduOverrideByFormulaLiterals::Operator2, foStruct_.operator2);
    setOptionalParameter(OsduOverrideByFormulaLiterals::Operand2Variable, foStruct_.operand2Variable);
    setOptionalParameter(OsduOverrideByFormulaLiterals::Operand2Constant, foStruct_.operand2Constant);
    setOptionalParameter(OsduOverrideByFormulaLiterals::Remark, foStruct_.remarks);
}

void
OsduOverrideByFormula::initializeMetadataStruct()
{
    foStruct_ = OsduOverrideByFormulaStruct(getOsduPartition());
}

void
OsduOverrideByFormula::populateMetadataStruct()
{
    initializeMetadataStruct();

    if (isMember(OsduOverrideByFormulaLiterals::Input)) {
        foStruct_.input = getParameter<std::string>(OsduOverrideByFormulaLiterals::Input);
    }
    foStruct_.operator1 = getOptionalParameter<std::string>(OsduOverrideByFormulaLiterals::Operator1);
    foStruct_.operand1Variable = getOptionalParameter<std::string>(OsduOverrideByFormulaLiterals::Operand1Variable);
    foStruct_.operand1Constant = getOptionalParameter<double>(OsduOverrideByFormulaLiterals::Operand1Constant);
    foStruct_.operator2 = getOptionalParameter<std::string>(OsduOverrideByFormulaLiterals::Operator2);
    foStruct_.operand2Variable = getOptionalParameter<std::string>(OsduOverrideByFormulaLiterals::Operand2Variable);
    foStruct_.operand2Constant = getParameter<double>(OsduOverrideByFormulaLiterals::Operand2Constant);
    foStruct_.remarks = getOptionalParameter<std::string>(OsduOverrideByFormulaLiterals::Remark);
}

std::shared_ptr<OsduMetadataStructBase>
OsduOverrideByFormula::getMetaDataStruct()
{
    populateMetadataStruct();
    return std::make_shared<OsduOverrideByFormulaStruct>(foStruct_);
}

} // namespace segysdk::metadata::osdu
