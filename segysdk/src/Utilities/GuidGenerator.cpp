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

#include "GuidGenerator.h"

#if defined(__linux__) || defined(__unix__)
#define GUIDGENERATOR_USE_LIBUUID
#endif

#ifdef _WIN32

#include <windows.h>

#include <stdexcept>

namespace segysdk::utilities::details
{

static std::string
newGuid()
{
    UUID uuid;
    RPC_CSTR uuidStr;
    auto success = UuidCreate(&uuid);
    if (success != S_OK) {
        throw std::runtime_error("UuidCreate failed.");
    }

    success = UuidToString(&uuid, &uuidStr);
    if (success != S_OK) {
        throw std::runtime_error("UuidToString failed.");
    }

    std::string result = (char*)uuidStr;

    if (uuidStr != NULL)
        RpcStringFree(&uuidStr);

    return result;
}

} // namespace segysdk::utilities::details

#elif defined(GUIDGENERATOR_USE_LIBUUID)

#include <uuid/uuid.h>

namespace segysdk::utilities::details
{

static std::string
newGuid()
{
    uuid_t uuid;
    uuid_generate(uuid);
    char buffer[37];
    uuid_unparse_lower(uuid, buffer);
    return buffer;
}

} // namespace segysdk::utilities::details

#else
#error Unsupported platform for GUID generator.
#endif

namespace segysdk::utilities
{

std::string
GuidGenerator::newGuid()
{
    return details::newGuid();
}

} // namespace segysdk::utilities
