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

#include "../segysdk_details/DLL_EXPORT.h"

namespace segysdk::segyio
{

class SEGYSDK_DLL_PUBLIC AsyncActivity
{
public:
    AsyncActivity();
    virtual ~AsyncActivity();

    virtual float getProgress() = 0;

    virtual bool isCanceled() = 0;
    virtual bool isCompleted() = 0;
    virtual bool isFaulted() = 0;
    virtual bool isSucceeded() = 0;

    virtual void cancel() = 0;

    virtual void wait() = 0;
    virtual bool wait(unsigned long millisecondsTimeout) = 0;
};

} // namespace segysdk::segyio
