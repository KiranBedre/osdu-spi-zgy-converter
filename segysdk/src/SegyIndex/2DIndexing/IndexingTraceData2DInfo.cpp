#include "IndexingTraceData2DInfo.h"
#include "../../SegyIO/SegyIOException.h"

#include <algorithm>

namespace segysdk::segyindex
{

namespace index_2d
{

void
IndexingTraceData2DInfo::updateWithIndexData(const IndexBuilderData& data)
{
    line_.append(data);
}

size_t
IndexingTraceData2DInfo::getIndexedTraceCount() const
{
    return line_.getIndexedTraceCount();
}

void
IndexingTraceData2DInfo::clear()
{
    line_.clear();
}

bool
IndexingTraceData2DInfo::isRegular() const
{
    return line_.isRegular();
}

bool
IndexingTraceData2DInfo::isValid() const
{
    return line_.isValid();
}

size_t
IndexingTraceData2DInfo::getFileOffsetForCMP(int CMP) const
{
    return line_.getFileOffsetForCMP(CMP);
}

size_t
IndexingTraceData2DInfo::getFileOffsetForTrace(size_t traceNo) const
{
    return line_.getFileOffsetForTrace(traceNo);
}

size_t
IndexingTraceData2DInfo::getTraceNoForCMP(int CMP, bool findNextClosest) const
{
    return line_.getTraceNoForCMP(CMP, findNextClosest);
}

void
IndexingTraceData2DInfo::setSampleSize(size_t sampleSize)
{
    line_.setSampleSize(sampleSize);
}

void
IndexingTraceData2DInfo::setTraceHeaderSize(size_t traceHeaderSize)
{
    line_.setTraceHeaderSize(traceHeaderSize);
}

void
IndexingTraceData2DInfo::setSeismicLineExtents(VerticalExtent sampleExtents)
{
    const auto& records = line_.getRecords();
    const auto& first = records.front();
    const auto& last = records.back();
    int cmpStep = (first.cmpEnd_ - first.cmpStart_) / (static_cast<int>(first.traceCount_) - 1);
    // No track of gaps in CMP
    lineExtents_ = SeismicLineExtents(HorizontalExtent(first.cmpStart_, last.cmpEnd_, cmpStep), std::move(sampleExtents));
}

void
IndexingTraceData2DInfo::setSeismicLineExtents(SeismicLineExtents lineExtents)
{
    lineExtents_ = std::move(lineExtents);
}

const SeismicLineExtents&
IndexingTraceData2DInfo::getLineExtents() const
{
    return lineExtents_;
}

bool
IndexingTraceData2DInfo::saveIndex(std::shared_ptr<segysdk::fileio::FileWriter>& writer) const
{
    const auto& records = line_.getRecords();
    if (records.empty()) {
        return true;
    }

    std::vector<Line2DRecord> indexVector(records.begin(), records.end());

    writer->write(reinterpret_cast<const uint8_t*>(indexVector.data()),
                  indexVector.size() * sizeof(Line2DRecord));

    return true;
}

bool
IndexingTraceData2DInfo::loadIndex(segysdk::fileio::BufferedFileReaderBase& reader, IndexData2DVersion&& version)
{
    line_.clear();

    if (version.major_ == index_config::IndexMajorVersion_2024_01_ and version.minor_ == index_config::IndexMinorVersion_2024_01_) {
        return populateIndexFrom_2024_01(reader);
    }

    const std::string msg = "IndexLoad 2D: unknown version Major:" + std::to_string(version.major_) + " Minor:" + std::to_string(version.minor_);
    throw SegyInvalidArgumentException(msg);
}

Json::Value
IndexingTraceData2DInfo::toJsonValue() const
{
    return line_.toJsonValue();
}

bool
IndexingTraceData2DInfo::populateIndexFrom_2024_01(segysdk::fileio::BufferedFileReaderBase& reader)
{
    constexpr size_t recordSize = sizeof(Line2DRecord);
    size_t recordsRead = 0;
    const size_t indexCount = reader.getFileSize() / recordSize;

    for (/* EMPTY */; recordsRead < indexCount; ++recordsRead) {
        size_t offset = recordsRead * recordSize;

        auto const* const indexRecordBuff = reader.readAligned(offset, recordSize, /*alignment*/ 0U);
        if (nullptr == indexRecordBuff) {
            throw segyio::SegyFileIOException("IndexLoad 2D populateIndexFrom_2024_01: cannot read binary index file.");
        }

        auto const* recordPtr = reinterpret_cast<Line2DRecord const*>(indexRecordBuff);
        Line2DRecord record(*recordPtr);
        line_.addRecord(std::move(record));
    }
    return true;
}

} // namespace index_2d
} // namespace segysdk::segyindex