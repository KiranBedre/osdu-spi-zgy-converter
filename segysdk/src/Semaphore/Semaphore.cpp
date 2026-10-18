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

#include "Semaphore.h"

namespace segysdk
{

CriticalSection::CriticalSection(Semaphore& semaphore)
  : semaphore_{semaphore}
{
    semaphore_.acquire();
}

CriticalSection::~CriticalSection()
{
    semaphore_.release();
}

Semaphore::Semaphore(const std::ptrdiff_t leastMaxValue)
  : count_{leastMaxValue}
{
}

void
Semaphore::acquire()
{
    auto lock = std::unique_lock{mutex_};
    sync_.wait(lock, [this] { return count_ > 0; });
    --count_;
}

void
Semaphore::release()
{
    auto lock = std::unique_lock{mutex_};
    ++count_;
    lock.unlock();
    sync_.notify_one();
}

} // namespace segysdk
