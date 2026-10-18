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

#include "SegyOverrideField.h"

#include <map>
#include <vector>

namespace segysdk::segyformat::external
{

class SEGYSDK_DLL_PUBLIC BinaryHeaderOverrides
{
private:
    std::map<std::string, SegyOverrideField> _overrides;

public:
    BinaryHeaderOverrides(std::vector<SegyOverrideField> overrides)
    {
        for (auto& ovr : overrides) // access by reference to avoid copying
        {
            if (_overrides.find(ovr.getKey()) == _overrides.end()) {
                _overrides.insert(std::make_pair(ovr.getKey(), ovr));
            }
        }
    }
};

} // namespace segysdk::segyformat::external
