/*
Copyright 2021-2022 Schlumberger

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

#include "OsduKind.h"

#include <stdexcept>
#include <utility>

namespace segysdk::utilities::osdu
{

// TODO rewrite internals to handle kind's parts instead of primitive string manipulation

OsduKind::OsduKind(std::string kind)
  : kind_(std::move(kind))
{
}

std::string
OsduKind::kind() const
{
    return kind_;
}

std::string
OsduKind::shortTypeName() const
{
    const auto ftn = fullTypeName();
    const auto lastColon = ftn.find_last_of(':');
    if (lastColon == std::string::npos) {
        throw std::runtime_error("Invalid format of kind.");
    }
    return ftn.substr(lastColon + 1);
}

std::string
OsduKind::fullTypeName() const
{
    const auto lastColon = kind_.find_last_of(':');
    if (lastColon == std::string::npos) {
        throw std::runtime_error("Invalid format of kind.");
    }
    return kind_.substr(0, lastColon);
}

std::string
OsduKind::version() const
{
    const auto lastColon = kind_.find_last_of(':');
    if (lastColon == std::string::npos) {
        throw std::runtime_error("Invalid format of kind.");
    }
    return kind_.substr(lastColon + 1);
}

OsduKind
OsduKind::changeType(const std::string& newType) const
{
    // TODO HACK - should handle the kind's components properly
    const auto ftn = fullTypeName();
    const auto lastColon = ftn.find_last_of(':');
    if (lastColon == std::string::npos) {
        throw std::runtime_error("Invalid format of kind.");
    }
    return OsduKind(ftn.substr(0, lastColon + 1) + newType);
}

} // namespace segysdk::utilities::osdu
