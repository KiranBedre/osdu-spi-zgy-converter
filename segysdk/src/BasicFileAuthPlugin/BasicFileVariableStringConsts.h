/*
Copyright 2023 Schlumberger

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

namespace segysdk::auth
{

class SEGYSDK_DLL_PUBLIC BasicFileVariableStringConsts
{
public:
    inline static const std::string SEGYSDK_BASIC_FILE_TOKEN_PATH = "SEGYSDK_BASIC_FILE_TOKEN_PATH";
    inline static const std::string TOKEN_EXPIRATION_THRESHOLD_SECONDS = "TOKEN_EXPIRATION_THRESHOLD_SECONDS";
    inline static const std::string InsecurePrintFlag = "BASIC_FILE_INSECURE_PRINT_FLAG";
};

} // namespace segysdk::auth
