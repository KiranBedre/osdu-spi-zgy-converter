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

#include "WktCoordinateSystem.h"

#include <stdexcept>
#include <utility>

static constexpr char PROJCS[]{"PROJCS"};
static constexpr char GEOGCS[]{"GEOGCS"};
static constexpr char GEOCCS[]{"GEOCCS"};
static constexpr char PARAMETER[]{"PARAMETER"};
static constexpr char PROJECTION[]{"PROJECTION"};
static constexpr char UNIT[]{"UNIT"};
static constexpr char DATUM[]{"DATUM"};
static constexpr char PRIMEM[]{"PRIMEM"};

class SimpleWktParser
{
public:
    explicit SimpleWktParser(std::string wktString)
      : input_(std::move(wktString))
    {
    }

    static bool isWordChar(const char c) { return std::isalpha(c); }
    static bool isDigit(const char c) { return std::isdigit(c); }

    std::string peekWord()
    {
        const auto start = pos_;
        const auto end = skipWhile(isWordChar);
        return input_.substr(start, end - start);
    }

    void skipWord(const std::string& expected = "")
    {
        if (!expected.empty() && peekWord() != expected)
            fail("Expected '" + expected + "'");
        pos_ = skipWhile(isWordChar);
    }

    void skipComma()
    {
        if (!is(','))
            fail("Expected comma");
        ++pos_;
    }

    bool is(char expected)
    {
        return pos_ < input_.size() && input_[pos_] == expected;
    }

    bool isLeftDelimiter() { return is('['); }
    bool isRightDelimiter() { return is(']'); }

    size_t skipWhile(bool (*f)(char))
    {
        auto pos = pos_;
        while (pos < input_.size() && f(input_[pos])) {
            ++pos;
        }
        return pos;
    }

    static bool isStringChar(const char c)
    {
        return c != '\"';
    }

    std::string readString()
    {
        if (!is('\"'))
            fail("Expected double quote");
        ++pos_;
        const auto start = pos_;
        pos_ = skipWhile(isStringChar);
        const auto end = pos_;
        if (!is('\"'))
            fail("Expected double quote");
        ++pos_;
        return input_.substr(start, end - start);
    }

    void skipString()
    {
        readString();
    }

    double readDouble()
    {
        const auto start = pos_;
        if (is('-'))
            ++pos_;
        pos_ = skipWhile(isDigit);
        if (is('.')) {
            ++pos_;
            pos_ = skipWhile(isDigit);
        }
        if (is('E')) {
            ++pos_;
            pos_ = skipWhile(isDigit);
        }
        const auto end = pos_;
        const auto s = input_.substr(start, end - start);
        return std::stod(s);
    }

    void skipBlock(const std::string& word = "")
    {
        if (peekWord() != word)
            fail("Expected '" + word + "' block");
        skipWord();
        open();
        bool terminated = 0;
        while (!terminated) {
            if (pos_ == input_.size())
                fail("Unexpected end of WKT input");
            const auto c = input_[pos_];
            switch (c) {
            case '\"':
                skipString();
                break;
            case '[':
                skipBlock();
                break;
            case ']':
                terminated = true;
                close();
                break;
            default:
                ++pos_;
                break;
            }
        }

        if (is(','))
            skipComma();
    }

    void open()
    {
        if (!isLeftDelimiter())
            fail("Expected left delimiter");
        ++pos_;
    }

    void close()
    {
        if (!isRightDelimiter())
            fail("Expected right delimiter");
        ++pos_;
    }

    ConversionUnit getUnit()
    {
        skipWord(UNIT);
        open();
        const auto unitName = readString();
        skipComma();
        const auto unitScalar = readDouble();
        close();
        return ConversionUnit{unitName, unitScalar};
    }

private:
    const std::string input_;
    size_t pos_{0};

    void fail(const std::string& message)
    {
        throw std::invalid_argument("Invalid WKT input. Position: " + std::to_string(pos_) + " " + message);
    }
};

/*
* from "OpenGIS Implementation Specification for Geographic information - Simple feature access - Part 1: Common architecture"
*
* <spatial reference system> ::= <projected cs> | <geographic cs> | <geocentric cs>
*
* <projected cs> ::= PROJCS <left delimiter> <csname>
*                     <comma> <geographic cs>
*                     <comma> <projection>
*                     (<comma> <parameter> )*
*                     <comma> <linear unit>
*                     <right delimiter>
*
* <geographic cs> ::= GEOGCS <left delimiter> <csname>
*                     <comma> <datum>
*                     <comma> <prime meridian>
*                     <comma> <angular unit>
*                     (<comma> <linear unit> )
*                     <right delimiter>
*
* <geocentric cs> ::= GEOCCS <left delimiter> <name>
*                     <comma> <datum>
*                     <comma> <prime meridian>
*                     <comma> <linear unit>
*                     <right delimiter>
*/

bool
isValidCoordinateSystem(const std::string& word)
{
    return word == PROJCS || word == GEOGCS || word == GEOCCS;
}

WktCoordinateSystem::WktCoordinateSystem(const std::string& wktString)
{
    SimpleWktParser parser(wktString);
    const auto word = parser.peekWord();
    if (!isValidCoordinateSystem(word))
        throw std::invalid_argument("WKT input is not a coordinate system.");
    parser.skipWord();
    parser.open();
    parser.skipString(); // name (for all)
    parser.skipComma();
    if (word == PROJCS) {
        parser.skipBlock(GEOGCS);
        parser.skipBlock(PROJECTION);
        while (parser.peekWord() == PARAMETER) {
            parser.skipBlock(PARAMETER);
        }
        conversionUnit_ = parser.getUnit();
    }
    else if (word == GEOGCS) {
        parser.skipBlock(DATUM);
        parser.skipBlock(PRIMEM);
        parser.skipBlock(UNIT);          // angular unit
        if (parser.peekWord() == UNIT) { // linear unit is optional
            conversionUnit_ = parser.getUnit();
        }
        else {
            throw std::invalid_argument("Linear unit is missing.");
        }
    }
    else if (word == GEOCCS) {
        parser.skipBlock(DATUM);
        parser.skipBlock(PRIMEM);
        conversionUnit_ = parser.getUnit();
    }
    // ignore anything else after we found the unit
}

ConversionUnit
WktCoordinateSystem::getConversionUnit()
{
    return conversionUnit_;
}
