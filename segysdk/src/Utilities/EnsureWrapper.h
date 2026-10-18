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

#include "Ensure.h"

#include "../segysdk_details/DLL_EXPORT.h"

#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>

namespace segysdk::utilities
{

class SEGYSDK_DLL_PUBLIC EnsureWrapper
{
public:
    static void isNotNull(void* pointer, int& errors)
    {
        try {
            Ensure::isNotNull(pointer);
        }
        catch (...) {
            errors++;
        }
    }

    static void isGreater(int a, int b, int& errors, const std::string& dataSetName = "", const std::string& testName = "")
    {
        try {
            Ensure::isGreater(a, b);
        }
        catch (...) {
            errors++;
            const std::string msg = GenerateMessage("isGreater: Unexpected low value. " + std::to_string(a) + " is not greater than " + std::to_string(b), dataSetName, testName);
            std::cerr << msg << std::endl;
        }
    }

    // must fix this ...
    template<typename T>
    static void areEqual(T a, T b, int& errors, const std::string& dataSetName = "", const std::string& testName = "")
    {
        try {
            Ensure::areEqual(a, b);
        }
        catch (...) {
            errors++;
            const std::string msg = GenerateMessage("areEqual: Values are NOT equal. (" + std::to_string(a) + " vs " + std::to_string(b) + ") ", dataSetName, testName);
            std::cerr << msg << std::endl;
        }
    }

    static void areAlmostEqual(double a, double b, uint16_t precision, int& errors, const std::string& dataSetName = "", const std::string& testName = "")
    {
        try {
            Ensure::areAlmostEqual(a, b, precision);
        }
        catch (...) {
            errors++;
            const std::string err = "Unexpected value.  " + std::to_string(a) + " is not almost equal to " + std::to_string(b);
            const std::string msg = GenerateMessage("areAlmostEqual: " + err, dataSetName, testName);
            std::cerr << msg << std::endl;
        }
    }

    static void isTrue(bool expression, int& errors, const std::string& dataSetName = "", const std::string& testName = "")
    {
        try {
            Ensure::isTrue(expression);
        }
        catch (...) {
            errors++;
            const std::string msg = GenerateMessage("isTrue: Unexpected value - condition is not True. ", dataSetName, testName);
            std::cerr << msg << std::endl;
        }
    }

    static std::string
    GenerateMessage(const std::string& text, const std::string& dataSetName = "", const std::string& testName = "")
    {
        std::string msg;
        if (!dataSetName.empty()) {
            const std::string substr = "DataSet: " + dataSetName + "\n";
            msg += substr;
        }
        if (!testName.empty()) {
            const std::string substr = "Test: " + testName + "\n";
            msg += substr;
        }
        return msg + text;
    }
};

} // namespace segysdk::utilities
