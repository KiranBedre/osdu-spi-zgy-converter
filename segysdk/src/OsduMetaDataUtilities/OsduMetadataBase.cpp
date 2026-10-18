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

#include "../OsduMetaDataUtilities/OsduMetadataBase.h"
#include "../Utilities/JsonException.h"

#include <iostream>
#include <sstream>
#include <optional>
#include <regex>

namespace segysdk::metadata::osdu
{

OsduMetadataBase::OsduMetadataBase(std::string osduPartition, const bool isArrayElement)
    : isArrayElement_(isArrayElement), partition_(std::move(osduPartition))
{
    if (isArrayElement_) {
        const Json::Value tmp(Json::arrayValue);
        myJson_ = tmp;
    }
}

OsduMetadataBase::OsduMetadataBase(Json::Value json, std::string osduPartition, const bool isArrayElement)
    : myJson_(std::move(json)),
      isArrayElement_(isArrayElement),
      partition_(std::move(osduPartition)) {}


OsduMetadataBase::OsduMetadataBase(OsduMetadataBase&& other) noexcept
    : myJson_(std::move(other.myJson_)),
      isArrayElement_(other.isArrayElement_),
      partition_(std::move(other.partition_))
{
    /* Empty */
}

OsduMetadataBase&
OsduMetadataBase::operator=(OsduMetadataBase&& rhs) noexcept
{
    myJson_ = std::move(rhs.myJson_);
    isArrayElement_ = rhs.isArrayElement_;

    return *this;
}

OsduMetadataBase::~OsduMetadataBase()
{
    myJson_.clear();
}


void
OsduMetadataBase::assignRootJsonObject(Json::Value json)
{
    myJson_ = std::move(json);
}

void
OsduMetadataBase::removeMember(const std::string& jsonPath)
{
    auto elementsChain = parseJsonPath(jsonPath);

    if (elementsChain.size() > 0) {
        auto memberName = elementsChain.back();
        elementsChain.pop_back();

        auto parentJson = getJsonElement(elementsChain);
        if (parentJson) {
            parentJson->removeMember(memberName);
        }
    }
}

void
OsduMetadataBase::mergeInJson(const Json::Value& json)
{
    const std::vector<std::string> memberList = json.getMemberNames();
    for (const auto& member : memberList) {
        myJson_[member] = json[member];
    }
}

void
OsduMetadataBase::mergeInJsonRecursively(const Json::Value& json, bool overwriteConflicting /* = false*/)
{
    mergeJsonRecursively(myJson_, json, overwriteConflicting);
}

void
OsduMetadataBase::InitializeJsonRootAsArray()
{
    myJson_ = Json::Value(Json::arrayValue);
}

void
OsduMetadataBase::InitializeJsonArray(const std::string& jsonPath)
{
    auto jsonPtr = getJsonElement(jsonPath);
    if (jsonPtr) {
        *jsonPtr = Json::Value(Json::arrayValue);
    }
}

int64_t
OsduMetadataBase::getArraySize(const std::string& jsonPath) const
{
    auto jsonPtr = getJsonElement(jsonPath);
    if (jsonPtr and jsonPtr->isArray()) {
        return jsonPtr->size();
    }

    return -1;
}

bool
OsduMetadataBase::isJsonArray(const std::string& jsonPath) const
{
    auto jsonPtr = getJsonElement(jsonPath);
    return jsonPtr and jsonPtr->isArray();
}

bool
OsduMetadataBase::isRootJsonArray() const
{
    return myJson_.empty() or myJson_.isArray();
}

bool
OsduMetadataBase::isRootEmptyArray() const
{
    return myJson_.empty() and myJson_.isArray();
}

//---------------------------------


bool
OsduMetadataBase::isEmptyArray(const std::string& jsonPath) const
{
    auto jsonPtr = getJsonElement(jsonPath);
    return jsonPtr and jsonPtr->isArray() and jsonPtr->empty();
}

// ----------------

std::string
OsduMetadataBase::getAsFormattedString() const
{
    std::string json_styled_string = myJson_.toStyledString();
    return json_styled_string;
}

std::string
OsduMetadataBase::getAsCompactString() const
{
    Json::FastWriter fastWriter;
    std::string json_string = fastWriter.write(myJson_);

    return json_string;
}

Json::Value
OsduMetadataBase::getAsJson() const
{
    return myJson_;
}

std::vector<std::string>
OsduMetadataBase::getJsonMemberNames() const
{
    return myJson_.getMemberNames();
}

bool
OsduMetadataBase::isMember(const std::string& jsonPath) const
{
    return getJsonElement(jsonPath) != nullptr;
}

void
OsduMetadataBase::mergeJsonRecursively(Json::Value& target, const Json::Value& source, bool overwriteConflicting /*= false*/)
{
    const std::vector<std::string> memberList = source.getMemberNames();
    for (const auto& member : memberList) {
        if (!target.isMember(member)) { // Add new fields
            target[member] = source[member];
            continue;
        }

        if (target[member].isObject() and source[member].isObject()) { // merge objects if exists in both
            mergeJsonRecursively(target[member], source[member], overwriteConflicting);
            continue;
        }

        if (target[member].isArray() and source[member].isArray()) { // append target array if arrays exists in both
            auto& targetArray = target[member];
            auto& sourceArray = source[member];
            for (const auto& element : sourceArray) {
                const auto& res = std::find(targetArray.begin(), targetArray.end(), element);
                if (res == targetArray.end()) {
                    targetArray.append(element);
                }
            }
            continue;
        }

        if (overwriteConflicting) { // overwrite conflicting elements
            target[member] = source[member];
        }
    }
}

std::vector<std::string>
OsduMetadataBase::parseJsonPath(const std::string& jsonPath)
{
    static const std::regex re("(:)");

    std::sregex_token_iterator it(jsonPath.begin(), jsonPath.end(), re, -1);
    std::vector<std::string> tokenized{it, {}};

    // Filter out empty strings
    std::vector<std::string> elementsChain;
    for (const auto& token : tokenized) {
        if (!token.empty()) {
            elementsChain.emplace_back(token);
        }
    }

    return elementsChain;
}

std::string
OsduMetadataBase::toJsonPath(const std::vector<std::string>& elementsChain)
{
    std::string jsonPath;
    for (const auto& element : elementsChain) {
        jsonPath += element + ":";
    }

    // Remove tailing colon
    jsonPath.pop_back();

    return jsonPath;
}

Json::Value*
OsduMetadataBase::getJsonElement(const std::string& jsonPath)
{
    return getJsonElement(parseJsonPath(jsonPath));
}

Json::Value*
OsduMetadataBase::getJsonElement(const std::vector<std::string>& elementsChain)
{
    Json::Value* jsonPtr = &myJson_;
    for (const auto& element : elementsChain) {
        jsonPtr = &(*jsonPtr)[element];
    }

    return jsonPtr;
}

const Json::Value*
OsduMetadataBase::getJsonElement(const std::string& jsonPath) const
{
    return getJsonElement(parseJsonPath(jsonPath));
}

const Json::Value*
OsduMetadataBase::getJsonElement(const std::vector<std::string>& elementsChain) const
{
    const Json::Value* jsonPtr = &myJson_;
    for (const auto& element : elementsChain) {
        if (jsonPtr->isObject() and jsonPtr->isMember(element)) {
            jsonPtr = &(*jsonPtr)[element];
        }
        else {
            return nullptr;
        }
    }

    return jsonPtr;
}


} // namespace segysdk::metadata::osdu
