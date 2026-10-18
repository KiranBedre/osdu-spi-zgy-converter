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

#include "OSDUReferenceData.h"

#include "../segysdk_details/SegyException.h"
#include "../Utilities/StringUtils.h"

#include <stdexcept>
#include <vector>

namespace segysdk::segyformat::external
{

OSDUReferenceData::OSDUReferenceData(const std::string& referenceString)
{
    const std::string delimiter = ":";

    std::vector<std::string> fields;

    std::string parseString = referenceString;
    auto pos = parseString.find(delimiter);
    while (pos != std::string::npos) {
        std::string token = parseString.substr(0, pos);
        fields.push_back(token);
        parseString = parseString.substr(pos + 1);
        pos = parseString.find(delimiter);
    }
    if (!parseString.empty()) {
        fields.push_back(parseString);
    }

    if (fields.size() < 3) {
        const std::string msg = "OSDU VectorHeaderMapping: reference data string does not contain enough information [ \"" + referenceString + "\" ]\nExpected format: e.g.: <partition>:reference-data--WordFormatType:<Format>.";
        throw SegyInvalidArgumentException(msg);
    }

    _schema = fields.at(0);
    _referenceType = fields.at(1);
    _referenceValue = fields.at(2);
    _referenceValueUpperCase = utilities::StringUtils::ToUppercase(_referenceValue);
}

OSDUReferenceData::~OSDUReferenceData()
{
}

std::string
OSDUReferenceData::getReferenceValue() const
{
    return _referenceValue;
}
std::string
OSDUReferenceData::getReferenceValueToUpper() const
{
    return _referenceValueUpperCase;
}

std::string
OSDUReferenceData::getSchema() const
{
    return _schema;
}

std::string
OSDUReferenceData::getReferenceType() const
{
    return _referenceType;
}

bool
OSDUReferenceData::requiredFieldsPopulated() const
{
    return !_referenceValue.empty() && !_referenceType.empty(); // Don't really care about schema
}
} // namespace segysdk::segyformat::external
