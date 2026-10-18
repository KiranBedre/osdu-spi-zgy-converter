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

#include "../../segysdk_details/DLL_EXPORT.h"

#include <memory>
#include <string>

namespace segysdk::utilities::auth
{

/**
 * @brief Abstract authentication token source class.
 *
 * @details A token source can provide an authentication token (a.k.a. Bearer token) for calling other services.
 *         This is an abstract class, you can use the factory methods to create one of the default ones,
 *         or implement your own authentication by implementing the token() function.
 *         Custom implementations MUST be thread-safe.
 *
 **/
struct SEGYSDK_DLL_PUBLIC TokenSource
{
    /**
     * @brief Returns the authentication token according to the actual token source implementation.
     *
     * @details The returned token can be assumed to be valid at the moment of the call.
     *          You should not store the token, only the token source.
     **/
    virtual std::string token() const = 0;

    /**
     * @brief Returns the authentication provider name.
     *
     * @details Will provide the authentication provider scheme name.
     **/
    virtual std::string authProviderName() const = 0;

    /*
     * @brief Default virtual destructor.
     */
    virtual ~TokenSource();

    /**
     * @brief A token source for no authentication token.
     *
     * @details This token source will throw an exception when a token is requested.
     *
     * @param message An optional exception message.
     *
     * @return unique pointer to TokenSource interface
     **/
    static std::unique_ptr<TokenSource> none(std::string const& message = "");

    /**
     * @brief A token source for a static Bearer token.
     *
     * @details This token source will always return the given token, does not support token expiration nor refreshing.
     *
     * @param token Bearer token (only the token value, "Bearer: " prefix should be omitted)
     *
     * @return unique pointer to TokenSource interface
     **/
    static std::unique_ptr<TokenSource> fromToken(std::string const& token);

    /**
     * @brief A token source for a refresh-able Bearer token.
     *
     * @details This token source will return the token that has been fetched from authentication plugin/interface.
     *
     * @param authPluginFileName specifies full file path name of authentication plugin to be used to fetch the token value from
     * @param safetyThreshold specifies minimal interval till the token expiration (in seconds) the token fetch procedure has to be triggered
     *
     * @return unique pointer to TokenSource interface
     **/
    static std::unique_ptr<TokenSource> fromAuthenticationPlugin(std::string authPluginFileName, size_t safetyThreshold);

    /**
     * @brief A token source for a refresh-able Bearer token.
     *
     * @details This is the simplified version of the fromAuthenticationPlugin above; the parameters are taked from the configuration and the one above is invoked.
     *
     * @return unique pointer to TokenSource interface
     **/
    static std::unique_ptr<TokenSource> fromAuthenticationPlugin();

};

} // namespace segysdk::utilities::auth
