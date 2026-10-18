/*
Copyright 2023 Schlumberger

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

#include "../BasicFileAuthPlugin/BasicFileVariableStringConsts.h"
#include "../Utilities/EnvironmentVariables.h"

#include <memory>
#include <string>

namespace segysdk::auth
{

class BasicFileEnvironment final : segysdk::utilities::EnvironmentVariable, BasicFileVariableStringConsts
{

public:
    virtual ~BasicFileEnvironment();

    static BasicFileEnvironment& instance();

    void dump() const;

    std::string getSegySDKTokenFile();

    int getAuthenticationSafetyThreshold();

    

private:
    static std::shared_ptr<BasicFileEnvironment> instance_;

    static bool insecurePrintTokenToDebug();

    static std::string getRawValue(const std::string& variableName);

    static std::string skipBearerPrefix(const std::string& auth);

    static bool getInsecurePrintFlag();
};

} // namespace Slb::Internal::SAuth2::Client
