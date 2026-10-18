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

#include "AzureAuthPluginImpl.h"

#include <azure/core/credentials/credentials.hpp>

#include <cstdlib>
#include <iostream>
#include <string>

namespace segysdk::auth
{

// ---------------------------------------------------------------------------
// Construction / destruction
// ---------------------------------------------------------------------------

AzureAuthPluginImpl::AzureAuthPluginImpl(size_t RefreshBeforeTokenExpirationTime)
    : AuthPluginInterface(RefreshBeforeTokenExpirationTime),
      tokenExpiry_(std::chrono::system_clock::now())
{
    // Read configurable scope (default: Azure Resource Manager)
    const char* scopeEnv = std::getenv("AZURE_AUTH_SCOPE");
    scope_ = (scopeEnv != nullptr) ? scopeEnv : "https://management.azure.com/.default";

    // DefaultAzureCredential automatically tries credentials in order:
    //   EnvironmentCredential -> WorkloadIdentityCredential ->
    //   ManagedIdentityCredential -> AzureCliCredential -> ...
    credential_ = std::make_shared<Azure::Identity::DefaultAzureCredential>();

    std::cout << "AzureAuthPlugin: Initialized with DefaultAzureCredential, scope=" << scope_ << std::endl;
}

AzureAuthPluginImpl::~AzureAuthPluginImpl() = default;

// ---------------------------------------------------------------------------
// AuthPluginInterface overrides
// ---------------------------------------------------------------------------

int64_t
AzureAuthPluginImpl::GetAuthProviderName(std::string& AuthProviderName)
{
    AuthProviderName = "Azure-DefaultCredential";
    return AUTH_SUCCESS;
}

int64_t
AzureAuthPluginImpl::GetToken(std::string& Token)
{
    std::lock_guard<std::mutex> lock(tokenMutex_);

    auto now = std::chrono::system_clock::now();
    auto threshold = std::chrono::seconds(refreshBeforeTokenExpirationTime_);

    if (cachedToken_.empty() or now >= (tokenExpiry_ - threshold)) {
        try {
            Azure::Core::Credentials::TokenRequestContext tokenRequestContext;
            tokenRequestContext.Scopes.push_back(scope_);

            auto accessToken = credential_->GetToken(tokenRequestContext, Azure::Core::Context());

            cachedToken_ = accessToken.Token;
            tokenExpiry_ = static_cast<std::chrono::system_clock::time_point>(accessToken.ExpiresOn);

            std::cout << "AzureAuthPlugin: Successfully retrieved token" << std::endl;
        }
        catch (const std::exception& e) {
            std::cerr << "AzureAuthPlugin: Token fetch failed: " << e.what() << std::endl;
            return AUTH_UNRECOVERABLE_ERROR;
        }
        catch (...) {
            std::cerr << "AzureAuthPlugin: Token fetch failed with unknown error" << std::endl;
            return AUTH_UNRECOVERABLE_ERROR;
        }
    }

    Token = cachedToken_;
    return AUTH_SUCCESS;
}

int64_t
AzureAuthPluginImpl::IsTokenValid()
{
    std::lock_guard<std::mutex> lock(tokenMutex_);

    if (cachedToken_.empty()) {
        return AUTH_UNRECOVERABLE_ERROR;
    }

    if (std::chrono::system_clock::now() >= tokenExpiry_) {
        return AUTH_UNRECOVERABLE_ERROR;
    }

    return AUTH_SUCCESS;
}

int64_t
AzureAuthPluginImpl::GetTimeToExpiry()
{
    std::lock_guard<std::mutex> lock(tokenMutex_);

    auto remaining = std::chrono::duration_cast<std::chrono::seconds>(
        tokenExpiry_ - std::chrono::system_clock::now());

    return remaining.count();
}

int64_t
AzureAuthPluginImpl::CanRefreshToken()
{
    return AUTH_SUCCESS;
}

std::string
AzureAuthPluginImpl::GetErrorDescription(int64_t ErrorCode)
{
    switch (ErrorCode) {
    case AUTH_SUCCESS:
        return "Success";
    case AUTH_UNRECOVERABLE_ERROR:
        return "Unrecoverable error in Azure authentication";
    default:
        return "Unknown error code: " + std::to_string(ErrorCode);
    }
}

} // namespace segysdk::auth
