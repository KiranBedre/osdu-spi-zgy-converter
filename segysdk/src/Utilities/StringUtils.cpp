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

#include "StringUtils.h"

#include <algorithm>
#include <cctype>
#include <codecvt>
#include <iostream>
#include <locale>

namespace segysdk::utilities
{

std::string
StringUtils::ToUppercase(std::string msg)
{

    for (auto& c : msg)
        c = static_cast<char>(toupper(c));

    return msg;
}

std::string
StringUtils::ToLowercase(std::string msg)
{

    for (auto& c : msg)
        c = static_cast<char>(tolower(c));

    return msg;
}

void
StringUtils::ReplaceAll(std::string& sourceStr, const char toSearch, const char replace)
{
    for (auto& c : sourceStr) {
        if (c == toSearch) {
            c = static_cast<char>(replace);
        }
    }
}

void
StringUtils::ReplaceAll(std::string& sourceStr, const std::string& toSearch, const std::string& replace)
{
    size_t pos = sourceStr.find(toSearch);
    while (pos != std::string::npos) {
        // Replace this occurrence of toSearch
        sourceStr.replace(pos, toSearch.size(), replace);
        // Get the next occurrence from the current position
        pos = sourceStr.find(toSearch, pos + replace.size());
    }
}

bool
StringUtils::keyMatches(const std::string& key, const std::string& value)
{
    const size_t match = key.find(value);

    return match < key.size();
}

bool
StringUtils::keyBeginsWith(const std::string& key, const std::string& value)
{
    return key.rfind(value, 0) == 0;
}

bool
StringUtils::keyEndsWith(const std::string& key, const std::string& value)
{
    const size_t match = key.find(value);
    return match + value.length() == key.size();
}

// trim from start (in place)
void
StringUtils::LTrim(std::string& s)
{
    s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
                return !std::isspace(ch);
            }));
}

// trim from end (in place)
void
StringUtils::RTrim(std::string& s)
{
    s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
                return !std::isspace(ch);
            }).base(),
            s.end());
}

// trim from both ends (in place)
void
StringUtils::Trim(std::string& s)
{
    LTrim(s);
    RTrim(s);
}

// trim from start (copying)
std::string
StringUtils::LTrim_copy(std::string s)
{
    LTrim(s);
    return s;
}

// trim from end (copying)
std::string
StringUtils::RTrim_copy(std::string s)
{
    RTrim(s);
    return s;
}

std::vector<std::string>
StringUtils::split(std::string text, const std::string& delimiter, bool keepDelimiter)
{
    std::vector<std::string> result;
    while (true) {
        auto pos = text.find(delimiter);
        if (pos == std::string::npos) {
            result.emplace_back(text);
            return result;
        }
        if (keepDelimiter) {
            pos += delimiter.size();
        }
        result.emplace_back(text.substr(0, pos));
        if (!keepDelimiter) {
            pos += delimiter.size();
        }
        text = text.substr(pos);
    }
    return result;
}

// trim from both ends (copying)
std::string
StringUtils::Trim_copy(std::string s)
{
    Trim(s);
    return s;
}

} // namespace segysdk::utilities
