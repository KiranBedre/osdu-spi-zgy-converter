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

#include "../Utilities/osdu/OsduSeismicTraceData140.h"

#include "../segysdk_details/DLL_EXPORT.h"

#include <json/json.h>

namespace segysdk::utilities::osdu
{

/*
    Class to return the latest known OsduSeismicTraceData object.
    Will elevate previous version to the most current one in the getOsduSeismicTraceData method
*/
class SEGYSDK_DLL_PUBLIC OsduSeismicTraceDataFactory
{
public:
    static OsduSeismicTraceData140 getOsduSeismicTraceData(const Json::Value& json);
};

} // namespace segysdk::utilities::osdu
