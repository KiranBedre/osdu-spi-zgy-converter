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

#include "BrickedConversion.h"

#include "../Concurrent/ThreadPool.h"
#include "../Logger/Logger.h"
#include "../Logger/ProgressReporting.h"
#include "../FileIO/FileIOException.h"
#include "../Utilities/Environment.h"

#include <algorithm>
#include <cstring>
#include <chrono>

BrickedConversion::BrickedConversion(
    segysdk::api::SegySession* segySession,
    const SegyInputParameters& p,
    OpenZGY::IZgyWriter* zgy_writer,
    const segysdk::utilities::BrickSize& brick_size)
  : segy_session_(segySession),
    params_(p),
    zgy_writer_(zgy_writer),
    brick_size_(brick_size),
    brick_samples_{brick_size.sizeI() * brick_size.sizeJ() * brick_size.sizeK()},
    reader_count_(segysdk::utilities::Environment::instance().readerCount()),
    writer_count_(segysdk::utilities::Environment::instance().writerCount()),
    reader_brick_column_buffer_(segysdk::utilities::Environment::instance().readerCount(), std::vector<float>(brick_size.sizeI() * brick_size.sizeJ() * p.getVerticalCount())),
    progress_counter_(0),
    progress_mutex_(),
    segy_bulk_readers_()
{
    
    for (auto i = 0U; i < reader_count_; i++) {
        segy_bulk_readers_.emplace_back(segySession->createBulkReader());
    }
}

void
BrickedConversion::run()
{
    SEGYLOG_DEBUG("Input sample count: " + std::to_string(params_.getVerticalCount()));
    
    segysdk::concurrent::BufferQueue<std::shared_ptr<ReadJob>> readJobQueue(segysdk::utilities::Environment::instance().readJobQueueSize());
    segysdk::concurrent::BufferQueue<std::shared_ptr<BrickColumn>> brickColumnQueue(segysdk::utilities::Environment::instance().brickColumnQueueSize());
    SEGYLOG_DEBUG("Reader job queue size: " + std::to_string(segysdk::utilities::Environment::instance().readJobQueueSize()));
    SEGYLOG_DEBUG("Brick column queue size: " + std::to_string(segysdk::utilities::Environment::instance().brickColumnQueueSize()));
    SEGYLOG_DEBUG("Bytes per brick column: " + std::to_string(sizeof(float) * params_.getVerticalCount() * brick_size_.sizeI() * brick_size_.sizeJ()));
    SEGYLOG_DEBUG("Reader count: " + std::to_string(reader_count_));
    SEGYLOG_DEBUG("Writer count: " + std::to_string(writer_count_));

    // define writer job
    auto writerTask = [this, &brickColumnQueue]() {
        auto brickColumn = std::make_shared<BrickColumn>();
        while (brickColumnQueue.Consume(brickColumn) && brickColumn->write_size_[0] + brickColumn->write_size_[1] + brickColumn->write_size_[2] > 0) {
            zgy_writer_->write(brickColumn->write_pos_, brickColumn->write_size_, brickColumn->brick_buffer_.data());
            this->increaseProgressCounter();
        }
    };

    // creater writers
    auto writers = std::list<std::thread>();
    for (auto writer_id = 0U; writer_id < writer_count_; writer_id++) {
        writers.emplace_back(std::thread(writerTask));
    }

    // define readers job
    auto readerTask = [this, &readJobQueue, &brickColumnQueue](size_t reader_id) {
        auto readJob = std::make_shared<ReadJob>();
        while (readJobQueue.Consume(readJob) && readJob->line_offset_ != END_OF_READ_JOB) {
            auto brickColumn = this->createBrickColumn(reader_id, readJob->line_offset_, readJob->trace_offset_);
            brickColumnQueue.Produce(brickColumn);

        }
    };

    // create readers
    auto readers = std::list<std::thread>();
    for (auto reader_id = 0U; reader_id < reader_count_; reader_id++) {
        readers.emplace_back(std::thread(readerTask, reader_id));
    }

    auto progress_monitor = std::thread([this]() {
        constexpr auto reportFrequency = std::chrono::seconds(5);
        segysdk::logger::ProgressReporting progress(reportFrequency, "segy-to-zgy");
        const std::string reportMessage = "writingBricks";
        progress.report(reportMessage, 0.0);
        auto totalCount = std::ceil(static_cast<double>(params_.getInlineCount()) / brick_size_.sizeI()) * std::ceil(static_cast<double>(params_.getCrosslineCount()) / brick_size_.sizeJ());

        auto previousProgress = 0.0;
        using namespace std::chrono_literals;
        SEGYLOG_DEBUG("Number of brick columns to convert: " + std::to_string(totalCount));
        auto progressCount = this->getProgressCount(); 
        while (progressCount < totalCount)
        {
            auto currentProgress = 100.0 * progressCount / totalCount;
            if (currentProgress > previousProgress) {
                SEGYLOG_DEBUG("Converted brick columns : " + std::to_string(progressCount));
                progress.report(reportMessage, currentProgress);
                previousProgress = currentProgress;
            }
            std::this_thread::sleep_for(1s);
            progressCount = this->getProgressCount();
        }
        SEGYLOG_DEBUG("Converted brick columns : " + std::to_string(progressCount));
    }); 

        // publish reader job
    for (size_t inline_offset = 0; inline_offset < params_.getInlineCount(); inline_offset += brick_size_.sizeI()) {
        for (size_t crossline_offset = 0; crossline_offset < params_.getCrosslineCount(); crossline_offset += brick_size_.sizeJ()) {
            auto readJob = std::make_shared<ReadJob>(inline_offset, crossline_offset);
            readJobQueue.Produce(readJob);
        }
    }

    SEGYLOG_DEBUG("All reader jobs published");

    // Default ReadJob signals end of read jobs. 
    for (auto reader_id = 0U; reader_id < reader_count_; reader_id++) {
        auto endOfReadJob = std::make_shared<ReadJob>();
        readJobQueue.Produce(endOfReadJob);
    }

    // Join all readers
    for (auto& reader : readers) {
        reader.join();
    }
    SEGYLOG_DEBUG("All reader joined");

    // Default ReadJob signals end of read jobs.
    for (auto writer_id = 0U; writer_id < writer_count_; writer_id++) {
        auto endOfBrickColumn = std::make_shared<BrickColumn>();
        brickColumnQueue.Produce(endOfBrickColumn);
    }

    // Join all writers
    for (auto& writer : writers) {
        writer.join();
    }
    SEGYLOG_DEBUG("All writers joined");

    progress_monitor.join();

    SEGYLOG_DEBUG("Finalizing ZGY");
    zgy_writer_->finalize();
}

