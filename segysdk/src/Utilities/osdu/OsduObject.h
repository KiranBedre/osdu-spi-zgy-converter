/*
Copyright 2021-2022 Schlumberger

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

#include "OsduKind.h"

#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>

#include <string>

namespace segysdk::utilities::osdu
{

class SEGYSDK_DLL_PUBLIC OsduObject
{
public:
    explicit OsduObject(Json::Value root);
    virtual ~OsduObject();

    virtual std::string id() const final;
    virtual std::string version() const final;
    virtual std::string legalTag() const final;
    virtual std::string ns() const final;
    virtual OsduKind kind() const final;
    virtual std::string content() const final;

protected:
    Json::Value root_;
};

} // namespace segysdk::utilities::osdu
