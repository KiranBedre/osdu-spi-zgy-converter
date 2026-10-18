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

#include "../FileIO/CacheSizeOption.h"
#include "../segysdk/SegySessionFactory.h"
#include "../segysdk/SegySession.h"
#include "../segysdk/SegySessionImpl.h"

#include <json/json.h>
#include <memory>
#include <string>


namespace segysdk::utilities
{

class SEGYSDK_DLL_PUBLIC SessionUtilities
{

public:

    static bool TestIfCloudSession(const std::string& file_name);
    static std::unique_ptr<api::SegySession> GetSegySessionFromFileCollectionJson(const std::string& osduFileCollectionJson);
    static std::shared_ptr<segyindex::SegyIndex> GetSegyIndex(const std::unique_ptr<api::SegySession>& segySession);
    static std::shared_ptr<segyindex::SegyIndex> GetSegyIndex(const std::shared_ptr<api::SegySession>& segySession);

    static CacheSizeOption getSDCacheSize();

private:
    static std::unique_ptr<api::SegySession> initSession(const bool& isCloudSession, const api::SegySessionFactory& sessionFactory);
    static bool isInputSEGYFileInCloud(const std::string& jsonParams);

    inline static const std::string fileCollectionClass_{":dataset--FileCollection.SEGY:"};
};

} // namespace segysdk::utilities
