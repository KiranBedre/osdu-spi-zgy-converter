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

#include "../segysdk_details/DLL_EXPORT.h"

#include <cstddef>
#include <functional>
#include <mutex>
#include <thread>
#include <vector>

namespace segysdk::concurrent
{

struct SEGYSDK_DLL_PUBLIC ThreadPool final
{
    explicit ThreadPool(size_t threadsCount);

    ~ThreadPool();

    // sets the thread for the specified thread cell (if only the cell is not running at the moment) in the thread pool
    bool setWorker(size_t idx, std::thread&& worker);

    // sets the thread for the first not running thread cell in the thread pool
    bool addWorker(std::thread&& worker);

    // joins all the running threads in the pool
    void joinAll();

private:
    using ThreadArray = std::vector<std::thread>;

private:
    size_t const threadsCount_;
    std::mutex poolMutex_;
    ThreadArray threads_;
};

} // namespace segysdk::concurrent
