/*
Copyright 2024 Schlumberger

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

#include "../BulkAccess/AsyncReader.h"

#include <memory>

namespace segysdk::bulkaccess
{

class ReaderConnector;

namespace async
{

using ReaderConnector = std::shared_ptr<ReaderConnector>;

class SEGYSDK_DLL_PUBLIC ReaderImpl : public Reader
{
public:
    explicit ReaderImpl(ReaderConnector connector);

private:
    Samples doRead(const ReaderParams& params) override;
    ReaderConnector connector_;
};

} // namespace async

} // namespace segysdk::bulkaccess
