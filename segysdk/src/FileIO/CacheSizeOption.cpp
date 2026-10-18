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

#include "../FileIO/CacheSizeOption.h"
#include "../Utilities/Environment.h"


namespace segysdk
{

struct CacheSizeOption::Impl
{
    Impl(const size_t blockSize, const size_t pages, const size_t blocksPerPage, size_t chunkSize)
      : blockSize_(blockSize), pages_(pages), blocksPerPage_(blocksPerPage), chunkSize_(chunkSize) {}

    size_t
    blockSize() const
    {
        return blockSize_;
    }

    size_t
    blocksPerPage() const
    {
        return blocksPerPage_;
    }

    size_t
    pages() const
    {
        return pages_;
    }

    size_t
    chunkSize() const
    {
        return chunkSize_;
    }

private:
    size_t blockSize_{};
    size_t pages_{};
    size_t blocksPerPage_{};
    size_t chunkSize_{};
};

CacheSizeOption::CacheSizeOption(const size_t blockSize, const size_t pages,
                                 const size_t blocksPerPage, size_t chunkSize)
  : impl_(std::make_shared<Impl>(blockSize, pages,
                                 blocksPerPage, chunkSize))
{
    /* EMPTY */
}

size_t
CacheSizeOption::blockSize() const
{
    return impl_->blockSize();
}

size_t
CacheSizeOption::blocksPerPage() const
{
    return impl_->blocksPerPage();
}

size_t
CacheSizeOption::pageSize() const
{
    return blockSize() * blocksPerPage();
}

size_t
CacheSizeOption::pages() const
{
    return impl_->pages();
}

size_t
CacheSizeOption::chunkSize() const
{
    return impl_->chunkSize();
}

CacheSizeOption
CacheSizeOption::none()
{
    const size_t block_size = utilities::Environment::instance().sdReadCachePageSize();
    if (block_size > 0) {
        return {0, 0, 1, block_size};
    }

    return {0, 0, 1, static_cast<size_t>(1024 * 1024)};
}
} // namespace segysdk
