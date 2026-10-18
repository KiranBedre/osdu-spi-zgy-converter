/*
Copyright 2021-2022 Schlumberger

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

/** @file
 * @brief CacheSizeOption.h contains the interface to setting the read cache for SEG-Y files.
 *
 */

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include <cstddef>
#include <memory>

namespace segysdk
{

/**
 * @brief Cache size setting for the read cache.
 *
 * Reads from Seismic Data Management Services (SDMS) or similar data sources can be cached.
 * The cache size must be specified in page size (I/O block size) and number of pages (blocks).
 *
 */
struct SEGYSDK_DLL_PUBLIC CacheSizeOption
{
    /**
     * @brief Constructor to initialize all fields.
     *
     * @param blockSize     Size of a cache block in bytes.
     * @param pages         Maximum number of cache pages.
     * @param blocksPerPage Number of blocks in a cache page.
     * @param chunkSize Optimal reading size to load the cache in bytes.
     */
    CacheSizeOption(size_t blockSize, size_t pages, size_t blocksPerPage, size_t chunkSize);

    /**
     * @brief Size of a cache block in bytes. Block is the unit used to fill a page in parallel.
     */
    size_t blockSize() const;

    /**
     * @brief Number of blocks (parallel I/O units) to fill a page.
     */
    size_t blocksPerPage() const;

    /**
     * @brief Size of a cache page in bytes.
     */
    size_t pageSize() const;

    /**
     * @brief Maximum number of cache pages.
     */
    size_t pages() const;

    /**
     * @brief Size of an individual read portion
     */
    size_t chunkSize() const;

    /**
     * @brief Cache size setting where no cache should be used.
     */
    static CacheSizeOption none();

private:
    struct Impl;
    std::shared_ptr<Impl> impl_;
};

} // namespace segysdk
