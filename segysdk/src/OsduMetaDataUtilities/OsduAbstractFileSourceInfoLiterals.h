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

class SEGYSDK_DLL_PUBLIC OsduAbstractFileSourceInfoLiterals
{
public:
    // osdu:DatasetProperties
    inline static constexpr const char* const FileSource{"FileSource"};
    inline static constexpr const char* const PreloadFilePath{"PreloadFilePath"};
    inline static constexpr const char* const PreloadFileCreateUser{"PreloadFileCreateUser"};
    inline static constexpr const char* const PreloadFileCreateDate{"PreloadFileCreateDate"};
    inline static constexpr const char* const PreloadFileModifyUser{"PreloadFileModifyUser"};
    inline static constexpr const char* const PreloadFileModifyDate{"PreloadFileModifyDate"};
    inline static constexpr const char* const Name{"Name"};
    inline static constexpr const char* const FileSize{"FileSize"};
    inline static constexpr const char* const EncodingFormatTypeID{"EncodingFormatTypeID"};
    inline static constexpr const char* const Checksum{"Checksum"};
    inline static constexpr const char* const ChecksumAlgorithm{"ChecksumAlgorithm"};

    // These value types are intended for unit test purposes, do the actual reference lookup to get the proper string values
    inline static constexpr const char* const EncodingFormatTypeReferenceType{"reference-data--EncodingFormatType"};
    inline static constexpr const char* const EncodingFormatTypeGeoJsonValue{"application%2Fgeo%2Bjson"};
    inline static constexpr const char* const EncodingFormatTypeJsonValue{"text%2Fjson"};

    inline static constexpr const char* const SHA256Value{"SHA-256"};
    inline static constexpr const char* const MD5Value{"MD5"};

};

} // namespace segysdk::metadata::osdu
