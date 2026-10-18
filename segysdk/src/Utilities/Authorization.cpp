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

#include "../Utilities/Authorization.h"

#include "../Logger/Logger.h"
#include "../Utilities/auth/BearerTokenAuth.h"
#include "../Utilities/auth/CachingTokenSource.h"
#include "../Utilities/TokenSourceFactory.h"
#include "../segysdk_details/SegyException.h"

#include <cpprest/asyncrt_utils.h>
#include <cpprest/base_uri.h>

#include <chrono>

namespace segysdk::utilities
{

std::unique_ptr<segysdk::utilities::auth::TokenSource>
Authorization::sdTokenSource()
{
    return TokenSourceFactory::getTokenSource();
}

std::unique_ptr<segysdk::utilities::auth::HttpCredentialProvider>
Authorization::storageSvcTokenProvider()
{
    return std::make_unique<segysdk::utilities::auth::BearerTokenAuth>(
        segysdk::utilities::TokenSourceFactory::getTokenSource());
}

} // namespace segysdk::utilities
