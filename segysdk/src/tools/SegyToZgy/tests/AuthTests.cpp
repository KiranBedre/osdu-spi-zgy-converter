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

#include "../Utilities/auth/BearerTokenAuth.h"
#include "../Utilities/auth/Jwt.h"

#include <gtest/gtest.h>

TEST(AuthTests, SimpleBearerToken)
{
    const segysdk::utilities::auth::BearerTokenAuth sut{
        segysdk::utilities::auth::TokenSource::fromToken("TestToken")};

    const auto result = sut.headers();
    ASSERT_EQ(result.size(), 1);
    EXPECT_EQ(result[0].name, "Authorization");
    EXPECT_EQ(result[0].value, "Bearer TestToken");
}

TEST(AuthTests, JwtExpiration)
{
    const segysdk::utilities::auth::Jwt sut{"eyJ0eXAiOiJKV1QiLCJhbGciOiJIUzI1NiJ9.eyJpc3MiOiJPbmxpbmUgSldUIEJ1aWxkZXIiLCJpYXQiOjE2Mjg5NDE2NzQsImV4cCI6MTY2MDQ3NzY3NCwiYXVkIjoid3d3LmV4YW1wbGUuY29tIiwic3ViIjoianJvY2tldEBleGFtcGxlLmNvbSIsIkdpdmVuTmFtZSI6IkpvaG5ueSIsIlN1cm5hbWUiOiJSb2NrZXQiLCJFbWFpbCI6Impyb2NrZXRAZXhhbXBsZS5jb20iLCJSb2xlIjpbIk1hbmFnZXIiLCJQcm9qZWN0IEFkbWluaXN0cmF0b3IiXX0.WjUFD07NRclMNj2_GLEcDxp-NReDbykEIPOXWVY-ooI"};

    EXPECT_EQ(sut.expiration().time_since_epoch(), std::chrono::seconds(1660477674));
}
