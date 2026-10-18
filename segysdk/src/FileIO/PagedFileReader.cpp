/*
Copyright 2021 Schlumberger

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

#include "../FileIO/PagedFileReader.h"

#include <algorithm>
#include <cassert>
#include <cstring>
#include <limits>

namespace segysdk::fileio
{

PagedFileReader::PagedFileReader(const std::shared_ptr<FileReader>& reader)
  : FileReader(reader->getFileName()),
    reader_(reader)
{
    setBufferSize(reader_->getBufferSize());
    lastUsedPage_ = &page_[1];
}

void
PagedFileReader::setBufferSize(size_t bufferSize)
{
    std::lock_guard lock(mutex_);

    // Reasonable limitation that also avoids some logical traps later on
    bufferSize = std::min(bufferSize, getFileSize());

    FileReader::setBufferSize(bufferSize);

    for (int i = 0; i < 2; ++i) {
        page_[i].index_ = std::numeric_limits<size_t>::max();
        page_[i].offset_ = 0;
        page_[i].capacity_ = bufferSize;
        ByteBuffer(bufferSize).swap(page_[i].data_);
        page_[i].other_ = &page_[1 - i];
    }
}

PagedFileReader::Page*
PagedFileReader::findCachedPage(size_t pageIndex)
{
    if (page_[0].index_ == pageIndex)
        return &page_[0];
    if (page_[1].index_ == pageIndex)
        return &page_[1];
    return nullptr;
}

ByteBuffer
PagedFileReader::doRead(const FileReadParams& params)
{
    if (params.size_ == 0)
        return {};

    // Bypass large requests, caching is not likely to have benign effect
    if (params.size_ > page_[0].capacity_)
        return reader_->read(params);

    const uint64_t fileSize = reader_->getFileSize();
    if (params.offset_ >= fileSize)
        return {};

    std::lock_guard lock(mutex_);

    uint64_t outOffset = params.offset_;
    uint64_t outSize = std::min(params.size_, fileSize - params.offset_);

    const size_t pageIndexes[2] = {
        outOffset / page_[0].capacity_,
        (outOffset + outSize - 1) / page_[0].capacity_};
    const size_t pageCount = pageIndexes[1] - pageIndexes[0] + 1;

    assert(pageCount <= 2);

    Page* pages[2] = {
        findCachedPage(pageIndexes[0]),
        findCachedPage(pageIndexes[1])};

    if (pageCount == 2 && pages[1] != nullptr) {
        lastUsedPage_ = pages[1]; // Direct page 0 to the other one
    }

    auto result = ByteBuffer{};
    for (size_t i = 0; i < pageCount; ++i) {
        Page* page = pages[i];
        if (page == nullptr) {
            page = lastUsedPage_->other_;
            page->index_ = pageIndexes[i];
            page->offset_ = page->index_ * page->capacity_;
            const size_t readSize = std::min(page->capacity_, fileSize - page->offset_);
            page->data_ = reader_->read(FileReadParams{}.withOffset(page->offset_).withSize(readSize));
        }

        lastUsedPage_ = page;

        if (outOffset < page->offset_)
            break;

        const uint64_t copyOffset = outOffset - page->offset_;
        if (copyOffset >= page->data_.size())
            break;

        const uint64_t copySize = std::min(outSize, page->data_.size() - copyOffset);
        appendContainer(result, std::next(page->data_.begin(), copyOffset), std::next(page->data_.begin(), copyOffset + copySize));

        outOffset += copySize;
        outSize -= copySize;
    }

    return result;
}

ByteBuffer
PagedFileReader::doReadUnbuffered(const FileReadParams& params)
{
    return reader_->read(params);
}

uint64_t
PagedFileReader::getFileSize()
{
    return reader_->getFileSize();
}

void
PagedFileReader::close()
{
    reader_->close();
}

} // namespace segysdk::fileio
