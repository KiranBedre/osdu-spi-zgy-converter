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

#pragma once

#include "SegyInputParameters.h"

#include "../BulkAccess/SegyBulkReader.h"
#include "../Utilities/BrickSize.h"
#include "../segysdk/SegySession.h"

#include <openzgy/api.h>

#include <memory>
#include <vector>
#include <mutex>
#define END_OF_READ_JOB 0xFFFFFFFFFFFFFFFF

class BrickedConversion
{
private:
    using SegyBulkReaderPtr = std::unique_ptr<segysdk::bulkaccess::SegyBulkReader>;

    struct ReadJob
    {
        const size_t line_offset_;
        const size_t trace_offset_;
        ReadJob()
          : line_offset_(END_OF_READ_JOB), trace_offset_(END_OF_READ_JOB) {}

        ReadJob(const size_t line_offset, const size_t trace_offset)
          : line_offset_(line_offset), trace_offset_(trace_offset) {}

    };

    struct BrickColumn
    {
        BrickColumn() : 
            write_pos_(OpenZGY::IZgyMeta::size3i_t()), 
            write_size_(OpenZGY::IZgyMeta::size3i_t()), 
            brick_buffer_(std::vector<float>()) {}

        BrickColumn(OpenZGY::IZgyMeta::size3i_t write_pos, OpenZGY::IZgyMeta::size3i_t write_size, std::vector<float> brick_buffer) : 
            write_pos_(write_pos), write_size_(write_size), brick_buffer_(brick_buffer.size()) 
        {
            std::copy(brick_buffer.begin(), brick_buffer.end(), brick_buffer_.begin());
        }

        OpenZGY::IZgyMeta::size3i_t write_pos_;
        OpenZGY::IZgyMeta::size3i_t write_size_;
        std::vector<float> brick_buffer_;
    };

private:
    segysdk::api::SegySession* segy_session_{nullptr};
    const SegyInputParameters& params_;
    OpenZGY::IZgyWriter* zgy_writer_{nullptr};
    const segysdk::utilities::BrickSize& brick_size_;
    size_t const brick_samples_;
    size_t const reader_count_;
    size_t const writer_count_;
    std::vector<std::vector<float>> reader_brick_column_buffer_;
    size_t progress_counter_;
    std::mutex progress_mutex_;
    std::vector<SegyBulkReaderPtr> segy_bulk_readers_;

public:
    BrickedConversion(
        segysdk::api::SegySession* segySession,
        const SegyInputParameters& p,
        OpenZGY::IZgyWriter* zgy_writer,
        const segysdk::utilities::BrickSize& brick_size);

    void run();
    std::shared_ptr<BrickedConversion::BrickColumn> createBrickColumn(size_t reader_id, size_t line_offset, size_t trace_offset);
    void increaseProgressCounter()
    {
        std::unique_lock<std::mutex> lock(progress_mutex_);
        progress_counter_++;
    }

    size_t getProgressCount()
    {
        std::unique_lock<std::mutex> lock(progress_mutex_);
        return progress_counter_;
    }
};

