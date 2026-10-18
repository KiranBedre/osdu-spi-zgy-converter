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

#include "../LibraryLoader/DynamicLibraryLoader.h"
#include "../Logger/Logger.h"
#include "../Utilities/PluginUtilities.h"

#include <gtest/gtest.h>

#include <filesystem>

namespace segysdk::utilities::tests
{

std::filesystem::path
getGsmModulePath()
{
    // Name should be the same as the built target of GSM module
    static const std::string dllName = "segysdk.logger.gsm";

    return segysdk::utilities::PluginUtilities::getPluginPath(dllName);
}

TEST(GsmModuleLoader, CheckSharedObject)
{
    auto dllPath = getGsmModulePath();
    // Check that the library can be loaded
    std::shared_ptr<segysdk::library_loader::DynamicLibraryLoader> plugin;
    EXPECT_NO_THROW(plugin = std::make_shared<segysdk::library_loader::DynamicLibraryLoader>(dllPath.string()));

    // Functions may be loaded
    void* creator = plugin->getFunction(segysdk::logger::Logger::CreateLogWriterFunctionName_);
    EXPECT_NE(creator, nullptr);

    void* deleter = plugin->getFunction(segysdk::logger::Logger::DeleteLogWriterFunctionName_);
    EXPECT_NE(deleter, nullptr);
}

} // namespace segysdk::utilities::tests

