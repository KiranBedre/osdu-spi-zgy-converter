/*
Copyright (c) Microsoft Corporation.

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

#include "../AuthPlugin/AuthPluginInterface.h"

#include <azure/identity/default_azure_credential.hpp>

#include <chrono>
#include <memory>
#include <mutex>
#include <string>

namespace segysdk::auth
{

/**
 * @brief Azure authentication plugin implementation.
 *
 * @details Uses the Azure Identity SDK's DefaultAzureCredential to acquire tokens.
 *          DefaultAzureCredential automatically tries multiple credential types in order:
 *            1. EnvironmentCredential
 *            2. WorkloadIdentityCredential
 *            3. ManagedIdentityCredential
 *            4. AzureCliCredential
 *            5. AzurePowerShellCredential
 *
 *          The correct credential is selected automatically based on the runtime
 *          environment — no manual strategy selection or env-var checks are needed.
 *
 *          The OAuth2 scope defaults to "https://management.azure.com/.default"
 *          and can be overridden via the AZURE_AUTH_SCOPE environment variable.
 **/
struct SEGYSDK_DLL_PUBLIC AzureAuthPluginImpl final : AuthPluginInterface
{

    /**
     * @brief Creates and initializes Azure authentication plugin instance.
     *
     * @param [IN] RefreshBeforeTokenExpirationTime specifies minimal interval till the token expiration
     *             (in seconds) the token refresh procedure has to be triggered
     **/
    AzureAuthPluginImpl(size_t RefreshBeforeTokenExpirationTime);

    ~AzureAuthPluginImpl() override;

    int64_t GetAuthProviderName(std::string& AuthProviderName) override;
    int64_t GetToken(std::string& Token) override;
    int64_t IsTokenValid() override;
    int64_t GetTimeToExpiry() override;
    int64_t CanRefreshToken() override;
    std::string GetErrorDescription(int64_t ErrorCode) override;

private:
    /// Azure Identity SDK credential — auto-selects the right auth method
    std::shared_ptr<Azure::Identity::DefaultAzureCredential> credential_;

    /// OAuth2 scope for token requests
    std::string scope_;

    // Token caching
    mutable std::mutex tokenMutex_;
    mutable std::string cachedToken_;
    mutable std::chrono::system_clock::time_point tokenExpiry_;
};

} // namespace segysdk::auth
