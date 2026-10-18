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

#include "../Utilities/auth/CachingTokenSource.h"

#include "../AuthPlugin/AuthPluginFactory.h"
#include "../Utilities/auth/Jwt.h"
#include "../segysdk_details/SegyException.h"

#include "../Logger/Logger.h"

#include <chrono>
#include <filesystem>
#include <mutex>

namespace segysdk::utilities::auth
{

struct CachingTokenSource::CachingTokenSourceImpl
{
    using Duration = std::chrono::seconds;

    CachingTokenSourceImpl(
        std::unique_ptr<TokenSource>&& tokenSource,
        size_t safetyThreshold)
      : safetyThreshold_(safetyThreshold),
        tokenSource_(std::move(tokenSource))
    {
        if (not tokenSource_) {
            throw segysdk::SegyInvalidArgumentException("CachingTokenSource: token source is nullptr");
        }
    }

    CachingTokenSourceImpl(
        std::string authPluginFileName,
        size_t safetyThreshold)
      : safetyThreshold_(safetyThreshold),
        authPluginFileName_{std::move(authPluginFileName)}
    {
        // Sanity test - does the plugin file actually exist?
        if (!std::filesystem::exists(authPluginFileName_)) {
            const std::string msg = "CachingTokenSource: authentication plugin does not exist: [" + authPluginFileName_ + "]";
            throw segysdk::SegyInvalidArgumentException(msg);
        }

        authPlugin_ = segysdk::auth::AuthPluginFactory::instance().CreatePluginInteface(authPluginFileName_, safetyThreshold);

        // Sanity test - Did the plugin get properly created?
        if (not authPlugin_) {
            const std::string msg = "CachingTokenSource: authentication plugin interface is nullptr";
            throw segysdk::SegyInvalidArgumentException(msg);
        }
    }

    std::string authProviderName() const
    {
        std::lock_guard lock(mutex_);

        if (tokenSource_) {

            return fromToken;
        }
        std::string authProvider;
        if (segysdk::auth::AUTH_SUCCESS == authPlugin_->GetAuthProviderName(authProvider)) {
            return authProvider;
        }

        throw segysdk::SegyInvalidArgumentException("CachingTokenSource: unable to retrieve auth provider name from authentication plugin");

    }

    std::string token() const
    {
        std::lock_guard lock(mutex_);

        if (tokenSource_) {
            token_ = tokenSource_->token();
        }
        else {
            std::string tokenValue;
            const auto status = authPlugin_->GetToken(tokenValue);
            if (status == segysdk::auth::AUTH_SUCCESS ) {
                token_ = tokenValue;
            }
            else {
                std::string error_descr = authPlugin_->GetErrorDescription(status);
                throw segysdk::SegyInvalidArgumentException("CachingTokenSource: unable to retrieve token from authentication plugin.\nError: " + error_descr);
            }
        }

        if (token_.length()) {
            // this is a not empty token, perform the expiration check
            // to minimize the number of the token origin invocations

            expiration_ = Jwt(token_).expiration();
        }
        else {
            // this is an empty token, set its expiration time to the present time
            // so we will have to retrieve the token value next time
            expiration_ = std::chrono::system_clock::now();
        }

        return token_;
    }


private:
    mutable std::mutex mutex_;

    const Duration safetyThreshold_;
    std::unique_ptr<TokenSource> tokenSource_;
    std::string const authPluginFileName_;
    std::shared_ptr<segysdk::auth::AuthPluginInterface> authPlugin_;

    mutable std::string token_;
    mutable std::chrono::time_point<std::chrono::system_clock> expiration_{};
};

CachingTokenSource::CachingTokenSource(
    std::unique_ptr<TokenSource>&& tokenSource,
    size_t safetyThreshold)
  : impl_(std::make_shared<CachingTokenSourceImpl>(std::move(tokenSource), safetyThreshold))
{
    
    /* EMPTY */
}

CachingTokenSource::CachingTokenSource(
    std::string authPluginFileName,
    size_t safetyThreshold)
  : impl_(std::make_shared<CachingTokenSourceImpl>(std::move(authPluginFileName), safetyThreshold))
{
    /* EMPTY */
}

std::string
CachingTokenSource::token() const
{
    return impl_->token();
}

std::string
CachingTokenSource::authProviderName() const
{
    return impl_->authProviderName();
}


} // namespace segysdk::utilities::auth
