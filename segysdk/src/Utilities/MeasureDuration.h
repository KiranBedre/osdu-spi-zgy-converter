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

#include <chrono>
#include <cstdint>

namespace segysdk::utilities
{

/**
 * @brief struct MeasureDuration Implements the RII pattern to measure the duration from the instance creation till the destruction
 **/
template<typename DURATION_TYPE>
struct SEGYSDK_DLL_PUBLIC MeasureDuration
{
    /**
     * @brief Constructs the MeasureDuration instance
     *
     * @param durationAccumulator - the duration accumulator instance that must have the += operation defined and have the scope wider than the current MeasureDuration instance.
     **/
    MeasureDuration(DURATION_TYPE& durationAccumulator)
      : durationAccumulator_{durationAccumulator}
    {
        start_ = std::chrono::steady_clock::now();
    }

    /**
     * @brief Destructs the MeasureDuration instance and increments the duration accumulator instance's value by the elapsed duration
     **/
    ~MeasureDuration()
    {
        durationAccumulator_ += getElapsedDuration();
    }

    /**
     * @brief Returns the elapsed duration starting from the current instance creation
     **/
    DURATION_TYPE getElapsedDuration()
    {
        end_ = std::chrono::steady_clock::now();
        duration_ = end_ - start_;
        return duration_.count();
    }

private:
    MeasureDuration(MeasureDuration const&) = delete;
    MeasureDuration& operator=(MeasureDuration const&) = delete;
    MeasureDuration(MeasureDuration&&) = delete;
    MeasureDuration& operator=(MeasureDuration&&) = delete;

private:
    DURATION_TYPE& durationAccumulator_;
    std::chrono::steady_clock::time_point start_;
    std::chrono::steady_clock::time_point end_;
    std::chrono::duration<DURATION_TYPE> duration_;
};

} // namespace segysdk::utilities
