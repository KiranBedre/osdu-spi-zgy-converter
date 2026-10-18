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

#include "../SegyIndex/Utilities/TraceIndexStorage.h"
#include "../SegyIndex/Utilities/TraceRange.h"

#include <cstdint>
#include <deque>
#include <functional>
#include <initializer_list>
#include <map>
#include <optional>

namespace segysdk::segyindex
{
namespace trace_range_map
{

/**
 * struct TraceIndexMapKey Implements the index map key represented by a pair of the primary and the secondary indexes
 **/

struct SEGYSDK_DLL_PUBLIC TraceIndexMapKey final
{
    using TraceIndexMapKeyBase = std::pair<PrimaryIndex, SecondaryIndex>;

    /**
     * @brief The Order enumeration defines the map keys sorting order modes
     *
     * @param Asc - expects the index steps/increments are positive
     * @param Desc - expects the index steps/increments are negative
     **/
    enum class Order
    {
        Asc,
        Desc
    };

    TraceIndexMapKey();
    TraceIndexMapKey(PrimaryIndex prii, SecondaryIndex seci);
    explicit TraceIndexMapKey(TraceIndexMapKey&& other);

    /**
     * @brief Sets the map keys sorting order
     *
     * @param orderPrimary - primary index order, please refer to TraceIndexMapKey::Order enum description
     * @param orderSecondary - secondary index order, please refer to TraceIndexMapKey::Order enum description
     **/
    void setOrderMode(TraceIndexMapKey::Order orderPrimary,
                      TraceIndexMapKey::Order orderSecondary);

    /**
     * @brief Returns the primary index value
     **/
    PrimaryIndex getPrimary() const;

    /**
     * @brief Returns the secondary index value
     **/
    SecondaryIndex getSecondary() const;

    /**
     * @brief The main/required Less operator (the std::map key requirements)
     *
     * @param other - represents another instance to compare with
     *
     * @return the boolean value if the current instance is less then other
     **/
    bool operator<(TraceIndexMapKey const& other) const noexcept;

    /**
     * @brief The equality operator
     *
     * @param other - represents another instance to compare with
     *
     * @return the boolean value if the the current instance equals to other
     **/
    bool operator==(TraceIndexMapKey const& other) const noexcept;

    /**
     * @brief The not equality operator
    *
     * @param other - represents another instance to compare with
     *
     * @return the boolean value if the the current instance does not equal to other
     **/
    bool operator!=(TraceIndexMapKey const& other) const noexcept;

private:
    typedef bool (*ComparisonFunction)(TraceIndexMapKey const&, TraceIndexMapKey const&);

private:
    /**
     * @brief The actual main Less operator implementation for Primary Index Order::Asc sorting mode
     *
     * @param This - represents the current instance
     * @param other - represents another instance to compare with
     *
     * @return the boolean value if the This is less then other
     **/
    static bool operatorLessPrimary(TraceIndexMapKey const& This, TraceIndexMapKey const& other);

    /**
     * @brief The actual main Less operator implementation for Primary Index Order::Desc sorting mode
     *
     * @param This - represents the current instance
     * @param other - represents another instance to compare with
     *
     * @return the boolean value if the This is greater then other
     **/
    static bool operatorGreaterPrimary(TraceIndexMapKey const& This, TraceIndexMapKey const& other);

    /**
     * @brief The actual main Less operator implementation for Secondary Index Order::Asc sorting mode
     *
     * @param This - represents the current instance
     * @param other - represents another instance to compare with
     *
     * @return the boolean value if the This is less then other
     **/
    static bool operatorLessSecondary(TraceIndexMapKey const& This, TraceIndexMapKey const& other);

    /**
     * @brief The actual main Less operator implementation for Secondary Index Order::Desc sorting mode
     *
     * @param This - represents the current instance
     * @param other - represents another instance to compare with
     *
     * @return the boolean value if the This is greater then other
     **/
    static bool operatorGreaterSecondary(TraceIndexMapKey const& This, TraceIndexMapKey const& other);

    TraceIndexMapKey(TraceIndexMapKey const&) = delete;
    TraceIndexMapKey& operator=(TraceIndexMapKey const&) = delete;
    TraceIndexMapKey& operator=(TraceIndexMapKey&& other) = delete;

private:
    TraceIndexMapKeyBase key_;
    ComparisonFunction comparisonFunctionPrimary_;
    ComparisonFunction comparisonFunctionSecondary_;
};

/**
 * @brief struct TraceIndexMap Implements the index map represented by a pair of the TraceIndexMapKey key and a reference to the TraceRange instance
 **/

struct SEGYSDK_DLL_PUBLIC TraceIndexMap final
{
    using TraceIndexMapValueType = std::reference_wrapper<TraceRange const>;
    using TraceIndexMapItem = std::pair<TraceIndexMapKey, TraceIndexMapValueType>;
    using TraceIndexMapBase = std::map<TraceIndexMapItem::first_type, TraceIndexMapItem::second_type>;

