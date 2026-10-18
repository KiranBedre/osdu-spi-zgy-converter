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

#include "HeaderOverride.h"

#include <utility>

namespace segysdk::segyio
{

HeaderOverride::HeaderOverride(std::string headerId, float value)
  : headerId_(std::move(headerId)), value_(value)
{
}

HeaderOverride::~HeaderOverride()
{
}

float
HeaderOverride::getValue() const
{
    return this->value_;
}

void
HeaderOverride::setValue(float value)
{
    this->value_ = value;
}

std::string
HeaderOverride::getHeaderId() const
{
    return this->headerId_;
}
} // namespace segysdk::segyio
