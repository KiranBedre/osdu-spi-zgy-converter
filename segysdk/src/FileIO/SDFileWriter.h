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

#pragma once

#include "../segysdk_details/DLL_EXPORT.h"

#include "../FileIO/FileWriter.h"
#include "../FileIO/WriteMode.h"

#pragma warning(push)
#pragma warning(disable : 4251)
#include <SDAPI/SDGenericDataset.h>
#include <SDAPI/SDManager.h>
#include <SDAPI/SDUtils.h>
#pragma warning(pop)

#include <cstdlib>
#include <memory>
#include <mutex>
#include <optional>

namespace segysdk::fileio
{

class SEGYSDK_DLL_PUBLIC SDFileWriter : public FileWriter
{
private:
    class Stage final
    {
    private:
        size_t capacity_{};
        size_t size_{};
        int index_{};
        std::unique_ptr<uint8_t[]> data_;

    public:
        explicit Stage(size_t blockSize)
          : capacity_(blockSize) {}
        Stage(const Stage&) = delete;
        Stage(Stage&&) = delete;
        Stage& operator=(const Stage&) = delete;
        Stage& operator=(Stage&&) = delete;

        inline size_t getBlockSize() const { return capacity_; }
        inline size_t getSize() const { return size_; }
        inline size_t getAvailable() const { return capacity_ - size_; }
        inline int getBlockIndex() const { return index_; }

        inline bool isEmpty() const { return size_ == 0; }
        inline bool isFull() const { return size_ == capacity_; }

        inline void moveNext()
        {
            index_++;
            size_ = 0;
        }
        inline void moveTo(int index)
        {
            index_ = index;
            size_ = 0;
        }

        uint8_t* getData();
        uint8_t* getTail() { return getData() + size_; }

        size_t append(const uint8_t* data, size_t size);
    };

private:
    const std::shared_ptr<seismicdrive::SDManager> sdManager;
    seismicdrive::SDGenericDataset dataSet_;
    std::recursive_mutex mutex_{};
    Stage stage_;
    std::optional<std::string> sdLegalTag_ = std::nullopt;

    bool isOpen_ = false;

    bool open(WriteMode mode);
    size_t writeBlock(const uint8_t* data, size_t size);
    size_t stageData(const uint8_t* data, size_t size);
    void flushStage();

public:
    SDFileWriter(std::shared_ptr<seismicdrive::SDManager> manager, const std::string& fileName, WriteMode mode, size_t blockSize,
                 std::optional<std::string> sdLegalTag = std::nullopt);
    virtual ~SDFileWriter() override;

    virtual uint64_t getFileSize() override;
    virtual bool write(const uint8_t* buffer, size_t bufferSize) override;
    virtual void close() override;
};

} // namespace segysdk::fileio
