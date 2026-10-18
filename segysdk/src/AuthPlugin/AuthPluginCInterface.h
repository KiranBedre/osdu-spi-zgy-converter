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

#ifndef AUTHPLUGCININTERFACE_H_INCLUDED
#define AUTHPLUGCININTERFACE_H_INCLUDED

#include "../segysdk_details/DLL_EXPORT.h"

#include <stdint.h>

/**
 * @brief Here the common interface is declared; any authentication plugin shall strictly follow the interface
 *
 * @details
 * The authentication plugin shall satisfy the following requirements
 * - The authentication object instance shall be created and initialized on the plugin's implementation side
 * - The pointer to the authentication object instance shall be used as a description to distinguish/select the proper authentication object instance
 * - The API functions shall be thread safe; the last one is the plugin's implementation side responsibility
 * - The authentication object instance shall be disposed on the plugin's implementation side
 *
 **/

extern "C" {

/**
 * @brief Creates and initializes authentication object instance.
 *
 * @param [IN] RefreshBeforeTokenExpirationTime specifies minimal interval till the token expiration (in seconds) the token refresh procedure has to be triggered
 *
 * @return a pointer to the just created and initialized authentication object instance on success, NULL otherwise
 *
 **/
SEGYSDK_DLL_PUBLIC void*
Init(uint64_t RefreshBeforeTokenExpirationTime);

/**
 * @brief Obtains the Authentication Provider name as a zero terminated string
 *
 * @param [IN] Instance represents a pointer to the authentication object instance returned by Init(.) function
 * @param [IN] AuthProviderNamePtr points to allocated buffer to store the authentication provider name value as a zero terminated string to
 * @param [IN] AuthProviderNameMaxLength specifies the allocated buffer maximum capacity; '\0' value shall terminate the token value within the buffer capacity
 *
 * @return a zero (0) value on success, negative integer specifying the error otherwise; value -1 is reserved for unrecoverable error
 *
 **/
SEGYSDK_DLL_PUBLIC int64_t
GetAuthProviderName(void* Instance, char* AuthProviderNamePtr, uint64_t AuthProviderNameMaxLength);

/**
 * @brief Checks if the available token is not empty string and is not expired
 *
 * @param [IN] Instance represents a pointer to the authentication object instance returned by Init(.) function
 *
 * @return a zero (0) value on valid token, negative integer specifying the error otherwise; value -1 is reserved for unrecoverable error
 *
 **/
SEGYSDK_DLL_PUBLIC int64_t
GetToken(void* Instance, char* TokenPtr, uint64_t TokenMaxLength);

/**
 * @brief Checks if the available token is not empty string and is not expired
 *
 * @param [IN] Instance represents a pointer to the authentication object instance returned by Init(.) function
 *
 * @return a zero (0) value on valid token, negative integer specifying the error otherwise; value -1 is reserved for unrecoverable error
 *
 **/
SEGYSDK_DLL_PUBLIC int64_t
IsTokenValid(void* Instance);

/**
 * @brief Returns time left before the available token is expired (in seconds)
 *
 * @param [IN] Instance represents a pointer to the authentication object instance returned by Init(.) function
 *
 * @return positive integer means time left before the available token is expired (in seconds), negative integer specifies the error; value -1 is reserved for unrecoverable error
 *
 **/
SEGYSDK_DLL_PUBLIC int64_t
GetTimeToExpiry(void* Instance);

/**
 * @brief Checks if the token refresh feature is supported and no errors have been detected during previous refresh sessions
 *
 * @param [IN] Instance represents a pointer to the authentication object instance returned by Init(.) function
 *
 * @return a zero (0) value if the refreshing is available, negative integer specifying the error otherwise; value -1 is reserved for unrecoverable error
 *
 **/
SEGYSDK_DLL_PUBLIC int64_t
CanRefreshToken(void* Instance);

/**
 * @brief Obtains the textual description of the error code
 *
 * @param [IN] Instance represents a pointer to the authentication object instance returned by Init(.) function
 * @param [IN] ErrorCode specifies the error message returned GetToken(.) function is to be described
 * @param [IN] ErrorDescriptionPtr points to allocated buffer to store the textual description as a zero terminated string to
 * @param [IN] ErrorDescriptionMaxLength specifies the allocated buffer maximum capacity; '\0' value shall terminate the textual description within the buffer capacity
 *
 * @return a zero (0) value on success, negative integer specifying the error otherwise; value -1 is reserved for unrecoverable error
 *
 **/
SEGYSDK_DLL_PUBLIC int64_t
GetErrorDescription(void* Instance, int64_t ErrorCode, char* ErrorDescriptionPtr, uint64_t ErrorDescriptionMaxLength);

/**
 * @brief Disposes the authentication object instance returned by Init(.) function
 *
 * @param [IN] Instance represents a pointer to the authentication object instance returned by Init(.) function
 *
 * @return a zero (0) value on success, negative integer specifying the error otherwise; value -1 is reserved for unrecoverable error
 *
 **/
SEGYSDK_DLL_PUBLIC int64_t
Destroy(void* Instance);

} /* extern "C" */

#endif /* AUTHPLUGCININTERFACE_H_INCLUDED */
