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

#include "DummyDLL.h"

namespace segysdk::utilities::tests
{

NumberInDll::NumberInDll()
  : number_(0) {}

int
NumberInDll::getNumber() const
{
    return number_;
}

NumberInDll&
NumberInDll::getInstance()
{
    static NumberInDll object{};
    return object;
}

void
NumberInDll::setNumber(int number)
{
    number_ = number;
}

} // namespace segysdk::utilities::tests

int
getNumber()
{
    return segysdk::utilities::tests::NumberInDll::getInstance().getNumber();
}

void
setNumber(int number)
{
    segysdk::utilities::tests::NumberInDll::getInstance().setNumber(number);
}
