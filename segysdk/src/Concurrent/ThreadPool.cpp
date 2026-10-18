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

#include "../Concurrent/ThreadPool.h"

namespace segysdk::concurrent
{

ThreadPool::ThreadPool(size_t threadsCount)
  : threadsCount_{threadsCount},
    threads_(threadsCount_)
{
    /* EMPTY */
}

ThreadPool::~ThreadPool()
{
    joinAll();
}

bool
ThreadPool::setWorker(size_t idx, std::thread&& worker)
{
    bool result = false;
    std::unique_lock<std::mutex> lock(poolMutex_);

    do {
        if (idx >= threads_.size()) {
            break;
        }

        if (threads_[idx].joinable()) {
            break;
        }

        threads_[idx].swap(worker);

        result = true;

    } while (false);
    return result;
}

bool
ThreadPool::addWorker(std::thread&& worker)
{
    bool result = false;
    std::unique_lock<std::mutex> lock(poolMutex_);

    for (auto& th : threads_) {
        if (th.joinable()) {
            continue;
        }
        th.swap(worker);
        result = true;
        break;
    }

    return result;
}

void
ThreadPool::joinAll()
{
    for (auto& th : threads_) {
        if (th.joinable()) {
            th.join();
        }
    }
}

} // namespace segysdk::concurrent
