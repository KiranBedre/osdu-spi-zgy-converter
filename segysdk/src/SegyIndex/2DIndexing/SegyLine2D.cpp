#include "SegyLine2D.h"

#include <algorithm>

namespace segysdk::segyindex::index_2d
{

SegyLine2D::SegyLine2D() noexcept
  : cmpMap_(SegyLine2D::compare)
{
}

void
SegyLine2D::append(const IndexBuilderData& data)
{
    Line2DRecord record{data.TraceNo, data.CMP, data.CMP, data.SampleCount, 1, data.Offset};

    auto updateLastRecord = [this, &data]() {
        auto& range = records_.back();
        range.traceLastNo_ = data.TraceNo;
        range.cmpEnd_ = data.CMP;
        ++range.traceCount_;
    };

    if (records_.empty()) {
        records_.emplace_back(record);
    }
    else if (records_.back().samplesCount_ != data.SampleCount) {
        isRegular_ = false;
        records_.emplace_back(record);
    }
    else if (records_.back().cmpStart_ == records_.back().cmpEnd_) {
        // Record at the back was added at the previous step. CMP step is unknown, just update the record
        updateLastRecord();
    }
    else {
        auto& range = records_.back();
        int cmpStep = (range.cmpEnd_ - range.cmpStart_) / (static_cast<int>(range.traceCount_) - 1);
        int curCmpStep = data.CMP - range.cmpEnd_;
        if (cmpStep == curCmpStep) {
            updateLastRecord();
        }
        else {
            isRegular_ = false;
            records_.emplace_back(record);
        }
    }
}

size_t
SegyLine2D::getSamplesCount() const noexcept
{
    size_t samplesCount = 0;
    std::for_each(records_.begin(), records_.end(),
                  [&samplesCount](const auto& r) { samplesCount += r.samplesCount_; });
    return samplesCount;
}

size_t
SegyLine2D::getFileOffset() const noexcept
{
    return records_.empty() ? 0 : records_[0].fileOffset_;
}

void
SegyLine2D::populateCmpMap() const
{
    if (cmpMap_.empty() and not records_.empty()) {
        // Lazy loading as the function will be called after all records representing SEGY data will be read
        for (size_t i = 0; i < records_.size(); ++i) {
            const auto& record = records_[i];
            cmpMap_[{record.cmpStart_, record.cmpEnd_}] = i;
        }
    }
}

size_t
SegyLine2D::getFileOffsetForCMP(int CMP) const
{
    populateCmpMap();

    size_t offset = 0;
    auto recordIt = cmpMap_.find({CMP, CMP});
    if (recordIt != cmpMap_.end()) {
        const auto& record = records_[recordIt->second];
        if (record.cmpStart_ == record.cmpEnd_) {
            // Record of only one CMP range. Return offset without calculation
            return record.fileOffset_;
        }
        int cmpStep = (record.cmpEnd_ - record.cmpStart_) / (static_cast<int>(record.traceCount_) - 1);
        int cmpDif = CMP - record.cmpStart_;
        if (cmpDif % cmpStep != 0)  {
            // Requested CMP is in range however hits between step
            return 0;
        }
        int traceOffset = cmpDif / cmpStep;
        auto traceSize = getTraceSize(record.samplesCount_);
        offset = record.fileOffset_ + traceOffset * traceSize;
    }
    return offset;
}

size_t
SegyLine2D::getFileOffsetForTrace(size_t traceNo) const
{
    // All records are sorted by TraceNo on insertion (update call)
    const auto& recordIt = std::lower_bound(records_.begin(), records_.end(), traceNo,
                                            [](const auto& r, size_t tNo) {
                                                return r.traceLastNo_ < tNo;
                                            });

    if (recordIt == records_.end()) {
        return 0;
    }
    // Find offset in the current Trace Range
    size_t traceOffset = recordIt != records_.begin() ? std::prev(recordIt)->traceLastNo_ : recordIt->traceLastNo_ - recordIt->traceCount_;
    traceOffset += 1;
    if (traceNo < traceOffset) {
        return 0;
    }
    traceOffset = traceNo - traceOffset;
    auto traceSize = getTraceSize(recordIt->samplesCount_);
    return traceOffset * traceSize + recordIt->fileOffset_;
}

size_t
SegyLine2D::getTraceNoForCMP(int CMP, bool findNextClosest) const
{
    populateCmpMap();

    auto getTraceNo = [](const segysdk::segyindex::index_2d::Line2DRecord& record, size_t currentTraceNo) {
        size_t startTraceNo = record.traceLastNo_ - record.traceCount_ + 1;
        return startTraceNo + currentTraceNo;
    };

    auto cmpIt = cmpMap_.find({CMP, CMP});
    if (cmpIt != cmpMap_.end()) {
        const auto& record = records_[cmpIt->second];
        if (record.cmpStart_ == record.cmpEnd_) {
            // Record of only one CMP range. Return traceNo without calculation
            return record.traceLastNo_;
        }
        int cmpStep = (record.cmpEnd_ - record.cmpStart_) / (static_cast<int>(record.traceCount_) - 1);
        int cmpDif = CMP - record.cmpStart_;
        if (cmpDif % cmpStep != 0)  {
            // Requested CMP is in range however hits between step
            if (findNextClosest) {
                // get next traceNo that is in range
                size_t traceNo = cmpDif / cmpStep + 1;
                traceNo = getTraceNo(record, traceNo);
                if (traceNo > records_.back().traceLastNo_) {
                    traceNo = records_.back().traceLastNo_;
                }
                return traceNo;
            }
            return 0;
        }
        int traceNo = cmpDif / cmpStep;
        return getTraceNo(record, traceNo);
    }

    if (not findNextClosest) {
        return 0;
    }

    auto firstRange = records_.front();
    auto lastRange = records_.back();

    int cmpFirst = std::min({firstRange.cmpStart_, firstRange.cmpEnd_, lastRange.cmpStart_, lastRange.cmpEnd_});
    int cmpLast = std::max({firstRange.cmpStart_, firstRange.cmpEnd_, lastRange.cmpStart_, lastRange.cmpEnd_});
    if (CMP < cmpFirst or CMP > cmpLast) {
        // outside of the range, return last traceNo
        return records_.back().traceLastNo_;
    }
    // somewhere in the range, find existing TraceNo for existing CMP which is next from requested
    bool positiveDirection = std::min(lastRange.cmpStart_, lastRange.cmpEnd_) > std::max(firstRange.cmpStart_, firstRange.cmpEnd_);
    const auto& recordIt = std::lower_bound(records_.begin(), records_.end(), CMP,
                                     [&positiveDirection](const auto& r, int cmp) {
                                        if (positiveDirection) {
                                            return cmp < std::max(r.cmpStart_, r.cmpEnd_);
                                        }
                                        return cmp > std::min(r.cmpStart_, r.cmpEnd_);
                                     });
    return recordIt->traceLastNo_;
}

std::deque<Line2DRecord> const&
SegyLine2D::getRecords() const
{
    return records_;
}

Json::Value
SegyLine2D::toJsonValue() const
{
    constexpr const char* traceLastNo = "traceLastNo";
    constexpr const char* cmpStart = "cmpStart";
    constexpr const char* cmpEnd = "cmpEnd";
    constexpr const char* samplesCount = "samplesCount";
    constexpr const char* traceCount = "traceCount";
    constexpr const char* fileOffset = "fileOffset";

    Json::Value records(Json::arrayValue);
    for (const auto& rec : records_) {
        Json::Value jsonRecord;
        jsonRecord[traceLastNo] = static_cast<Json::UInt64>(rec.traceLastNo_);
        jsonRecord[cmpStart] = rec.cmpStart_;
        jsonRecord[cmpEnd] = rec.cmpEnd_;
        jsonRecord[samplesCount] = static_cast<Json::UInt64>(rec.samplesCount_);
        jsonRecord[traceCount] = static_cast<Json::UInt64>(rec.traceCount_);
        jsonRecord[fileOffset] = static_cast<Json::UInt64>(rec.fileOffset_);
        records.append(jsonRecord);
    }
    return records;
}

size_t
SegyLine2D::getIndexedTraceCount() const
{
    size_t indexedTraces = 0;
    std::for_each(records_.begin(), records_.end(), [&indexedTraces](const auto& r) { indexedTraces += r.traceCount_; });
    return indexedTraces;
}

bool
SegyLine2D::isRegular() const
{
    return isRegular_;
}

void
SegyLine2D::addRecord(Line2DRecord&& record)
{
    records_.emplace_back(record);

    if (records_.size() > 2) {
        isRegular_ = false;
    }
}

size_t
SegyLine2D::size() const
{
    return records_.size();
}

bool
SegyLine2D::isValid() const
{
    return isRegular() ? records_.size() == 1 : records_.size() > 1;
}

void
SegyLine2D::clear()
{
    records_.clear();
    cmpMap_.clear();
    isRegular_ = true;
}

void
SegyLine2D::setSampleSize(size_t sampleSize)
{
    sampleSize_ = sampleSize;
}

void
SegyLine2D::setTraceHeaderSize(size_t traceHeaderSize)
{
    traceHeaderSize_ = traceHeaderSize;
}

bool
SegyLine2D::compare(const CMPRange& a, const CMPRange& b)
{
    // Based on an asumption that CMP ranges don't overlap (no duplicate CMP values)
    auto cmpMaxA = std::max(a.first, a.second);
    auto cmpMinB = std::min(b.first, b.second);

    return cmpMaxA < cmpMinB;
}

size_t
SegyLine2D::getTraceSize(size_t samplesCount) const
{
    return traceHeaderSize_ + samplesCount * sampleSize_;
}

} // namespace segysdk::segyindex::index_2d