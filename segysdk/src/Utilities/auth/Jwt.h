/*
Copyright 2021 Schlumberger

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

#include <chrono>
#include <map>
#include <string>

namespace segysdk::utilities::auth
{

/*
 * @brief Very limited JWT implementation, only to get the expiration claim from a token string.
 */
struct SEGYSDK_DLL_PUBLIC Jwt
{
    explicit Jwt(const std::string& token);

    std::chrono::time_point<std::chrono::system_clock> expiration() const;

private:
    const int64_t expiration_;
};

} // namespace segysdk::utilities::auth
