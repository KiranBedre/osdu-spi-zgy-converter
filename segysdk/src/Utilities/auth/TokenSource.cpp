/*
Copyright 2022 Schlumberger

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

#include "../Utilities/auth/TokenSource.h"

#include "../Utilities/Environment.h"
#include "../Utilities/auth/CachingTokenSource.h"
#include "../Utilities/auth/FailingTokenSource.h"
#include "../Utilities/auth/StaticTokenSource.h"
#include "../Utilities/EnvironmentVariableStringConsts.h"
#include "../segysdk_details/SegyException.h"
#include "../Logger/Logger.h"

#include <filesystem>

namespace segysdk::utilities::auth
{

TokenSource::~TokenSource()
{
    /* EMPTY */
}

std::unique_ptr<TokenSource>
TokenSource::none(std::string const& message)
{
    SEGYLOG_DEBUG("  **** From none ");
    return std::make_unique<FailingTokenSource>(message.empty() ? "No token credentials are available." : message);
}

std::unique_ptr<TokenSource>
TokenSource::fromToken(std::string const& token)
{
    // SEGYLOG_DEBUG("  **** From Token ");
    return std::make_unique<StaticTokenSource>(token);
}

std::unique_ptr<TokenSource>
TokenSource::fromAuthenticationPlugin(std::string authPluginFileName, size_t safetyThreshold)
{
    SEGYLOG_DEBUG("From Authentication plugin ");
    if (authPluginFileName.empty()) {
        const std::string msg = "TokenSource::fromAuthenticationPlugin: argument [authPluginFileName] must be set to a valid plugin file.";
        throw SegyInvalidArgumentException(msg);
    }
    return std::make_unique<CachingTokenSource>(std::move(authPluginFileName), safetyThreshold);
}

std::unique_ptr<TokenSource>
TokenSource::fromAuthenticationPlugin()
{
    std::string authPluginFileName = Environment::instance().authenticationPluginFile();
    if (authPluginFileName.empty()) {
        const std::string msg = "TokenSource::fromAuthenticationPlugin:\nEnv variable [" + EnvironmentVariableStringConsts::AUTHENTICATION_PLUGIN_FILE + "] must be set to a valid plugin file.";
        throw SegyInvalidArgumentException(msg);
    }

    auto const safetyThresholdRaw = Environment::instance().tokenExpirationThresholdSeconds();

    if (0 > safetyThresholdRaw) {
        throw SegyInvalidArgumentException("TokenSource::fromAuthenticationPlugin: negative value of token expiration time");
    }

    const auto safetyThreshold = static_cast<size_t>(safetyThresholdRaw);
    return std::make_unique<CachingTokenSource>(std::move(authPluginFileName), safetyThreshold);
}

} // namespace segysdk::utilities::auth
