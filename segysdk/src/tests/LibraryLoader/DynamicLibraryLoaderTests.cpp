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

#include "../../LibraryLoader/DynamicLibraryLoader.h"
#include "../../Utilities/PluginUtilities.h"

#include <gtest/gtest.h>

#include <memory>
#include <filesystem>

namespace segysdk::utilities::tests
{

class DynamicLibraryLoaderTest : public testing::Test
{

protected:
    void SetUp() override
    {
        setDllPath();
        plugin_ = std::make_shared<segysdk::library_loader::DynamicLibraryLoader>(dllPath_.string());
        loadFunctions();
    }

    void TearDown() override
    {
    }

    void setDllPath()
    {
       
        dllPath_ = segysdk::utilities::PluginUtilities::getPluginPath("DummyDLL").string();
    }

    void loadFunctions()
    {
        setNumberF_ = reinterpret_cast<setNumberFunction>(plugin_->getFunction(setNumberFunctionName));
        EXPECT_TRUE(setNumberF_ != nullptr);
        getNumberF_ = reinterpret_cast<getNumberFunction>(plugin_->getFunction(getNumberFunctionName));
        EXPECT_TRUE(getNumberF_ != nullptr);
    }

    int getNumber()
    {
        return getNumberF_();
    }

    void setNumber(int number)
    {
        setNumberF_(number);
    }

private:
    using setNumberFunction = void (*) (int);
    using getNumberFunction = int (*) ();
    static inline const std::string setNumberFunctionName = "setNumber";
    static inline const std::string getNumberFunctionName = "getNumber";
    std::filesystem::path dllPath_;

private:
    std::shared_ptr<segysdk::library_loader::DynamicLibraryLoader> plugin_;
    getNumberFunction getNumberF_;
    setNumberFunction setNumberF_;
};

TEST_F(DynamicLibraryLoaderTest, PutAndSetNumber)
{
    // Check the default
    EXPECT_EQ(getNumber(), 0);
    // Set to some
    const int check_value = 42;
    setNumber(check_value);
    EXPECT_EQ(getNumber(), check_value);
}


} // namespace segysdk::utilities::tests
