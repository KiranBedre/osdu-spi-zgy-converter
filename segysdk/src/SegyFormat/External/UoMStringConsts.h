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

#include "../segysdk_details/DLL_EXPORT.h"

#include <string>

namespace segysdk::segyformat::external
{

class SEGYSDK_DLL_PUBLIC UoMStringConsts
{
public:
    inline static const std::string ms = "ms";
    inline static const std::string us = "us";
    inline static const std::string m = "m";
    inline static const std::string ft = "ft";
    inline static const std::string ftUS = "ftUS";
    inline static const std::string mps = "m/s";
    inline static const std::string ftps = "ft/s";

    inline static const std::string dB = "dB";
    inline static const std::string Hz = "Hz";
    inline static const std::string dBpOctave = "db/Octave";

    inline static const std::string Volts = "Volt";

    inline static const std::string Unitless = "Unitless";
};

} // namespace segysdk::segyformat::external
