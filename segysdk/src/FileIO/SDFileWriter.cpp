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

#include "../FileIO/SDFileWriter.h"

#include "../FileIO/FileIOException.h"

#include <algorithm>
#include <cstring>
#include <iostream>
#include <map>
#include <string>

using seismicdrive::SDDatasetDisposition;
using seismicdrive::SDGenericDataset;
using seismicdrive::SDManager;

namespace segysdk::fileio
{

//***************************************************************************
// class SDFileWriter::Stage

uint8_t*
SDFileWriter::Stage::getData()
{
    if (data_ == nullptr) {
        data_.reset(new uint8_t[capacity_]);
    }

    return data_.get();
}

size_t
SDFileWriter::Stage::append(const uint8_t* data, size_t size)
{
    const size_t copySize = std::min(size, getAvailable());
    std::copy(&data[0], &data[copySize], getTail());
    size_ += copySize;

    return copySize;
}

//***************************************************************************
// class SDFileWriter

SDFileWriter::SDFileWriter(std::shared_ptr<seismicdrive::SDManager> manager, const std::string& fileName, WriteMode mode, size_t blockSize,
                           std::optional<std::string> sdLegalTag)
  : FileWriter(fileName),
    sdManager(std::move(manager)),
    dataSet_(sdManager.get(), fileName),
    stage_(blockSize),
    sdLegalTag_(sdLegalTag)
{
    isOpen_ = open(mode);

    if (isOpen_ && mode == WriteMode::Append) {
        stage_.moveTo(static_cast<int>(dataSet_.getBlockNum()));
    }
}

SDFileWriter::~SDFileWriter()
{
    std::lock_guard lock(mutex_);

    if (isOpen_) {
        close();
    }
}

uint64_t
SDFileWriter::getFileSize()
{
    std::lock_guard lock(mutex_);

    return dataSet_.getSize() + stage_.getSize();
}

size_t
SDFileWriter::writeBlock(const uint8_t* data, size_t size)
{
    const size_t writeSize = std::min(size, stage_.getBlockSize());

    dataSet_.writeBlock(stage_.getBlockIndex(), reinterpret_cast<const char*>(&data[0]), writeSize);
    stage_.moveNext();

    return writeSize;
}

void
SDFileWriter::flushStage()
{
    if (!stage_.isEmpty()) {
        writeBlock(stage_.getData(), stage_.getSize());
    }
}

size_t
SDFileWriter::stageData(const uint8_t* data, size_t size)
{
    if (stage_.isEmpty() && size >= stage_.getBlockSize())
        return writeBlock(data, size);

    const size_t stagedSize = stage_.append(data, size);

    if (stage_.isFull()) {
        flushStage();
    }

    return stagedSize;
}

bool
SDFileWriter::write(const uint8_t* buffer, size_t bufferSize)
{
    if (buffer == nullptr || bufferSize == 0)
        return false;

    if (!isOpen_)
        return false;

    std::lock_guard lock(mutex_);

    try {
        const uint8_t* data = buffer;
        size_t dataSize = bufferSize;

        while (dataSize > 0) {
            const size_t size = stageData(data, dataSize);

            data += size;
            dataSize -= size;
        }
    }
    catch (const std::exception& ex) {
        // Treat as fatal, abort further writing (avoiding further output corruption)
        stage_.moveTo(0);
        close();

        throw FileIOException("SDFileWriter::write: Failure writing file - " + getFileName() + ex.what());
    }

    return true;
}

void
SDFileWriter::close()
{
    std::lock_guard lock(mutex_);

    if (isOpen_) {
        flushStage();
        dataSet_.flush();
        dataSet_.close();
        seismicdrive::SDUtils(sdManager.get()).datasetUnlock(getFileName());
        isOpen_ = false;
    }
}

bool
SDFileWriter::open(WriteMode mode)
{
    std::lock_guard lock(mutex_);

    try {
        static const std::map<fileio::WriteMode, seismicdrive::SDDatasetDisposition> args_map = {
            {WriteMode::Create, seismicdrive::SDDatasetDisposition::CREATE},
            {WriteMode::Truncate, seismicdrive::SDDatasetDisposition::OVERWRITE},
            {WriteMode::Append, seismicdrive::SDDatasetDisposition::READ_WRITE},
        };
        if (args_map.find(mode) == args_map.end()) {
            throw FileIOInvalidArgumentException("Unknown WriteMode");
        }

        if (dataSet_.exists() && dataSet_.checkWriteAccess()){
            seismicdrive::SDUtils(sdManager.get()).datasetUnlock(getFileName());
        }

        if (sdLegalTag_.has_value()) {
            dataSet_.open(args_map.at(mode), *sdLegalTag_);
        }
        else {
            dataSet_.open(args_map.at(mode));
        }
        return true;
    }
    catch (const std::runtime_error& ex) {
        throw FileIOException("SDFileWriter::open - Unable to open file - " + getFileName() + ex.what());
    }

    return false;
}

} // namespace segysdk::fileio
