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

class SEGYSDK_DLL_PUBLIC JSONNumericalFormatStrings
{

private:
public:
    inline static const std::string Text = "Text";
    inline static const std::string Integer = "Integer";
    inline static const std::string UnsignedInteger = "UnsignedInteger";
    inline static const std::string Float = "Float";
    inline static const std::string Other = "Other";
    inline static const std::string IBMFloat = "IBMFloat";
    inline static const std::string IEEEFloat = "IEEEFloat";
    inline static const std::string Undefined = "Undefined";
};
} // namespace segysdk::details
