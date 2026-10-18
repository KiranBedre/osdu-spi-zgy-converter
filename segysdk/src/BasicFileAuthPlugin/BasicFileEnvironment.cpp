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

#include "../BasicFileAuthPlugin/BasicFileEnvironment.h"
#include "../Utilities/EnvironmentVariableStringConsts.h"

#include "../Logger/Logger.h"

#include <array>
#include <functional>
#include <memory>

namespace segysdk::auth
{

std::shared_ptr<BasicFileEnvironment> BasicFileEnvironment::instance_;

BasicFileEnvironment::~BasicFileEnvironment()
{
    /* EMPTY */
}


BasicFileEnvironment&
BasicFileEnvironment::instance()
{
    if (!instance_) {
        instance_ = std::make_shared<BasicFileEnvironment>();
    }

    return *instance_;
}

void
BasicFileEnvironment::dump() const
{
    const std::array<std::reference_wrapper<std::string const>, 2U> publicEnvVars = {
        SEGYSDK_BASIC_FILE_TOKEN_PATH,
        TOKEN_EXPIRATION_THRESHOLD_SECONDS

    };

    SEGYLOG_DEBUG("BasicFileAuth Environment variables:");
    for (const auto& var : publicEnvVars) {
        SEGYLOG_DEBUG(std::string(var) + "=[" + getRawValue(var) + "]");
    }
       
    SEGYLOG_DEBUG("END BasicFileAuth Environment variables");
}


bool
BasicFileEnvironment::insecurePrintTokenToDebug()
{
    return getIntEnv(segysdk::utilities::EnvironmentVariableStringConsts::SEGYTOZGY_INSECURE_PRINT_TOKEN, 0) > 0;
}

std::string
BasicFileEnvironment::getSegySDKTokenFile()
{
    return getRawValue(BasicFileEnvironment::SEGYSDK_BASIC_FILE_TOKEN_PATH);
}

int
BasicFileEnvironment::getAuthenticationSafetyThreshold()
{
    return getIntEnv(BasicFileVariableStringConsts::TOKEN_EXPIRATION_THRESHOLD_SECONDS, 600);
}

bool
BasicFileEnvironment::getInsecurePrintFlag()
{
    return getIntEnv(BasicFileVariableStringConsts::InsecurePrintFlag, 0) != 0;
}

std::string
BasicFileEnvironment::getRawValue(const std::string& variableName)
{
    return getStringEnv(variableName.c_str());
}

} // namespace segysdk::auth
