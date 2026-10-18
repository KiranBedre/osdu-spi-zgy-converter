/*
Copyright 2021-2022 Schlumberger

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

/** @file
 * @brief SDCredentials.h contains the declaration of the SDCredentials class used to provide authentication information to the Seismic Data Management Services (SDMS).
 *
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../Utilities/auth/TokenSource.h"

#include <memory>
#include <string>
#include <map>

namespace segysdk
{

/**
 * @brief Seismic Data Management Services (SDMS) endpoint and credentials.
 *
 * Contains an endpoint URL and a set of credentials for SDMS access.
 * Currently, only one kind of authentication workflow is supported: with API key and Bearer token.
 **/
struct SEGYSDK_DLL_PUBLIC SDCredentials
{
    /**
     * @brief Constructor to use with a dynamic access token source.
     *
     * Constructor to use SDMS service with API key and a token source.
     *
     * @param url     Base URL of the SDMS service.
     * @param apiKey  API key for the SDMS service.
     * @param tokenSource   token source for a static Bearer token.
     * @param logging Enable SDAPI logging.
     **/
    SDCredentials(std::string url,
                  std::string apiKey,
                  std::unique_ptr<segysdk::utilities::auth::TokenSource> tokenSource,
                  std::map<std::string, std::string> headers = std::map<std::string, std::string>(),
                  bool logging = false);

    /**
     * @brief Base URL of the SDMS service.
     **/
    std::string url() const;

    /**
     * @brief API key for the SDMS service.
     **/
    std::string apiKey() const;

    /**
     * @brief Access token, without the "Bearer: " prefix.
     *
     * @remarks Be aware that the token may change over time, depending on the authorization provider.
     **/
    std::string token() const;

    /**
     * @brief Access headers associated with SDMS user context.
     *
     * @remarks Be aware that the token may change over time, depending on the authorization provider.
     **/
    const std::map<std::string, std::string>& headers() const;

    /**
     * @brief Access token, without the "Bearer: " prefix.
     *
     * @remarks Be aware that the token may change over time, depending on the authorization provider.
     **/
    std::string authProvider() const;

    /**
     * @brief Direct access to the authorization token source.
     **/
    std::unique_ptr<segysdk::utilities::auth::TokenSource>& tokenSource() const;

    /**
     * @brief Enable SDAPI logging.
     **/
    bool logging() const;

    /**
     * @brief Null credentials.
     *
     * Empty SDCredentials instance to use as default for services where no authentication is necessary.
     **/
    static SDCredentials none();

private:
    struct Impl;
    std::shared_ptr<Impl> impl_;
};

} // namespace segysdk
