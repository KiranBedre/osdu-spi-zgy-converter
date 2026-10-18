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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../AuthPlugin/AuthPluginInterface.h"

#include <array>

namespace segysdk::auth
{

struct SEGYSDK_DLL_PUBLIC AuthPluginBasicImpl final : AuthPluginInterface
{
    /**
     * @brief Creates and initializes authentication object instance.
     *
     * @param [IN] TokenValue a STL string containing the access token value
     *
     **/
    AuthPluginBasicImpl(std::string const& TokenValue);

    ~AuthPluginBasicImpl() override;

    /**
     * @brief Returns a textual string allowing the API to know current authentication method employed
     *
     * @param [OUT] AuthProviderName points to STL string to store the authentication token value to
     *
     * @return a zero (0) on success, negative integer specifying the error otherwise.
     *
     **/
    virtual int64_t GetAuthProviderName(std::string& AuthProviderName) override;

    /**
     * @brief Obtains the valid authentication token value
     *
     * @param [OUT] Token points to STL string to store the authentication token value to
     *
     * @return a zero (0) on success, negative integer specifying the error otherwise.
     *
     **/
    virtual int64_t GetToken(std::string& Token) override;

    /**
     * @brief Checks if the available token is not empty string and is not expired
     *
     * @return a zero (0) value on valid token, negative integer specifying the error otherwise; value -1 is reserved for unrecoverable error
     *
     **/
    virtual int64_t IsTokenValid() override;

    /**
     * @brief Returns time left before the available token is expired (in seconds)
     *
     * @return positive integer means time left before the available token is expired (in seconds), negative integer specifies the error; value -1 is reserved for unrecoverable error
     *
     **/
    virtual int64_t GetTimeToExpiry() override;

    /**
     * @brief Checks if the token refresh feature is supported and no errors have been detected during previous refresh sessions
     *
     * @return a zero (0) value if the refreshing is available, negative integer specifying the error otherwise; value -1 is reserved for unrecoverable error
     *
     **/
    virtual int64_t CanRefreshToken() override;

    /**
     * @brief Obtains the textual description of the error code
     *
     * @param [IN] ErrorCode specifies the error message returned GetToken(.) function is to be described
     *
     * @return a STL string that contains the textual description of the error code
     *
     **/
    virtual std::string GetErrorDescription(int64_t ErrorCode) override;

private:
    enum class ErrorCode : int64_t
    {
        Success = AUTH_SUCCESS,
        UnrecoverableError = -AUTH_UNRECOVERABLE_ERROR,
        TokenIsExpired,
        TokenIsGiven,

        ERROR_CODES_COUNT
    };

    static constexpr std::array<char const* const, static_cast<size_t>(ErrorCode::ERROR_CODES_COUNT)> const errorMessages_ = {
        "Success",
        "Unrecoverable Error",
        "Token is expired",
        "Cannot refresh token: token is given"};

private:
    std::string const tokenValue_;
};

} // namespace segysdk::auth
