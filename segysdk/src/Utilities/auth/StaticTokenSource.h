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
#include <string>

namespace segysdk::utilities::auth
{

/**
 * @brief Static token source class.
 *
 * @details A token source that returns given token value on any usage attempt.
 *
 **/
struct SEGYSDK_DLL_PUBLIC StaticTokenSource final : public TokenSource
{
    explicit StaticTokenSource(std::string token);

    /**
     * @brief Returns given token value.
     **/
    virtual std::string token() const override;

    /**
     * @brief Returns the authentication provider name.
     *
     * @details Will provide the authentication provider scheme name.
     **/
    virtual std::string authProviderName() const override;


private:
    std::string const token_;
};

} // namespace segysdk::utilities::auth
