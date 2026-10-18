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

#include <string>

namespace tool
{

class StorageClient
{
public:
    virtual ~StorageClient() = default;
    std::string getRecord(const std::string& recordId) const;
    std::string storeRecord(const std::string& record) const;

private:
    virtual std::string doGetRecord(const std::string& recordId) const = 0;
    virtual std::string doStoreRecord(const std::string& record) const = 0;
};

} // namespace tool
