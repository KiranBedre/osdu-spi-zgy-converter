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

#include "tools/StorageConnector/test/StorageClient.h"

namespace tool::tests
{

StorageClient::StorageClient(StorageRecords&& records)
  : records_{std::move(records)}
{
}

std::string
StorageClient::doGetRecord(const std::string& recordId) const
{
    if (const auto it = records_.find(recordId); it != records_.end()) {
        return it->second;
    }
    return std::string{R"({"recordId":")"}.append(recordId).append(R"("})");
}

std::string
StorageClient::doStoreRecord(const std::string& record) const
{
    return record;
}

} // namespace tool::tests
