/*
Copyright 2025 Schlumberger

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

#include "Utilities/Containers.h"
#include "tools/Context/Context.h"
#include "tools/Params/Params.h"

#include <memory>
#include <string>
#include <unordered_map>

namespace tool
{

class StorageClient;

using StorageClientPtr = std::unique_ptr<StorageClient>;

class StorageConnector
{
public:
    ~StorageConnector();
    explicit StorageConnector(StorageClientPtr&& client);
    Context getContext(const Params& params) const;
    std::string addFileImagePngRecord(const Params& params, const std::string& fileSource, std::uint64_t fileSize) const;
    std::string addFileImagePngArtefacts(const Params& params, const segysdk::Strings& resourceIds) const;
    Json::Value getJsonRecord(const std::string& recordId) const;
    std::string storeJsonRecord(const Json::Value& record) const;

private:
    StorageClientPtr client_;
    mutable std::unordered_map<std::string, Json::Value> recordsCache_;
};

using StorageConnectorPtr = std::unique_ptr<StorageConnector>;

} // namespace tool
