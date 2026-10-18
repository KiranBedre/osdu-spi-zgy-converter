/*
Copyright 2024-2025 Schlumberger

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

#include <cstdint>

namespace segysdk::navigation_extractor
{

constexpr auto SUCCESS = std::int64_t{0};
constexpr auto UNRECOVERABLE_ERROR = std::int64_t{-1};
constexpr auto NOT_EXTRACTED = std::int64_t{-2};
constexpr auto NOT_STORED_SEISMIC_TRACE_NAVIGATION = std::int64_t{-3};
constexpr auto NOT_STORED_FILE_SEISMIC_LINE_GEOMETRY = std::int64_t{-4};
constexpr auto NOT_STORED_SEISMIC_TRACE_DATA = std::int64_t{-5};
constexpr auto NOT_STORED_SEISMIC_2D_INTERPRETATION_SET = std::int64_t{-6};

enum class StatusCodes : int64_t {
    // note: first two error codes are reserved for predefined meaning
    Success = SUCCESS,
    UnrecoverableError = -UNRECOVERABLE_ERROR,

    // starting from this value there are neither reserved values nor meanings...
    NavigationNotExtracted,
    SeismicTraceGeometryNotSaved,
    FileSeismicLineGeometryNotSaved,
    SeismicTraceDataNotSaved,
    Seismic2DInterpretationSetNotSaved,
    ErrorMessageCodes_Count, // this is a special value to estimate number of error codes in our set
};

} // namespace segysdk::navigation_extractor
