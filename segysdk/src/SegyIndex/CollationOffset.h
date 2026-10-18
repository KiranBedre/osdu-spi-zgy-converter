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

#include <cstdint>

namespace segysdk::segyindex
{
struct CollationOffset
{
    int CoordinateScalar = -1;
    int Tofs = -1;
    int SampleRate = -1;
    int SampleCount = -1;
    int TraceWeightingFactor = -1;
    int InlineNo = -1;
    int CrosslineNo = -1;
    int SourceXCoordinate = -1;
    int SourceYCoordinate = -1;
    int TraceNo = -1;
    int CMP = -1;
};

} // namespace segysdk::segyindex
