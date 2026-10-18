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

#include "InputParser.h"

#include <algorithm>

namespace segysdk::utilities
{

InputParser::InputParser(int const& argc, char const* const* argv)
  : tokens{argv, argv + argc}
{
    /* EMPTY */
}

const std::string&
InputParser::getCmdOption(const std::string& option) const
{
    if (cmdOptionExists(option)){
        auto found = std::find(this->tokens.begin(), this->tokens.end(), option);
        if ( ++found != this->tokens.end()) {
            return *found;
        }
    }

    static const std::string empty_string("");
    return empty_string;
}

bool
InputParser::cmdOptionExists(const std::string& option) const
{
    return std::find(this->tokens.begin(), this->tokens.end(), option)
           != this->tokens.end();
}

bool
InputParser::hasValidOption(const std::string& option, const std::string& pattern) const
{
    return getCmdOption(option).find(pattern) != std::string::npos;
}

} // namespace segysdk::utilities
