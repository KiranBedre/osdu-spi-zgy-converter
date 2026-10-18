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

#include "TraceIndexMap.h"

namespace segysdk::segyindex
{
namespace trace_range_map
{

//-----------------------------------------------------------------------------
//   TraceIndexMapKey class
//-----------------------------------------------------------------------------

TraceIndexMapKey::TraceIndexMapKey()
  : key_{0U, 0U},
    comparisonFunctionPrimary_{TraceIndexMapKey::operatorLessPrimary},
    comparisonFunctionSecondary_{TraceIndexMapKey::operatorLessSecondary}
{
    /* EMPTY */
}

TraceIndexMapKey::TraceIndexMapKey(PrimaryIndex prii, SecondaryIndex seci)
  : key_{prii, seci},
    comparisonFunctionPrimary_{TraceIndexMapKey::operatorLessPrimary},
    comparisonFunctionSecondary_{TraceIndexMapKey::operatorLessSecondary}
{
    /* EMPTY */
}

TraceIndexMapKey::TraceIndexMapKey(TraceIndexMapKey&& other)
{
    key_ = other.key_;
    comparisonFunctionPrimary_ = other.comparisonFunctionPrimary_;
    comparisonFunctionSecondary_ = other.comparisonFunctionSecondary_;
}

void
TraceIndexMapKey::setOrderMode(TraceIndexMapKey::Order orderPrimary,
                               TraceIndexMapKey::Order orderSecondary)
{
    switch (orderPrimary) {
    case Order::Asc:
        comparisonFunctionPrimary_ = TraceIndexMapKey::operatorLessPrimary;
        break;
    case Order::Desc:
        comparisonFunctionPrimary_ = TraceIndexMapKey::operatorGreaterPrimary;
        break;
    }

    switch (orderSecondary) {
    case Order::Asc:
        comparisonFunctionSecondary_ = TraceIndexMapKey::operatorLessSecondary;
        break;
    case Order::Desc:
        comparisonFunctionSecondary_ = TraceIndexMapKey::operatorGreaterSecondary;
        break;
    }
}

PrimaryIndex
TraceIndexMapKey::getPrimary() const
{
    return key_.first;
}

SecondaryIndex
TraceIndexMapKey::getSecondary() const
{
    return key_.second;
}

bool
TraceIndexMapKey::operator<(TraceIndexMapKey const& other) const noexcept
{
    if (getPrimary() == other.getPrimary()) {
        return comparisonFunctionSecondary_(*this, other);
    }
    else {
        return comparisonFunctionPrimary_(*this, other);
    }
}

bool
TraceIndexMapKey::operator==(TraceIndexMapKey const& other) const noexcept
{
    return (getPrimary() == other.getPrimary()) && (getSecondary() == other.getSecondary());
}

bool
TraceIndexMapKey::operator!=(TraceIndexMapKey const& other) const noexcept
{
    return not(*this == other);
}

bool
TraceIndexMapKey::operatorLessPrimary(TraceIndexMapKey const& This, TraceIndexMapKey const& other)
{
    return This.getPrimary() < other.getPrimary();
}

bool
TraceIndexMapKey::operatorGreaterPrimary(TraceIndexMapKey const& This, TraceIndexMapKey const& other)
{
    return This.getPrimary() > other.getPrimary();
}

bool
TraceIndexMapKey::operatorLessSecondary(TraceIndexMapKey const& This, TraceIndexMapKey const& other)
{
    return This.getSecondary() < other.getSecondary();
}

bool
TraceIndexMapKey::operatorGreaterSecondary(TraceIndexMapKey const& This, TraceIndexMapKey const& other)
{
    return This.getSecondary() > other.getSecondary();
}

//-----------------------------------------------------------------------------
//   TraceIndexMap class
//-----------------------------------------------------------------------------

TraceIndexMap::TraceIndexMap()
  : map_()
  , orderPrimary_{TraceIndexMapKey::Order::Asc}
  , orderSecondary_{TraceIndexMapKey::Order::Asc}
{
    /* EMPTY */
}

TraceIndexMap::TraceIndexMap(TraceIndexStorage& rangeStorage)
  : map_()
  , orderPrimary_{TraceIndexMapKey::Order::Asc}
  , orderSecondary_{TraceIndexMapKey::Order::Asc}
{
    addRanges(rangeStorage);
}

void
TraceIndexMap::setOrderMode(TraceIndexMapKey::Order orderPrimary,
                            TraceIndexMapKey::Order orderSecondary)
{
    orderPrimary_ = orderPrimary;
    orderSecondary_ = orderSecondary;
}

size_t
TraceIndexMap::size() const
{
    return map_.size();
}

bool
TraceIndexMap::isEmpty() const
{
    return map_.empty();
}

TraceIndexMap::const_iterator
TraceIndexMap::cbegin() const
{
    return map_.cbegin();
}

TraceIndexMap::const_iterator
TraceIndexMap::cend() const
{
    return map_.cend();
}

TraceIndexMap::const_iterator
TraceIndexMap::begin() const
{
    // note: allow readonly operations solely
    return map_.cbegin();
}

TraceIndexMap::const_iterator
TraceIndexMap::end() const
{
    // note: allow readonly operations solely
    return map_.cend();
}

void
TraceIndexMap::clearRanges()
{
    map_.clear();
}

void
TraceIndexMap::addRanges(TraceIndexStorage const& rangeStorage)
{
    if (rangeStorage.size() > 0U) {
        addRanges(rangeStorage.begin(), rangeStorage.end());
    }
}

void
TraceIndexMap::addRanges(TraceIndexStorage::const_iterator begin,
                         TraceIndexStorage::const_iterator end)
{
    for (auto it = begin; it not_eq end; ++it) {
        TraceIndexMapItem::first_type key{
            it->getPrimaryNo(),
            it->getSecondaryNo()};
        key.setOrderMode(orderPrimary_, orderSecondary_);
        map_.try_emplace(std::move(key), *it);
    }
}

size_t
TraceIndexMap::enumerateLiveTraceCount(PrimaryIndex const prii0, PrimaryIndex const priiN, IndexStep const priStep,
                                       SecondaryIndex const seci0, SecondaryIndex const seciN, IndexStep const seciStep) const
{
    size_t result{};

    for (PrimaryIndex prii = prii0; prii <= priiN; prii += priStep) {
        for (SecondaryIndex seci = seci0; seci <= seciN; seci += seciStep) {
            auto const range = getRange(prii, seci);

            bool const isLiveTrace = range.has_value()
                                     && range.value()->second.get().isLiveTrace(prii,
                                                                                seci,
                                                                                seciStep);
            if (isLiveTrace) {
                ++result;
            }
        }
    }

    return result;
}

TraceIndexStorage::const_iterator
TraceIndexMap::enumerateMissingRanges(PrimaryIndex const prii0, PrimaryIndex const priiN, IndexStep const priStep,
                                      SecondaryIndex const seci0, SecondaryIndex const seciN, IndexStep const seciStep,
                                      TraceIndexStorage& missingRangeStorage)
{
    TraceIndexStorage::const_iterator result{};

    auto pushBackMissingRange = [&missingRangeStorage](PrimaryIndex prii, SecondaryIndex seci, size_t rangeCount) {
        if (rangeCount) {
            TraceRange r{/*exists*/ false,
                         prii,
                         seci,
                         TraceRange::missingFileNo_,
                         /*samplesCount*/ 0U,
                         rangeCount,
                         TraceRange::missingFileOffset_};
            missingRangeStorage.addRange(std::move(r));
        }
    };

    size_t const rangeStorageInitialSize = missingRangeStorage.size();

    //
    // the map is not empty, so let us scan along every primary index for the missing entries...
    //

    for (PrimaryIndex prii = prii0; prii <= priiN; prii += priStep) {
        SecondaryIndex seciMissing = std::numeric_limits<SecondaryIndex>::max();
        size_t rangeCount = 0;
        for (SecondaryIndex seci = seci0; seci <= seciN; seci += seciStep) {
            auto const range = getRange(prii, seci);

            bool const isHit = range.has_value()
                               && range.value()->second.get().isHit(prii,
                                                                    seci,
                                                                    seciStep);
            if (isHit) {
                pushBackMissingRange(prii, seciMissing, rangeCount);
                rangeCount = 0U;
            }
            else {
                if (not rangeCount) {
                    seciMissing = seci;
                }
                ++rangeCount;
            }
        } // for (SecondaryIndex seci

        pushBackMissingRange(prii, seciMissing, rangeCount);
        rangeCount = 0U;

    } // for (PrimaryIndex prii

    //
    // return an iterator to the first added range
    //
    if (rangeStorageInitialSize < missingRangeStorage.size()) {
        result = missingRangeStorage.cbegin() + static_cast<TraceIndexStorage::difference_type>(rangeStorageInitialSize);
    }
    else {
        result = missingRangeStorage.end();
    }

    return result;
}

TraceIndexMap::MatchingRangeOptional
TraceIndexMap::getRange(PrimaryIndex primaryIndex, SecondaryIndex secondaryIndex) const
{
    MatchingRangeOptional result;

    TraceIndexMapKey key{primaryIndex, secondaryIndex};
    key.setOrderMode(orderPrimary_, orderSecondary_);

    MatchingRange foundRange{
        map_.lower_bound(key)};

    bool const isLowerOk = map_.cend() != foundRange;

    if (isLowerOk) {
        if (key < (*foundRange).first) {
            // adjust the lower bound to point to the range
            // that contains the specified key
            if (map_.cbegin() != foundRange) {
                --foundRange;
            }
        }
    }
    else {
        if (map_.size()) {
            --foundRange;
        }
    }

    bool const isAdjustedLowerOk = map_.cend() != foundRange;

    if (isAdjustedLowerOk) {
        result = MatchingRangeOptional{std::move(foundRange)};
    }

    return result;
}

} // namespace trace_range_map
} // namespace segysdk::segyindex
