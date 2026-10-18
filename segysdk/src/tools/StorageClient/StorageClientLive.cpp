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

#include "tools/StorageClient/StorageClientLive.h"

#include "Utilities/Authorization.h"

namespace tool::live
{

StorageClient::StorageClient()
  : client_{segysdk::utilities::Authorization::storageSvcTokenProvider()}
{
}

std::string
StorageClient::doGetRecord(const std::string& recordId) const
{
    return client_.getRecord(recordId);
}

std::string
StorageClient::doStoreRecord(const std::string& record) const
{
    return client_.storeRecord(record);
}

} // namespace tool::live
