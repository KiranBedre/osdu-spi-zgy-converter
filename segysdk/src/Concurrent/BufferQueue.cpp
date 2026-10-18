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

#include "../Concurrent/BufferQueue.h"

namespace segysdk::concurrent
{

namespace details
{

SemaphoreCounter::SemaphoreCounter(uint64_t countStart, size_t decreaseTimeout)
  : _counter(countStart),
    _decreaseTimeout(decreaseTimeout)
{
    /* EMPTY */
}

void
SemaphoreCounter::Increase()
{
    std::unique_lock<std::mutex> lock(_mtx);
    _counter++;
    _cv.notify_one();
}

bool
SemaphoreCounter::DecreaseAndWaitWhenZero()
{
    std::unique_lock<std::mutex> lock(_mtx);
    while (_counter == 0) {
        if (std::cv_status::timeout == _cv.wait_for(lock, _decreaseTimeout)) {
            return false;
        }
    }
    _counter--;
    return true;
}

std::unique_ptr<SemaphoreCounter>
SemaphoreCounter::Create(uint64_t countStart, size_t decreaseTimeout)
{
    return std::unique_ptr<SemaphoreCounter>(
        new SemaphoreCounter(countStart, decreaseTimeout));
}

} // namespace details

} // namespace segysdk::concurrent
