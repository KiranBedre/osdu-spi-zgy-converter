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

#include "../AuthPlugin/AuthPluginCFunctionType.h"
#include "../AuthPlugin/AuthPluginInterface.h"
#include "../LibraryLoader/DynamicLibraryLoader.h"

#include <memory>

namespace segysdk::auth
{

struct SEGYSDK_DLL_PUBLIC AuthPluginLibWrapperImpl final : AuthPluginInterface
{
    /**
     * @brief Creates and initializes authentication object instance.
     *
     * @param [IN] LibraryLoader a STL shared pointer referencing the shared library loader instance;
     * @param [IN] RefreshBeforeTokenExpirationTime specifies minimal interval till the token expiration (in seconds) the token refresh procedure has to be triggered
     *
     **/
    AuthPluginLibWrapperImpl(std::unique_ptr<segysdk::library_loader::DynamicLibraryLoader> LibraryLoader,
                             size_t RefreshBeforeTokenExpirationTime);

    ~AuthPluginLibWrapperImpl() override;

    /**
     * @brief Obtains the authentication provider name 
     *
     * @param [OUT] AuthProviderName points to STL string to store the authentication token value to
     *
     * @return a zero (0) on success, negative integer specifying the error otherwise.
     *
     **/
    int64_t GetAuthProviderName(std::string& AuthProviderName);

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
    bool InitFunctionPointers();
    bool InitPluginInstance();
    void DisposePluginInstance();

private:
    static constexpr size_t const tokenBufferMax_ = 4096U;

    static constexpr char const* const functionInit_ = "Init";
    static constexpr char const* const functionGetToken_ = "GetToken";
    static constexpr char const* const functionGetAuthProviderName_ = "GetAuthProviderName";
    static constexpr char const* const functionIsTokenValid_ = "IsTokenValid";
    static constexpr char const* const functionGetTimeToExpiry_ = "GetTimeToExpiry";
    static constexpr char const* const functionCanRefreshToken_ = "CanRefreshToken";
    static constexpr char const* const functionGetErrorDescription_ = "GetErrorDescription";
    static constexpr char const* const functionDestroy_ = "Destroy";

    std::unique_ptr<segysdk::library_loader::DynamicLibraryLoader> libraryLoader_;

    segysdk::auth::c_function::Init_t InitFn_ = nullptr;
    segysdk::auth::c_function::GetToken_t GetTokenFn_ = nullptr;
    segysdk::auth::c_function::GetToken_t GetAuthProviderNameFn_ = nullptr;
    segysdk::auth::c_function::IsTokenValid_t IsTokenValidFn_ = nullptr;
    segysdk::auth::c_function::GetTimeToExpiry_t GetTimeToExpiryFn_ = nullptr;
    segysdk::auth::c_function::CanRefreshToken_t CanRefreshTokenFn_ = nullptr;
    segysdk::auth::c_function::GetErrorDescription_t GetErrorDescriptionFn_ = nullptr;
    segysdk::auth::c_function::Destroy_t DestroyFn_ = nullptr;

    void* pluginInstance_ = nullptr;
};

} // namespace segysdk::auth
