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

namespace segysdk::utilities
{

class SEGYSDK_DLL_PUBLIC EnvironmentVariable
{
public:
    static std::string getRawValue(const std::string& variableName);
    static std::string getStringEnv(const std::string& variableName);

    static bool setStringEnv(const std::string& variableName, const std::string& value);
    static bool unsetStringEnv(const std::string& variableName);

    static int getIntEnv(const std::string& variableName, int fallback);
    static double getDoubleEnv(const std::string& variableName, int fallback);

};

} // namespace segysdk::utilities
