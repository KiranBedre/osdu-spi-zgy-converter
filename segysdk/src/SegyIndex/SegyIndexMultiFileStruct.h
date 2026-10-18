/*
Copyright 2022 Schlumberger

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

#include "../SegyIndex/Range.h"

#include <cstddef>
#include <cstdint>
#include <map>
#include <string>

namespace segysdk::segyindex
{
namespace multi_file
{

enum class FileNameStatus
{
    Match,
    FileNameMatch,
    NoMatch,
    FileCountDiffers,
    NamesNotUnique
};

struct MultiFileStruct final
{
    uint16_t FileNo{};
    std::string SegyFileName{};
    Range<int> PrimaryKeyRange{};
    size_t FirstTraceOffset{};

    MultiFileStruct()
    {
        /* EMPTY */
    }

    MultiFileStruct(std::string const& segyFileName)
      : SegyFileName{segyFileName}
    {
        /* EMPTY */
    }
};

using MultiFileMap = std::map<int, uint16_t>;

} // namespace multi_file
} // namespace segysdk::segyindex