std::shared_ptr<BrickedConversion::BrickColumn>
BrickedConversion::createBrickColumn(size_t reader_id, size_t line_offset, size_t trace_offset)
{
    auto sizeI = brick_size_.sizeI();
    auto sizeJ = brick_size_.sizeJ();

    OpenZGY::IZgyMeta::size3i_t const write_pos = {
        static_cast<OpenZGY::IZgyMeta::size3i_t::value_type>(line_offset),
        static_cast<OpenZGY::IZgyMeta::size3i_t::value_type>(trace_offset),
        static_cast<OpenZGY::IZgyMeta::size3i_t::value_type>(0)};
    OpenZGY::IZgyMeta::size3i_t const write_size = {
        static_cast<OpenZGY::IZgyMeta::size3i_t::value_type>(sizeI),
        static_cast<OpenZGY::IZgyMeta::size3i_t::value_type>(sizeJ),
        static_cast<OpenZGY::IZgyMeta::size3i_t::value_type>(params_.getVerticalCount())};

    const auto is_whole_brick = line_offset + sizeI <= params_.getInlineCount()
                                && trace_offset + sizeJ <= params_.getCrosslineCount();

    auto brick_column_buffer = reader_brick_column_buffer_[reader_id];

    if (is_whole_brick) {
        segy_bulk_readers_[reader_id]->read(line_offset,
                          trace_offset,
                          0,
                          sizeI,
                          sizeJ,
                          params_.getVerticalCount(),
                          brick_column_buffer.data());
    }
    else {
        const auto i_leftover = std::min(sizeI, params_.getInlineCount() - line_offset);
        const auto j_leftover = std::min(sizeJ, params_.getCrosslineCount() - trace_offset);
        std::vector<float> brick_column_buffer_partial(brick_column_buffer.size(), 0);
        segy_bulk_readers_[reader_id]->read(line_offset,
                          trace_offset,
                          0,
                          i_leftover,
                          j_leftover,
                          params_.getVerticalCount(),
                          brick_column_buffer_partial.data());

        // Set entire brick_column_buffer initially to have 0 value.
        memset(brick_column_buffer.data(), 0, sizeof(float) * brick_column_buffer.size());

        // brick_column_buffer and brick_column_buffer_partial are both organized by inline, 
        // and then by crossline.
        // The issue is to honor any shortened traces in the inline and/or crossline direction.
        // brick_column_buffer has dimensions           sizeI * sizeJ * k_tracesamples_size
        // brick_column_buffer_partial has dimensions  i_leftover * j_leftover * params_.getVerticalCount()
        // k_tracesamples_size is measured in bytes, not in number of samples!
        // i_dimension is the size in bytes of one full brick inline
        // j_dimension is the size in bytes of one full brick trace
        size_t const k_tracesamples_size = sizeof(float) * params_.getVerticalCount();
        for (size_t i = 0; i < i_leftover; ++i) {
            size_t const i_dimension = i * sizeJ * params_.getVerticalCount();
            size_t const i_leftover_dimension = i * j_leftover * params_.getVerticalCount();
            for (size_t j = 0; j < j_leftover; ++j) {
                size_t const j_dimension = i_dimension + j * params_.getVerticalCount();
                size_t const j_leftover_dimension = i_leftover_dimension + j * params_.getVerticalCount();
                memcpy(&brick_column_buffer[j_dimension], &brick_column_buffer_partial[j_leftover_dimension], k_tracesamples_size);
            }
        }
    }
    
    return std::make_shared<BrickColumn>(write_pos, write_size, brick_column_buffer);
}
