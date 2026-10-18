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

#include "Coordinate.h"

#include <cmath>

namespace segysdk::segyindex
{

Coordinate::Coordinate()
{
    /* EMPTY */
}

Coordinate::Coordinate(double x, double y)
  : X(x), Y(y)
{
    /* EMPTY */
}

bool
Coordinate::IsValid() const
{
    return std::abs(X) > std::numeric_limits<double>::min() 
           && std::abs(X) < std::numeric_limits<double>::max() 
           && std::abs(Y) > std::numeric_limits<double>::min() 
           && std::abs(Y) < std::numeric_limits<double>::max();
}

} // namespace segysdk::segyindex
