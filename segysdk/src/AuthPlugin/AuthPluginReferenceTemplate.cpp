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

#include "../AuthPlugin/AuthPluginCInterface.h"

#include "../AuthPlugin/AuthPluginBasicImpl.h"

#include <string>
#include <string_view>

/**
 * @brief This is a reference implementation of the authentication plugin.
 *        Here the basic authentication is used that means the plugin will return given token's value
 **/

namespace
{

std::string const s_dummyTokenValue{"This is a dummy token value"};

segysdk::auth::AuthPluginBasicImpl s_plugin(s_dummyTokenValue);

enum class ErrorMessageCodes : int64_t
{
    // note: first two error codes are reserved for predefined meaning
    Success = segysdk::auth::AUTH_SUCCESS,
    UnrecoverableError = -segysdk::auth::AUTH_UNRECOVERABLE_ERROR,

    // starting from this value there is neither reserved values nor meanings...
    WrongInstanceIsGiven = UnrecoverableError + 1,
    CannotRetrieveTokenFromFakeSystem,
    InsufficientBufferArraySize,
    NotImplemented,
    SomeStrangeErrorCodeThatIsOutOfRange,
    NotEnoughRoomToStoreTheErrorDescriptionText,

    ErrorMessageCodes_Count // this is a special value to estimate number of error codes in our set
};

std::array<std::string_view const, static_cast<size_t>(ErrorMessageCodes::ErrorMessageCodes_Count)> s_messages = {
    "Success",
    "Unrecoverable Error",
    "Oops: a wrong instance is given",
    "Oops: cannot retrieve token from fake system",
    "Oops: not enough room to store the token value",
    "Oops: not implemented",
    "Oops: some strange error code value that is out of implemented range",
    "Oops: not enough room to store the error description text"};

} // namespace

void*
Init([[maybe_unused]] uint64_t RefreshBeforeTokenExpirationTime)
try {

    // here we just return address of the fake instance
    // it is expected that the real system will create a new instance
    return reinterpret_cast<void*>(&s_plugin);
}
catch (...) {
    return nullptr;
}

int64_t
GetAuthProviderName(void* Instance, char* AuthProviderNamePtr, uint64_t AuthProviderNameMaxLength)
{
    try {

        if (Instance not_eq &s_plugin) {
            return -static_cast<int64_t>(ErrorMessageCodes::WrongInstanceIsGiven);
        }

        std::string authProviderName{};
        const int64_t errorCode = s_plugin.GetAuthProviderName(authProviderName);

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
try {

    if (Instance not_eq &s_plugin) {
        return -static_cast<int64_t>(ErrorMessageCodes::WrongInstanceIsGiven);
    }

    std::string token{};
    int64_t const errorCode = s_plugin.GetToken(token);

    if (segysdk::auth::AUTH_SUCCESS not_eq errorCode) {
        return -static_cast<int64_t>(ErrorMessageCodes::CannotRetrieveTokenFromFakeSystem);
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

int64_t
IsTokenValid(void* Instance)
try {

    if (Instance not_eq &s_plugin) {
        return -static_cast<int64_t>(ErrorMessageCodes::WrongInstanceIsGiven);
    }

    // the fake token value is not valid at all
    return -static_cast<int64_t>(ErrorMessageCodes::NotImplemented);
}
catch (...) {
    return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
}

int64_t
GetTimeToExpiry(void* Instance)
try {

    if (Instance not_eq &s_plugin) {
        return -static_cast<int64_t>(ErrorMessageCodes::WrongInstanceIsGiven);
    }

    // the fake token value does not have any expiration
    return -static_cast<int64_t>(ErrorMessageCodes::NotImplemented);
}
catch (...) {
    return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
}

int64_t
CanRefreshToken(void* Instance)
try {

    if (Instance not_eq &s_plugin) {
        return -static_cast<int64_t>(ErrorMessageCodes::WrongInstanceIsGiven);
    }

    // the fake system is not able to refresh the token value
    return -static_cast<int64_t>(ErrorMessageCodes::NotImplemented);
}
catch (...) {
    return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
}

int64_t
GetErrorDescription(void* Instance, int64_t ErrorCode, char* ErrorDescriptionPtr, uint64_t ErrorDescriptionMaxLength)
try {

    // Note: technically speaking, each instance may have some individual error code set
    if (Instance not_eq &s_plugin) {
        return -static_cast<int64_t>(ErrorMessageCodes::WrongInstanceIsGiven);
    }

    auto const errorCodeAsIndex = static_cast<size_t>(-ErrorCode);

    if (static_cast<size_t>(ErrorMessageCodes::ErrorMessageCodes_Count) <= errorCodeAsIndex) {
        return -static_cast<int64_t>(ErrorMessageCodes::SomeStrangeErrorCodeThatIsOutOfRange);
    }

    auto const& messageText = s_messages[errorCodeAsIndex];

    if (messageText.length() >= ErrorDescriptionMaxLength) {
        return -static_cast<int64_t>(ErrorMessageCodes::NotEnoughRoomToStoreTheErrorDescriptionText);
    }

    messageText.copy(ErrorDescriptionPtr, ErrorDescriptionMaxLength, 0U);

    // Note: the description text is expected to be a zero terminated string
    ErrorDescriptionPtr[messageText.length()] = '\0';

    return static_cast<int64_t>(ErrorMessageCodes::Success);
}
catch (...) {
    return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
}

int64_t
Destroy(void* Instance)
try {

    if (Instance not_eq &s_plugin) {
        return -static_cast<int64_t>(ErrorMessageCodes::WrongInstanceIsGiven);
    }
    return static_cast<int64_t>(ErrorMessageCodes::Success);
    // this is a fake instance, so no disposal if feasible
    // it is expected that the real system will dispose the instance
}
catch (...) {
    return -static_cast<int64_t>(ErrorMessageCodes::UnrecoverableError);
}
