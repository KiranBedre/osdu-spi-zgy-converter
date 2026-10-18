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

#include "../segysdk_details/SegyException.h"

#include <dlfcn.h>


namespace segysdk::library_loader
{

void
DynamicLibraryLoader::loadLibrary(std::string&& libraryPath)
{
    handle_ = dlopen(libraryPath.c_str(), RTLD_NOW);
    if (!handle_) {
        std::string err_msg = "Couldn't load:" + libraryPath;
        throw SegyInvalidArgumentException(err_msg);
    }
    libraryPath_ = std::move(libraryPath);
}

void*
DynamicLibraryLoader::getFunction(const std::string& functionName)
{
    if (handle_) {
        void* function = dlsym(handle_, functionName.c_str());
        if (function) {
            return function;
        }
    }
    return nullptr;
}

void
DynamicLibraryLoader::closeLibrary()
{
    if (handle_) {
        dlclose(handle_);
    }
}

} // namespace segysdk::library_loader
