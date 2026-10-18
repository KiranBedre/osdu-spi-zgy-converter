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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include <string>

namespace segysdk::utilities
{

class SEGYSDK_DLL_PUBLIC BrickSize
{
public:
    BrickSize();
    explicit BrickSize(const std::string& string);
    BrickSize(size_t i, size_t j, size_t k);

    size_t sizeI() const;
    size_t sizeJ() const;
    size_t sizeK() const;

    static size_t sizeIDefault();
    static size_t sizeJDefault();
    static size_t sizeKDefault();

    static BrickSize GetBrickSize();

private:
    static constexpr size_t DefaultI = 64;
    static constexpr size_t DefaultJ = 64;
    static constexpr size_t DefaultK = 64;

    size_t sizeI_;
    size_t sizeJ_;
    size_t sizeK_;
};

} // namespace segysdk::utilities
