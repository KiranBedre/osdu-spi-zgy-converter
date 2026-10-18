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

#include "EnvironmentVariables.h"

#include <cstdlib>
#include <iostream>

namespace segysdk::utilities
{

std::string
EnvironmentVariable::getRawValue(const std::string& variableName)
{
    return getStringEnv(variableName.c_str());
}

int
EnvironmentVariable::getIntEnv(const std::string& variableName, int fallback)
{
    const std::string value = getRawValue(variableName.c_str());
    if (value.empty())
        return fallback;
    else
        return atoi(value.c_str());
}

double
EnvironmentVariable::getDoubleEnv(const std::string& variableName, int fallback)
{
    const std::string value = getRawValue(variableName.c_str());
    if (value.empty())
        return fallback;
    else
        return atof(value.c_str());
}

std::string
EnvironmentVariable::getStringEnv(const std::string& variableName)
{
    char* tmp = std::getenv(variableName.c_str());
    return tmp == nullptr ? "" : tmp;
}

bool
EnvironmentVariable::setStringEnv(const std::string& variableName, const std::string& value)
{
    bool success = false;
#ifdef _WIN32
    // set new shell environmental variable using _putenv (setenv not available on Windows)
    const std::string envVar = variableName + "=" + value;
    success = _putenv(envVar.data()) == 0;
    if (!success) {
        std::cerr << "setStringEnv: _putenv failed\n";
    }

#else
    // Linux or Unix
    constexpr bool const overwrite = true;
    success = setenv(variableName.data(), value.data(), overwrite) == 0;
    if (!success) {
        std::cerr << "setStringEnv: setenv failed\n";
    }
#endif

    return success;
}

bool
EnvironmentVariable::unsetStringEnv(const std::string& variableName)
{
    bool success = false;
#ifdef _WIN32
    // set new shell environmental variable using _putenv (setenv not available on Windows)
    const std::string envVar = variableName + "=";
    success = _putenv(envVar.data()) == 0;
    if (!success) {
        std::cerr << "unsetStringEnv: _putenv failed" << std::endl;
    }
#else
    // Linux or Unix
    constexpr bool const overwrite = true;
    std::string value{};
    success = setenv(variableName.data(), value.data(), overwrite) == 0;
    if (!success) {
        std::cerr << "unsetStringEnv: unsetenv failed" << std::endl;
    }
#endif



    return success;
}

} // namespace segysdk::utilities
