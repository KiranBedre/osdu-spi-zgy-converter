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

#include "../AuthPlugin/AuthPluginBasicImpl.h"

#include "../AuthPlugin/AuthPluginException.h"
#include "../Utilities/auth/Jwt.h"

#include <chrono>
#include <exception>
#include <sstream>
#include <stdexcept>

namespace segysdk::auth
{

AuthPluginBasicImpl::AuthPluginBasicImpl(std::string const& TokenValue)
  : AuthPluginInterface(0U),
    tokenValue_(TokenValue)
{
    /* EMPTY */
}

AuthPluginBasicImpl::~AuthPluginBasicImpl()
{
    /* EMPTY */
}

int64_t
AuthPluginBasicImpl::GetAuthProviderName(std::string& AuthProviderName)
{
    AuthProviderName = "Basic";
    return 0;
}

int64_t
AuthPluginBasicImpl::GetToken(std::string& Token)
try {
    Token = tokenValue_;
    return 0;
}
catch (...) {
    return -static_cast<int64_t>(ErrorCode::UnrecoverableError);
}

int64_t
AuthPluginBasicImpl::IsTokenValid()
try {
    auto const timeToExpiry = GetTimeToExpiry();

    return timeToExpiry > 0
               ? 0
               : -static_cast<int64_t>(ErrorCode::TokenIsExpired);
}
catch (...) {
    return -static_cast<int64_t>(ErrorCode::UnrecoverableError);
}

int64_t
AuthPluginBasicImpl::GetTimeToExpiry()
try {
    const segysdk::utilities::auth::Jwt jwt(tokenValue_);

    auto const now = std::chrono::system_clock::now();
    // cast the duration into seconds
    const auto seconds = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch());
    const auto jwt_test = std::chrono::duration_cast<std::chrono::seconds>(jwt.expiration().time_since_epoch());

    if (jwt_test > seconds) {
        auto const timeToExpiry = jwt_test - seconds;
        const int64_t result = static_cast<int64_t>(timeToExpiry.count());
        return result;
    }
    else {
        return -static_cast<int64_t>(ErrorCode::TokenIsExpired);
    }
}
catch (const std::exception& ) {
    return -static_cast<int64_t>(ErrorCode::UnrecoverableError);
}

int64_t
AuthPluginBasicImpl::CanRefreshToken()
{
    return -static_cast<int64_t>(ErrorCode::TokenIsGiven);
}

std::string
AuthPluginBasicImpl::GetErrorDescription(int64_t ErrorCode)
{
    size_t const positiveErorCode = static_cast<size_t>(-ErrorCode);

    if (positiveErorCode < static_cast<int64_t>(ErrorCode::ERROR_CODES_COUNT)) {
        return {errorMessages_[positiveErorCode]};
    }

    return {};
}

} // namespace segysdk::auth
