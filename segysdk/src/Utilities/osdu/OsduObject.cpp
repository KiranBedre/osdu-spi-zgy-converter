/*
Copyright 2021-2024 Schlumberger

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

#include "OsduObject.h"

#include "OsduFieldNameLiterals.h"
#include "Logger/Logger.h"

namespace segysdk::utilities::osdu
{

OsduObject::OsduObject(Json::Value root)
  : root_(std::move(root))
{
}

OsduObject::~OsduObject()
{
}

std::string
OsduObject::id() const
{
    return root_[OsduFieldNameLiterals::Id].asString();
}

std::string
OsduObject::version() const
{
    return root_[OsduFieldNameLiterals::Version].asString();
}

std::string
OsduObject::legalTag() const
{
    if (root_.isMember(OsduFieldNameLiterals::Legal)) {
        const auto& legal = root_[OsduFieldNameLiterals::Legal];
        if (legal.isMember(OsduFieldNameLiterals::LegalTags)) {
            const auto& legalTags = legal[OsduFieldNameLiterals::LegalTags];
            if (legalTags.size() > 0) {
                return legalTags[0].asString();
            }
        }
    }
    return "";
}

std::string
OsduObject::ns() const
{
    const auto tmp = id();
    if (tmp.empty()) {
        std::string msg = "OsduObject::ns() : id cannot be empty!";
        SEGYLOG_REPORT_ERROR(msg);
        throw std::invalid_argument(msg);
    }
    const auto colonPos = tmp.find(':');
    if (colonPos == std::string::npos) {
        throw std::invalid_argument("Not a recognized OSDU metadata file!");
    }
    return tmp.substr(0, colonPos);
}

OsduKind
OsduObject::kind() const
{
    return OsduKind(root_[OsduFieldNameLiterals::Kind].asString());
}

std::string
OsduObject::content() const
{
    return root_.toStyledString();
}

} // namespace segysdk::utilities::osdu
