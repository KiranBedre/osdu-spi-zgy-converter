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

#include "../FileIO/CacheSizeOption.h"
#include "../segysdk/SegySessionFactory.h"

#include <json/json.h>

#include <string>

namespace SampleCode::Common
{
class SessionUtilities
{
private:
    static segysdk::CacheSizeOption getSDCacheSize();
    static std::unique_ptr<segysdk::api::SegySession> initSession(const bool& isCloudSession, const segysdk::api::SegySessionFactory& sessionFactory);

    static bool isInputSEGYFileInCloud(const std::string& jsonParams);

public:
    static bool TestIfCloudSession(const std::string& file_name);
    static std::unique_ptr<segysdk::api::SegySession> GetSegySessionFromFileCollectionJson(const std::string& osduFileCollectionJson);
    static std::shared_ptr<segysdk::segyindex::SegyIndex> GetSegyIndex(const std::unique_ptr<segysdk::api::SegySession>& segySession);
};

} // namespace SampleCode::Common
