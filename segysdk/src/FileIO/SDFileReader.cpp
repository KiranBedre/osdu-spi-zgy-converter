/*
Copyright 2021-2024 Schlumberger

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

#include "../FileIO/SDFileReader.h"
#include "../FileIO/FileIOException.h"
#include "../Logger/Logger.h"
#include "../Telemetry/Connector.h"

#include <algorithm>
#include <exception>
#include <iostream>

using seismicdrive::SDDatasetDisposition;
using seismicdrive::SDGenericDataset;
using seismicdrive::SDManager;

namespace segysdk::fileio
{

SDFileReader::SDFileReader(const SDFileReaderParams& params)
  : FileReader(params.fileName_),
    sdConnection_(params.connection_),
    dataSet_(sdConnection_.sdManager.get(), params.fileName_),
    fileSize_(0),
    blockSize_(0),
    hasVariableBlockSize_(false),
    isOpen_(false)
{
    const auto& scope = START_TRACE_SPAN(SpanContext{}.withAttributes({{"sd.filename", params.fileName_}, params.getCorrelationId()}));
    try {
        SEGYLOG_DEBUG("Opening " + params.fileName_);
        dataSet_.open(SDDatasetDisposition::READ_ONLY);
        isOpen_ = true;
        const int64_t blockSize = dataSet_.getBlockSize(0);
        blockSize_ = static_cast<uint64_t>(blockSize);

        const auto blockCount = static_cast<int>(dataSet_.getBlockNum());
        if (blockCount > 1) {
            // Last block is allowed to be smaller than the rest, but not larger
            const auto lastBlockId = blockCount - 1;
            const auto lastBlockSize = dataSet_.getBlockSize(lastBlockId);
            if (lastBlockSize > blockSize) {
                hasVariableBlockSize_ = true;
            }

            // Verify that other blocks are same size
            // TODO make it an option to get block sizes for the whole file or only first few blocks (safe vs fast)
            // TODO getBlocksSize may be a better choice in the long run (or not, depending on implementation)
            for (int block = 1; block < 2 /*blockCount - 1*/ && !hasVariableBlockSize_; ++block) {
                if (dataSet_.getBlockSize(block) != blockSize) {
                    hasVariableBlockSize_ = true;
                }
            }
        }

        if (blockSize_ == 0 && !hasVariableBlockSize_) {
            throw FileIOException("SDMS reported fixed block size 0 for " + params.fileName_ + ".");
        }

        fileSize_ = dataSet_.getSize();
    }
    catch (const std::exception& ex) {
        std::cerr << "SDFileReader::SDFileReader: Error opening sd file: " << std::endl
                  << ex.what() << std::endl;
        throw FileIOException(std::string("SDFileReader::SDFileReader: Error opening sd file: ") + ex.what());
    }
}

SDFileReader::~SDFileReader()
{
    closeDataset();
}

void
SDFileReader::close()
{
    closeDataset();
}

void
SDFileReader::closeDataset()
{
    auto lock = std::lock_guard{mutex_};
    if (!isOpen_) {
        return;
    }
    isOpen_ = false;
    try {
        dataSet_.close();
    }
    catch (...) {
        // Suppress exceptions from seismicdrive::SDGenericDataset::close() as we can be called from noexcept context
    }
}

ByteBuffer
SDFileReader::doRead(const FileReadParams& params)
{
    const auto& scope = START_TRACE_SPAN((SpanContext{}
                                              .withTraceHeader(params.traceHeader_)
                                              .withAttributes({{"offset", std::to_string(params.offset_)},
                                                               {"size", std::to_string(params.size_)},
                                                               params.getCorrelationId()})));
    ByteBuffer result;
    try {     
        

        if (params.offset_ >= getFileSize() || params.size_ == 0)
            return result;

        // Truncate requests beyond EOF (BC behavior)
        auto bufferSize = params.size_;
        if (params.offset_ + bufferSize > getFileSize()) {
            bufferSize = getFileSize() - params.offset_;
        }

        result.resize(bufferSize);
        if (params.offset_ + bufferSize <= blockSize_) {
            dataSet_.readBlock(0, reinterpret_cast<char*>(result.data()), params.offset_, bufferSize);
            return result;
        }

        std::size_t block = 0;
        size_t localOffset = params.offset_;

        if (hasVariableBlockSize_) {
            size_t blockSize = blockSize_;
            while (localOffset >= blockSize) {
                localOffset -= blockSize;
                block++;
                blockSize = dataSet_.getBlockSize(static_cast<int>(block));
            }
        }
        else {
            block = static_cast<int>(params.offset_ / blockSize_);
            localOffset %= blockSize_;
        }

        auto output = reinterpret_cast<char*>(result.data());
        size_t remainingSize = bufferSize;
        const size_t blockCount = dataSet_.getBlockNum();
        for (; block < blockCount && remainingSize > 0; ++block) {
            const size_t blockSize = dataSet_.getBlockSize(static_cast<int>(block));
            const size_t readSize = std::min(remainingSize, blockSize - localOffset);

            dataSet_.readBlock(static_cast<int>(block), output, localOffset, readSize);

            remainingSize -= readSize;
            output += readSize;
            localOffset = 0;
        }
        result.resize(bufferSize - remainingSize);
        return result;
    }
    catch (std::exception& e) {
        SEGYLOG_REPORT_ERROR("SDFileReader::doRead - Exception caught: " + std::string(e.what()));
    }

    return result;
}

uint64_t
SDFileReader::getFileSize()
{
    return fileSize_;
}

SDFileReaderParams&
SDFileReaderParams::withConnection(const SDConnection& value)
{
    connection_ = value;
    return *this;
}

} // namespace segysdk::fileio
