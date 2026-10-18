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

#include "../SegyIndex/Utilities/TraceRange.h"

#include <cstdint>
#include <deque>
#include <initializer_list>

namespace segysdk::segyindex
{
namespace trace_range_map
{

/**
 * @brief struct TraceIndexStorage Implements the trace range container to keep the ranges that the index map refers to
 **/
using TraceIndexStorageBase = std::deque<TraceRange>;

struct SEGYSDK_DLL_PUBLIC TraceIndexStorage final : protected TraceIndexStorageBase
{
    using difference_type = TraceIndexStorageBase::difference_type;
    using const_iterator = TraceIndexStorageBase::const_iterator;

    TraceIndexStorage();
    explicit TraceIndexStorage(std::initializer_list<TraceRange>&& other);

    /**
     * @brief Returns the number of ranges in the storage
     **/
    size_t size() const;

    /**
     * @brief Returns the constant iterator to the beginning of the storage contents
     **/
    const_iterator cbegin() const;

    /**
     * @brief Returns the constant iterator beyond the end of the storage contents
     **/
    const_iterator cend() const;

    /**
     * @brief Returns the constant iterator to the beginning of the storage contents
     **/
    const_iterator begin() const;

    /**
     * @brief Returns the constant iterator beyond the end of the storage contents
     **/
    const_iterator end() const;

    /**
     * @brief Implements the random access to the storage contents
     *
     * @param i - the integer index of the range item to access to
     *
     * @return The reference to the requested range item if the requested index falls into the storage contents or a reference to an empty range otherwise
     **/
    TraceRange const& operator[](size_t i) const;

    /**
     * @brief Removes all the storage contents
     **/
    void clearStorage();

    /**
     * @brief Adds a single TraceRange item to the storage
     *
     * @param range - the range to _move_ to the storage
     **/
    void addRange(TraceRange&& range);

private:
    TraceIndexStorage(TraceIndexStorage const&) = delete;
    TraceIndexStorage& operator=(TraceIndexStorage const&) = delete;
    TraceIndexStorage(TraceIndexStorage&& other) = delete;
    TraceIndexStorage& operator=(TraceIndexStorage&&) = delete;
};

} // namespace trace_range_map
} // namespace segysdk::segyindex
