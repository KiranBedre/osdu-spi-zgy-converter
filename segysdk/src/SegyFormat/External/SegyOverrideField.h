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

#include <string>
#include <utility>

namespace segysdk::segyformat::external
{

class SEGYSDK_DLL_PUBLIC SegyOverrideField
{

private:
    std::string key_;
    double value_;

public:
    SegyOverrideField() = default;

    SegyOverrideField(std::string key, const double value)
      : key_(std::move(key)), value_(value)
    {}

    std::string getKey() const { return key_; }

    double getValue() const { return value_; }
};

} // namespace segysdk::segyformat::external
