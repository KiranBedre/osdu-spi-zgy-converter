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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include <string>

namespace segysdk::utilities::osdu
{

class SEGYSDK_DLL_PUBLIC OsduKind
{
public:
    explicit OsduKind(std::string kind);

    std::string kind() const;
    std::string shortTypeName() const;
    std::string fullTypeName() const;
    std::string version() const;

    OsduKind changeType(const std::string& newType) const;

private:
    std::string kind_;
};

} // namespace segysdk::utilities::osdu
