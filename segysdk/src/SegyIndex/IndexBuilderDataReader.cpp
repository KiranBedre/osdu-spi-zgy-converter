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

#include "IndexBuilderDataReader.h"
#include "IndexBuilderDataManager.h"

#include "../FileIO/DiskFileReader.h"

#include <cstring>
#include <filesystem>

namespace segysdk::segyindex
{

IndexBuilderDataReader::IndexBuilderDataReader(std::string& tmpFileName)
  : tmpFileName_(tmpFileName),
    buffer_(maxBufferedTraceCount * sizeof(IndexBuilderData))
{
    if (std::filesystem::exists(tmpFileName)) {
        reader_ = std::static_pointer_cast<fileio::FileReader>(std::make_shared<fileio::DiskFileReader>(tmpFileName));

        reader_->setDefaultBufferSize(maxBufferedTraceCount * sizeof(IndexBuilderData));

        tmpFileSize_ = std::filesystem::file_size(tmpFileName);
        tmpFileTraceCount_ = tmpFileSize_ / sizeof(IndexBuilderData);

        bufferStartTraceNo_ = 0;
        bufferTraceNo_ = 0;
    }
}

IndexBuilderData
IndexBuilderDataReader::ReadTrace(size_t traceNo)
{

    const size_t offset = traceNo * sizeof(IndexBuilderData);
    if (traceNo < tmpFileTraceCount_) {
        size_t const remainingDataSize = tmpFileSize_ - offset;
        size_t const readSize = remainingDataSize > buffer_.size() ? buffer_.size() : remainingDataSize;
        buffer_ = reader_->read(fileio::FileReadParams{}.withOffset(offset).withSize(readSize));
        myBufferSize_ = buffer_.size();

        if (myBufferSize_ >= sizeof(IndexBuilderData)) {
            bufferStartTraceNo_ = traceNo;
            bufferTraceNo_ = 0;
            return getDataAtTraceOffset(0);
        }
    }
    else {
        myBufferSize_ = 0;
        bufferStartTraceNo_ = tmpFileTraceCount_ + 1;
    }
    return IndexBuilderData();
}

IndexBuilderData
IndexBuilderDataReader::ReadNext()
{
    if (AtEnd())
        return IndexBuilderData();
    bufferTraceNo_++;
    if (ReaderInitialized() && myBufferSize_ > bufferTraceNo_ * sizeof(IndexBuilderData)) {
        // Buffered read
        return getDataAtTraceOffset(bufferTraceNo_);
    }

    // Disk read operation required
    return ReadTrace(bufferStartTraceNo_ + bufferTraceNo_);
}

IndexBuilderData
IndexBuilderDataReader::getDataAtTraceOffset(const size_t offset) const
{
    IndexBuilderData data{};
    const auto pos = offset * sizeof(IndexBuilderData);
    auto* const ptr = &buffer_[pos];
    memcpy(&data, ptr, sizeof data);
    return data;
}

bool
IndexBuilderDataReader::ReaderInitialized() const
{
    return myBufferSize_ >= sizeof(IndexBuilderData);
}

// Shows if current trace is last trace in file or not.
// Depends on bufferTraceNo_ only being advanced just before reading next file data, not after read
bool
IndexBuilderDataReader::AtEnd() const
{
    if (tmpFileSize_ == 0)
        return true;
    if (!ReaderInitialized())
        return false;

    // Check against  (bufferTraceNo_ + 1) * sizeof(IndexBuilderData) < myBufferSize_)
    // To ensure we have buffer space to hold the current trace data and the next trace
    if ((bufferTraceNo_ + 1) * sizeof(IndexBuilderData) < myBufferSize_)
        return false;

    // We are at the last trace in the current buffer. Next traces will need to be read from file.
    // Check we are not at end of file
    if ((bufferStartTraceNo_ + bufferTraceNo_ + 1) * sizeof(IndexBuilderData) < tmpFileSize_)
        return false;

    // At the end of the file.
    return true;
}

void
IndexBuilderDataReader::Close() const
{
    reader_->close();
}
} // namespace segysdk::segyindex
