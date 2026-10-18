/*
Copyright 2024 Schlumberger

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
#include <string>
#include <vector>

namespace segysdk
{

namespace utilities
{

using Strings = std::vector<std::string>;
using ByteBuffer = std::vector<std::uint8_t>;
using StringsPair = std::pair<std::string, std::string>;
using StringPairs = std::vector<StringsPair>;
using Samples = std::vector<float>;

template<class Destination, class Source>
typename Source::size_type
appendContainer(Destination& dest, const Source& src)
{
    dest.insert(std::end(dest), std::cbegin(src), std::cend(src));
    return src.size();
}

template<class Destination, class SourceIter>
void
appendContainer(Destination& dest, const SourceIter begin, const SourceIter end)
{
    dest.insert(std::end(dest), begin, end);
}

template<class Buffer>
std::string
stringFromBuffer(const Buffer& buffer)
{
    return {std::cbegin(buffer), std::cend(buffer)};
}

} // namespace utilities

using utilities::ByteBuffer;
using utilities::Samples;
using utilities::Strings;
using utilities::StringsPair;

using utilities::appendContainer;
using utilities::stringFromBuffer;

} // namespace segysdk
