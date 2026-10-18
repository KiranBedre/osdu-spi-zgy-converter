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

#include "../AuthPlugin/AuthPluginLibWrapperImpl.h"

#include "../AuthPlugin/AuthPluginException.h"

#include <array>

namespace segysdk::auth
{

AuthPluginLibWrapperImpl::AuthPluginLibWrapperImpl(std::unique_ptr<segysdk::library_loader::DynamicLibraryLoader> LibraryLoader,
                                                   size_t RefreshBeforeTokenExpirationTime)
  : AuthPluginInterface(RefreshBeforeTokenExpirationTime),
    libraryLoader_(std::move(LibraryLoader))
{
    if (not libraryLoader_) {
        throw segysdk::auth::AuthPluginException("AuthPluginLibWrapperImpl: given shared library loader is empty");
    }

    if (not InitFunctionPointers()) {
        throw segysdk::auth::AuthPluginException("AuthPluginLibWrapperImpl: missing interface function(s) in authentication plugin shared library");
    }

    if (not InitPluginInstance()) {
        throw segysdk::auth::AuthPluginException("AuthPluginLibWrapperImpl: cannot create authentication plugin instance");
    }
}

AuthPluginLibWrapperImpl::~AuthPluginLibWrapperImpl()
{
    DisposePluginInstance();
}

int64_t
AuthPluginLibWrapperImpl::GetAuthProviderName(std::string& AuthProviderName)
try {
    std::array<char, tokenBufferMax_> authProviderNameBuffer{};

    AuthProviderName.clear();

    decltype(auto) result = GetTokenFn_(pluginInstance_,
                                        authProviderNameBuffer.data(),
                                        authProviderNameBuffer.size());

    if (AUTH_SUCCESS == result) {
        AuthProviderName.assign(authProviderNameBuffer.data());
    }

    return result;
}
catch (...) {
    return AUTH_UNRECOVERABLE_ERROR;
}


int64_t
AuthPluginLibWrapperImpl::GetToken(std::string& Token)
try {
    std::array<char, tokenBufferMax_> tokenBuffer{};

    Token.clear();

    decltype(auto) result = GetTokenFn_(pluginInstance_,
                                        tokenBuffer.data(),
                                        tokenBuffer.size());

    if (AUTH_SUCCESS == result) {
        Token.assign(tokenBuffer.data());
    }

    return result;
}
catch (...) {
    return AUTH_UNRECOVERABLE_ERROR;
}

int64_t
AuthPluginLibWrapperImpl::IsTokenValid()
try {
    return IsTokenValidFn_(pluginInstance_);
}
catch (...) {
    return AUTH_UNRECOVERABLE_ERROR;
}

int64_t
AuthPluginLibWrapperImpl::GetTimeToExpiry()
try {
    return GetTimeToExpiryFn_(pluginInstance_);
}
catch (...) {
    return AUTH_UNRECOVERABLE_ERROR;
}

int64_t
AuthPluginLibWrapperImpl::CanRefreshToken()
try {
    return CanRefreshTokenFn_(pluginInstance_);
}
catch (...) {
    return AUTH_UNRECOVERABLE_ERROR;
}

std::string
AuthPluginLibWrapperImpl::GetErrorDescription(int64_t ErrorCode)
try {
    std::array<char, tokenBufferMax_> descriptionBuffer{};

    [[maybe_unused]] auto const errorCode = GetErrorDescriptionFn_(pluginInstance_,
                                                                   ErrorCode,
                                                                   descriptionBuffer.data(),
                                                                   descriptionBuffer.size());

    return std::string{descriptionBuffer.data()};
}
catch (...) {
    return std::string{};
}

bool
AuthPluginLibWrapperImpl::InitFunctionPointers()
{
    InitFn_ = reinterpret_cast<segysdk::auth::c_function::Init_t>(libraryLoader_->getFunction(functionInit_));
    GetAuthProviderNameFn_ = reinterpret_cast<segysdk::auth::c_function::GetAuthProviderName_t>(libraryLoader_->getFunction(functionGetAuthProviderName_));
    GetTokenFn_ = reinterpret_cast<segysdk::auth::c_function::GetToken_t>(libraryLoader_->getFunction(functionGetToken_));
    IsTokenValidFn_ = reinterpret_cast<segysdk::auth::c_function::IsTokenValid_t>(libraryLoader_->getFunction(functionIsTokenValid_));
    GetTimeToExpiryFn_ = reinterpret_cast<segysdk::auth::c_function::GetTimeToExpiry_t>(libraryLoader_->getFunction(functionGetTimeToExpiry_));
    CanRefreshTokenFn_ = reinterpret_cast<segysdk::auth::c_function::CanRefreshToken_t>(libraryLoader_->getFunction(functionCanRefreshToken_));
    GetErrorDescriptionFn_ = reinterpret_cast<segysdk::auth::c_function::GetErrorDescription_t>(libraryLoader_->getFunction(functionGetErrorDescription_));
    DestroyFn_ = reinterpret_cast<segysdk::auth::c_function::Destroy_t>(libraryLoader_->getFunction(functionDestroy_));

    return nullptr not_eq InitFn_
           and nullptr not_eq GetAuthProviderNameFn_
           and nullptr not_eq GetTokenFn_
           and nullptr not_eq IsTokenValidFn_
           and nullptr not_eq GetTimeToExpiryFn_
           and nullptr not_eq CanRefreshTokenFn_
           and nullptr not_eq GetErrorDescriptionFn_
           and nullptr not_eq DestroyFn_;
}

bool
AuthPluginLibWrapperImpl::InitPluginInstance()
{
    pluginInstance_ = InitFn_(AuthPluginInterface::refreshBeforeTokenExpirationTime_);

    return nullptr not_eq pluginInstance_;
}

void
AuthPluginLibWrapperImpl::DisposePluginInstance()
try {
    if (nullptr not_eq pluginInstance_) {
        [[maybe_unused]] auto const errorCode = DestroyFn_(pluginInstance_);
        pluginInstance_ = nullptr;
    }
}
catch (...) {
    /* EMPTY */
}

} // namespace segysdk::auth
