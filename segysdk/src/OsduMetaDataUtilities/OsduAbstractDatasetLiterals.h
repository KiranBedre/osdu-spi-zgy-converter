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
class SEGYSDK_DLL_PUBLIC OsduAbstractDatasetLiterals
{
public:
    // osdu:AbstractCommonResources
    inline static constexpr const char* const Name{"Name"};
    inline static constexpr const char* const Description{"Description"};
    inline static constexpr const char* const TotalSize{"TotalSize"};
    inline static constexpr const char* const EncodingFormatTypeID{"EncodingFormatTypeID"};
    inline static constexpr const char* const SchemaFormatTypeID{"SchemaFormatTypeID"};
    inline static constexpr const char* const Endian{"Endian"};
    inline static constexpr const char* const DatasetProperties{"DatasetProperties"};

    inline static constexpr const char* const BigEndian{"BIG"};
    inline static constexpr const char* const LittleEndian{"LITTLE"};

    // These value types are intended for unit test purposes, do the actual reference lookup to get the proper string values
    inline static constexpr const char* const SchemaFormatTypeReference{"reference-data--SchemaFormatType"};
    inline static constexpr const char* const SchemaFormatTypeReferencValueSEGY{"SEGY"};
    inline static constexpr const char* const SchemaFormatTypeReferencValueOpenZGY{"OpenZGY"};

    inline static constexpr const char* const EncodingFormatTypeReference{"reference-data--EncodingFormatType"};
    inline static constexpr const char* const TextCsvValue{"text%2Fcsv"};
    inline static constexpr const char* const TextJsonValue{"application%2Fjson"};
    inline static constexpr const char* const TextGeoJsonValue{"application%2Fgeo%2Bjson"};
};
} // namespace segysdk::metadata::osdu
