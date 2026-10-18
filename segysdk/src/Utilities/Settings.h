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

#include <map>
#include <memory>
#include <mutex>
#include <shared_mutex>
#include <string>

namespace segysdk::utilities
{

class SEGYSDK_DLL_PUBLIC Settings
{
public:
    static Settings& instance();

    bool addHttpHeader(const std::string& headerName, const std::string& headerValue);

    const std::map<std::string, std::string>& getHttpHeaders() const;

    Settings();

private:
    static std::mutex instanceMtx_;
    static std::shared_ptr<Settings> instance_;

    mutable std::shared_mutex headersMtx_;
    std::map<std::string, std::string> httpHeaders_;
};

}
