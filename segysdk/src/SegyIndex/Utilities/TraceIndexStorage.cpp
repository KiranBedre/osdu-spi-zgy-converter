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

#include "TraceIndexStorage.h"

namespace segysdk::segyindex
{
namespace trace_range_map
{

TraceIndexStorage::TraceIndexStorage()
  : TraceIndexStorageBase()
{
    /* EMPTY */
}

TraceIndexStorage::TraceIndexStorage(std::initializer_list<TraceRange>&& other)
  : TraceIndexStorageBase(std::move(other))
{
    /* EMPTY */
}

size_t
TraceIndexStorage::size() const
{
    return TraceIndexStorageBase::size();
}

TraceIndexStorage::const_iterator
TraceIndexStorage::cbegin() const
{
    return TraceIndexStorageBase::cbegin();
}

TraceIndexStorage::const_iterator
TraceIndexStorage::cend() const
{
    return TraceIndexStorageBase::cend();
}

TraceIndexStorage::const_iterator
TraceIndexStorage::begin() const
{
    // note: allow readonly operations solely
    return TraceIndexStorageBase::cbegin();
}

TraceIndexStorage::const_iterator
TraceIndexStorage::end() const
{
    // note: allow readonly operations solely
    return TraceIndexStorageBase::cend();
}

TraceRange const&
TraceIndexStorage::operator[](size_t i) const
{
    static TraceRange const r{};

    if (i < TraceIndexStorageBase::size()) {
        return TraceIndexStorageBase::operator[](i);
    }
    else {
        return r;
    }
}

void
TraceIndexStorage::clearStorage()
{
    TraceIndexStorageBase::clear();
}

void
TraceIndexStorage::addRange(TraceRange&& range)
{
    TraceIndexStorageBase::emplace_back(std::move(range));
}

} // namespace trace_range_map
} // namespace segysdk::segyindex
