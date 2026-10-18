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

#include <cstddef>
#include <string>

namespace segysdk::segyindex
{
namespace index_config
{

inline static const std::string indexMetaFileExtension_ = ".idx";
inline static const std::string indexFileExtension_ = ".idx.bin";
inline static const std::string indexMissingIndexExtension_ = ".missing.idx.bin";

inline static constexpr int const IndexMajorVersion_2024_11_ = 2024;
inline static constexpr int const IndexMinorVersion_2024_11_ = 11;

inline static constexpr int const IndexMajorVersion_2024_01_ = 2024;
inline static constexpr int const IndexMinorVersion_2024_01_ = 01;

inline static constexpr int const IndexMajorVersion_2022_06_ = 2022;
inline static constexpr int const IndexMinorVersion_2022_06_ = 06;

inline static constexpr int const IndexMajorVersion_2022_01_ = 2022;
inline static constexpr int const IndexMinorVersion_2022_01_ = 01;

inline static constexpr int const IndexMajorVersion_2021_01_ = 2021;
inline static constexpr int const IndexMinorVersion_2021_01_ = 01;

inline static constexpr int const IndexMajorVersion_2020_01_ = 2020;
inline static constexpr int const IndexMinorVersion_2020_01_ = 01;

inline static constexpr int const IndexMajorVersionCurrent_ = IndexMajorVersion_2024_11_;
inline static constexpr int const IndexMinorVersionCurrent_ = IndexMinorVersion_2024_11_;

inline static constexpr size_t const numberOfTraceScannerPrefetchReaders_ = 3U;

inline static constexpr size_t const binaryIndexReadersCount_ = 8U;

} // namespace index_config
} // namespace segysdk::segyindex
