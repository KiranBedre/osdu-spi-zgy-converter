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

#include "OsduWorkProduct100.h"

#include "OsduFieldNameLiterals.h"

namespace segysdk::utilities::osdu
{

OsduWorkProduct100::OsduWorkProduct100(const Json::Value& root)
  : OsduObject(root)
{
}

std::vector<std::string>
OsduWorkProduct100::componentIds() const
{
    std::vector<std::string> ret;
    const auto& components = root_[OsduFieldNameLiterals::Data][OsduFieldNameLiterals::Components];
    for (const auto& id : components) {
        ret.push_back(id.asString());
    }
    return ret;
}

} // namespace segysdk::utilities::osdu