    using const_iterator = TraceIndexMapBase::const_iterator;

    using MatchingRange = TraceIndexMapBase::const_iterator;
    using MatchingRangeOptional = std::optional<MatchingRange>;

    /**
     * @brief Constructs an empty index map instance
     **/
    TraceIndexMap();

    /**
     * @brief Constructs and initializes the current index map instance
     *
     * @param rangeStorage - the storage to take the items references from
     *
     * @remarks The storage must have the scope that is wider than the current map one
     **/
    TraceIndexMap(TraceIndexStorage& rangeStorage);

    /**
     * @brief Sets the map keys sorting order
     *
     * @param orderPrimary - primary index order, please refer to TraceIndexMapKey::Order enum description
     * @param orderSecondary - secondary index order, please refer to TraceIndexMapKey::Order enum description
     **/
    void setOrderMode(TraceIndexMapKey::Order orderPrimary,
                      TraceIndexMapKey::Order orderSecondary);

    /**
     * @brief Returns the number of key-value pairs in the map
     **/
    size_t size() const;

    /**
     * @brief Returns true if the map does not contain any key-value pair
     **/
    bool isEmpty() const;

    /**
     * @brief Returns the constant iterator to the beginning of the map contents
     **/
    const_iterator cbegin() const;

    /**
     * @brief Returns the constant iterator beyond the end of the map contents
     **/
    const_iterator cend() const;

    /**
     * @brief Returns the constant iterator to the beginning of the map contents
     **/
    const_iterator begin() const;

    /**
     * @brief Returns the constant iterator beyond the end of the map contents
     **/
    const_iterator end() const;

    /**
     * @brief Removes all the map contents
     **/
    void clearRanges();

    /**
     * @brief Adds a whole TraceIndexStorage instance items references to the map
     *
     * @param rangeStorage - the storage to take the items references from
     *
     * @remarks The storage must have the scope that is wider than the current map one
     **/
    void addRanges(TraceIndexStorage const& rangeStorage);

    /**
     * @brief Adds a subset of the TraceIndexStorage instance items references to the map
     *
     * @param begin - the iterator pointing to the first storage item to take the references from
     * @param end - the iterator pointing beyond the last storage item to take the references from
     *
     * @remarks The storage must have the scope that is wider than the current map one
     **/
    void addRanges(TraceIndexStorage::const_iterator begin,
                   TraceIndexStorage::const_iterator end);

    /**
     * @brief Returns a number of present traces that the map ranges are describing
     *
     * @param prii0 - the first primary index value
     * @param priiN - the last primary index value
     * @param priStep - the primary index step/increment value
     * @param seci0 - the first secondary index value
     * @param seciN - the last secondary index value
     * @param seciStep - the secondary index step/increment value
     *
     * @return number of present traces that the map ranges are describing
     **/
    size_t enumerateLiveTraceCount(PrimaryIndex const prii0, PrimaryIndex const priiN, IndexStep const priStep,
                                   SecondaryIndex const seci0, SecondaryIndex const seciN, IndexStep const seciStep) const;

    /**
     * @brief Scans the map contents and creates the new ranges to cover all the missing traces according the specified area.
     *        The just created ranges are stored to the TraceIndexStorage instance
     *
     * @param prii0 - the first primary index value
     * @param priiN - the last primary index value
     * @param priStep - the primary index step/increment value
     * @param seci0 - the first secondary index value
     * @param seciN - the last secondary index value
     * @param seciStep - the secondary index step/increment value
     * @param missingRangeStorage - the storage to keep all the created ranges
     *
     * @return constant iterator to the first added range item; otherwise the missingRangeStorage.cend()
     **/
    TraceIndexStorage::const_iterator
    enumerateMissingRanges(PrimaryIndex const prii0, PrimaryIndex const priiN, IndexStep const priStep,
                           SecondaryIndex const seci0, SecondaryIndex const seciN, IndexStep const seciStep,
                           TraceIndexStorage& missingRangeStorage);

    /**
     * @brief Returns an optional range that matches the requested indexes pair
     *
     * @param primaryIndex - the required trace's primary index
     * @param secondaryIndex - the required trace's secondary index
     *
     * @return an optional range that matches the requested indexes pair; the optional container remains empty otherwise
     **/
    MatchingRangeOptional getRange(PrimaryIndex primaryIndex, SecondaryIndex secondaryIndex) const;

private:
    TraceIndexMap(TraceIndexMap const&) = delete;
    TraceIndexMap& operator=(TraceIndexMap const&) = delete;
    TraceIndexMap(TraceIndexMap&&) = delete;
    TraceIndexMap& operator=(TraceIndexMap&&) = delete;

private:
    TraceIndexMapBase map_;
    TraceIndexMapKey::Order orderPrimary_;
    TraceIndexMapKey::Order orderSecondary_;
};

} // namespace trace_range_map
} // namespace segysdk::segyindex
