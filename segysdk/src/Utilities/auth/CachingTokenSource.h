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

#include "../Utilities/auth/TokenSource.h"

#include <memory>

namespace segysdk::utilities::auth
{

/**
 * @brief Caching authentication token source class.
 *
 * @details A token source provides the authentication token minimizing a number of bottom layer invocations.
 *
 **/
struct SEGYSDK_DLL_PUBLIC CachingTokenSource final : public TokenSource
{
    /**
     * @brief Constructs the caching authentication token source based on another token source.
     *
     * @param tokenSource another token source to be used to fetch the token value
     * @param safetyThreshold specifies minimal interval till the token expiration (in seconds) the token fetch procedure has to be triggered
     **/
    CachingTokenSource(
        std::unique_ptr<TokenSource>&& tokenSource,
        size_t safetyThreshold);

    /**
     * @brief Constructs the caching authentication token source based on authentication plugin.
     *
     * @param authPluginFileName specifies full file path name of authentication plugin to be used to fetch the token value from
     * @param safetyThreshold specifies minimal interval till the token expiration (in seconds) the token fetch procedure has to be triggered
     **/
    CachingTokenSource(
        std::string authPluginFileName,
        size_t safetyThreshold);

    /**
     * @brief Returns the authentication token.
     *
     * @details The returned token can be assumed to be valid at the moment of the call.
     *          You should not store the token, only the token source.
     **/
    virtual std::string token() const override;

    /**
     * @brief Returns the authentication provider name.
     *
     * @details Will provide the authentication provider scheme name.
     **/
    virtual std::string authProviderName() const override;


private:
    struct CachingTokenSourceImpl;
    inline static const std::string fromToken = "Token";


private:
    std::shared_ptr<CachingTokenSourceImpl> impl_;
};

} // namespace segysdk::utilities::auth
