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

#include "../AuthPlugin/AuthPluginCInterface.h"
#include "../BasicFileAuthPlugin/AuthPluginBasicFileImpl.h"

#include <memory>
#include <string>
#include <string_view>

/**
 * @brief This is a reference implementation of the authentication plugin.
 *        Here the basic authentication is used that means the plugin will return given token's value
 **/

namespace
{

enum class ErrorMessageCodes : int64_t
{
    // note: first two error codes are reserved for predefined meaning
    Success = segysdk::auth::AUTH_SUCCESS,
    UnrecoverableError = -segysdk::auth::AUTH_UNRECOVERABLE_ERROR,

    // starting from this value there are neither reserved values nor meanings...
    WrongInstanceIsGiven = UnrecoverableError + 1,
    UnableToReadTokenFile,
    BasicFileTokenFileIsGiven,
    InsufficientBufferArraySize,
    NotImplemented,
    OutOfRangeErrorCode,
    ErrorMessageCodes_Count, // this is a special value to estimate number of error codes in our set
};

} // namespace

void*
Init([[maybe_unused]] uint64_t RefreshBeforeTokenExpirationTime)
{
    try {

        std::string const s_dummyTokenValue{"Not defined"};

        auto s_plugin = std::make_unique<segysdk::auth::AuthPluginBasicFileImpl>(s_dummyTokenValue);

        // it is expected that the real system will create a new instance
        return reinterpret_cast<void*>(s_plugin.release());
    }
    catch (...) {
        return nullptr;
    }
}

int64_t
GetAuthProviderName(void* Instance, char* AuthProviderNamePtr, uint64_t AuthProviderNameMaxLength)
{
    try {
        auto* s_plugin = reinterpret_cast<segysdk::auth::AuthPluginBasicFileImpl*>(Instance);
        std::string authProviderName{};
        const int64_t errorCode = s_plugin->GetAuthProviderName(authProviderName);

        if (segysdk::auth::AUTH_SUCCESS not_eq errorCode) {
            return errorCode;
        }

        if (authProviderName.length() >= AuthProviderNameMaxLength) {
            return -static_cast<int64_t>(ErrorMessageCodes::InsufficientBufferArraySize);
        }

        authProviderName.copy(AuthProviderNamePtr, AuthProviderNameMaxLength, 0U);

        // Note: the token value is expected to be a zero terminated string
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
        auto* s_plugin = reinterpret_cast<segysdk::auth::AuthPluginBasicFileImpl*>(Instance);
        std::string token{};
        int64_t const errorCode = s_plugin->GetToken(token);

        if (segysdk::auth::AUTH_SUCCESS not_eq errorCode) {
            return errorCode;
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

        auto* s_plugin = reinterpret_cast<segysdk::auth::AuthPluginBasicFileImpl*>(Instance);
        if (s_plugin != nullptr) {
            return s_plugin->IsTokenValid();
        }

        // the fake token value is not valid at all
        return -static_cast<int64_t>(ErrorMessageCodes::NotImplemented);
    }
    catch (...) {
        return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
    }
}

int64_t
GetTimeToExpiry(void* Instance)
{
    try {

        auto* s_plugin = reinterpret_cast<segysdk::auth::AuthPluginBasicFileImpl*>(Instance);
        if (s_plugin != nullptr) {
            return s_plugin->GetTimeToExpiry();
        }

        // the fake token value does not have any expiration
        return -static_cast<int64_t>(ErrorMessageCodes::NotImplemented);
    }
    catch (...) {
        return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
    }
}

int64_t
CanRefreshToken(void* Instance)
{
    try {
        auto* s_plugin = reinterpret_cast<segysdk::auth::AuthPluginBasicFileImpl*>(Instance);
        if (s_plugin != nullptr) {
            return s_plugin->CanRefreshToken();
        }

        // the fake system is not able to refresh the token value
        return -static_cast<int64_t>(ErrorMessageCodes::NotImplemented);
    }
    catch (...) {
        return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
    }
}

int64_t
GetErrorDescription(void* Instance, int64_t ErrorCode, char* ErrorDescriptionPtr, uint64_t ErrorDescriptionMaxLength)
{
    try {

        auto* s_plugin = reinterpret_cast<segysdk::auth::AuthPluginBasicFileImpl*>(Instance);
        if (s_plugin != nullptr) {
            const auto& messageText = s_plugin->GetErrorDescription(ErrorCode);

            if (messageText.length() >= ErrorDescriptionMaxLength) {
                return -static_cast<int64_t>(ErrorMessageCodes::InsufficientBufferArraySize);
            }

            messageText.copy(ErrorDescriptionPtr, ErrorDescriptionMaxLength, 0U);

            // Note: the description text is expected to be a zero terminated string
            ErrorDescriptionPtr[messageText.length()] = '\0';

            return static_cast<int64_t>(ErrorMessageCodes::Success);
        }

    }
    catch (...) {
    }
    return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
}

int64_t
Destroy(void* Instance)
{
    try {
        const auto* s_plugin = reinterpret_cast<segysdk::auth::AuthPluginBasicFileImpl*>(Instance);

        delete s_plugin;

    }
    catch (...) {
        return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
    }

    return static_cast<int64_t>(ErrorMessageCodes::Success);
}