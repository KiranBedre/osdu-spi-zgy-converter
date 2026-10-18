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

#include "ApiTestResult.h"
#include "ApiTestParams.h"

#include <json/json.h>

namespace segysdk::api::tests
{
class ApiTestConfig : public ApiTestResult, public ApiTestParams
{
public:
    ApiTestConfig(const Json::Value& data)
        : ApiTestResult(data),
        ApiTestParams(data)
    {
    }
};
} // namespace segysdk::api::tests