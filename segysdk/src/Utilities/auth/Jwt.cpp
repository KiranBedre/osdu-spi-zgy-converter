/*
Copyright 2022 Schlumberger

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

#include "Jwt.h"
#include "../Utilities/JsonUtilities.h"

#include <json/json.h>

#include <stdexcept>

namespace segysdk::utilities::auth
{

// base64_url_decode from https://stackoverflow.com/a/57314480
static constexpr char base64_url_alphabet[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '-', '_'};

static std::string
base64_url_decode(const std::string& in)
{
    std::string out;
    std::vector T(256, -1);
    unsigned int i;
    for (i = 0; i < 64; i++) {
        T[base64_url_alphabet[i]] = i;
    }

    int val = 0, valb = -8;
    for (i = 0; i < in.length(); i++) {
        const unsigned char c = in[i];
        if (T[c] == -1) {
            break;
        }
        val = (val << 6) + T[c];
        valb += 6;
        if (valb >= 0) {
            out.push_back(static_cast<char>((val >> valb) & 0xFF));
            valb -= 8;
        }
    }
    return out;
}

static long
parseJwtExpiration(const std::string& token)
{
    const auto jsonBegin = token.find('.');
    if (jsonBegin == std::string::npos) {
        throw std::invalid_argument("Invalid JWT");
    }

    const auto jsonEnd = token.find('.', jsonBegin + 1);
    if (jsonEnd == std::string::npos) {
        throw std::invalid_argument("Invalid JWT");
    }

    const auto payloadBase64 = token.substr(jsonBegin + 1, jsonEnd - jsonBegin - 1);
    const auto payloadDecoded = base64_url_decode(payloadBase64);

    const auto json = JsonUtilities::getJsonFromString(payloadDecoded);

    const auto exp = json["exp"];
    if (!exp) {
        throw std::invalid_argument("Invalid JWT: Missing expiration claim.");
    }

    return exp.asInt64();
}

Jwt::Jwt(const std::string& token)
  : expiration_((parseJwtExpiration(token)))
{
}

std::chrono::time_point<std::chrono::system_clock>
Jwt::expiration() const
{
    return std::chrono::time_point<std::chrono::system_clock>{std::chrono::seconds(expiration_)};
}

} // namespace segysdk::utilities::auth
