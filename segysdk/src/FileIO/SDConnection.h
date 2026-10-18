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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../FileIO/SDCredentials.h"

#include <SDAPI/SDManager.h>

namespace segysdk::fileio
{

/*
 * @brief An SDManager instance authenticated with a specific SDCredentials.
 *
 * @detail The purpose of this object is to ensure that the token source of the SDCredentials
 *         is not destructed while it is in use by the SDManager.
 */
struct SEGYSDK_DLL_PUBLIC SDConnection
{
    SDConnection() = default;
    SDConnection(std::shared_ptr<seismicdrive::SDManager> sdManager,
                 std::shared_ptr<SDCredentials> sdCredentials);

    std::shared_ptr<seismicdrive::SDManager> sdManager;
    std::shared_ptr<SDCredentials> sdCredentials;
};

} // namespace segysdk::fileio
