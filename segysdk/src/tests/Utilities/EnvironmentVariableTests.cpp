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

#include "../Utilities/EnvironmentVariables.h"

#include <gtest/gtest.h>

namespace segysdk::utilities::tests
{
class EnvironmentVariableTests : public testing::Test
{

protected:
    void SetUp() override
    {
//EMPTY
    }

    void TearDown() override
    {
        segysdk::utilities::EnvironmentVariable::unsetStringEnv(env_var_name);
    }

   const std::string env_var_name = "EnvironmentTestVariable";
};

TEST_F(EnvironmentVariableTests, PutAndSetString)
{
    std::string value = "Success";
    segysdk::utilities::EnvironmentVariable::setStringEnv(env_var_name, value);

    std::string result = segysdk::utilities::EnvironmentVariable::getStringEnv(env_var_name);
    EXPECT_EQ(value, result);

    value = "Success 2.0";
    segysdk::utilities::EnvironmentVariable::setStringEnv(env_var_name, value);

    result = segysdk::utilities::EnvironmentVariable::getStringEnv(env_var_name);
    EXPECT_EQ(value, result);

}

TEST_F(EnvironmentVariableTests, PutAndSetInteger)
{
    int value = 256;
    segysdk::utilities::EnvironmentVariable::setStringEnv(env_var_name, std::to_string(value));

    int result = segysdk::utilities::EnvironmentVariable::getIntEnv(env_var_name, 0);
    EXPECT_EQ(value, result);

}

} // namespace segysdk::utilities::tests
