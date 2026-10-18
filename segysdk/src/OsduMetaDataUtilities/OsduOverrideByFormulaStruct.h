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

#pragma once

#include "../OsduMetaDataUtilities/OsduMetadataHelper.h"
#include "../OsduMetaDataUtilities/OsduMetadataStructBase.h"

#include <optional>

namespace segysdk::metadata::osdu
{

enum class SEGYSDK_DLL_PUBLIC operators
{
    Plus = 0,
    Minus,
    Multiply,
    Divide,
    Div,
    Mod
};

struct SEGYSDK_DLL_PUBLIC OsduOverrideByFormulaStruct final : public OsduMetadataStructBase
{
    explicit OsduOverrideByFormulaStruct(std::string osduPartition);
    OsduOverrideByFormulaStruct(const OsduOverrideByFormulaStruct& other);
    OsduOverrideByFormulaStruct(OsduOverrideByFormulaStruct&& other) noexcept;
    OsduOverrideByFormulaStruct& operator=(const OsduOverrideByFormulaStruct& rhs);
    OsduOverrideByFormulaStruct& operator=(OsduOverrideByFormulaStruct&& rhs) noexcept;

    ~OsduOverrideByFormulaStruct() override = default;

    std::optional<std::string> input;
    std::optional<std::string> operator1;
    std::optional<double> operand1Constant;
    std::optional<std::string> operand1Variable;
    std::optional<std::string> operator2;
    std::optional<double> operand2Constant;
    std::optional<std::string> operand2Variable;
    std::optional<std::string> remarks;

    bool isValid() const override;
    std::vector<std::string> performAttributeValidityScan() const override;

private:
    bool inputValid() const;
    static bool isOperatorValid(const std::optional<std::string>& myOperator);
    bool isEquationDefined(const std::optional<std::string>& myOperator, const std::optional<double> eqConstant, const std::optional<std::string>& eqVariable) const;

public:
    static std::string getClassPartition() { return partition_; }
    static std::string getClassAreaPath() { return classAreaPath_; }
    static std::string getClassName() { return className_; }
    static std::string getClassVersion() { return classVersion_; }
    static std::string getOsduKind(const std::string& version = getClassVersion()) { return OsduMetadataHelper::osduWksReferenceLookupString(getClassName(), version); }

private:
    inline static constexpr const char* const partition_{"osdu"};
    inline static constexpr const char* const classAreaPath_{"wks"};
    inline static constexpr const char* const className_{"OverrideByFormula"};
    inline static constexpr const char* const classVersion_{"1.0.0"};

    
};

} // namespace segysdk::metadata::osdu
