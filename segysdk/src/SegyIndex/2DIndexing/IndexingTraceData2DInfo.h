/*
Copyright 2024 Schlumberger

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

#include "SegyLine2D.h"

#include "../../FileIO/FileWriter.h"
#include "../../FileIO/SequentialFileReader.h"
#include "../../SegyIndex/SegyIndexConfig.h"
#include "../../SegyIndex/VerticalExtent.h"
#include "../2DIndexing/SeismicLineExtents.h"
#include "../SegyIndex/IndexBuilderData.h"

#include <json/json.h>

#include <memory>
#include <string>

namespace segysdk::segyindex
{

namespace index_2d
{

struct IndexData2DVersion
{
    const int major_ = index_config::IndexMajorVersion_2024_01_;
    const int minor_ = index_config::IndexMinorVersion_2024_01_;
};

struct SEGYSDK_DLL_PUBLIC IndexingTraceData2DInfo
{
    IndexingTraceData2DInfo() = default;

    ~IndexingTraceData2DInfo() = default;

    void updateWithIndexData(const IndexBuilderData& data);

    size_t getIndexedTraceCount() const;

    void clear();

    bool isRegular() const;
    bool isValid() const;

    size_t getFileOffsetForCMP(int CMP) const;
    size_t getFileOffsetForTrace(size_t traceNo) const;
    size_t getTraceNoForCMP(int CMP, bool findNextClosest = false) const;

    void setSampleSize(size_t sampleSize);
    void setTraceHeaderSize(size_t traceHeaderSize);

    void setSeismicLineExtents(VerticalExtent sampleExtents);
    void setSeismicLineExtents(SeismicLineExtents lineExtents);
    const SeismicLineExtents& getLineExtents() const;

    bool saveIndex(std::shared_ptr<segysdk::fileio::FileWriter>& writer) const;
    bool loadIndex(segysdk::fileio::BufferedFileReaderBase& reader, IndexData2DVersion&& version);

    Json::Value toJsonValue() const;

private:
    IndexingTraceData2DInfo(IndexingTraceData2DInfo const&) = default;
    IndexingTraceData2DInfo& operator=(IndexingTraceData2DInfo const&) = default;
    IndexingTraceData2DInfo(IndexingTraceData2DInfo&&) = default;
    IndexingTraceData2DInfo& operator=(IndexingTraceData2DInfo&&) = default;

    bool populateIndexFrom_2024_01(segysdk::fileio::BufferedFileReaderBase& reader);

private:
    SegyLine2D line_;
    SeismicLineExtents lineExtents_{{0, 0, 0}, {0, 0, 0}};
};

} // namespace index_2d
} // namespace segysdk::segyindex
