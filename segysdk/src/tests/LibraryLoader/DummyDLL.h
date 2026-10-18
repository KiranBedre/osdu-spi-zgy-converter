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

#pragma once

#include "../../segysdk_details/DLL_EXPORT.h"

namespace segysdk::utilities::tests
{

class NumberInDll
{
public:
    NumberInDll();

    int getNumber() const;

    static NumberInDll& getInstance();

    void setNumber(int number);

private:
    int number_;
};

} // namespace segysdk::utilities::tests

extern "C" {
SEGYSDK_DLL_PUBLIC int getNumber();

SEGYSDK_DLL_PUBLIC void setNumber(int number);
}
