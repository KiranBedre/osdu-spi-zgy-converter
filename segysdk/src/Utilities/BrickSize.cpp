/*
Copyright 2021-2022 Schlumberger

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

#include "BrickSize.h"
#include "Environment.h"

#include <algorithm>
#include <stdexcept>
#include <vector>

namespace segysdk::utilities
{
static bool
is_digit(const unsigned char& c)
{
    return std::isdigit(c);
}
static bool
is_not_digit(const unsigned char& c)
{
    return !std::isdigit(c);
}

static bool
looks_like_a_number(const std::string& str)
{
    return !str.empty() && str.size() < 8 && std::all_of(str.begin(), str.end(), is_digit);
}

BrickSize::BrickSize()
  : BrickSize(DefaultI, DefaultJ, DefaultK)
{
}

BrickSize::BrickSize(const std::string& string)
{
    if (string.empty()) {
        sizeI_ = sizeJ_ = sizeK_ = 64;
    }
    else if (looks_like_a_number(string)) {
        sizeI_ = sizeJ_ = sizeK_ = std::stoul(string);
    }
    else {
        // regex would be simpler
        const auto message = "Invalid format of brick size. Valid formats: '<size>' or '<sizeI>x<sizeJ>x<sizeK>'";
        std::vector<std::string> parts;
        const auto firstNonDigit = std::find_if(string.begin(), string.end(), is_not_digit);
        const auto secondNonDigit = std::find_if(firstNonDigit + 1, string.end(), is_not_digit);
        if (secondNonDigit == string.end()) {
            throw std::invalid_argument(message);
        }
        const std::string sizeIStr(string.begin(), firstNonDigit);
        const std::string sizeJStr(firstNonDigit + 1, secondNonDigit);
        const std::string sizeKStr(secondNonDigit + 1, string.end());

        if (!looks_like_a_number(sizeIStr) || !looks_like_a_number(sizeJStr) || !looks_like_a_number(sizeKStr)
            || (*firstNonDigit != 'x' && *firstNonDigit != 'X')
            || (*secondNonDigit != 'x' && *secondNonDigit != 'X')) {
            throw std::invalid_argument(message);
        }

        sizeI_ = std::stoul(sizeIStr);
        sizeJ_ = std::stoul(sizeJStr);
        sizeK_ = std::stoul(sizeKStr);
    }
}

BrickSize::BrickSize(size_t i, size_t j, size_t k)
  : sizeI_(i), sizeJ_(j), sizeK_(k)
{
}

size_t
BrickSize::sizeI() const
{
    return sizeI_;
}

size_t
BrickSize::sizeJ() const
{
    return sizeJ_;
}

size_t
BrickSize::sizeK() const
{
    return sizeK_;
}

BrickSize
BrickSize::GetBrickSize()
{
    const auto customSize = Environment::instance().BrickSize();
    return customSize.empty() ? BrickSize() : BrickSize(customSize);
}
} // namespace segysdk::utilities
