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

#include <string>

namespace segysdk::library_loader
{

class SEGYSDK_DLL_PUBLIC DynamicLibraryLoader
{
public:
    explicit DynamicLibraryLoader(std::string libraryPath);

    DynamicLibraryLoader(const DynamicLibraryLoader&) = delete;
    DynamicLibraryLoader& operator=(const DynamicLibraryLoader&) = delete;

    ~DynamicLibraryLoader();

    void* getFunction(const std::string& functionName);
    void closeLibrary();

    std::string getLibraryPath() const;

private:
    void loadLibrary(std::string&& libraryPath);

private:
    void* handle_ = nullptr;
    std::string libraryPath_;
};

} // namespace segysdk::library_loader
