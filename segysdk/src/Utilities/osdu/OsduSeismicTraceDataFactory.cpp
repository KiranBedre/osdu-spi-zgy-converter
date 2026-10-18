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

#include "../Utilities/osdu/OsduSeismicTraceDataFactory.h"

#include "OsduObject.h"
#include "WellKnownTypes.h"

namespace segysdk::utilities::osdu
{

OsduSeismicTraceData140
OsduSeismicTraceDataFactory::getOsduSeismicTraceData(const Json::Value& json)
{
    return OsduSeismicTraceData140(json);
}

} // namespace segysdk::utilities::osdu
