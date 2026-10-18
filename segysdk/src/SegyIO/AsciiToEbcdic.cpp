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

#include "AsciiToEbcdic.h"

#include <cstdint>

namespace segysdk::segyio
{

static constexpr unsigned char const ASCII_TO_EBCDIC[] = {
    0, 1, 2, 3, 38, 45, 46, 47, 22, 5,                // Dec 0 - 9
    37, 11, 12, 13, 14, 15, 16, 17, 18, 59,           // Dec 10 - 19
    60, 61, 50, 38, 24, 25, 63, 39, 28, 29,           // Dec 20 - 29
    30, 31, 64, 90, 127, 123, 91, 108, 80, 125,       // Dec 30 - 39
    77, 93, 92, 78, 107, 96, 75, 97, 240, 241,        // Dec 40 - 49
    242, 243, 244, 245, 246, 247, 248, 249, 122, 94,  // Dec 50 - 59
    76, 126, 110, 111, 124, 193, 194, 195, 196, 197,  // Dec 60 - 69 (... A - E)
    198, 199, 200, 201, 209, 210, 211, 212, 213, 214, // Dec 70 - 79 (F - O)
    215, 216, 217, 226, 227, 228, 229, 230, 231, 232, // Dec 80 - 89 (P - Y)
    233, 77, 109, 93, 109, 109, 185, 129, 130, 131,   // Dec 90 - 99 (Z ... a - c)
    132, 133, 134, 135, 136, 137, 145, 146, 147, 148, // Dec 100 - 109 (c - m )
    149, 150, 151, 152, 153, 162, 163, 164, 165, 166, // Dec 110 - 119 (n - w)
    168, 169, 77, 109, 93, 109, 7, 109, 109, 109,     // Dec 120 - 129 (y - z ...)
    109, 109, 109, 109, 109, 109, 109, 109, 109, 109, // Dec 130 - 139
    109, 109, 109, 109, 109, 109, 109, 109, 109, 109, // Dec 140 - 149
    109, 109, 109, 109, 109, 109, 109, 109, 109, 109, // Dec 150 - 159
    109, 109, 109, 109, 109, 109, 109, 109, 109, 109, // Dec 160 - 169
    109, 109, 109, 109, 109, 109, 109, 109, 109, 109, // Dec 170 - 179
    109, 109, 109, 109, 109, 109, 109, 109, 109, 109, // Dec 180 - 189
    109, 109, 109, 109, 109, 109, 109, 109, 109, 109, // Dec 190 - 199
    109, 109, 109, 109, 109, 109, 109, 109, 109, 109, // Dec 200 - 209
    109, 109, 109, 109, 109, 109, 109, 109, 109, 109, // Dec 210 - 219
    109, 109, 109, 109, 109, 109, 109, 109, 109, 109, // Dec 220 - 229
    109, 109, 109, 109, 109, 109, 109, 109, 109, 109, // Dec 230 - 239
    109, 109, 109, 109, 109, 109, 109, 109, 109, 109, // Dec 240 - 249
    109, 109, 109, 109, 109, 109                      // Dec 250 - 255
};

void
AsciiToEbcdic::bytesToEbcdic(char const* bytes, char* out, size_t count)
{
    auto const * const in = reinterpret_cast<uint8_t const*>(bytes);
    auto * const outBytes = reinterpret_cast<uint8_t*>(out);
    for (size_t i = 0; i < count; ++i) {
        outBytes[i] = ASCII_TO_EBCDIC[in[i]];
    }
}

} // namespace segysdk::segyio
