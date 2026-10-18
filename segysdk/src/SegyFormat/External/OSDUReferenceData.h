/*
Copyright 2021 Schlumberger

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

#include "../segysdk_details/DLL_EXPORT.h"

#include "../Utilities/StringUtils.h"

#include <string>

namespace segysdk::segyformat::external
{

class SEGYSDK_DLL_PUBLIC OSDUReferenceData
{
private:
    std::string _referenceValue;
    std::string _referenceValueUpperCase;
    std::string _referenceType;
    std::string _schema;

public:
    explicit OSDUReferenceData(const std::string& referenceString);

    virtual ~OSDUReferenceData();

    std::string getReferenceValue() const;
    std::string getReferenceValueToUpper() const;

    std::string getSchema() const;

    std::string getReferenceType() const;

    virtual bool requiredFieldsPopulated() const;

    virtual bool isValidReference() const = 0;
};

class SEGYSDK_DLL_PUBLIC OSDUKeyNameReference : public OSDUReferenceData
{
public:
    explicit OSDUKeyNameReference(const std::string& referenceString)
      : OSDUReferenceData(referenceString) {}

    bool isValidReference() const override { return requiredFieldsPopulated() && utilities::StringUtils::keyEndsWith(getReferenceType(), "--HeaderKeyName"); }
};

class SEGYSDK_DLL_PUBLIC OSDUWordFormatReference : public OSDUReferenceData
{
public:
    explicit OSDUWordFormatReference(const std::string& referenceString)
      : OSDUReferenceData(referenceString) {}

    bool isValidReference() const override { return requiredFieldsPopulated() && utilities::StringUtils::keyEndsWith(getReferenceType(), "--WordFormatType"); }
};

class SEGYSDK_DLL_PUBLIC OSDUUnitOfMeasureReference : public OSDUReferenceData
{
public:
    explicit OSDUUnitOfMeasureReference(const std::string& referenceString)
      : OSDUReferenceData(referenceString) {}

    bool isValidReference() const override { return requiredFieldsPopulated() && utilities::StringUtils::keyEndsWith(getReferenceType(), "--UnitOfMeasure"); }
};

} // namespace segysdk::segyformat::external
