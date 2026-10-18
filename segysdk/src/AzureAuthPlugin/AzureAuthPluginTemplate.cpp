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

#include "../AuthPlugin/AuthPluginCInterface.h"
#include "../AzureAuthPlugin/AzureAuthPluginImpl.h"

#include <memory>
#include <string>
#include <string_view>
#include <array>
#include <limits>

/**
 * @brief Azure authentication plugin implementing the AuthPlugin C interface.
 *
 *        This plugin uses Azure Identity SDK's DefaultAzureCredential, which
 *        automatically selects the best available credential from the chain:
 *        EnvironmentCredential -> WorkloadIdentityCredential ->
 *        ManagedIdentityCredential -> AzureCliCredential -> ...
 *
 *        No manual mode selection is needed; the SDK handles it.
 **/

namespace
{

enum class ErrorMessageCodes : int64_t
{
    // note: first two error codes are reserved for predefined meaning
    Success = segysdk::auth::AUTH_SUCCESS,
    UnrecoverableError = -segysdk::auth::AUTH_UNRECOVERABLE_ERROR,

    // starting from this value there are neither reserved values nor meanings...
    WrongInstanceIsGiven,
    TokenFetchFailed,
    InsufficientBufferArraySize,
    MissingEnvironmentVariable,
    OutOfRangeErrorCode,
    NotEnoughRoomForErrorDescription,

    ErrorMessageCodes_Count // this is a special value to estimate number of error codes in our set
};

std::array<std::string_view const,
    static_cast<size_t>(ErrorMessageCodes::ErrorMessageCodes_Count)> s_messages = {
    "Success",
    "Unrecoverable Error",
    "Invalid plugin instance",
    "Failed to fetch Azure token",
    "Insufficient buffer size for token or provider name",
    "Missing required Azure environment variable",
    "Error code out of implemented range",
    "Insufficient buffer size for error description"
};

} // namespace

void*
Init(uint64_t RefreshBeforeTokenExpirationTime)
{
    try {
        auto plugin = std::make_unique<segysdk::auth::AzureAuthPluginImpl>(
            static_cast<size_t>(RefreshBeforeTokenExpirationTime));

        // Release ownership - the caller is responsible via Destroy()
        return reinterpret_cast<void*>(plugin.release());
    }
    catch (...) {
        return nullptr;
    }
}

int64_t
GetAuthProviderName(void* Instance, char* AuthProviderNamePtr, uint64_t AuthProviderNameMaxLength)
{
    try {
        auto* plugin = reinterpret_cast<segysdk::auth::AzureAuthPluginImpl*>(Instance);

        if (plugin == nullptr) {
            return -static_cast<int64_t>(ErrorMessageCodes::WrongInstanceIsGiven);
        }

        std::string authProviderName{};
        const int64_t errorCode = plugin->GetAuthProviderName(authProviderName);

        if (segysdk::auth::AUTH_SUCCESS not_eq errorCode) {
            return errorCode;
        }

        if (authProviderName.length() >= AuthProviderNameMaxLength) {
            return -static_cast<int64_t>(ErrorMessageCodes::InsufficientBufferArraySize);
        }

        authProviderName.copy(AuthProviderNamePtr, AuthProviderNameMaxLength, 0U);

        // Note: the provider name is expected to be a zero terminated string
        AuthProviderNamePtr[authProviderName.length()] = '\0';

        return static_cast<int64_t>(ErrorMessageCodes::Success);
    }
    catch (...) {
        return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
    }
}

int64_t
GetToken(void* Instance, char* TokenPtr, uint64_t TokenMaxLength)
{
    try {
        auto* plugin = reinterpret_cast<segysdk::auth::AzureAuthPluginImpl*>(Instance);

        if (plugin == nullptr) {
            return -static_cast<int64_t>(ErrorMessageCodes::WrongInstanceIsGiven);
        }

        std::string token{};
        int64_t const errorCode = plugin->GetToken(token);

        if (segysdk::auth::AUTH_SUCCESS not_eq errorCode) {
            return -static_cast<int64_t>(ErrorMessageCodes::TokenFetchFailed);
        }

        if (token.length() >= TokenMaxLength) {
            return -static_cast<int64_t>(ErrorMessageCodes::InsufficientBufferArraySize);
        }

        token.copy(TokenPtr, TokenMaxLength, 0U);

        // Note: the token value is expected to be a zero terminated string
        TokenPtr[token.length()] = '\0';

        return static_cast<int64_t>(ErrorMessageCodes::Success);
    }
    catch (...) {
        return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
    }
}

