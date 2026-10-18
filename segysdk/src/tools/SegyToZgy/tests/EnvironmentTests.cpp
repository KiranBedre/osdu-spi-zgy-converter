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

#include "../Utilities/Environment.h"

#include <gtest/gtest.h>

#include <algorithm>

class TestableEnvironment final : public segysdk::utilities::Environment
{
public:
    explicit TestableEnvironment(std::vector<std::pair<std::string, std::string>> variables)
      : variables_(std::move(variables))
    {
    }

    std::string getRawValue(const std::string& variableName) const override
    {
        const auto found = std::find_if(variables_.begin(), variables_.end(), [&variableName](const std::pair<std::string, std::string>& p) { return p.first == variableName; });
        if (found == variables_.end())
            return "";
        return found->second;
    }

private:
    const std::vector<std::pair<std::string, std::string>> variables_;
};

TEST(EnvironmentTests, OutputPathNoOverride)
{
    const TestableEnvironment sut{{}};
    EXPECT_EQ(sut.outputPathOverrideOrDefault("default"), "default");
}

TEST(EnvironmentTests, OutputPathOverride)
{
    const TestableEnvironment sut{{{"SEGYTOZGY_OUTPUT_PATH", "override"}}};
    EXPECT_EQ(sut.outputPathOverrideOrDefault("default"), "override");
}
