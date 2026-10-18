/*
Copyright 2024 Schlumberger

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

#include "PluginUtilities.h"

#ifdef _WIN32
#include <windows.h>
#endif

namespace segysdk::utilities
{
std::filesystem::path 
PluginUtilities::getPluginPath(const std::string& pluginName)
{
    std::filesystem::path dllParentPath;
#ifdef _WIN32
    static const std::string postfix = ".dll";
    static const std::string prefix = "";
    constexpr size_t pathBufferSize = 1024;
    char pathBuffer[pathBufferSize] = {0};
    GetModuleFileNameA(nullptr, pathBuffer, pathBufferSize);
    dllParentPath = std::filesystem::path(pathBuffer).parent_path();

#else
    static const std::string postfix = ".so";
    static const std::string prefix = "lib";
    dllParentPath = std::filesystem::canonical("/proc/self/exe").parent_path();
#endif

    return dllParentPath.append(prefix + pluginName + postfix);
}

} // namespace segysdk::utilities
