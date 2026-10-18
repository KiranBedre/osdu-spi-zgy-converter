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

#include <stdint.h>

/**
 * @brief Here the C-stype interface functions' types are declared; any authentication plugin shall strictly follow the interface
 *
 * @details
 * Please refer to 'LibraryLoader/DynamicLibraryLoader.h' file for more details on each function purpose
 *
 **/

namespace segysdk::auth::c_function
{

typedef void* (*Init_t)(uint64_t RefreshTillTokenExpirationTime);
typedef int64_t (*GetToken_t)(void* Instance, char* TokenPtr, uint64_t TokenMaxLength);
typedef int64_t (*GetAuthProviderName_t)(void* Instance, char* AuthProviderNamePtr, uint64_t AuthProviderNameMaxLength);
typedef int64_t (*IsTokenValid_t)(void* Instance);
typedef int64_t (*GetTimeToExpiry_t)(void* Instance);
typedef int64_t (*CanRefreshToken_t)(void* Instance);
typedef int64_t (*GetErrorDescription_t)(void* Instance, int64_t ErrorCode, char* ErrorDescriptionPtr, uint64_t ErrorDescriptionMaxLength);
typedef int64_t (*Destroy_t)(void* Instance);

} // namespace segysdk::auth::c_function
