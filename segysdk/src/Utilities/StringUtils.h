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

#include <locale>
#include <string>
#include <vector>

namespace segysdk::utilities
{

class SEGYSDK_DLL_PUBLIC StringUtils
{

public:
    static std::string ToUppercase(std::string msg);
    static std::string ToLowercase(std::string msg);

    static void ReplaceAll(std::string& sourceStr, const std::string& toSearch, const std::string& replace);
    static void ReplaceAll(std::string& sourceStr, const char toSearch, const char replace);

    static bool keyMatches(const std::string& key, const std::string& value);
    static bool keyBeginsWith(const std::string& key, const std::string& value);
    static bool keyEndsWith(const std::string& key, const std::string& value);

    static void Trim(std::string& s);
    static void LTrim(std::string& s);
    static void RTrim(std::string& s);

    static std::string Trim_copy(std::string s);
    static std::string LTrim_copy(std::string s);
    static std::string RTrim_copy(std::string s);

    static std::vector<std::string> split(std::string text, const std::string& delimiter, bool keepDelimiter = false);
};

} // namespace segysdk::utilities
