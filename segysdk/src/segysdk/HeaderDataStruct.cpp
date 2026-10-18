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

#include "HeaderDataStruct.h"

namespace segysdk::api
{

HeaderDataStruct::HeaderDataStruct(std::string id, const size_t start, const size_t length, const double hdrValue)
  : id(std::move(id)), offset(start + 1), end(start + length), value(hdrValue) {}

} // namespace segysdk::api
