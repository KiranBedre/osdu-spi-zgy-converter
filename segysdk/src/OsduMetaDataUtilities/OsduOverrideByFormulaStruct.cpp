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

#include "../OsduMetaDataUtilities/OsduOverrideByFormulaStruct.h"


namespace segysdk::metadata::osdu
{
OsduOverrideByFormulaStruct::OsduOverrideByFormulaStruct(std::string osduPartition)
  : OsduMetadataStructBase(std::move(osduPartition))
{
    /* Empty */
}

OsduOverrideByFormulaStruct::OsduOverrideByFormulaStruct(const OsduOverrideByFormulaStruct& other)
  : OsduMetadataStructBase(other.getPartition()),
    input(other.input),
    operator1(other.operator1),
    operand1Constant(other.operand1Constant),
    operand1Variable(other.operand1Variable),
    operator2(other.operator2),
    operand2Constant(other.operand2Constant),
    operand2Variable(other.operand2Variable),
    remarks(other.remarks)
{
    /* Empty */
}

OsduOverrideByFormulaStruct::OsduOverrideByFormulaStruct(OsduOverrideByFormulaStruct&& other) noexcept
  : OsduMetadataStructBase(other.getPartition()),
    input(std::move(other.input)),
    operator1(std::move(other.operator1)),
    operand1Constant(other.operand1Constant),
    operand1Variable(std::move(other.operand1Variable)),
    operator2(std::move(other.operator2)),
    operand2Constant(other.operand2Constant),
    operand2Variable(std::move(other.operand2Variable)),
    remarks(std::move(other.remarks))
{
    /* Empty */
}

OsduOverrideByFormulaStruct&
OsduOverrideByFormulaStruct::operator=(const OsduOverrideByFormulaStruct& rhs)
{
    return *this = OsduOverrideByFormulaStruct(rhs);
}

OsduOverrideByFormulaStruct&
OsduOverrideByFormulaStruct::operator=(OsduOverrideByFormulaStruct&& rhs) noexcept
{
    setPartition(rhs.getPartition());

    input = std::move(rhs.input);
    operator1 = std::move(rhs.operator1);
    operand1Constant = rhs.operand1Constant;
    operand1Variable = std::move(rhs.operand1Variable);
    operator2 = std::move(rhs.operator2);
    operand2Constant = rhs.operand2Constant;
    operand2Variable = std::move(rhs.operand2Variable);
    remarks = std::move(rhs.remarks);

    return *this;
}

bool
OsduOverrideByFormulaStruct::inputValid() const
{
    return input.has_value() && !input.value().empty();
}

bool
OsduOverrideByFormulaStruct::isOperatorValid(const std::optional<std::string>& myOperator)
{
    // myOperator must have a value

    // Operator can be one of: Plus, Minus, DivideBy, DivideByInteger, MultiplyBy,  Modulo, Equals
    return myOperator.has_value() and !myOperator.value().empty();
}

bool
OsduOverrideByFormulaStruct::isEquationDefined(const std::optional<std::string>& myOperator, const std::optional<double> eqConstant, const std::optional<std::string>& eqVariable) const
{
    // operator1 is not just an empty string
    // equation can be of form:
    //  input <Equals> <Constant>
    // or
    // input <Equals> <Variable1>
    // or
    // input <Equals> <Variable1> <Plus> <Constant>
    // We must have an input trace header attribute specified, otherwise nothing makes sense...

    if (!isOperatorValid(myOperator)) {
        return false;
    }

    bool constOK = eqConstant.has_value();

    bool variableOK = eqVariable.has_value() and !eqVariable.value().empty();

    // We need at least either a constant OR a variable to be defined.
    // If operator is defined, we also need at least a variable or a constant (or both?)

    return constOK or variableOK;

}


bool
OsduOverrideByFormulaStruct::isValid() const
{
    // We need input to be defined, otherwise nothing makes sense
    if (!inputValid()) {
        return false;
    }

    // We need at least one equation component defined
    return isEquationDefined(operator1, operand1Constant, operand1Variable) or isEquationDefined(operator2, operand2Constant, operand2Variable);
}

std::vector<std::string>
OsduOverrideByFormulaStruct::performAttributeValidityScan() const
{
    std::vector<std::string> results;
    if (!isValid()) {
        if (!operator1.has_value() and !operator2.has_value()) {
            results.emplace_back("OsduOverrideByFormulaStruct: At least one operator must be defined");
        }
        if (!operand1Constant.has_value() and !operand2Constant.has_value() and operand1Variable.has_value() and operand2Variable.has_value()) {
            results.emplace_back("OsduOverrideByFormulaStruct: At least one operand (variable or constant) must be defined");
        }
    }
    return results;
}

} // namespace segysdk::metadata::osdu
