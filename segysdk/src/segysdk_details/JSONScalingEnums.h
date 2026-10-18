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

namespace segysdk::details
{

class SEGYSDK_DLL_PUBLIC JSONScalingEnums
{

private:
public:
    inline static const std::string NoScaling = "NoScaling";
    inline static const std::string Default = "Default";
    inline static const std::string Override = "Override";
};
} // namespace segysdk::details
