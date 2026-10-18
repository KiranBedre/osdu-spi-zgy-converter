/*
Copyright 2021 Schlumberger

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

#include "../Utilities/osdu/OsduFileCollectionSegy100.h"
#include "../Utilities/osdu/OsduObject.h"
#include "../tools/SegyToZgy/library/OpenZGYFileProperties.h"

class OsduFileCollectionOpenZGY100 : public segysdk::utilities::osdu::OsduObject
{
public:
    explicit OsduFileCollectionOpenZGY100(Json::Value root);
    OsduFileCollectionOpenZGY100(const segysdk::utilities::osdu::OsduFileCollectionSegy100& source, const OpenZGYFileProperties& fileProperties);
    std::string zgyFileName() const;
    std::string zgyFileSize() const;
    std::string totalFileSize() const;
    std::vector<std::string> legalTags() const;
    std::vector<std::string> parentIds() const;
};