int64_t
IsTokenValid(void* Instance)
{
    try {
        auto* plugin = reinterpret_cast<segysdk::auth::AzureAuthPluginImpl*>(Instance);
        if (plugin != nullptr) {
            return plugin->IsTokenValid();
        }
        return -static_cast<int64_t>(ErrorMessageCodes::WrongInstanceIsGiven);
    }
    catch (...) {
        return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
    }
}

int64_t
GetTimeToExpiry(void* Instance)
{
    try {
        auto* plugin = reinterpret_cast<segysdk::auth::AzureAuthPluginImpl*>(Instance);
        if (plugin != nullptr) {
            return plugin->GetTimeToExpiry();
        }
        return -static_cast<int64_t>(ErrorMessageCodes::WrongInstanceIsGiven);
    }
    catch (...) {
        return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
    }
}

int64_t
CanRefreshToken(void* Instance)
{
    try {
        auto* plugin = reinterpret_cast<segysdk::auth::AzureAuthPluginImpl*>(Instance);
        if (plugin != nullptr) {
            return plugin->CanRefreshToken();
        }
        return -static_cast<int64_t>(ErrorMessageCodes::WrongInstanceIsGiven);
    }
    catch (...) {
        return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
    }
}

int64_t
GetErrorDescription(void* Instance, int64_t ErrorCode, char* ErrorDescriptionPtr, uint64_t ErrorDescriptionMaxLength)
{
    try {
        // First try the plugin's own error descriptions
        auto* plugin = reinterpret_cast<segysdk::auth::AzureAuthPluginImpl*>(Instance);
        if (plugin != nullptr) {
            const auto& messageText = plugin->GetErrorDescription(ErrorCode);

            if (messageText.length() >= ErrorDescriptionMaxLength) {
                return -static_cast<int64_t>(ErrorMessageCodes::NotEnoughRoomForErrorDescription);
            }

            messageText.copy(ErrorDescriptionPtr, ErrorDescriptionMaxLength, 0U);

            // Note: the description text is expected to be a zero terminated string
            ErrorDescriptionPtr[messageText.length()] = '\0';

            return static_cast<int64_t>(ErrorMessageCodes::Success);
        }

        // Fallback to template error messages
        if (ErrorCode == std::numeric_limits<int64_t>::min()) {
            return -static_cast<int64_t>(ErrorMessageCodes::OutOfRangeErrorCode);
        }

        auto const errorCodeAsIndex = static_cast<size_t>(-ErrorCode);

        if (static_cast<size_t>(ErrorMessageCodes::ErrorMessageCodes_Count) <= errorCodeAsIndex) {
            return -static_cast<int64_t>(ErrorMessageCodes::OutOfRangeErrorCode);
        }

        auto const& messageText = s_messages[errorCodeAsIndex];

        if (messageText.length() >= ErrorDescriptionMaxLength) {
            return -static_cast<int64_t>(ErrorMessageCodes::NotEnoughRoomForErrorDescription);
        }

        messageText.copy(ErrorDescriptionPtr, ErrorDescriptionMaxLength, 0U);
        ErrorDescriptionPtr[messageText.length()] = '\0';

        return static_cast<int64_t>(ErrorMessageCodes::Success);
    }
    catch (...) {
        return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
    }
}

int64_t
Destroy(void* Instance)
{
    try {
        auto* plugin = reinterpret_cast<segysdk::auth::AzureAuthPluginImpl*>(Instance);
        delete plugin;
    }
    catch (...) {
        return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
    }

    return static_cast<int64_t>(ErrorMessageCodes::Success);
}
