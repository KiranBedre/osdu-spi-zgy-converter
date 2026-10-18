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

#include "../Utilities/TokenSourceFactory.h"
#include "../Utilities/Environment.h"
#include "../FileIO/GenericFileReader.h"
#include "../FileIO/FileIOException.h"
#include "../Logger/Logger.h"

#include <iostream>

namespace segysdk::utilities
{

std::string TokenSourceFactory::static_token_;

void
TokenSourceFactory::configureForStaticToken(const std::string token)
{
    static_token_ = token;
}

std::unique_ptr<auth::TokenSource>
TokenSourceFactory::getTokenSource(const std::string token)
{
    if (std::getenv("AZURE_ENABLE_MSI")) {
        std::cerr << "WARNING: AZURE_ENABLE_MSI is no longer supported. "
                  << "Use AUTHENTICATION_PLUGIN_FILE with the Azure auth plugin instead." << std::endl;
    }

    if (!token.empty())
    {
        std::cerr << "Using token authentication (provided token is not empty)" << std::endl;
        return auth::TokenSource::fromToken(token);
    }

    if (!static_token_.empty()) 
    {
        return auth::TokenSource::fromToken(static_token_);
    }
    
    const std::string sd_token = Environment::instance().sdToken();
    const std::string auth_plugin_name = Environment::instance().authenticationPluginFile();
    
    auto tokenSource = !auth_plugin_name.empty()
                           ? auth::TokenSource::fromAuthenticationPlugin()
                           : auth::TokenSource::fromToken(sd_token);

    if (tokenSource == nullptr) {
        std::cerr << token_source_error_msg_ << std::endl;
    }

    return tokenSource;
}

} // namespace segysdk::utilities
