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

#include "../Utilities/Settings.h"
#include "../Utilities/Environment.h"

namespace segysdk::utilities
{

std::shared_ptr<Settings> Settings::instance_;
std::mutex Settings::instanceMtx_;

Settings::Settings() : httpHeaders_() 
{
    std::unique_lock lock(headersMtx_);
    auto partitionId = Environment::instance().osduDataPartitionId();
    if (!partitionId.empty()) {
        httpHeaders_.emplace("data-partition-id", partitionId);
    }

    auto corrId = Environment::instance().correlationId();
    if (!corrId.empty()) {
        httpHeaders_.emplace("correlation-id", corrId);
    }

    auto userId = Environment::instance().userId();
    if (!userId.empty()) {
        httpHeaders_.emplace("x-on-behalf-of", userId);
    }

    auto subscriptionKey = Environment::instance().subscriptionKey();
    if (!subscriptionKey.empty()) {
        httpHeaders_.emplace("subscription-key", subscriptionKey);
    }
}

Settings&
Settings::instance() {
    // Don't lock if was already created
    if (!instance_) {
        std::lock_guard lock(instanceMtx_);
        if (!instance_) {
            instance_ = std::make_shared<Settings>();
        }
    }

    return *instance_;
}

bool 
Settings::addHttpHeader(const std::string& headerName, const std::string& headerValue)
{
    std::unique_lock lock(headersMtx_);
    httpHeaders_[headerName] = headerValue;
    return true;
}

const std::map<std::string, std::string>&
Settings::getHttpHeaders() const
{
    std::shared_lock lock(headersMtx_);
    return httpHeaders_;
}

}
