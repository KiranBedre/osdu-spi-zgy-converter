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

#include "OsduObjectReference.h"

#include <algorithm>
#include <cstdint>
#include <stdexcept>

namespace segysdk::utilities::osdu
{

OsduObjectReference::OsduObjectReference(const std::string& ref)
{
    if (ref.empty()) {
        throw std::invalid_argument("Object reference must not be empty.");
    }
    const auto lastColon = ref.find_last_of(':');
    if (lastColon == std::string::npos) {
        throw std::runtime_error("Invalid format of object reference.");
    }

    if (lastColon == ref.size() - 1) {
        // ends with ':' -> latest version
        objectId_ = ref.substr(0, lastColon);
        version_ = noVersion;
    }
    else if (std::any_of(ref.begin() + lastColon + 1, ref.end(), [&](const char& c) { return !std::isdigit(c); })) {
        // WORKAROUND for invalid objects already in storage
        // not a valid number (e.g. GUID or hexadecimal ID) after last ':' -> latest version
        objectId_ = ref;
        version_ = noVersion;
    }
    else {
        // number after last ':' -> version reference
        objectId_ = ref.substr(0, lastColon);
        version_ = std::stoull(ref.substr(lastColon + 1));
    }
}

std::string
OsduObjectReference::objectId() const
{
    return objectId_;
}

bool
OsduObjectReference::isLatestVersion() const
{
    return version_ == noVersion;
}

uint64_t
OsduObjectReference::specifiedVersion() const
{
    return version_;
}

} // namespace segysdk::utilities::osdu
