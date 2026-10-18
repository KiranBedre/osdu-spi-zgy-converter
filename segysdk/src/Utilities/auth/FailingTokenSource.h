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
 * @brief Auxiliary fake token source class.
 *
 * @details A token source that throws provided error message on any usage attempt.
 *
 **/
struct SEGYSDK_DLL_PUBLIC FailingTokenSource final : public TokenSource
{
    explicit FailingTokenSource(std::string message);

    /**
     * @brief Throws provided error message on any usage attempt.
     **/
    virtual std::string token() const override;

    /**
     * @brief Returns the authentication provider name.
     *
     * @details Will provide the authentication provider scheme name.
     **/
    virtual std::string authProviderName() const override;

private:
    std::string const message_;
};

} // namespace segysdk::utilities::auth
