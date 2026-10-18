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

#include "../FileIO/SDCredentials.h"

#include "../segysdk_details/SegyException.h"

#include <utility>
#include <map>

namespace segysdk
{

struct SDCredentials::Impl
{
    Impl(std::string url,
         std::string apiKey,
         std::unique_ptr<utilities::auth::TokenSource> tokenSource,
         std::map<std::string, std::string> headers,
         bool logging)
      : url_(std::move(url)),
        apiKey_(std::move(apiKey)),
        tokenSource_(std::move(tokenSource)),
        headers_(std::move(headers)),
        logging_(logging)
    {
        /* EMPTY */
    }

    std::string url() const { return url_; }
    std::string apiKey() const { return apiKey_; }
    const std::map<std::string, std::string>& headers() const { return headers_; }
    std::unique_ptr<utilities::auth::TokenSource>& tokenSource() { return tokenSource_; }
    std::string token() const { return tokenSource_->token(); }
    bool logging() const { return logging_; }

private:
    std::string url_;
    std::string apiKey_;
    std::unique_ptr<utilities::auth::TokenSource> tokenSource_;
    std::map<std::string, std::string> headers_;
    bool logging_{};
};

bool
SDCredentials::logging() const
{
    return impl_->logging();
}

SDCredentials
SDCredentials::none()
{
    return {/*url*/ "",
            /*apiKey*/ "",
            utilities::auth::TokenSource::fromToken(""),
            std::map<std::string, std::string>()};
}

std::string
SDCredentials::authProvider() const
{
    if (tokenSource()!= nullptr && tokenSource()->fromAuthenticationPlugin() != nullptr) {
        return tokenSource()->fromAuthenticationPlugin()->authProviderName();
    }
    return "Token";
}

std::string
SDCredentials::token() const
{
    return tokenSource() != nullptr
               ? tokenSource()->token()
               : "";
}

std::unique_ptr<segysdk::utilities::auth::TokenSource>&
SDCredentials::tokenSource() const
{
    return impl_->tokenSource();
}

SDCredentials::SDCredentials(std::string url, std::string apiKey, std::unique_ptr<segysdk::utilities::auth::TokenSource> tokenSource, std::map<std::string, std::string> headers, bool logging)
  : impl_(std::make_shared<Impl>(std::move(url), std::move(apiKey), std::move(tokenSource), std::move(headers), logging))
{
    /* EMPTY */
}

std::string
SDCredentials::url() const
{
    return impl_->url();
}

std::string
SDCredentials::apiKey() const
{
    return impl_->apiKey();
}

const std::map<std::string, std::string>&
SDCredentials::headers() const
{
    return impl_->headers();
}
} // namespace segysdk
