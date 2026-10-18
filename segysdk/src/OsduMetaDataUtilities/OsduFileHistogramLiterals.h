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

#include "../segysdk_details/DLL_EXPORT.h"

namespace segysdk::metadata::osdu
{

class SEGYSDK_DLL_PUBLIC OsduFileHistogramLiterals
{
public:
    // osdu:Acl
    inline static constexpr const char* const Id{"id"};
    inline static constexpr const char* const Kind{"kind"};
    inline static constexpr const char* const Version{"version"};
    inline static constexpr const char* const Acl{"acl"};
    inline static constexpr const char* const Legal{"legal"};
    inline static constexpr const char* const Tags{"tags"};
    inline static constexpr const char* const CreateTime{"createTime"};
    inline static constexpr const char* const CreateUser{"createUser"};
    inline static constexpr const char* const ModifyTime{"modifyTime"};
    inline static constexpr const char* const ModifyUser{"modifyUser"};
    inline static constexpr const char* const Ancestry{"ancestry"};
    inline static constexpr const char* const Meta{"meta"};
    inline static constexpr const char* const Data{"data"};
    inline static constexpr const char* const ContentSchemaKind{"ContentSchemaKind"};

};

} // namespace segysdk::metadata::osdu
