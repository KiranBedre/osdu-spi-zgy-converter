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

#include <cmath>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <typeinfo>

namespace segysdk::utilities
{

class SEGYSDK_DLL_PUBLIC Ensure
{
public:
    static void isNotNull(void* pointer)
    {
        if (pointer == nullptr) {
            const std::string msg = "Unexpected null pointer.";
            std::cout << msg << std::endl;
            throw std::runtime_error(msg);
        }
    }

    static void isGreater(int a, int b)
    {
        if (a <= b) {
            const std::string msg = "isGreater: Unexpected low value. ";
            throw std::runtime_error(msg);
        }
    }

    // must fix this ...
    template<typename T>
    static void areEqual(T a, T b)
    {
        if (a != b) {
            std::string myType = typeid(a).name();

            const std::string msg = "areEqual: Values are NOT equal.";
            throw std::runtime_error(msg);
        }
    }

    static void areAlmostEqual(double a, double b, uint16_t precision)
    {

        if (precision > 6) {
            const std::string msg = "Precision can only be up to 6 (decimal values)";
            throw std::runtime_error(msg);
        }

        const double scalar = static_cast<int>(pow(10, precision));
        const double tolerance = 1 / scalar * 10; // Allow rounding differences in last decimal

        const long compA = static_cast<long>(a * scalar);
        const long compB = static_cast<long>(b * scalar);
        const double result = std::abs(compA - compB) / scalar;
        if (std::abs(result) > tolerance) {
            const std::string msg = "Unexpected value. Values are NOT almost equal to the given precision.";
            throw std::runtime_error(msg);
        }
    }

    static void isTrue(bool expression)
    {
        if (!expression) {
            const std::string msg = "isTrue: Unexpected value - condition is not True. ";
            throw std::runtime_error(msg);
        }
    }
};

} // namespace segysdk::utilities
