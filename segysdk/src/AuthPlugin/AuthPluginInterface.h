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

#include <cstddef>
#include <cstdint>
#include <string>

namespace segysdk::auth
{

/**
 * @brief Error code when the last operation succeeds
 *
 * Provides simple and self descriptive constant to check for success
 *
 */
static constexpr int64_t const AUTH_SUCCESS = 0;

/**
 * @brief Error code when the last operation fails as unrecoverable error
 *
 * Provides simple and self descriptive constant to check for unrecoverable error
 *
 */
static constexpr int64_t const AUTH_UNRECOVERABLE_ERROR = -1;

/**
 * @class AuthPluginInterface
 *
 * @brief The Authentication Plugin Interface
 *
 * Provides C++ wrapper of the C-style authentication plugin interface. Use AuthPluginFactory::CreatePluginInteface(.) to create an instance.
 *
 */
struct SEGYSDK_DLL_PUBLIC AuthPluginInterface
{
    /**
     * @brief Creates and initializes authentication object instance.
     *
     * @param [IN] RefreshBeforeTokenExpirationTime specifies minimal interval till the token expiration (in seconds) the token refresh procedure has to be triggered
     *
     **/
    AuthPluginInterface(size_t RefreshBeforeTokenExpirationTime);

    virtual ~AuthPluginInterface();

    /**
     * @brief Returns a textual string allowing the API to know current authentication method employed
     *
     * @return a text string describing the current authentication provider scheme
     *
     **/
    virtual int64_t GetAuthProviderName(std::string& AuthProviderName) = 0;


    /**
     * @brief Obtains the valid authentication token value
     *
     * @param [OUT] Token points to STL string to store the authentication token value to
     *
     * @return a zero (0) value on success, negative integer specifying the error otherwise.
     *
     **/
    virtual int64_t GetToken(std::string& Token) = 0;

    /**
     * @brief Checks if the available token is not empty string and is not expired
     *
     * @return a zero (0) value on valid token, negative integer specifying the error otherwise; value -1 is reserved for unrecoverable error
     *
     **/
    virtual int64_t IsTokenValid() = 0;

    /**
     * @brief Returns time left before the available token is expired (in seconds)
     *
     * @return positive integer means time left before the available token is expired (in seconds), negative integer specifies the error; value -1 is reserved for unrecoverable error
     *
     **/
    virtual int64_t GetTimeToExpiry() = 0;

    /**
     * @brief Checks if the token refresh feature is supported and no errors have been detected during previous refresh sessions
     *
     * @return a zero (0) value if the refreshing is available, negative integer specifying the error otherwise; value -1 is reserved for unrecoverable error
     *
     **/
    virtual int64_t CanRefreshToken() = 0;

    /**
     * @brief Obtains the textual description of the error code
     *
     * @param [IN] ErrorCode specifies the error message returned GetToken(.) function is to be described
     *
     * @return a STL string that contains the textual description of the error code
     *
     **/
    virtual std::string GetErrorDescription(int64_t ErrorCode) = 0;

protected:
    size_t const refreshBeforeTokenExpirationTime_;
};

} // namespace segysdk::auth
